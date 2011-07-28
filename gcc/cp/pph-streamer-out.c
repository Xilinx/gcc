/* Routines for writing PPH data.
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
#include "cgraph.h"

/* FIXME pph.  This holds the FILE handle for the current PPH file
   that we are writing.  It is necessary because the LTO callbacks do
   not allow passing a FILE handle to them.  */
static FILE *current_pph_file = NULL;

/* List of declarations to register with the middle end.  This is
   collected as the compiler instantiates symbols and functions.  Once
   we finish parsing the header file, this array is written out to the
   PPH image.  This way, the reader will be able to instantiate these
   symbols in the same order that they were instantiated originally.  */
typedef struct decls_to_register_t {
  /* Table of all the declarations to register in declaration order.  */
  VEC(tree,heap) *v;

  /* Set of declarations to register used to avoid adding duplicate
     entries to the table.  */
  struct pointer_set_t *m;
} decls_to_register_t;

static decls_to_register_t decls_to_register = { NULL, NULL };

/* Callback for packing value fields in ASTs.  BP is the bitpack 
   we are packing into.  EXPR is the tree to pack.  */

void
pph_pack_value_fields (struct bitpack_d *bp, tree expr)
{
  if (TYPE_P (expr))
    {
      bp_pack_value (bp, TYPE_LANG_FLAG_0 (expr), 1);
      bp_pack_value (bp, TYPE_LANG_FLAG_1 (expr), 1);
      bp_pack_value (bp, TYPE_LANG_FLAG_2 (expr), 1);
      bp_pack_value (bp, TYPE_LANG_FLAG_3 (expr), 1);
      bp_pack_value (bp, TYPE_LANG_FLAG_4 (expr), 1);
      bp_pack_value (bp, TYPE_LANG_FLAG_5 (expr), 1);
      bp_pack_value (bp, TYPE_LANG_FLAG_6 (expr), 1);
    }
  else if (DECL_P (expr))
    {
      bp_pack_value (bp, DECL_LANG_FLAG_0 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_1 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_2 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_3 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_4 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_5 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_6 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_7 (expr), 1);
      bp_pack_value (bp, DECL_LANG_FLAG_8 (expr), 1);
    }

  bp_pack_value (bp, TREE_LANG_FLAG_0 (expr), 1);
  bp_pack_value (bp, TREE_LANG_FLAG_1 (expr), 1);
  bp_pack_value (bp, TREE_LANG_FLAG_2 (expr), 1);
  bp_pack_value (bp, TREE_LANG_FLAG_3 (expr), 1);
  bp_pack_value (bp, TREE_LANG_FLAG_4 (expr), 1);
  bp_pack_value (bp, TREE_LANG_FLAG_5 (expr), 1);
  bp_pack_value (bp, TREE_LANG_FLAG_6 (expr), 1);
}


/* Initialize buffers and tables in STREAM for writing.  */

void
pph_init_write (pph_stream *stream)
{
  lto_streamer_init ();
  stream->encoder.w.out_state = lto_new_out_decl_state ();
  lto_push_out_decl_state (stream->encoder.w.out_state);
  stream->encoder.w.decl_state_stream = XCNEW (struct lto_output_stream);
  stream->encoder.w.ob = create_output_block (LTO_section_decls);

  /* Associate STREAM with stream->encoder.w.ob so we can recover it from the
     streamer hooks.  */
  stream->encoder.w.ob->sdata = (void *) stream;
}


/* Callback for lang_hooks.lto.begin_section.  Open file NAME.  */

static void
pph_begin_section (const char *name ATTRIBUTE_UNUSED)
{
}


/* Callback for lang_hooks.lto.append_data.  Write LEN bytes from DATA
   into current_pph_file.  BLOCK is currently unused.  */

static void
pph_out (const void *data, size_t len, void *block ATTRIBUTE_UNUSED)
{
  if (data)
    fwrite (data, len, 1, current_pph_file);
}


/* Callback for lang_hooks.lto.end_section.  */

static void
pph_end_section (void)
{
}


/* Write the header for the PPH file represented by STREAM.  */

static void
pph_out_header (pph_stream *stream)
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
  header.strtab_size = stream->encoder.w.ob->string_stream->total_size;

  memset (&header_stream, 0, sizeof (header_stream));
  lto_output_data_stream (&header_stream, &header, sizeof (header));
  lto_write_stream (&header_stream);
}


/* Write the body of the PPH file represented by STREAM.  */

static void
pph_out_body (pph_stream *stream)
{
  /* Write the string table.  */
  lto_write_stream (stream->encoder.w.ob->string_stream);

  /* Write out the physical representation for every AST in all the
     streams in STREAM->ENCODER.W.OUT_STATE.  */
  lto_output_decl_state_streams (stream->encoder.w.ob,
				 stream->encoder.w.out_state);

  /* Now write the vector of all AST references.  */
  lto_output_decl_state_refs (stream->encoder.w.ob,
			      stream->encoder.w.decl_state_stream,
			      stream->encoder.w.out_state);

  /* Finally, physically write all the streams.  */
  lto_write_stream (stream->encoder.w.ob->main_stream);
}


/* Flush all the in-memory buffers for STREAM to disk.  */

void
pph_flush_buffers (pph_stream *stream)
{
  gcc_assert (current_pph_file == NULL);
  current_pph_file = stream->file;

  /* Redirect the LTO basic I/O langhooks.  */
  lang_hooks.lto.begin_section = pph_begin_section;
  lang_hooks.lto.append_data = pph_out;
  lang_hooks.lto.end_section = pph_end_section;

  /* Write the state buffers built by pph_out_*() calls.  */
  lto_begin_section (stream->name, false);
  pph_out_header (stream);
  pph_out_body (stream);
  lto_end_section ();
  current_pph_file = NULL;
}


/* Start a new record in STREAM for data in DATA.  If DATA is NULL
   write an end-of-record marker and return false.  If DATA is not NULL
   and did not exist in the pickle cache, add it, write a
   start-of-record marker and return true.  If DATA existed in the
   cache, write a shared-record marker and return false.  */

static inline bool
pph_out_start_record (pph_stream *stream, void *data)
{
  if (data)
    {
      bool existed_p;
      unsigned ix;
      enum pph_record_marker marker;

      /* If the memory at DATA has already been streamed out, make
	 sure that we don't write it more than once.  Otherwise,
	 the reader will instantiate two different pointers for
	 the same object.

	 Write the index into the cache where DATA has been stored.
	 This way, the reader will know at which slot to
	 re-materialize DATA the first time and where to access it on
	 subsequent reads.  */
      existed_p = pph_cache_add (stream, data, &ix);
      marker = (existed_p) ? PPH_RECORD_SHARED : PPH_RECORD_START;
      pph_out_uchar (stream, marker);
      pph_out_uint (stream, ix);
      return marker == PPH_RECORD_START;
    }
  else
    {
      pph_out_uchar (stream, PPH_RECORD_END);
      return false;
    }
}


