/* Routines for streaming PPH data.
   Copyright (C) 2011 Free Software Foundation, Inc.
   Contributed by Diego Novillo <dnovillo@google.com>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "langhooks.h"
#include "tree-iterator.h"
#include "tree-pretty-print.h"
#include "lto-streamer.h"
#include "pph-streamer.h"
#include "pph.h"
#include "tree-pass.h"
#include "version.h"
#include "cppbuiltin.h"

/* FIXME pph.  This holds the FILE handle for the current PPH file
   that we are writing.  It is necessary because the LTO callbacks do
   not allow passing a FILE handle to them.  */
static FILE *current_pph_file = NULL;


/* Get the section with name NAME and type SECTION_TYPE from FILE_DATA.
   Return a pointer to the start of the section contents and store
   the length of the section in *LEN_P.

   FIXME pph, this does not currently handle multiple sections.  It
   assumes that the file has exactly one section.  */

static const char *
pph_get_section_data (struct lto_file_decl_data *file_data,
		      enum lto_section_type section_type ATTRIBUTE_UNUSED,
		      const char *name ATTRIBUTE_UNUSED,
		      size_t *len)
{
  /* FIXME pph - Stop abusing lto_file_decl_data fields.  */
  const pph_stream *stream = (const pph_stream *) file_data->file_name;
  *len = stream->file_size - sizeof (pph_file_header);
  return (const char *) stream->file_data + sizeof (pph_file_header);
}


/* Free the section data from FILE_DATA of SECTION_TYPE and NAME that
   starts at OFFSET and has LEN bytes.  */

static void
pph_free_section_data (struct lto_file_decl_data *file_data,
		   enum lto_section_type section_type ATTRIBUTE_UNUSED,
		   const char *name ATTRIBUTE_UNUSED,
		   const char *offset ATTRIBUTE_UNUSED,
		   size_t len ATTRIBUTE_UNUSED)
{
  /* FIXME pph - Stop abusing lto_file_decl_data fields.  */
  const pph_stream *stream = (const pph_stream *) file_data->file_name;
  free (stream->file_data);
}


/* Read into memory the contents of the file in STREAM.  Initialize
   internal tables and data structures needed to re-construct the
   ASTs in the file.  */

static void
pph_stream_init_read (pph_stream *stream)
{
  struct stat st;
  size_t i, bytes_read, strtab_size, body_size;
  int retcode;
  pph_file_header *header;
  const char *strtab, *body;

  /* Read STREAM->NAME into the memory buffer STREAM->FILE_DATA.
     FIXME pph, we are reading the whole file at once.  This seems
     wasteful.  */
  retcode = fstat (fileno (stream->file), &st);
  gcc_assert (retcode == 0);
  stream->file_size = (size_t) st.st_size;
  stream->file_data = XCNEWVEC (char, stream->file_size);
  bytes_read = fread (stream->file_data, 1, stream->file_size, stream->file);
  gcc_assert (bytes_read == stream->file_size);

  /* Set LTO callbacks to read the PPH file.  */
  stream->pph_sections = XCNEWVEC (struct lto_file_decl_data *,
				   PPH_NUM_SECTIONS);
  for (i = 0; i < PPH_NUM_SECTIONS; i++)
    {
      stream->pph_sections[i] = XCNEW (struct lto_file_decl_data);
      /* FIXME pph - Stop abusing fields in lto_file_decl_data.  */
      stream->pph_sections[i]->file_name = (const char *) stream;
    }

  lto_set_in_hooks (stream->pph_sections, pph_get_section_data,
		    pph_free_section_data);

  header = (pph_file_header *) stream->file_data;
  strtab = (const char *) header + sizeof (pph_file_header);
  strtab_size = header->strtab_size;
  body = strtab + strtab_size;
  gcc_assert (stream->file_size >= strtab_size + sizeof (pph_file_header));
  body_size = stream->file_size - strtab_size - sizeof (pph_file_header);

  /* Create an input block structure pointing right after the string
     table.  */
  stream->ib = XCNEW (struct lto_input_block);
  LTO_INIT_INPUT_BLOCK_PTR (stream->ib, body, 0, body_size);
  stream->data_in = lto_data_in_create (stream->pph_sections[0], strtab,
                                        strtab_size, NULL);
}


