/* Routines for reading PPH data.
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

/* Callback for unpacking value fields in ASTs.  BP is the bitpack 
   we are unpacking from.  EXPR is the tree to unpack.  */

void
pph_stream_unpack_value_fields (struct bitpack_d *bp, tree expr)
{
  if (TYPE_P (expr))
    {
      TYPE_LANG_FLAG_0 (expr) = bp_unpack_value (bp, 1);
      TYPE_LANG_FLAG_1 (expr) = bp_unpack_value (bp, 1);
      TYPE_LANG_FLAG_2 (expr) = bp_unpack_value (bp, 1);
      TYPE_LANG_FLAG_3 (expr) = bp_unpack_value (bp, 1);
      TYPE_LANG_FLAG_4 (expr) = bp_unpack_value (bp, 1);
      TYPE_LANG_FLAG_5 (expr) = bp_unpack_value (bp, 1);
      TYPE_LANG_FLAG_6 (expr) = bp_unpack_value (bp, 1);
    }
  else if (DECL_P (expr))
    {
      DECL_LANG_FLAG_0 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_1 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_2 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_3 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_4 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_5 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_6 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_7 (expr) = bp_unpack_value (bp, 1);
      DECL_LANG_FLAG_8 (expr) = bp_unpack_value (bp, 1);
    }

  TREE_LANG_FLAG_0 (expr) = bp_unpack_value (bp, 1);
  TREE_LANG_FLAG_1 (expr) = bp_unpack_value (bp, 1);
  TREE_LANG_FLAG_2 (expr) = bp_unpack_value (bp, 1);
  TREE_LANG_FLAG_3 (expr) = bp_unpack_value (bp, 1);
  TREE_LANG_FLAG_4 (expr) = bp_unpack_value (bp, 1);
  TREE_LANG_FLAG_5 (expr) = bp_unpack_value (bp, 1);
  TREE_LANG_FLAG_6 (expr) = bp_unpack_value (bp, 1);
}


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

void
pph_stream_init_read (pph_stream *stream)
{
  struct stat st;
  size_t i, bytes_read, strtab_size, body_size;
  int retcode;
  pph_file_header *header;
  const char *strtab, *body;

  lto_reader_init ();

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

  /* Associate STREAM with STREAM->DATA_IN so we can recover it from
     the streamer hooks.  */
  stream->data_in->sdata = (void *) stream;
}


/* Read and return a record marker from STREAM.  The marker
   must be one of PPH_RECORD_START or PPH_RECORD_END.  If PPH_RECORD_END
   is read, return false.  Otherwise, return true.  */

static inline bool
pph_start_record (pph_stream *stream)
{
  unsigned char marker = pph_input_uchar (stream);
  gcc_assert (marker == PPH_RECORD_START || marker == PPH_RECORD_END);
  return (marker == PPH_RECORD_START);
}


/* Read all fields in lang_decl_base instance LDB from STREAM.  */

static void
pph_stream_read_ld_base (pph_stream *stream, struct lang_decl_base *ldb)
{
  struct bitpack_d bp;

  if (!pph_start_record (stream))
    return;

  bp = pph_input_bitpack (stream);
  ldb->selector = bp_unpack_value (&bp, 16);
  ldb->language = (enum languages) bp_unpack_value (&bp, 4);
  ldb->use_template = bp_unpack_value (&bp, 2);
  ldb->not_really_extern = bp_unpack_value (&bp, 1);
  ldb->initialized_in_class = bp_unpack_value (&bp, 1);
  ldb->repo_available_p = bp_unpack_value (&bp, 1);
  ldb->threadprivate_or_deleted_p = bp_unpack_value (&bp, 1);
  ldb->anticipated_p = bp_unpack_value (&bp, 1);
  ldb->friend_attr = bp_unpack_value (&bp, 1);
  ldb->template_conv_p = bp_unpack_value (&bp, 1);
  ldb->odr_used = bp_unpack_value (&bp, 1);
  ldb->u2sel = bp_unpack_value (&bp, 1);
}


/* Read all the fields in lang_decl_min instance LDM from STREAM.  */

static void
pph_stream_read_ld_min (pph_stream *stream, struct lang_decl_min *ldm)
{
  if (!pph_start_record (stream))
    return;

  gcc_assert (ldm->base.selector == 0);

  ldm->template_info = pph_input_tree (stream);
  if (ldm->base.u2sel == 0)
    ldm->u2.access = pph_input_tree (stream);
  else if (ldm->base.u2sel == 1)
    ldm->u2.discriminator = pph_input_uint (stream);
  else
    gcc_unreachable ();
}