/* Compute an index value for TYPE suitable for restoring it later
   from global_trees[] or integer_types.  The index is saved
   in TYPE_IX_P and the number category (one of CPP_N_INTEGER,
   CPP_N_FLOATING, etc) is saved in CATEGORY_P.  */

static void
pph_get_index_from_type (tree type, unsigned *type_ix_p, unsigned *category_p)
{
  void **val_p;
  static struct pointer_map_t *type_cache = NULL;

  /* For complex types we will just use the type of the components.  */
  if (TREE_CODE (type) == COMPLEX_TYPE)
    {
      *type_ix_p = 0;
      *category_p = CPP_N_IMAGINARY;
      return;
    }

  if (type_cache == NULL)
    type_cache = pointer_map_create ();

  val_p = pointer_map_contains (type_cache, type);
  if (val_p)
    *type_ix_p = *((unsigned *) val_p);
  else
    {
      if (CP_INTEGRAL_TYPE_P (type))
	{
	  unsigned i;
	  for (i = itk_char; i < itk_none; i++)
	    if (type == integer_types[i])
	      {
		*type_ix_p = (unsigned) i;
		break;
	      }

	  gcc_assert (i != itk_none);
	}
      else if (FLOAT_TYPE_P (type) || FIXED_POINT_TYPE_P (type))
	{
	  unsigned i;

	  for (i = TI_ERROR_MARK; i < TI_MAX; i++)
	    if (global_trees[i] == type)
	      {
		*type_ix_p = (unsigned) i;
		break;
	      }

	  gcc_assert (i != TI_MAX);
	}
      else
	gcc_unreachable ();
    }

  if (CP_INTEGRAL_TYPE_P (type))
    *category_p = CPP_N_INTEGER;
  else if (FLOAT_TYPE_P (type))
    *category_p = CPP_N_FLOATING;
  else if (FIXED_POINT_TYPE_P (type))
    *category_p = CPP_N_FRACT;
  else
    gcc_unreachable ();
}


/* Save the number VAL to file F.  */

static void
pph_out_number (pph_stream *f, tree val)
{
  unsigned type_idx, type_kind;

  pph_get_index_from_type (TREE_TYPE (val), &type_idx, &type_kind);

  pph_out_uint (f, type_idx);
  pph_out_uint (f, type_kind);

  if (type_kind == CPP_N_INTEGER)
    {
      HOST_WIDE_INT v[2];

      v[0] = TREE_INT_CST_LOW (val);
      v[1] = TREE_INT_CST_HIGH (val);
      pph_out_bytes (f, v, 2 * sizeof (HOST_WIDE_INT));
    }
  else if (type_kind == CPP_N_FLOATING)
    {
      REAL_VALUE_TYPE r = TREE_REAL_CST (val);
      pph_out_bytes (f, &r, sizeof (REAL_VALUE_TYPE));
    }
  else if (type_kind == CPP_N_FRACT)
    {
      FIXED_VALUE_TYPE fv = TREE_FIXED_CST (val);
      pph_out_bytes (f, &fv, sizeof (FIXED_VALUE_TYPE));
    }
  else if (type_kind == CPP_N_IMAGINARY)
    {
      pph_out_number (f, TREE_REALPART (val));
      pph_out_number (f, TREE_IMAGPART (val));
    }
  else
    gcc_unreachable ();
}


/* Save the tree associated with TOKEN to file F.  */

static void
pph_out_token_value (pph_stream *f, cp_token *token)
{
  const char *str;
  unsigned len;
  tree val;

  val = token->u.value;
  switch (token->type)
    {
      case CPP_TEMPLATE_ID:
      case CPP_NESTED_NAME_SPECIFIER:
	break;

      case CPP_NAME:
	/* FIXME pph.  Hash the strings and emit a string table.  */
	str = IDENTIFIER_POINTER (val);
	len = IDENTIFIER_LENGTH (val);
	pph_out_string_with_length (f, str, len);
	break;

      case CPP_KEYWORD:
	/* Nothing to do.  We will reconstruct the keyword from
	   ridpointers[token->keyword] at load time.  */
	break;

      case CPP_CHAR:
      case CPP_WCHAR:
      case CPP_CHAR16:
      case CPP_CHAR32:
      case CPP_NUMBER:
	pph_out_number (f, val);
	break;

      case CPP_STRING:
      case CPP_WSTRING:
      case CPP_STRING16:
      case CPP_STRING32:
	/* FIXME pph.  Need to represent the type.  */
	str = TREE_STRING_POINTER (val);
	len = TREE_STRING_LENGTH (val);
	pph_out_string_with_length (f, str, len);
	break;

      case CPP_PRAGMA:
	/* Nothing to do.  Field pragma_kind has already been written.  */
	break;

      default:
	gcc_assert (token->u.value == NULL);
	pph_out_bytes (f, &token->u.value, sizeof (token->u.value));
    }
}


/* Save TOKEN on file F.  Return the number of bytes written on F.  */

static void
pph_out_token (pph_stream *f, cp_token *token)
{
  /* Do not write out the final field in TOKEN.  It contains
     pointers that need to be pickled separately.

     FIXME pph - Need to also emit the location_t table so we can
     reconstruct it when reading the PTH state.  */
  pph_out_bytes (f, token, sizeof (cp_token) - sizeof (void *));
  pph_out_token_value (f, token);
}


/* Save all the tokens in CACHE to PPH stream F.  */

static void
pph_out_token_cache (pph_stream *f, cp_token_cache *cache)
{
  unsigned i, num;
  cp_token *tok;

  if (cache == NULL)
    {
      pph_out_uint (f, 0);
      return;
    }

  for (num = 0, tok = cache->first; tok != cache->last; tok++)
    num++;

  pph_out_uint (f, num);
  for (i = 0, tok = cache->first; i < num; tok++, i++)
    pph_out_token (f, tok);
}

/* Write all the fields in lang_decl_base instance LDB to OB.  */

static void
pph_out_ld_base (pph_stream *stream, struct lang_decl_base *ldb)
{
  struct bitpack_d bp;

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
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
  pph_out_bitpack (stream, &bp);
}


/* Write all the fields in lang_decl_min instance LDM to STREAM.  */

static void
pph_out_ld_min (pph_stream *stream, struct lang_decl_min *ldm)
{
  pph_out_tree_or_ref_1 (stream, ldm->template_info, 1);
  if (ldm->base.u2sel == 0)
    pph_out_tree_or_ref_1 (stream, ldm->u2.access, 1);
  else if (ldm->base.u2sel == 1)
    pph_out_uint (stream, ldm->u2.discriminator);
  else
    gcc_unreachable ();
}