/* Initialize buffers and tables in STREAM for writing.  */

static void
pph_stream_init_write (pph_stream *stream)
{
  stream->out_state = lto_new_out_decl_state ();
  lto_push_out_decl_state (stream->out_state);
  stream->decl_state_stream = XCNEW (struct lto_output_stream);
  stream->ob = create_output_block (LTO_section_decls);
  pph_set_ob_stream (stream->ob, stream);
}


/* Write all the fields in lang_decl_base instance LDB to OB.  */

static void
pph_stream_write_ld_base (struct output_block *ob, struct lang_decl_base *ldb)
{
  struct bitpack_d bp;

  if (ldb == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  bp = bitpack_create (ob->main_stream);
  bp_pack_value (&bp, ldb->selector, 16);
  bp_pack_value (&bp, ldb->language, 4);
  bp_pack_value (&bp, ldb->use_template, 2);
  bp_pack_value (&bp, ldb->not_really_extern, 1);
  bp_pack_value (&bp, ldb->initialized_in_class, 1);
  bp_pack_value (&bp, ldb->repo_available_p, 1);
  bp_pack_value (&bp, ldb->threadprivate_or_deleted_p, 1);
  bp_pack_value (&bp, ldb->anticipated_p, 1);
  bp_pack_value (&bp, ldb->friend_attr, 1);
  bp_pack_value (&bp, ldb->template_conv_p, 1);
  bp_pack_value (&bp, ldb->odr_used, 1);
  bp_pack_value (&bp, ldb->u2sel, 1);
  lto_output_bitpack (&bp);
}


/* Write all the fields in lang_decl_min instance LDM to OB.  If REF_P
   is true, all tree fields should be written as references.  */

static void
pph_stream_write_ld_min (struct output_block *ob, struct lang_decl_min *ldm,
		         bool ref_p)
{
  if (ldm == 0)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  gcc_assert (ldm->base.selector == 0);

  lto_output_tree_or_ref (ob, ldm->template_info, ref_p);
  if (ldm->base.u2sel == 0)
    lto_output_tree_or_ref (ob, ldm->u2.access, ref_p);
  else if (ldm->base.u2sel == 1)
    lto_output_sleb128_stream (ob->main_stream, ldm->u2.discriminator);
  else
    gcc_unreachable ();
}


/* Write all the trees in VEC V to OB.  REF_P is true if the trees should
   be written as references.  */

static void
pph_stream_write_tree_vec (struct output_block *ob, VEC(tree,gc) *v, bool ref_p)
{
  unsigned i;
  tree t;

  lto_output_uleb128_stream (ob->main_stream, VEC_length (tree, v));
  for (i = 0; VEC_iterate (tree, v, i, t); i++)
    lto_output_tree_or_ref (ob, t, ref_p);
}

/* Forward declaration to break cyclic dependencies.  */
static void pph_stream_write_binding_level (struct output_block *,
					    struct cp_binding_level *, bool);


/* Helper for pph_stream_write_cxx_binding.  OB, CB and REF_P are as in
   pph_stream_write_cxx_binding.  */

static void
pph_stream_write_cxx_binding_1 (struct output_block *ob, cxx_binding *cb,
				bool ref_p)
{
  struct bitpack_d bp;

  if (cb == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  lto_output_tree_or_ref (ob, cb->value, ref_p);
  lto_output_tree_or_ref (ob, cb->type, ref_p);
  pph_stream_write_binding_level (ob, cb->scope, ref_p);
  bp = bitpack_create (ob->main_stream);
  bp_pack_value (&bp, cb->value_is_inherited, 1);
  bp_pack_value (&bp, cb->is_local, 1);
  lto_output_bitpack (&bp);
}


/* Write all the fields of cxx_binding instance CB to OB.  REF_P is
   true if the tree fields should be written as references.  */

static void
pph_stream_write_cxx_binding (struct output_block *ob, cxx_binding *cb,
			      bool ref_p)
{
  unsigned num_bindings;
  cxx_binding *prev;

  if (cb == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  for (num_bindings = 0, prev = cb->previous; prev; prev = prev->previous)
    num_bindings++;

  /* Write the list of previous bindings.  */
  lto_output_sleb128_stream (ob->main_stream, num_bindings);
  for (prev = cb->previous; prev; prev = prev->previous)
    pph_stream_write_cxx_binding_1 (ob, prev, ref_p);

  /* Write the current binding at the end.  */
  pph_stream_write_cxx_binding_1 (ob, cb, ref_p);
}


/* Write all the fields of cp_class_binding instance CB to OB.  REF_P
   is true if the tree fields should be written as references.  */

static void
pph_stream_write_class_binding (struct output_block *ob,
				cp_class_binding *cb, bool ref_p)
{
  if (cb == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  pph_stream_write_cxx_binding (ob, &cb->base, ref_p);
  lto_output_tree_or_ref (ob, cb->identifier, ref_p);
}


/* Write all the fields of cp_label_binding instance LB to OB.  If
   REF_P is true, tree fields will be written as references.  */

static void
pph_stream_write_label_binding (struct output_block *ob,
				cp_label_binding *lb, bool ref_p)
{
  if (lb == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  lto_output_tree_or_ref (ob, lb->label, ref_p);
  lto_output_tree_or_ref (ob, lb->prev_value, ref_p);
}


/* Write all the fields of cp_binding_level instance BL to OB.  If
   REF_P is true, tree fields will be written as references.  */

static void
pph_stream_write_binding_level (struct output_block *ob,
			        struct cp_binding_level *bl, bool ref_p)
{
  unsigned i;
  cp_class_binding *cs;
  cp_label_binding *sl;
  struct bitpack_d bp;

  if (bl == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  lto_output_chain (ob, bl->names, ref_p);
  lto_output_uleb128_stream (ob->main_stream, bl->names_size);
  lto_output_chain (ob, bl->namespaces, ref_p);

  pph_stream_write_tree_vec (ob, bl->static_decls, ref_p);

  lto_output_chain (ob, bl->usings, ref_p);
  lto_output_chain (ob, bl->using_directives, ref_p);

  lto_output_uleb128_stream (ob->main_stream, VEC_length (cp_class_binding,
					                  bl->class_shadowed));
  for (i = 0; VEC_iterate (cp_class_binding, bl->class_shadowed, i, cs); i++)
    pph_stream_write_class_binding (ob, cs, ref_p);

  lto_output_tree_or_ref (ob, bl->type_shadowed, ref_p);

  lto_output_uleb128_stream (ob->main_stream, VEC_length (cp_label_binding,
					                  bl->shadowed_labels));
  for (i = 0; VEC_iterate (cp_label_binding, bl->shadowed_labels, i, sl); i++)
    pph_stream_write_label_binding (ob, sl, ref_p);

  lto_output_chain (ob, bl->blocks, ref_p);
  lto_output_tree_or_ref (ob, bl->this_entity, ref_p);
  pph_stream_write_binding_level (ob, bl->level_chain, ref_p);
  pph_stream_write_tree_vec (ob, bl->dead_vars_from_for, ref_p);
  lto_output_chain (ob, bl->statement_list, ref_p);
  lto_output_sleb128_stream (ob->main_stream, bl->binding_depth);

  bp = bitpack_create (ob->main_stream);
  bp_pack_value (&bp, bl->kind, 4);
  bp_pack_value (&bp, bl->keep, 1);
  bp_pack_value (&bp, bl->more_cleanups_ok, 1);
  bp_pack_value (&bp, bl->have_cleanups, 1);
  lto_output_bitpack (&bp);
}


/* Write all the fields of c_language_function instance CLF to OB.  If
   REF_P is true, all tree fields should be written as references.  */

static void
pph_stream_write_c_language_function (struct output_block *ob,
				      struct c_language_function *clf,
				      bool ref_p)
{
  if (clf == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  lto_output_tree_or_ref (ob, clf->x_stmt_tree.x_cur_stmt_list, ref_p);
  lto_output_sleb128_stream (ob->main_stream,
			     clf->x_stmt_tree.stmts_are_full_exprs_p);
}


/* Write all the fields of language_function instance LF to OB.  If
   REF_P is true, all tree fields should be written as references.  */

static void
pph_stream_write_language_function (struct output_block *ob,
				    struct language_function *lf,
				    bool ref_p)
{
  struct bitpack_d bp;

  if (lf == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  pph_stream_write_c_language_function (ob, &lf->base, ref_p);
  lto_output_tree_or_ref (ob, lf->x_cdtor_label, ref_p);
  lto_output_tree_or_ref (ob, lf->x_current_class_ptr, ref_p);
  lto_output_tree_or_ref (ob, lf->x_current_class_ref, ref_p);
  lto_output_tree_or_ref (ob, lf->x_eh_spec_block, ref_p);
  lto_output_tree_or_ref (ob, lf->x_in_charge_parm, ref_p);
  lto_output_tree_or_ref (ob, lf->x_vtt_parm, ref_p);
  lto_output_tree_or_ref (ob, lf->x_return_value, ref_p);
  bp = bitpack_create (ob->main_stream);
  bp_pack_value (&bp, lf->x_returns_value, 1);
  bp_pack_value (&bp, lf->x_returns_null, 1);
  bp_pack_value (&bp, lf->x_returns_abnormally, 1);
  bp_pack_value (&bp, lf->x_in_function_try_handler, 1);
  bp_pack_value (&bp, lf->x_in_base_initializer, 1);
  bp_pack_value (&bp, lf->can_throw, 1);
  lto_output_bitpack (&bp);

  /* FIXME pph.  We are not writing lf->x_named_labels.  */

  pph_stream_write_binding_level (ob, lf->bindings, ref_p);
  pph_stream_write_tree_vec (ob, lf->x_local_names, ref_p);

  /* FIXME pph.  We are not writing lf->extern_decl_map.  */
}


/* Write all the fields of lang_decl_fn instance LDF to OB.  If REF_P
   is true, all tree fields should be written as references.  */

static void
pph_stream_write_ld_fn (struct output_block *ob, struct lang_decl_fn *ldf,
			bool ref_p)
{
  struct bitpack_d bp;

  if (ldf == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  bp = bitpack_create (ob->main_stream);
  bp_pack_value (&bp, ldf->operator_code, 16);
  bp_pack_value (&bp, ldf->global_ctor_p, 1);
  bp_pack_value (&bp, ldf->global_dtor_p, 1);
  bp_pack_value (&bp, ldf->constructor_attr, 1);
  bp_pack_value (&bp, ldf->destructor_attr, 1);
  bp_pack_value (&bp, ldf->assignment_operator_p, 1);
  bp_pack_value (&bp, ldf->static_function, 1);
  bp_pack_value (&bp, ldf->pure_virtual, 1);
  bp_pack_value (&bp, ldf->defaulted_p, 1);
  bp_pack_value (&bp, ldf->has_in_charge_parm_p, 1);
  bp_pack_value (&bp, ldf->has_vtt_parm_p, 1);
  bp_pack_value (&bp, ldf->pending_inline_p, 1);
  bp_pack_value (&bp, ldf->nonconverting, 1);
  bp_pack_value (&bp, ldf->thunk_p, 1);
  bp_pack_value (&bp, ldf->this_thunk_p, 1);
  bp_pack_value (&bp, ldf->hidden_friend_p, 1);
  lto_output_bitpack (&bp);

  lto_output_tree_or_ref (ob, ldf->befriending_classes, ref_p);
  lto_output_tree_or_ref (ob, ldf->context, ref_p);

  if (ldf->thunk_p == 0)
    lto_output_tree_or_ref (ob, ldf->u5.cloned_function, ref_p);
  else if (ldf->thunk_p == 1)
    lto_output_sleb128_stream (ob->main_stream, ldf->u5.fixed_offset);
  else
    gcc_unreachable ();

  if (ldf->pending_inline_p == 1)
    pth_save_token_cache (ldf->u.pending_inline_info, pph_get_ob_stream (ob));
  else if (ldf->pending_inline_p == 0)
    pph_stream_write_language_function (ob, ldf->u.saved_language_function,
					ref_p);
}


/* Write all the fields of lang_decl_ns instance LDNS to OB.  If REF_P
   is true, all tree fields should be written as references.  */

static void
pph_stream_write_ld_ns (struct output_block *ob, struct lang_decl_ns *ldns,
			bool ref_p)
{
  struct cp_binding_level *level;

  if (ldns == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  level = ldns->level;
  pph_stream_write_binding_level (ob, level, ref_p);
}


/* Write all the fields of lang_decl_parm instance LDP to OB.  If REF_P
   is true, all tree fields should be written as references.  */

static void
pph_stream_write_ld_parm (struct output_block *ob, struct lang_decl_parm *ldp)
{
  if (ldp == NULL)
    {
      pph_output_uint (pph_get_ob_stream (ob), 0);
      return;
    }

  lto_output_sleb128_stream (ob->main_stream, ldp->level);
  lto_output_sleb128_stream (ob->main_stream, ldp->index);
}


/* Write all the lang-specific data in DECL to OB.  REF_P is true if
   the trees referenced in lang-specific fields should be written as
   references.  */

static void
pph_stream_write_lang_specific_data (struct output_block *ob,
				     tree decl, bool ref_p)
{
  struct lang_decl *ld;
  struct lang_decl_base *ldb;

  gcc_assert (DECL_P (decl) && DECL_LANG_SPECIFIC (decl));

  ld = DECL_LANG_SPECIFIC (decl);
  ldb = &ld->u.base;

  /* Write all the fields in lang_decl_base.  */
  pph_stream_write_ld_base (ob, ldb);
  
  if (ldb->selector == 0)
    {
      /* Write all the fields in lang_decl_min.  */
      pph_stream_write_ld_min (ob, &ld->u.min, ref_p);
    }
  else if (ldb->selector == 1)
    {
      /* Write all the fields in lang_decl_fn.  */
      pph_stream_write_ld_fn (ob, &ld->u.fn, ref_p);
    }
  else if (ldb->selector == 2)
    {
      /* Write all the fields in lang_decl_ns.  */
      pph_stream_write_ld_ns (ob, &ld->u.ns, ref_p);
    }
  else if (ldb->selector == 3)
    {
      /* Write all the fields in lang_decl_parm.  */
      pph_stream_write_ld_parm (ob, &ld->u.parm);
    }
  else
    gcc_unreachable ();
}


/* Callback for writing ASTs to a stream.  This writes all the fields
   that are not processed by default by the common tree pickler.
   OB and REF_P are as in lto_write_tree.  EXPR is the tree to write.  */

static void
pph_stream_write_tree (struct output_block *ob, tree expr, bool ref_p)
{
  if (DECL_P (expr))
    {
      if (DECL_LANG_SPECIFIC (expr))
	pph_stream_write_lang_specific_data (ob, expr, ref_p);

      if (TREE_CODE (expr) == FUNCTION_DECL)
	lto_output_tree_or_ref (ob, DECL_SAVED_TREE (expr), ref_p);
    }
  else if (TREE_CODE (expr) == STATEMENT_LIST)
    {
      tree_stmt_iterator i;
      unsigned num_stmts;

      /* Compute and write the number of statements in the list.  */
      for (num_stmts = 0, i = tsi_start (expr); !tsi_end_p (i); tsi_next (&i))
	num_stmts++;

      lto_output_sleb128_stream (ob->main_stream, num_stmts);

      /* Write the statements.  */
      for (i = tsi_start (expr); !tsi_end_p (i); tsi_next (&i))
	lto_output_tree_or_ref (ob, tsi_stmt (i), ref_p);
    }
}


/* Callback for reading ASTs from a stream.  This reads all the fields
   that are not processed by default by the common tree pickler.
   IB, DATA_IN are as in lto_read_tree.  EXPR is the partially materialized
   tree.  */

static void
pph_stream_read_tree (struct lto_input_block *ib, struct data_in *data_in,
		      tree expr)
{
  if (TREE_CODE (expr) == FUNCTION_DECL)
    DECL_SAVED_TREE (expr) = lto_input_tree (ib, data_in);
  else if (TREE_CODE (expr) == STATEMENT_LIST)
    {
      HOST_WIDE_INT i, num_trees = lto_input_sleb128 (ib);
      for (i = 0; i < num_trees; i++)
	{
	  tree stmt = lto_input_tree (ib, data_in);
	  append_to_statement_list (stmt, &expr);
	}
    }
}


/* Return true if the given tree T is streamable.  */

static bool
pph_is_streamable (tree t ATTRIBUTE_UNUSED)
{
  /* We accept most trees.  */
  return TREE_CODE (t) != SSA_NAME
	 && (TREE_CODE (t) < OMP_PARALLEL
	     || TREE_CODE (t) > OMP_CRITICAL);
}


/* Callback for packing value fields in ASTs.  BP is the bitpack 
   we are packing into.  EXPR is the tree to pack.  */

static void
pph_stream_pack_value_fields (struct bitpack_d *bp ATTRIBUTE_UNUSED,
			      tree expr ATTRIBUTE_UNUSED)
{
  /* Do nothing for now.  */
}


/* Callback for unpacking value fields in ASTs.  BP is the bitpack 
   we are unpacking from.  EXPR is the tree to unpack.  */

static void
pph_stream_unpack_value_fields (struct bitpack_d *bp ATTRIBUTE_UNUSED,
				tree expr ATTRIBUTE_UNUSED)
{
  /* Do nothing for now.  */
}


/* Return true if T can be emitted in the decls table as a reference.
   This should only handle C++ specific declarations.  All others are
   handled by the LTO streamer directly.  */

static bool
pph_indexable_with_decls_p (tree t)
{
  return TREE_CODE (t) == TEMPLATE_DECL;
}


/* Initialize all the streamer hooks used for streaming ASTs.  */

static void
pph_stream_hooks_init (void)
{
  lto_streamer_hooks *h = streamer_hooks_init ();
  h->name = "C++ AST";
  h->is_streamable = pph_is_streamable;
  h->write_tree = pph_stream_write_tree;
  h->read_tree = pph_stream_read_tree;
  h->pack_value_fields = pph_stream_pack_value_fields;
  h->indexable_with_decls_p = pph_indexable_with_decls_p;
  h->unpack_value_fields = pph_stream_unpack_value_fields;
}


/* Create a new PPH stream to be stored on the file called NAME.
   MODE is passed to fopen directly.  */

pph_stream *
pph_stream_open (const char *name, const char *mode)
{
  pph_stream *stream;
  FILE *f;

  stream = NULL;
  f = fopen (name, mode);
  if (f)
    {
      pph_stream_hooks_init ();
      stream = XCNEW (pph_stream);
      stream->file = f;
      stream->name = xstrdup (name);
      stream->write_p = (strchr (mode, 'w') != NULL);
      if (stream->write_p)
	pph_stream_init_write (stream);
      else
	pph_stream_init_read (stream);
    }

  return stream;
}


/* Callback for lang_hooks.lto.begin_section.  Open file NAME.  */

static void
pph_stream_begin_section (const char *name ATTRIBUTE_UNUSED)
{
}


/* Callback for lang_hooks.lto.append_data.  Write LEN bytes from DATA
   into current_pph_file.  BLOCK is currently unused.  */

static void
pph_stream_write (const void *data, size_t len, void *block ATTRIBUTE_UNUSED)
{
  if (data)
    fwrite (data, len, 1, current_pph_file);
}


/* Callback for lang_hooks.lto.end_section.  */

static void
pph_stream_end_section (void)
{
}


/* Write the header for the PPH file represented by STREAM.  */

static void
pph_stream_write_header (pph_stream *stream)
{
  pph_file_header header;
  struct lto_output_stream header_stream;
  int major, minor, patchlevel;

  /* Collect version information.  */
  parse_basever (&major, &minor, &patchlevel);
  gcc_assert (major == (char) major);
  gcc_assert (minor == (char) minor);
  gcc_assert (patchlevel == (char) patchlevel);

  /* Write the header for the PPH file.  */
  memset (&header, 0, sizeof (header));
  strcpy (header.id_str, pph_id_str);
  header.major_version = (char) major;
  header.minor_version = (char) minor;
  header.patchlevel = (char) patchlevel;
  header.strtab_size = stream->ob->string_stream->total_size;

  memset (&header_stream, 0, sizeof (header_stream));
  lto_output_data_stream (&header_stream, &header, sizeof (header));
  lto_write_stream (&header_stream);
}


/* Write the body of the PPH file represented by STREAM.  */

static void
pph_stream_write_body (pph_stream *stream)
{
  /* Write the string table.  */
  lto_write_stream (stream->ob->string_stream);

  /* Write out the physical representation for every AST in all the
     streams in STREAM->OUT_STATE.  */
  lto_output_decl_state_streams (stream->ob, stream->out_state);

  /* Now write the vector of all AST references.  */
  lto_output_decl_state_refs (stream->ob, stream->decl_state_stream,
			      stream->out_state);

  /* Finally, physically write all the streams.  */
  lto_write_stream (stream->ob->main_stream);
}


/* Close PPH stream STREAM.  Write all the ASTs to disk and deallocate
   all memory used by it.  */

void
pph_stream_close (pph_stream *stream)
{
  if (stream->write_p)
    {
      gcc_assert (current_pph_file == NULL);
      current_pph_file = stream->file;

      /* Redirect the LTO basic I/O langhooks.  */
      lang_hooks.lto.begin_section = pph_stream_begin_section;
      lang_hooks.lto.append_data = pph_stream_write;
      lang_hooks.lto.end_section = pph_stream_end_section;

      /* Write the state buffers built by pph_output_*() calls.  */
      lto_begin_section (stream->name, false);
      pph_stream_write_header (stream);
      pph_stream_write_body (stream);
      lto_end_section ();
    }

  fclose (stream->file);
  stream->file = current_pph_file = NULL;
}


/* Data types supported by the PPH tracer.  */
enum pph_trace_type
{
    PPH_TRACE_TREE,
    PPH_TRACE_UINT,
    PPH_TRACE_BYTES,
    PPH_TRACE_STRING
};

/* Print tracing information for STREAM on pph_logfile.  DATA is the
   memory area to display, SIZE is the number of bytes to print, TYPE
   is the kind of data to print.  */

static void
pph_stream_trace (pph_stream *stream, const void *data, unsigned int nbytes,
		  enum pph_trace_type type)
{
  const char *op = (stream->write_p) ? "write" : "read";
  const char *type_s[] = { "tree", "uint", "bytes", "string" };

  fprintf (pph_logfile, "*** %s: op=%s, type=%s, size=%u, value=",
	   stream->name, op, type_s[type], (unsigned) nbytes);

  switch (type)
    {
    case PPH_TRACE_TREE:
      {
	const_tree t = (const_tree) data;
	print_generic_expr (pph_logfile, CONST_CAST (union tree_node *, t),
			    TDF_SLIM);
      }
      break;

    case PPH_TRACE_UINT:
      {
	unsigned int val = *((const unsigned int *) data);
	fprintf (pph_logfile, "%u (0x%x)", val, val);
      }
      break;

    case PPH_TRACE_BYTES:
      {
	size_t i;
	const char *buffer = (const char *) data;
	for (i = 0; i < MIN (nbytes, 100); i++)
	  {
	    if (ISPRINT (buffer[i]))
	      fprintf (pph_logfile, "%c", buffer[i]);
	    else
	      fprintf (pph_logfile, "[0x%02x]", (unsigned int) buffer[i]);
	  }
      }
      break;

    case PPH_TRACE_STRING:
      if (data)
	fprintf (pph_logfile, "%.*s", (int) nbytes, (const char *) data);
      else
	fprintf (pph_logfile, "<nil>");
      break;

    default:
      gcc_unreachable ();
    }

  fputc ('\n', pph_logfile);
}


/* Show tracing information for T on STREAM.  */

void
pph_stream_trace_tree (pph_stream *stream, tree t)
{
  pph_stream_trace (stream, t, tree_code_size (TREE_CODE (t)), PPH_TRACE_TREE);
}


/* Show tracing information for VAL on STREAM.  */

void
pph_stream_trace_uint (pph_stream *stream, unsigned int val)
{
  pph_stream_trace (stream, &val, sizeof (val), PPH_TRACE_UINT);
}


/* Show tracing information for NBYTES bytes of memory area DATA on
   STREAM.  */

void
pph_stream_trace_bytes (pph_stream *stream, const void *data, size_t nbytes)
{
  pph_stream_trace (stream, data, nbytes, PPH_TRACE_BYTES);
}


/* Show tracing information for S on STREAM.  */

void
pph_stream_trace_string (pph_stream *stream, const char *s)
{
  pph_stream_trace (stream, s, s ? strlen (s) : 0, PPH_TRACE_STRING);
}


/* Show tracing information for LEN bytes of S on STREAM.  */

void
pph_stream_trace_string_with_length (pph_stream *stream, const char *s,
				     unsigned int len)
{
  pph_stream_trace (stream, s, len, PPH_TRACE_STRING);
}