/* Read and return a VEC of trees from STREAM.  */

static VEC(tree,gc) *
pph_stream_read_tree_vec (pph_stream *stream)
{
  unsigned i, num;
  VEC(tree,gc) *v;

  num = pph_input_uint (stream);
  v = NULL;
  for (i = 0; i < num; i++)
    {
      tree t = pph_input_tree (stream);
      VEC_safe_push (tree, gc, v, t);
    }

  return v;
}

/* Forward declaration to break cyclic dependencies.  */
static struct cp_binding_level *pph_stream_read_binding_level (pph_stream *);

/* Helper for pph_stream_read_cxx_binding.  Read and return a cxx_binding
   instance from STREAM.  */

static cxx_binding *
pph_stream_read_cxx_binding_1 (pph_stream *stream)
{
  struct bitpack_d bp;
  cxx_binding *cb;
  tree value, type;

  if (!pph_start_record (stream))
    return NULL;

  value = pph_input_tree (stream);
  type = pph_input_tree (stream);
  cb = cxx_binding_make (value, type);
  cb->scope = pph_stream_read_binding_level (stream);
  bp = pph_input_bitpack (stream);
  cb->value_is_inherited = bp_unpack_value (&bp, 1);
  cb->is_local = bp_unpack_value (&bp, 1);

  return cb;
}


/* Read and return an instance of cxx_binding from STREAM.  */

static cxx_binding *
pph_stream_read_cxx_binding (pph_stream *stream)
{
  unsigned i, num_bindings;
  cxx_binding *curr, *cb;

  if (!pph_start_record (stream))
    return NULL;

  /* Read the list of previous bindings.  */
  num_bindings = pph_input_uint (stream);
  for (curr = NULL, i = 0; i < num_bindings; i++)
    {
      cxx_binding *prev = pph_stream_read_cxx_binding_1 (stream);
      if (curr)
	curr->previous = prev;
      curr = prev;
    }

  /* Read the current binding at the end.  */
  cb = pph_stream_read_cxx_binding_1 (stream);
  cb->previous = curr;

  return cb;
}


/* Read all the fields of cp_class_binding instance CB to OB.  REF_P
   is true if the tree fields should be written as references.  */

static cp_class_binding *
pph_stream_read_class_binding (pph_stream *stream)
{
  cp_class_binding *cb;

  if (!pph_start_record (stream))
    return NULL;

  cb = ggc_alloc_cleared_cp_class_binding ();
  memcpy (&cb->base, pph_stream_read_cxx_binding (stream),
	  sizeof (cxx_binding));
  cb->identifier = pph_input_tree (stream);

  return cb;
}


/* Read and return an instance of cp_label_binding from STREAM.  */

static cp_label_binding *
pph_stream_read_label_binding (pph_stream *stream)
{
  cp_label_binding *lb;

  if (!pph_start_record (stream))
    return NULL;

  lb = ggc_alloc_cleared_cp_label_binding ();
  lb->label = pph_input_tree (stream);
  lb->prev_value = pph_input_tree (stream);

  return lb;
}


/* Read and return an instance of cp_binding_level from STREAM.  */