/* Write all the trees in gc VEC V to STREAM.  */

static void
pph_out_tree_vec (pph_stream *stream, VEC(tree,gc) *v)
{
  unsigned i;
  tree t;

  pph_out_uint (stream, VEC_length (tree, v));
  FOR_EACH_VEC_ELT (tree, v, i, t)
    pph_out_tree_or_ref (stream, t);
}


/* Write all the qualified_typedef_usage_t in VEC V to STREAM.  */

static void
pph_out_qual_use_vec (pph_stream *stream, VEC(qualified_typedef_usage_t,gc) *v)
{
  unsigned i;
  qualified_typedef_usage_t *q;

  pph_out_uint (stream, VEC_length (qualified_typedef_usage_t, v));
  FOR_EACH_VEC_ELT (qualified_typedef_usage_t, v, i, q)
    {
      pph_out_tree_or_ref (stream, q->typedef_decl);
      pph_out_tree_or_ref (stream, q->context);
      /* FIXME pph: also write location.  */
    }
}


/* Forward declaration to break cyclic dependencies.  */
static void pph_out_binding_level (pph_stream *, cp_binding_level *);


/* Helper for pph_out_cxx_binding.  STREAM and CB are as in
   pph_out_cxx_binding.  */

static void
pph_out_cxx_binding_1 (pph_stream *stream, cxx_binding *cb)
{
  struct bitpack_d bp;

  if (!pph_out_start_record (stream, cb))
    return;

  pph_out_tree_or_ref (stream, cb->value);
  pph_out_tree_or_ref (stream, cb->type);
  pph_out_binding_level (stream, cb->scope);
  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, cb->value_is_inherited, 1);
  bp_pack_value (&bp, cb->is_local, 1);
  pph_out_bitpack (stream, &bp);
}


/* Write all the fields of cxx_binding instance CB to STREAM.  */

static void
pph_out_cxx_binding (pph_stream *stream, cxx_binding *cb)
{
  cxx_binding *prev;

  /* Write the current binding first.  */
  pph_out_cxx_binding_1 (stream, cb);

  /* Write the list of previous bindings.  */
  for (prev = cb ? cb->previous : NULL; prev; prev = prev->previous)
    pph_out_cxx_binding_1 (stream, prev);

  /* Mark the end of the list (if there was a list).  */
  if (cb)
    pph_out_cxx_binding_1 (stream, NULL);
}


/* Write all the fields of cp_class_binding instance CB to STREAM.  */

static void
pph_out_class_binding (pph_stream *stream, cp_class_binding *cb)
{
  if (!pph_out_start_record (stream, cb))
    return;

  pph_out_cxx_binding (stream, cb->base);
  pph_out_tree_or_ref (stream, cb->identifier);
}


/* Write all the fields of cp_label_binding instance LB to STREAM.  */

static void
pph_out_label_binding (pph_stream *stream, cp_label_binding *lb)
{
  if (!pph_out_start_record (stream, lb))
    return;

  pph_out_tree_or_ref (stream, lb->label);
  pph_out_tree_or_ref (stream, lb->prev_value);
}


/* Outputs chained tree T to STREAM by nulling out its chain first and
   restoring it after the streaming is done.  */

static inline void
pph_out_chained_tree (pph_stream *stream, tree t)
{
  tree saved_chain;

  saved_chain = TREE_CHAIN (t);
  TREE_CHAIN (t) = NULL_TREE;

  pph_out_tree_or_ref_1 (stream, t, 2);

  TREE_CHAIN (t) = saved_chain;
}


/* Output a chain of nodes to STREAM starting with FIRST.  Skip any
   nodes that do not match FILTER.  */

static void
pph_out_chain_filtered (pph_stream *stream, tree first, 
			enum chain_filter filter)
{
  unsigned count;
  tree t;

  /* Special case.  If the caller wants no filtering, it is much
     faster to just call pph_out_chain directly.  */
  if (filter == NONE)
    {
      pph_out_chain (stream, first);
      return;
    }

  /* Count all the nodes that match the filter.  */
  for (t = first, count = 0; t; t = TREE_CHAIN (t))
    {
      if (filter == NO_BUILTINS && DECL_P (t) && DECL_IS_BUILTIN (t))
	continue;
      count++;
    }
  pph_out_uint (stream, count);

  /* Output all the nodes that match the filter.  */
  for (t = first; t; t = TREE_CHAIN (t))
    {
      /* Apply filters to T.  */
      if (filter == NO_BUILTINS && DECL_P (t) && DECL_IS_BUILTIN (t))
	continue;

      pph_out_chained_tree (stream, t);
    }
}


/* Write all the fields of cp_binding_level instance BL to STREAM.  */

static void
pph_out_binding_level (pph_stream *stream, cp_binding_level *bl)
{
  unsigned i;
  cp_class_binding *cs;
  cp_label_binding *sl;
  struct bitpack_d bp;

  if (!pph_out_start_record (stream, bl))
    return;

  pph_out_chain_filtered (stream, bl->names, NO_BUILTINS);
  pph_out_chain_filtered (stream, bl->namespaces, NO_BUILTINS);

  pph_out_tree_vec (stream, bl->static_decls);

  pph_out_chain_filtered (stream, bl->usings, NO_BUILTINS);
  pph_out_chain_filtered (stream, bl->using_directives, NO_BUILTINS);

  pph_out_uint (stream, VEC_length (cp_class_binding, bl->class_shadowed));
  FOR_EACH_VEC_ELT (cp_class_binding, bl->class_shadowed, i, cs)
    pph_out_class_binding (stream, cs);

  pph_out_tree_or_ref (stream, bl->type_shadowed);

  pph_out_uint (stream, VEC_length (cp_label_binding, bl->shadowed_labels));
  FOR_EACH_VEC_ELT (cp_label_binding, bl->shadowed_labels, i, sl)
    pph_out_label_binding (stream, sl);

  pph_out_chain (stream, bl->blocks);
  pph_out_tree_or_ref (stream, bl->this_entity);
  pph_out_binding_level (stream, bl->level_chain);
  pph_out_tree_vec (stream, bl->dead_vars_from_for);
  pph_out_chain (stream, bl->statement_list);
  pph_out_uint (stream, bl->binding_depth);

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, bl->kind, 4);
  bp_pack_value (&bp, bl->keep, 1);
  bp_pack_value (&bp, bl->more_cleanups_ok, 1);
  bp_pack_value (&bp, bl->have_cleanups, 1);
  pph_out_bitpack (stream, &bp);
}


/* Write out the tree_common fields from T to STREAM.  */

static void
pph_out_tree_common (pph_stream *stream, tree t)
{
  /* The 'struct tree_typed typed' base class is handled in LTO.  */
  pph_out_tree_or_ref (stream, TREE_CHAIN (t));
}


/* Write all the fields of c_language_function instance CLF to STREAM.  */

static void
pph_out_c_language_function (pph_stream *stream,
			     struct c_language_function *clf)
{
  if (!pph_out_start_record (stream, clf))
    return;

  pph_out_tree_vec (stream, clf->x_stmt_tree.x_cur_stmt_list);
  pph_out_uint (stream, clf->x_stmt_tree.stmts_are_full_exprs_p);
}


/* Write all the fields of language_function instance LF to STREAM.  */

static void
pph_out_language_function (pph_stream *stream, struct language_function *lf)
{
  struct bitpack_d bp;

  if (!pph_out_start_record (stream, lf))
    return;

  pph_out_c_language_function (stream, &lf->base);
  pph_out_tree_or_ref (stream, lf->x_cdtor_label);
  pph_out_tree_or_ref (stream, lf->x_current_class_ptr);
  pph_out_tree_or_ref (stream, lf->x_current_class_ref);
  pph_out_tree_or_ref (stream, lf->x_eh_spec_block);
  pph_out_tree_or_ref (stream, lf->x_in_charge_parm);
  pph_out_tree_or_ref (stream, lf->x_vtt_parm);
  pph_out_tree_or_ref (stream, lf->x_return_value);
  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, lf->x_returns_value, 1);
  bp_pack_value (&bp, lf->x_returns_null, 1);
  bp_pack_value (&bp, lf->x_returns_abnormally, 1);
  bp_pack_value (&bp, lf->x_in_function_try_handler, 1);
  bp_pack_value (&bp, lf->x_in_base_initializer, 1);
  bp_pack_value (&bp, lf->can_throw, 1);
  pph_out_bitpack (stream, &bp);

  /* FIXME pph.  We are not writing lf->x_named_labels.  */

  pph_out_binding_level (stream, lf->bindings);
  pph_out_tree_vec (stream, lf->x_local_names);

  /* FIXME pph.  We are not writing lf->extern_decl_map.  */
}


/* Write all the fields of lang_decl_fn instance LDF to STREAM.  */

static void
pph_out_ld_fn (pph_stream *stream, struct lang_decl_fn *ldf)
{
  struct bitpack_d bp;

  /* Write all the fields in lang_decl_min.  */
  pph_out_ld_min (stream, &ldf->min);

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
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
  pph_out_bitpack (stream, &bp);

  pph_out_tree_or_ref (stream, ldf->befriending_classes);
  pph_out_tree_or_ref (stream, ldf->context);

  if (ldf->thunk_p == 0)
    pph_out_tree_or_ref (stream, ldf->u5.cloned_function);
  else if (ldf->thunk_p == 1)
    pph_out_uint (stream, ldf->u5.fixed_offset);
  else
    gcc_unreachable ();

  if (ldf->pending_inline_p == 1)
    pph_out_token_cache (stream, ldf->u.pending_inline_info);
  else if (ldf->pending_inline_p == 0)
    pph_out_language_function (stream, ldf->u.saved_language_function);
}


/* A callback of htab_traverse. Just extracts a (type) tree from SLOT
   and writes it out for PPH. */

struct pph_tree_info {
  pph_stream *stream;
};

static int
pph_out_used_types_slot (void **slot, void *aux)
{
  struct pph_tree_info *pti = (struct pph_tree_info *) aux;
  pph_out_tree_or_ref (pti->stream, (tree) *slot);
  return 1;
}


/* Write applicable fields of struct function instance FN to STREAM.  */

static void
pph_out_struct_function (pph_stream *stream, struct function *fn)
{
  struct pph_tree_info pti;

  if (!pph_out_start_record (stream, fn))
    return;

  pph_out_tree (stream, fn->decl);
  output_struct_function_base (stream->encoder.w.ob, fn);

  /* struct eh_status *eh;					-- ignored */
  gcc_assert (fn->cfg == NULL);
  gcc_assert (fn->gimple_body == NULL);
  gcc_assert (fn->gimple_df == NULL);
  gcc_assert (fn->x_current_loops == NULL);
  gcc_assert (fn->su == NULL);
  /* htab_t value_histograms;					-- ignored */
  /* tree decl;							-- ignored */
  /* tree static_chain_decl;					-- in base */
  /* tree nonlocal_goto_save_area;				-- in base */
  /* VEC(tree,gc) *local_decls;					-- in base */
  /* struct machine_function *machine;				-- ignored */
  pph_out_language_function (stream, fn->language);

  /* FIXME pph: We would like to detect improper sharing here.  */
  if (fn->used_types_hash)
    {
      /* FIXME pph: This write may be unstable.  */
      pph_out_uint (stream, htab_elements (fn->used_types_hash));
      pti.stream = stream;
      htab_traverse_noresize (fn->used_types_hash, pph_out_used_types_slot,
			      &pti);
    }
  else
    pph_out_uint (stream, 0);

  gcc_assert (fn->last_stmt_uid == 0);
  /* int funcdef_no;						-- ignored */
  /* location_t function_start_locus;				-- in base */
  /* location_t function_end_locus;				-- in base */
  /* unsigned int curr_properties;				-- in base */
  /* unsigned int last_verified;				-- ignored */
  /* const char *cannot_be_copied_reason;			-- ignored */

  /* unsigned int va_list_gpr_size : 8;				-- in base */
  /* unsigned int va_list_fpr_size : 8;				-- in base */
  /* unsigned int calls_setjmp : 1;				-- in base */
  /* unsigned int calls_alloca : 1;				-- in base */
  /* unsigned int has_nonlocal_label : 1;			-- in base */
  /* unsigned int cannot_be_copied_set : 1;			-- ignored */
  /* unsigned int stdarg : 1;					-- in base */
  /* unsigned int after_inlining : 1;				-- in base */
  /* unsigned int always_inline_functions_inlined : 1;		-- in base */
  /* unsigned int can_throw_non_call_exceptions : 1;		-- in base */
  /* unsigned int returns_struct : 1;				-- in base */
  /* unsigned int returns_pcc_struct : 1;			-- in base */
  /* unsigned int after_tree_profile : 1;			-- in base */
  /* unsigned int has_local_explicit_reg_vars : 1;		-- in base */
  /* unsigned int is_thunk : 1;					-- in base */
}


/* Write all the fields of lang_decl_ns instance LDNS to STREAM.  */

static void
pph_out_ld_ns (pph_stream *stream, struct lang_decl_ns *ldns)
{
  pph_out_binding_level (stream, ldns->level);
}


/* Write all the fields of lang_decl_parm instance LDP to STREAM.  */

static void
pph_out_ld_parm (pph_stream *stream, struct lang_decl_parm *ldp)
{
  pph_out_uint (stream, ldp->level);
  pph_out_uint (stream, ldp->index);
}