static struct cp_binding_level *
pph_stream_read_binding_level (pph_stream *stream)
{
  unsigned i, num;
  cp_label_binding *sl;
  struct cp_binding_level *bl;
  struct bitpack_d bp;

  if (!pph_start_record (stream))
    return NULL;

  bl = ggc_alloc_cleared_cp_binding_level ();
  bl->names = pph_input_chain (stream);
  bl->names_size = pph_input_uint (stream);
  bl->namespaces = pph_input_chain (stream);

  bl->static_decls = pph_stream_read_tree_vec (stream);

  bl->usings = pph_input_chain (stream);
  bl->using_directives = pph_input_chain (stream);

  num = pph_input_uint (stream);
  bl->class_shadowed = NULL;
  for (i = 0; i < num; i++)
    {
      cp_class_binding *cb = pph_stream_read_class_binding (stream);
      VEC_safe_push (cp_class_binding, gc, bl->class_shadowed, cb);
    }

  bl->type_shadowed = pph_input_tree (stream);

  num = pph_input_uint (stream);
  bl->shadowed_labels = NULL;
  for (i = 0; VEC_iterate (cp_label_binding, bl->shadowed_labels, i, sl); i++)
    {
      cp_label_binding *sl = pph_stream_read_label_binding (stream);
      VEC_safe_push (cp_label_binding, gc, bl->shadowed_labels, sl);
    }

  bl->blocks = pph_input_chain (stream);
  bl->this_entity = pph_input_tree (stream);
  bl->level_chain = pph_stream_read_binding_level (stream);
  bl->dead_vars_from_for = pph_stream_read_tree_vec (stream);
  bl->statement_list = pph_input_chain (stream);
  bl->binding_depth = pph_input_uint (stream);

  bp = pph_input_bitpack (stream);
  bl->kind = (enum scope_kind) bp_unpack_value (&bp, 4);
  bl->keep = bp_unpack_value (&bp, 1);
  bl->more_cleanups_ok = bp_unpack_value (&bp, 1);
  bl->have_cleanups = bp_unpack_value (&bp, 1);

  return bl;
}


/* Read and return an instance of struct c_language_function from STREAM.  */

static struct c_language_function *
pph_stream_read_c_language_function (pph_stream *stream)
{
  struct c_language_function *clf;

  if (!pph_start_record (stream))
    return NULL;

  clf = ggc_alloc_cleared_c_language_function ();
  clf->x_stmt_tree.x_cur_stmt_list = pph_input_tree (stream);
  clf->x_stmt_tree.stmts_are_full_exprs_p = pph_input_uint (stream);

  return clf;
}


/* Read and return an instance of struct language_function from STREAM.  */

static struct language_function *
pph_stream_read_language_function (pph_stream *stream)
{
  struct bitpack_d bp;
  struct language_function *lf;

  if (!pph_start_record (stream))
    return NULL;

  lf = ggc_alloc_cleared_language_function ();
  memcpy (&lf->base, pph_stream_read_c_language_function (stream),
	  sizeof (struct c_language_function));
  lf->x_cdtor_label = pph_input_tree (stream);
  lf->x_current_class_ptr = pph_input_tree (stream);
  lf->x_current_class_ref = pph_input_tree (stream);
  lf->x_eh_spec_block = pph_input_tree (stream);
  lf->x_in_charge_parm = pph_input_tree (stream);
  lf->x_vtt_parm = pph_input_tree (stream);
  lf->x_return_value = pph_input_tree (stream);
  bp = pph_input_bitpack (stream);
  lf->x_returns_value = bp_unpack_value (&bp, 1);
  lf->x_returns_null = bp_unpack_value (&bp, 1);
  lf->x_returns_abnormally = bp_unpack_value (&bp, 1);
  lf->x_in_function_try_handler = bp_unpack_value (&bp, 1);
  lf->x_in_base_initializer = bp_unpack_value (&bp, 1);
  lf->can_throw = bp_unpack_value (&bp, 1);

  /* FIXME pph.  We are not reading lf->x_named_labels.  */

  lf->bindings = pph_stream_read_binding_level (stream);
  lf->x_local_names = pph_stream_read_tree_vec (stream);

  /* FIXME pph.  We are not reading lf->extern_decl_map.  */

  return lf;
}


/* Read all the fields of lang_decl_fn instance LDF from STREAM.  */