/* Write all the lang-specific data in DECL to STREAM.  */

static void
pph_out_lang_specific (pph_stream *stream, tree decl)
{
  struct lang_decl *ld;
  struct lang_decl_base *ldb;

  ld = DECL_LANG_SPECIFIC (decl);
  if (!pph_out_start_record (stream, ld))
    return;

  /* Write all the fields in lang_decl_base.  */
  ldb = &ld->u.base;
  pph_out_ld_base (stream, ldb);

  if (ldb->selector == 0)
    {
      /* Write all the fields in lang_decl_min.  */
      pph_out_ld_min (stream, &ld->u.min);
    }
  else if (ldb->selector == 1)
    {
      /* Write all the fields in lang_decl_fn.  */
      pph_out_ld_fn (stream, &ld->u.fn);
    }
  else if (ldb->selector == 2)
    {
      /* Write all the fields in lang_decl_ns.  */
      pph_out_ld_ns (stream, &ld->u.ns);
    }
  else if (ldb->selector == 3)
    {
      /* Write all the fields in lang_decl_parm.  */
      pph_out_ld_parm (stream, &ld->u.parm);
    }
  else
    gcc_unreachable ();
}


/* Write all the fields in lang_type_header instance LTH to STREAM.  */

static void
pph_out_lang_type_header (pph_stream *stream, struct lang_type_header *lth)
{
  struct bitpack_d bp;

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, lth->is_lang_type_class, 1);
  bp_pack_value (&bp, lth->has_type_conversion, 1);
  bp_pack_value (&bp, lth->has_copy_ctor, 1);
  bp_pack_value (&bp, lth->has_default_ctor, 1);
  bp_pack_value (&bp, lth->const_needs_init, 1);
  bp_pack_value (&bp, lth->ref_needs_init, 1);
  bp_pack_value (&bp, lth->has_const_copy_assign, 1);
  pph_out_bitpack (stream, &bp);
}


/* Write the vector V of tree_pair_s instances to STREAM.  */

static void
pph_out_tree_pair_vec (pph_stream *stream, VEC(tree_pair_s,gc) *v)
{
  unsigned i;
  tree_pair_s *p;

  pph_out_uint (stream, VEC_length (tree_pair_s, v));
  FOR_EACH_VEC_ELT (tree_pair_s, v, i, p)
    {
      pph_out_tree_or_ref (stream, p->purpose);
      pph_out_tree_or_ref (stream, p->value);
    }
}


/* Write a struct sorted_fields_type instance SFT to STREAM.  */

static void
pph_out_sorted_fields_type (pph_stream *stream, struct sorted_fields_type *sft)
{
  int i;

  if (!pph_out_start_record (stream, sft))
    return;

  pph_out_uint (stream, sft->len);
  for (i = 0; i < sft->len; i++)
    pph_out_tree_or_ref (stream, sft->elts[i]);
}


/* Write all the fields in lang_type_class instance LTC to STREAM.  */

static void
pph_out_lang_type_class (pph_stream *stream, struct lang_type_class *ltc)
{
  struct bitpack_d bp;

  pph_out_uchar (stream, ltc->align);

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, ltc->has_mutable, 1);
  bp_pack_value (&bp, ltc->com_interface, 1);
  bp_pack_value (&bp, ltc->non_pod_class, 1);
  bp_pack_value (&bp, ltc->nearly_empty_p, 1);
  bp_pack_value (&bp, ltc->user_align, 1);
  bp_pack_value (&bp, ltc->has_copy_assign, 1);
  bp_pack_value (&bp, ltc->has_new, 1);
  bp_pack_value (&bp, ltc->has_array_new, 1);
  bp_pack_value (&bp, ltc->gets_delete, 2);
  bp_pack_value (&bp, ltc->interface_only, 1);
  bp_pack_value (&bp, ltc->interface_unknown, 1);
  bp_pack_value (&bp, ltc->contains_empty_class_p, 1);
  bp_pack_value (&bp, ltc->anon_aggr, 1);
  bp_pack_value (&bp, ltc->non_zero_init, 1);
  bp_pack_value (&bp, ltc->empty_p, 1);
  bp_pack_value (&bp, ltc->vec_new_uses_cookie, 1);
  bp_pack_value (&bp, ltc->declared_class, 1);
  bp_pack_value (&bp, ltc->diamond_shaped, 1);
  bp_pack_value (&bp, ltc->repeated_base, 1);
  bp_pack_value (&bp, ltc->being_defined, 1);
  bp_pack_value (&bp, ltc->java_interface, 1);
  bp_pack_value (&bp, ltc->debug_requested, 1);
  bp_pack_value (&bp, ltc->fields_readonly, 1);
  bp_pack_value (&bp, ltc->use_template, 2);
  bp_pack_value (&bp, ltc->ptrmemfunc_flag, 1);
  bp_pack_value (&bp, ltc->was_anonymous, 1);
  bp_pack_value (&bp, ltc->lazy_default_ctor, 1);
  bp_pack_value (&bp, ltc->lazy_copy_ctor, 1);
  bp_pack_value (&bp, ltc->lazy_copy_assign, 1);
  bp_pack_value (&bp, ltc->lazy_destructor, 1);
  bp_pack_value (&bp, ltc->has_const_copy_ctor, 1);
  bp_pack_value (&bp, ltc->has_complex_copy_ctor, 1);
  bp_pack_value (&bp, ltc->has_complex_copy_assign, 1);
  bp_pack_value (&bp, ltc->non_aggregate, 1);
  bp_pack_value (&bp, ltc->has_complex_dflt, 1);
  bp_pack_value (&bp, ltc->has_list_ctor, 1);
  bp_pack_value (&bp, ltc->non_std_layout, 1);
  bp_pack_value (&bp, ltc->is_literal, 1);
  bp_pack_value (&bp, ltc->lazy_move_ctor, 1);
  bp_pack_value (&bp, ltc->lazy_move_assign, 1);
  bp_pack_value (&bp, ltc->has_complex_move_ctor, 1);
  bp_pack_value (&bp, ltc->has_complex_move_assign, 1);
  bp_pack_value (&bp, ltc->has_constexpr_ctor, 1);
  pph_out_bitpack (stream, &bp);

  pph_out_tree_or_ref (stream, ltc->primary_base);
  pph_out_tree_pair_vec (stream, ltc->vcall_indices);
  pph_out_tree_or_ref (stream, ltc->vtables);
  pph_out_tree_or_ref (stream, ltc->typeinfo_var);
  pph_out_tree_vec (stream, ltc->vbases);
  if (pph_out_start_record (stream, ltc->nested_udts))
    pph_out_binding_table (stream, ltc->nested_udts);
  pph_out_tree_or_ref (stream, ltc->as_base);
  pph_out_tree_vec (stream, ltc->pure_virtuals);
  pph_out_tree_or_ref (stream, ltc->friend_classes);
  pph_out_tree_vec (stream, ltc->methods);
  pph_out_tree_or_ref (stream, ltc->key_method);
  pph_out_tree_or_ref (stream, ltc->decl_list);
  pph_out_tree_or_ref (stream, ltc->template_info);
  pph_out_tree_or_ref (stream, ltc->befriending_classes);
  pph_out_tree_or_ref (stream, ltc->objc_info);
  pph_out_sorted_fields_type (stream, ltc->sorted_fields);
  pph_out_tree_or_ref (stream, ltc->lambda_expr);
}