static void
pph_stream_read_ld_fn (pph_stream *stream, struct lang_decl_fn *ldf)
{
  struct bitpack_d bp;

  if (!pph_start_record (stream))
    return;

  bp = pph_input_bitpack (stream);
  ldf->operator_code = (enum tree_code) bp_unpack_value (&bp, 16);
  ldf->global_ctor_p = bp_unpack_value (&bp, 1);
  ldf->global_dtor_p = bp_unpack_value (&bp, 1);
  ldf->constructor_attr = bp_unpack_value (&bp, 1);
  ldf->destructor_attr = bp_unpack_value (&bp, 1);
  ldf->assignment_operator_p = bp_unpack_value (&bp, 1);
  ldf->static_function = bp_unpack_value (&bp, 1);
  ldf->pure_virtual = bp_unpack_value (&bp, 1);
  ldf->defaulted_p = bp_unpack_value (&bp, 1);
  ldf->has_in_charge_parm_p = bp_unpack_value (&bp, 1);
  ldf->has_vtt_parm_p = bp_unpack_value (&bp, 1);
  ldf->pending_inline_p = bp_unpack_value (&bp, 1);
  ldf->nonconverting = bp_unpack_value (&bp, 1);
  ldf->thunk_p = bp_unpack_value (&bp, 1);
  ldf->this_thunk_p = bp_unpack_value (&bp, 1);
  ldf->hidden_friend_p = bp_unpack_value (&bp, 1);

  ldf->befriending_classes = pph_input_tree (stream);
  ldf->context = pph_input_tree (stream);

  if (ldf->thunk_p == 0)
    ldf->u5.cloned_function = pph_input_tree (stream);
  else if (ldf->thunk_p == 1)
    ldf->u5.fixed_offset = pph_input_uint (stream);
  else
    gcc_unreachable ();

  if (ldf->pending_inline_p == 1)
    ldf->u.pending_inline_info = pth_load_token_cache (stream);
  else if (ldf->pending_inline_p == 0)
    ldf->u.saved_language_function = pph_stream_read_language_function (stream);
}


/* Read all the fields of lang_decl_ns instance LDNS from STREAM.  */

static void
pph_stream_read_ld_ns (pph_stream *stream, struct lang_decl_ns *ldns)
{
  if (!pph_start_record (stream))
    return;

  ldns->level = pph_stream_read_binding_level (stream);
}


/* Read all the fields of lang_decl_parm instance LDP from STREAM.  */

static void
pph_stream_read_ld_parm (pph_stream *stream, struct lang_decl_parm *ldp)
{
  if (!pph_start_record (stream))
    return;

  ldp->level = pph_input_uint (stream);
  ldp->index = pph_input_uint (stream);
}


/* Read language specific data in DECL from STREAM.  */

static void
pph_stream_read_lang_specific (pph_stream *stream, tree decl)
{
  struct lang_decl *ld;
  struct lang_decl_base *ldb;

  if (!pph_start_record (stream))
    return;

  /* Allocate a lang_decl structure for DECL.  */
  retrofit_lang_decl (decl);

  ld = DECL_LANG_SPECIFIC (decl);
  ldb = &ld->u.base;

  /* Read all the fields in lang_decl_base.  */
  pph_stream_read_ld_base (stream, ldb);

  if (ldb->selector == 0)
    {
      /* Read all the fields in lang_decl_min.  */
      pph_stream_read_ld_min (stream, &ld->u.min);
    }
  else if (ldb->selector == 1)
    {
      /* Read all the fields in lang_decl_fn.  */
      pph_stream_read_ld_fn (stream, &ld->u.fn);
    }
  else if (ldb->selector == 2)
    {
      /* Read all the fields in lang_decl_ns.  */
      pph_stream_read_ld_ns (stream, &ld->u.ns);
    }
  else if (ldb->selector == 3)
    {
      /* Read all the fields in lang_decl_parm.  */
      pph_stream_read_ld_parm (stream, &ld->u.parm);
    }
  else
    gcc_unreachable ();
}


/* Allocate a tree node with code CODE.  IB and DATA_IN are used to
   read more data from the stream, if needed to build this node.
   Return NULL if we did not want to handle this node.  In that case,
   the caller will call make_node to allocate this tree.  */

tree
pph_stream_alloc_tree (enum tree_code code,
	               struct lto_input_block *ib ATTRIBUTE_UNUSED,
		       struct data_in *data_in)
{
  pph_stream *stream = (pph_stream *) data_in->sdata;

  if (code == CALL_EXPR)
    {
      unsigned nargs = pph_input_uint (stream);
      return build_vl_exp (CALL_EXPR, nargs + 3);
    }

  return NULL_TREE;
}


/* Read all the fields in lang_type_header instance LTH from STREAM.  */

static void
pph_stream_read_lang_type_header (pph_stream *stream,
				   struct lang_type_header *lth)
{
  struct bitpack_d bp;

  if (!pph_start_record (stream))
    return;

  bp = pph_input_bitpack (stream);
  lth->is_lang_type_class = bp_unpack_value (&bp, 1);
  lth->has_type_conversion = bp_unpack_value (&bp, 1);
  lth->has_copy_ctor = bp_unpack_value (&bp, 1);
  lth->has_default_ctor = bp_unpack_value (&bp, 1);
  lth->const_needs_init = bp_unpack_value (&bp, 1);
  lth->ref_needs_init = bp_unpack_value (&bp, 1);
  lth->has_const_copy_assign = bp_unpack_value (&bp, 1);
}