/* Write struct lang_type_ptrmem instance LTP to STREAM.  */

static void
pph_out_lang_type_ptrmem (pph_stream *stream, struct lang_type_ptrmem *ltp)
{
  pph_out_tree_or_ref (stream, ltp->record);
}


/* Write all the lang-specific fields of TYPE to STREAM.  */

static void
pph_out_lang_type (pph_stream *stream, tree type)
{
  struct lang_type *lt;

  lt = TYPE_LANG_SPECIFIC (type);
  if (!pph_out_start_record (stream, lt))
    return;

  pph_out_lang_type_header (stream, &lt->u.h);
  if (lt->u.h.is_lang_type_class)
    pph_out_lang_type_class (stream, &lt->u.c);
  else
    pph_out_lang_type_ptrmem (stream, &lt->u.ptrmem);
}


/* Write saved_scope information stored in SS into STREAM.
   This does NOT output all fields, it is meant to be used for the
   global variable scope_chain only.  */

static void
pph_out_scope_chain (pph_stream *stream, struct saved_scope *ss)
{
  /* old_namespace should be global_namespace and all entries listed below
     should be NULL or 0; otherwise the header parsed was incomplete.  */
  gcc_assert (ss->old_namespace == global_namespace
	      && !(ss->class_name
		   || ss->class_type
		   || ss->access_specifier
		   || ss->function_decl
		   || ss->template_parms
		   || ss->x_saved_tree
		   || ss->class_bindings
		   || ss->prev
		   || ss->unevaluated_operand
		   || ss->inhibit_evaluation_warnings
		   || ss->x_processing_template_decl
		   || ss->x_processing_specialization
		   || ss->x_processing_explicit_instantiation
		   || ss->need_pop_function_context
		   || ss->x_stmt_tree.x_cur_stmt_list
		   || ss->x_stmt_tree.stmts_are_full_exprs_p));

  /* We only need to write out the bindings, everything else should
     be NULL or be some temporary disposable state.  */
  pph_out_binding_level (stream, ss->bindings);
}


/* Save the IDENTIFIERS to the STREAM.  */

static void
pph_out_identifiers (pph_stream *stream, cpp_idents_used *identifiers)
{
  unsigned int num_entries, active_entries, id;

  num_entries = identifiers->num_entries;
  pph_out_uint (stream, identifiers->max_ident_len);
  pph_out_uint (stream, identifiers->max_value_len);

  active_entries = 0;
  for ( id = 0; id < num_entries; ++id )
    {
      cpp_ident_use *entry = identifiers->entries + id;
      if (!(entry->used_by_directive || entry->expanded_to_text))
        continue;
      ++active_entries;
    }

  pph_out_uint (stream, active_entries);

  for ( id = 0; id < num_entries; ++id )
    {
      cpp_ident_use *entry = identifiers->entries + id;

      if (!(entry->used_by_directive || entry->expanded_to_text))
        continue;

      /* FIXME pph: We are wasting space; ident_len, used_by_directive
      and expanded_to_text together could fit into a single uint. */

      pph_out_uint (stream, entry->used_by_directive);
      pph_out_uint (stream, entry->expanded_to_text);

      gcc_assert (entry->ident_len <= identifiers->max_ident_len);
      pph_out_string_with_length (stream, entry->ident_str,
				     entry->ident_len);

      gcc_assert (entry->before_len <= identifiers->max_value_len);
      pph_out_string_with_length (stream, entry->before_str,
				     entry->before_len);

      gcc_assert (entry->after_len <= identifiers->max_value_len);
      pph_out_string_with_length (stream, entry->after_str,
				     entry->after_len);
    }
}


/* Emit symbol table MARKER to STREAM.  */

static inline void
pph_out_symtab_marker (pph_stream *stream, enum pph_symtab_marker marker)
{
  pph_out_uchar (stream, marker);
}


/* Emit the symbol table for STREAM.  When this image is read into
   another translation unit, we want to guarantee that the IL
   instances taken from this image are instantiated in the same order
   that they were instantiated when we generated this image.

   With this, we can generate code in the same order out of the
   original header files and out of PPH images.  */

static void
pph_out_symtab (pph_stream *stream)
{
  tree decl;
  unsigned i;

  pph_out_uint (stream, VEC_length (tree, decls_to_register.v));
  FOR_EACH_VEC_ELT (tree, decls_to_register.v, i, decl)
    if (TREE_CODE (decl) == FUNCTION_DECL && DECL_STRUCT_FUNCTION (decl))
      {
	if (DECL_SAVED_TREE (decl))
	  pph_out_symtab_marker (stream, PPH_SYMTAB_FUNCTION_BODY);
	else
	  pph_out_symtab_marker (stream, PPH_SYMTAB_FUNCTION);
	pph_out_struct_function (stream, DECL_STRUCT_FUNCTION (decl));
      }
    else
      {
	pph_out_symtab_marker (stream, PPH_SYMTAB_DECL);
	pph_out_tree (stream, decl);
      }

  if (decls_to_register.m)
    {
      VEC_free (tree, heap, decls_to_register.v);
      pointer_set_destroy (decls_to_register.m);
      decls_to_register.m = NULL;
    }
}


/* Write PPH output symbols and IDENTS_USED to STREAM as an object.  */

static void
pph_write_file_contents (pph_stream *stream, cpp_idents_used *idents_used)
{ 
  /* Emit all the identifiers and symbols in the global namespace.  */
  pph_out_identifiers (stream, idents_used);

  /* Emit the bindings for the global namespace.  */
  pph_out_scope_chain (stream, scope_chain);
  if (flag_pph_dump_tree)
    pph_dump_namespace (pph_logfile, global_namespace);

  /* Emit other global state kept by the parser.  FIXME pph, these
     globals should be fields in struct cp_parser.  */
  pph_out_tree (stream, keyed_classes);
  pph_out_tree_vec (stream, unemitted_tinfo_decls);

  pph_out_pending_templates_list (stream);
  pph_out_spec_entry_tables (stream);

  pph_out_tree (stream, static_aggregates);

  /* Emit the symbol table.  */
  pph_out_symtab (stream);
}


/* Write PPH output file.  */

void
pph_write_file (void)
{
  pph_stream *stream;
  cpp_idents_used idents_used;

  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Writing %s\n", pph_out_file);

  stream = pph_stream_open (pph_out_file, "wb");
  if (!stream)
    fatal_error ("Cannot open PPH file for writing: %s: %m", pph_out_file);

  idents_used = cpp_lt_capture (parse_in);
  pph_write_file_contents (stream, &idents_used);

  pph_stream_close (stream);
}


/* Write header information for some AST nodes not handled by the
   common streamer code.  EXPR is the tree to write to output block
   OB.  If EXPR does not need to be handled specially, do nothing.  */

void
pph_out_tree_header (struct output_block *ob, tree expr)
{
  pph_stream *stream = (pph_stream *) ob->sdata;

  if (TREE_CODE (expr) == CALL_EXPR)
    pph_out_uint (stream, call_expr_nargs (expr));
}


/* Emit the fields of FUNCTION_DECL FNDECL to STREAM.  */

static void
pph_out_function_decl (pph_stream *stream, tree fndecl)
{
  /* Note that we do not output DECL_STRUCT_FUNCTION here.  This is
     emitted at the end of the PPH file in pph_out_symtab.
     This way, we will be able to re-instantiate them in the same
     order when reading the image (the allocation of
     DECL_STRUCT_FUNCTION has the side effect of generating function
     sequence numbers (function.funcdef_no).  */
  pph_out_tree_or_ref_1 (stream, DECL_INITIAL (fndecl), 3);
  pph_out_lang_specific (stream, fndecl);
  pph_out_tree_or_ref_1 (stream, DECL_SAVED_TREE (fndecl), 3);
  pph_out_tree_or_ref_1 (stream, DECL_CHAIN (fndecl), 3);
}


/* Callback for writing ASTs to a stream.  This writes all the fields
   that are not processed by default by the common tree pickler.
   OB is as in lto_write_tree.  EXPR is the tree to write.  */