/* Read the vector V of tree_pair_s instances from STREAM.  */

static VEC(tree_pair_s,gc) *
pph_stream_read_tree_pair_vec (pph_stream *stream)
{
  unsigned i, num;
  VEC(tree_pair_s,gc) *v;

  num = pph_input_uint (stream);
  for (i = 0, v = NULL; i < num; i++)
    {
      tree_pair_s p;
      p.purpose = pph_input_tree (stream);
      p.value = pph_input_tree (stream);
      VEC_safe_push (tree_pair_s, gc, v, &p);
    }

  return v;
}


/* Read a struct sorted_fields_type instance SFT to STREAM.  REF_P is
   true if the tree nodes should be written as references.  */

static struct sorted_fields_type *
pph_stream_read_sorted_fields_type (pph_stream *stream)
{
  unsigned i, num_fields;
  struct sorted_fields_type *v;

  if (!pph_start_record (stream))
    return NULL;

  num_fields = pph_input_uint (stream);
  v = sorted_fields_type_new (num_fields);
  for (i = 0; i < num_fields; i++)
    v->elts[i] = pph_input_tree (stream);

  return v;
}


/* Read all the fields in lang_type_class instance LTC to STREAM.
   REF_P is true if all the trees in the structure should be written
   as references.  */

static void
pph_stream_read_lang_type_class (pph_stream *stream,
				  struct lang_type_class *ltc)
{
  struct bitpack_d bp;

  if (!pph_start_record (stream))
    return;

  ltc->align = pph_input_uchar (stream);

  bp = pph_input_bitpack (stream);
  ltc->has_mutable = bp_unpack_value (&bp, 1);
  ltc->com_interface = bp_unpack_value (&bp, 1);
  ltc->non_pod_class = bp_unpack_value (&bp, 1);
  ltc->nearly_empty_p = bp_unpack_value (&bp, 1);
  ltc->user_align = bp_unpack_value (&bp, 1);
  ltc->has_copy_assign = bp_unpack_value (&bp, 1);
  ltc->has_new = bp_unpack_value (&bp, 1);
  ltc->has_array_new = bp_unpack_value (&bp, 1);
  ltc->gets_delete = bp_unpack_value (&bp, 2);
  ltc->interface_only = bp_unpack_value (&bp, 1);
  ltc->interface_unknown = bp_unpack_value (&bp, 1);
  ltc->contains_empty_class_p = bp_unpack_value (&bp, 1);
  ltc->anon_aggr = bp_unpack_value (&bp, 1);
  ltc->non_zero_init = bp_unpack_value (&bp, 1);
  ltc->empty_p = bp_unpack_value (&bp, 1);
  ltc->vec_new_uses_cookie = bp_unpack_value (&bp, 1);
  ltc->declared_class = bp_unpack_value (&bp, 1);
  ltc->diamond_shaped = bp_unpack_value (&bp, 1);
  ltc->repeated_base = bp_unpack_value (&bp, 1);
  ltc->being_defined = bp_unpack_value (&bp, 1);
  ltc->java_interface = bp_unpack_value (&bp, 1);
  ltc->debug_requested = bp_unpack_value (&bp, 1);
  ltc->fields_readonly = bp_unpack_value (&bp, 1);
  ltc->use_template = bp_unpack_value (&bp, 2);
  ltc->ptrmemfunc_flag = bp_unpack_value (&bp, 1);
  ltc->was_anonymous = bp_unpack_value (&bp, 1);
  ltc->lazy_default_ctor = bp_unpack_value (&bp, 1);
  ltc->lazy_copy_ctor = bp_unpack_value (&bp, 1);
  ltc->lazy_copy_assign = bp_unpack_value (&bp, 1);
  ltc->lazy_destructor = bp_unpack_value (&bp, 1);
  ltc->has_const_copy_ctor = bp_unpack_value (&bp, 1);
  ltc->has_complex_copy_ctor = bp_unpack_value (&bp, 1);
  ltc->has_complex_copy_assign = bp_unpack_value (&bp, 1);
  ltc->non_aggregate = bp_unpack_value (&bp, 1);
  ltc->has_complex_dflt = bp_unpack_value (&bp, 1);
  ltc->has_list_ctor = bp_unpack_value (&bp, 1);
  ltc->non_std_layout = bp_unpack_value (&bp, 1);
  ltc->is_literal = bp_unpack_value (&bp, 1);
  ltc->lazy_move_ctor = bp_unpack_value (&bp, 1);
  ltc->lazy_move_assign = bp_unpack_value (&bp, 1);
  ltc->has_complex_move_ctor = bp_unpack_value (&bp, 1);
  ltc->has_complex_move_assign = bp_unpack_value (&bp, 1);
  ltc->has_constexpr_ctor = bp_unpack_value (&bp, 1);