void
pph_write_tree (struct output_block *ob, tree expr,
		bool ref_p ATTRIBUTE_UNUSED)
{
  pph_stream *stream = (pph_stream *) ob->sdata;

  switch (TREE_CODE (expr))
    {
    /* TREES NEEDING EXTRA WORK */

    /* tcc_declaration */

    case DEBUG_EXPR_DECL:
    case IMPORTED_DECL:
    case LABEL_DECL:
    case RESULT_DECL:
      pph_out_tree_or_ref_1 (stream, DECL_INITIAL (expr), 3);
      break;

    case CONST_DECL:
    case FIELD_DECL:
    case NAMESPACE_DECL:
    case PARM_DECL:
    case USING_DECL:
    case VAR_DECL:
      /* FIXME pph: Should we merge DECL_INITIAL into lang_specific? */
      pph_out_tree_or_ref_1 (stream, DECL_INITIAL (expr), 3);
      pph_out_lang_specific (stream, expr);
      /* DECL_CHAIN is handled by generic code, except for VAR_DECLs.  */
      if (TREE_CODE (expr) == VAR_DECL)
	pph_out_tree_or_ref_1 (stream, DECL_CHAIN (expr), 3);
      break;

    case FUNCTION_DECL:
      pph_out_function_decl (stream, expr);
      break;

    case TYPE_DECL:
      pph_out_tree_or_ref_1 (stream, DECL_INITIAL (expr), 3);
      pph_out_lang_specific (stream, expr);
      pph_out_tree_or_ref_1 (stream, DECL_ORIGINAL_TYPE (expr), 3);
      break;

    case TEMPLATE_DECL:
      pph_out_tree_or_ref_1 (stream, DECL_INITIAL (expr), 3);
      pph_out_lang_specific (stream, expr);
      pph_out_tree_or_ref_1 (stream, DECL_TEMPLATE_RESULT (expr), 3);
      pph_out_tree_or_ref_1 (stream, DECL_TEMPLATE_PARMS (expr), 3);
      pph_out_tree_or_ref_1 (stream, DECL_CONTEXT (expr), 3);
      break;

    /* tcc_type */

    case ARRAY_TYPE:
    case BOOLEAN_TYPE:
    case COMPLEX_TYPE:
    case ENUMERAL_TYPE:
    case FIXED_POINT_TYPE:
    case FUNCTION_TYPE:
    case INTEGER_TYPE:
    case LANG_TYPE:
    case METHOD_TYPE:
    case NULLPTR_TYPE:
    case OFFSET_TYPE:
    case POINTER_TYPE:
    case REAL_TYPE:
    case REFERENCE_TYPE:
    case VECTOR_TYPE:
    case VOID_TYPE:
      pph_out_lang_type (stream, expr);
      break;

    case QUAL_UNION_TYPE:
    case RECORD_TYPE:
    case UNION_TYPE:
      pph_out_lang_type (stream, expr);
      pph_out_tree_or_ref_1 (stream, TYPE_BINFO (expr), 3);
      break;

    case BOUND_TEMPLATE_TEMPLATE_PARM:
    case DECLTYPE_TYPE:
    case TEMPLATE_TEMPLATE_PARM:
    case TEMPLATE_TYPE_PARM:
    case TYPENAME_TYPE:
    case TYPEOF_TYPE:
      pph_out_lang_type (stream, expr);
      pph_out_tree_or_ref_1 (stream, TYPE_CACHED_VALUES (expr), 3);
      /* Note that we are using TYPED_CACHED_VALUES for it access to
         the generic .values field of types. */
      break;

    /* tcc_statement */

    case STATEMENT_LIST:
      {
        tree_stmt_iterator i;
        unsigned num_stmts;

        /* Compute and write the number of statements in the list.  */
        for (num_stmts = 0, i = tsi_start (expr); !tsi_end_p (i); tsi_next (&i))
	  num_stmts++;

        pph_out_uint (stream, num_stmts);

        /* Write the statements.  */
        for (i = tsi_start (expr); !tsi_end_p (i); tsi_next (&i))
	  pph_out_tree_or_ref_1 (stream, tsi_stmt (i), 3);
      }
      break;

    /* tcc_expression */

    /* tcc_unary */

    /* tcc_vl_exp */

    /* tcc_reference */

    /* tcc_constant */

    /* tcc_exceptional */

    case OVERLOAD:
      pph_out_tree_common (stream, expr);
      pph_out_tree_or_ref_1 (stream, OVL_CURRENT (expr), 3);
      break;

    case IDENTIFIER_NODE:
      {
        struct lang_identifier *id = LANG_IDENTIFIER_CAST (expr);
        pph_out_cxx_binding (stream, id->namespace_bindings);
        pph_out_cxx_binding (stream, id->bindings);
        pph_out_tree_or_ref_1 (stream, id->class_template_info, 3);
        pph_out_tree_or_ref_1 (stream, id->label_value, 3);
	pph_out_tree_or_ref_1 (stream, TREE_TYPE (expr), 3);
      }
      break;

    case BASELINK:
      pph_out_tree_common (stream, expr);
      pph_out_tree_or_ref_1 (stream, BASELINK_BINFO (expr), 3);
      pph_out_tree_or_ref_1 (stream, BASELINK_FUNCTIONS (expr), 3);
      pph_out_tree_or_ref_1 (stream, BASELINK_ACCESS_BINFO (expr), 3);
      break;

    case TEMPLATE_INFO:
      pph_out_tree_common (stream, expr);
      pph_out_qual_use_vec (stream,
          TI_TYPEDEFS_NEEDING_ACCESS_CHECKING (expr));
      break;

    case TEMPLATE_PARM_INDEX:
      {
        template_parm_index *p = TEMPLATE_PARM_INDEX_CAST (expr);
        pph_out_tree_common (stream, expr);
        pph_out_uint (stream, p->index);
        pph_out_uint (stream, p->level);
        pph_out_uint (stream, p->orig_level);
        pph_out_uint (stream, p->num_siblings);
        pph_out_tree_or_ref_1 (stream, p->decl, 3);
      }
      break;

    /* tcc_constant */

    case PTRMEM_CST:
      pph_out_tree_common (stream, expr);
      pph_out_tree_or_ref_1 (stream, PTRMEM_CST_MEMBER (expr), 3);
      break;

    /* tcc_exceptional */

    case DEFAULT_ARG:
      pph_out_tree_common (stream, expr);
      pph_out_token_cache (stream, DEFARG_TOKENS (expr));
      pph_out_tree_vec (stream, DEFARG_INSTANTIATIONS (expr));
      break;

    case STATIC_ASSERT:
      pph_out_tree_common (stream, expr);
      pph_out_tree_or_ref_1 (stream, STATIC_ASSERT_CONDITION (expr), 3);
      pph_out_tree_or_ref_1 (stream, STATIC_ASSERT_MESSAGE (expr), 3);
      /* FIXME pph: also STATIC_ASSERT_SOURCE_LOCATION (expr).  */
      break;

    case ARGUMENT_PACK_SELECT:
      pph_out_tree_common (stream, expr);
      pph_out_tree_or_ref_1 (stream, ARGUMENT_PACK_SELECT_FROM_PACK (expr), 3);
      pph_out_uint (stream, ARGUMENT_PACK_SELECT_INDEX (expr));
      break;

    case TRAIT_EXPR:
      pph_out_tree_common (stream, expr);
      pph_out_tree_or_ref_1 (stream, TRAIT_EXPR_TYPE1 (expr), 3);
      pph_out_tree_or_ref_1 (stream, TRAIT_EXPR_TYPE2 (expr), 3);
      pph_out_uint (stream, TRAIT_EXPR_KIND (expr));
      break;

    case LAMBDA_EXPR:
      {
        struct tree_lambda_expr *e
            = (struct tree_lambda_expr *)LAMBDA_EXPR_CHECK (expr);
        pph_out_tree_common (stream, expr);
        /* FIXME pph: also e->locus.  */
        pph_out_tree_or_ref_1 (stream, e->capture_list, 3);
        pph_out_tree_or_ref_1 (stream, e->this_capture, 3);
        pph_out_tree_or_ref_1 (stream, e->return_type, 3);
        pph_out_tree_or_ref_1 (stream, e->extra_scope, 3);
        pph_out_uint (stream, e->discriminator);
      }
      break;


    /* TREES ALREADY HANDLED */

    /* tcc_declaration */

    case TRANSLATION_UNIT_DECL:

    /* tcc_exceptional */

    case TREE_BINFO:
    case TREE_LIST:
    case TREE_VEC:

      break;

    /* TREES UNIMPLEMENTED */

    /* tcc_declaration */

    /* tcc_type */

    case TYPE_ARGUMENT_PACK:
    case TYPE_PACK_EXPANSION:
    case UNBOUND_CLASS_TEMPLATE:

    /* tcc_statement */

    case USING_STMT:
    case TRY_BLOCK:
    case EH_SPEC_BLOCK:
    case HANDLER:
    case CLEANUP_STMT:
    case IF_STMT:
    case FOR_STMT:
    case RANGE_FOR_STMT:
    case WHILE_STMT:
    case DO_STMT:
    case BREAK_STMT:
    case CONTINUE_STMT:
    case SWITCH_STMT:

    /* tcc_expression */

    case NEW_EXPR:
    case VEC_NEW_EXPR:
    case DELETE_EXPR:
    case VEC_DELETE_EXPR:
    case TYPE_EXPR:
    case VEC_INIT_EXPR:
    case THROW_EXPR:
    case EMPTY_CLASS_EXPR:
    case TEMPLATE_ID_EXPR:
    case PSEUDO_DTOR_EXPR:
    case MODOP_EXPR:
    case DOTSTAR_EXPR:
    case TYPEID_EXPR:
    case NON_DEPENDENT_EXPR:
    case CTOR_INITIALIZER:
    case MUST_NOT_THROW_EXPR:
    case EXPR_STMT:
    case TAG_DEFN:
    case OFFSETOF_EXPR:
    case SIZEOF_EXPR:
    case ARROW_EXPR:
    case ALIGNOF_EXPR:
    case AT_ENCODE_EXPR:
    case STMT_EXPR:
    case NONTYPE_ARGUMENT_PACK:
    case EXPR_PACK_EXPANSION:

    /* tcc_unary */

    case CAST_EXPR:
    case REINTERPRET_CAST_EXPR:
    case CONST_CAST_EXPR:
    case STATIC_CAST_EXPR:
    case DYNAMIC_CAST_EXPR:
    case NOEXCEPT_EXPR:
    case UNARY_PLUS_EXPR:

    /* tcc_reference */

    case MEMBER_REF:
    case OFFSET_REF:
    case SCOPE_REF:

    /* tcc_vl_exp */

    case AGGR_INIT_EXPR:


      if (flag_pph_untree)
        fprintf (pph_logfile, "PPH: unimplemented tree node %s\n",
                 tree_code_name[TREE_CODE (expr)]);
      break;


    /* TREES UNRECOGNIZED */

    default:
      if (flag_pph_untree)
        fprintf (pph_logfile, "PPH: unrecognized tree node %s\n",
                 tree_code_name[TREE_CODE (expr)]);
    }
}


/* Add DECL to the list of symbols that need to be registered with the
   middle end when reading current_pph_stream.  */

void
pph_add_decl_to_register (tree decl)
{
  if (decl)
    {
      if (decls_to_register.m == NULL)
	decls_to_register.m = pointer_set_create ();

      if (!pointer_set_insert (decls_to_register.m, decl))
	VEC_safe_push (tree, heap, decls_to_register.v, decl);
    }
}