  ltc->primary_base = pph_input_tree (stream);
  ltc->vcall_indices = pph_stream_read_tree_pair_vec (stream);
  ltc->vtables = pph_input_tree (stream);
  ltc->typeinfo_var = pph_input_tree (stream);
  ltc->vbases = pph_stream_read_tree_vec (stream);
  if (pph_start_record (stream))
    ltc->nested_udts = pph_stream_read_binding_table (stream);
  ltc->as_base = pph_input_tree (stream);
  ltc->pure_virtuals = pph_stream_read_tree_vec (stream);
  ltc->friend_classes = pph_input_tree (stream);
  ltc->methods = pph_stream_read_tree_vec (stream);
  ltc->key_method = pph_input_tree (stream);
  ltc->decl_list = pph_input_tree (stream);
  ltc->template_info = pph_input_tree (stream);
  ltc->befriending_classes = pph_input_tree (stream);
  ltc->objc_info = pph_input_tree (stream);
  ltc->sorted_fields = pph_stream_read_sorted_fields_type (stream);
  ltc->lambda_expr = pph_input_tree (stream);
}


/* Read all fields of struct lang_type_ptrmem instance LTP from STREAM.  */

static void
pph_stream_read_lang_type_ptrmem (pph_stream *stream,
				  struct lang_type_ptrmem *ltp)
{
  if (!pph_start_record (stream))
    return;

  ltp->record = pph_input_tree (stream);
}


/* Read all the lang-specific fields of TYPE from STREAM.  */

static void
pph_stream_read_lang_type (pph_stream *stream, tree type)
{
  struct lang_type *lt;

  if (!pph_start_record (stream))
    return;

  lt = ggc_alloc_cleared_lang_type (sizeof (struct lang_type));
  TYPE_LANG_SPECIFIC (type) = lt;

  pph_stream_read_lang_type_header (stream, &lt->u.h);
  if (lt->u.h.is_lang_type_class)
    pph_stream_read_lang_type_class (stream, &lt->u.c);
  else
    pph_stream_read_lang_type_ptrmem (stream, &lt->u.ptrmem);
}


/* Callback for reading ASTs from a stream.  This reads all the fields
   that are not processed by default by the common tree pickler.
   IB, DATA_IN are as in lto_read_tree.  EXPR is the partially materialized
   tree.  */

void
pph_stream_read_tree (struct lto_input_block *ib ATTRIBUTE_UNUSED,
		      struct data_in *data_in, tree expr)
{
  pph_stream *stream = (pph_stream *) data_in->sdata;

  if (DECL_P (expr))
    {
      if (TREE_CODE (expr) == FUNCTION_DECL
	  || TREE_CODE (expr) == NAMESPACE_DECL
	  || TREE_CODE (expr) == PARM_DECL
	  || LANG_DECL_HAS_MIN (expr))
	{
	  pph_stream_read_lang_specific (stream, expr);
	  if (TREE_CODE (expr) == FUNCTION_DECL)
	    DECL_SAVED_TREE (expr) = pph_input_tree (stream);
	  else if (TREE_CODE (expr) == PARM_DECL)
	    DECL_ARG_TYPE (expr) = pph_input_tree (stream);
	}

      if (TREE_CODE (expr) == TYPE_DECL)
	DECL_ORIGINAL_TYPE (expr) = pph_input_tree (stream);
    }
  else if (TREE_CODE (expr) == STATEMENT_LIST)
    {
      HOST_WIDE_INT i, num_trees = pph_input_uint (stream);
      for (i = 0; i < num_trees; i++)
	{
	  tree stmt = pph_input_tree (stream);
	  append_to_statement_list (stmt, &expr);
	}
    }
  else if (TYPE_P (expr))
    pph_stream_read_lang_type (stream, expr);
}
