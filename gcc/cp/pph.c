/* Factored pre-parsed header (PPH) support for C++
   Copyright (C) 2010, 2011 Free Software Foundation, Inc.
   Contributed by Lawrence Crowl <crowl@google.com> and
   Diego Novillo <dnovillo@google.com>.

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
#include "cpplib.h"
#include "toplev.h"
#include "tree.h"
#include "cp-tree.h"
#include "timevar.h"
#include "pointer-set.h"
#include "fixed-value.h"
#include "md5.h"
#include "tree-pass.h"
#include "tree-dump.h"
#include "tree-inline.h"
#include "tree-pretty-print.h"
#include "parser.h"
#include "pph.h"
#include "pph-streamer.h"

/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
FILE *pph_logfile = NULL;

/* Compute an index value for TYPE suitable for restoring it later
   from global_trees[] or integer_types.  The index is saved
   in TYPE_IX_P and the number category (one of CPP_N_INTEGER,
   CPP_N_FLOATING, etc) is saved in CATEGORY_P.  */

static void
pth_get_index_from_type (tree type, unsigned *type_ix_p, unsigned *category_p)
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
pth_write_number (pph_stream *f, tree val)
{
  unsigned type_idx, type_kind;

  pth_get_index_from_type (TREE_TYPE (val), &type_idx, &type_kind);

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
      pth_write_number (f, TREE_REALPART (val));
      pth_write_number (f, TREE_IMAGPART (val));
    }
  else
    gcc_unreachable ();
}


/* Save the tree associated with TOKEN to file F.  */

static void
pth_save_token_value (pph_stream *f, cp_token *token)
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
	pth_write_number (f, val);
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
pth_save_token (cp_token *token, pph_stream *f)
{
  /* Do not write out the final field in TOKEN.  It contains
     pointers that need to be pickled separately.

     FIXME pph - Need to also emit the location_t table so we can
     reconstruct it when reading the PTH state.  */
  pph_out_bytes (f, token, sizeof (cp_token) - sizeof (void *));
  pth_save_token_value (f, token);
}


/* Save all the tokens in CACHE to PPH stream F.  */

void
pth_save_token_cache (cp_token_cache *cache, pph_stream *f)
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
    pth_save_token (tok, f);
}


/* Given a type index TYPE_IDX and TYPE_KIND specifying the kind of type,
   return a type from integer_types or global_trees.  */

static tree
pth_get_type_from_index (unsigned type_idx, unsigned type_kind)
{
  if (type_kind == CPP_N_INTEGER)
    return integer_types[type_idx];
  else if (type_kind == CPP_N_FLOATING || type_kind == CPP_N_FRACT)
    return global_trees[type_idx];
  else if (type_kind == CPP_N_IMAGINARY)
    {
      /* We don't need a type for the complex number.  The type is
	 associated with the real and imaginary parts.  */
      return NULL_TREE;
    }
  else
    gcc_unreachable ();
}


/* Load a numeric value from file F.  Return the corresponding tree.  */

static tree
pth_load_number (pph_stream *f)
{
  unsigned type_idx, type_kind;
  tree type, val;

  type_idx = pph_in_uint (f);
  type_kind = pph_in_uint (f);

  type = pth_get_type_from_index (type_idx, type_kind);

  if (type_kind == CPP_N_INTEGER)
    {
      HOST_WIDE_INT v[2];
      pph_in_bytes (f, v, 2 * sizeof (HOST_WIDE_INT));
      val = build_int_cst_wide (type, v[0], v[1]);
    }
  else if (type_kind == CPP_N_FLOATING)
    {
      REAL_VALUE_TYPE r;
      pph_in_bytes (f, &r, sizeof (REAL_VALUE_TYPE));
      val = build_real (type, r);
    }
  else if (type_kind == CPP_N_FRACT)
    {
      FIXED_VALUE_TYPE fv;
      pph_in_bytes (f, &fv, sizeof (FIXED_VALUE_TYPE));
      val = build_fixed (type, fv);
    }
  else if (type_kind == CPP_N_IMAGINARY)
    {
      tree r = pth_load_number (f);
      tree i = pth_load_number (f);
      val = build_complex (NULL_TREE, r, i);
    }
  else
    gcc_unreachable ();

  return val;
}


/* Load the tree value associated with TOKEN to file F.  */

static void
pth_load_token_value (cp_token *token, pph_stream *f)
{
  const char *str;

  switch (token->type)
    {
      case CPP_TEMPLATE_ID:
      case CPP_NESTED_NAME_SPECIFIER:
	break;

      case CPP_NAME:
	str = pph_in_string (f);
	token->u.value = get_identifier (str);
	break;

      case CPP_KEYWORD:
	token->u.value = ridpointers[token->keyword];
	break;

      case CPP_CHAR:
      case CPP_WCHAR:
      case CPP_CHAR16:
      case CPP_CHAR32:
      case CPP_NUMBER:
	token->u.value = pth_load_number (f);
	break;

      case CPP_STRING:
      case CPP_WSTRING:
      case CPP_STRING16:
      case CPP_STRING32:
	str = pph_in_string (f);
	token->u.value = build_string (strlen (str), str);
	break;

      case CPP_PRAGMA:
	/* Nothing to do.  Field pragma_kind has already been loaded.  */
	break;

      default:
	pph_in_bytes (f, &token->u.value, sizeof (token->u.value));
	gcc_assert (token->u.value == NULL);
    }
}


/* Read and return a token from STREAM.  */

static cp_token *
pth_load_token (pph_stream *stream)
{
  cp_token *token = ggc_alloc_cleared_cp_token ();

  /* Do not read the whole structure, the token value has
     dynamic size as it contains swizzled pointers.
     FIXME pph, restructure to allow bulk reads of the whole
     section.  */
  pph_in_bytes (stream, token, sizeof (cp_token) - sizeof (void *));

  /* FIXME pph.  Use an arbitrary (but valid) location to avoid
     confusing the rest of the compiler for now.  */
  token->location = input_location;

  /* FIXME pph: verify that pth_load_token_value works with no tokens.  */
  pth_load_token_value (token, stream);

  return token;
}


/* Read and return a cp_token_cache instance from STREAM.  */

cp_token_cache *
pth_load_token_cache (pph_stream *stream)
{
  unsigned i, num;
  cp_token *first, *last;

  num = pph_in_uint (stream);
  for (last = first = NULL, i = 0; i < num; i++)
    {
      last = pth_load_token (stream);
      if (first == NULL)
	first = last;
    }

  return cp_token_cache_new (first, last);
}


/* Dump a complicated name for tree T to FILE using FLAGS.
   See TDF_* in tree-pass.h for flags.  */

static void
pph_dump_tree_name (FILE *file, tree t, int flags)
{
  enum tree_code code = TREE_CODE (t);
  fprintf (file, "%s\t", tree_code_name[code]);
  if (code == FUNCTION_TYPE || code == METHOD_TYPE)
    {
      dump_function_to_file (t, file, flags);
    }
  else
    {
      print_generic_expr (file, TREE_TYPE (t), flags);
      /* FIXME pph: fprintf (file, " ", cxx_printable_name (t, 0)); */
      fprintf (file, " " );
      print_generic_expr (file, t, flags);
    }
  fprintf (file, "\n");
}


/* Dump namespace NS for PPH.  */

void
pph_dump_namespace (FILE *file, tree ns)
{
  struct cp_binding_level *level;
  tree t, chain;
  level = NAMESPACE_LEVEL (ns);

  fprintf (file, "namespace ");
  print_generic_expr (file, ns, 0);
  fprintf (file, " {\n");
  for (t = level->names; t; t = chain)
    {
      chain = DECL_CHAIN (t);
      if (!DECL_IS_BUILTIN (t))
        pph_dump_tree_name (file, t, 0);
    }
  for (t = level->namespaces; t; t = chain)
    {
      chain = DECL_CHAIN (t);
      if (!DECL_IS_BUILTIN (t))
        pph_dump_namespace (file, t);
    }
  fprintf (file, "}\n");
}


/* Record a #include or #include_next for PPH.  */

static bool
pph_include_handler (cpp_reader *reader,
                     location_t loc ATTRIBUTE_UNUSED,
                     const unsigned char *dname,
                     const char *name,
                     int angle_brackets,
                     const cpp_token **tok_p ATTRIBUTE_UNUSED)
{
  const char *pph_file;
  bool read_text_file_p;

  if (flag_pph_debug >= 1)
    {
      fprintf (pph_logfile, "PPH: #%s", dname);
      fprintf (pph_logfile, " %c", angle_brackets ? '<' : '"');
      fprintf (pph_logfile, "%s", name);
      fprintf (pph_logfile, "%c\n", angle_brackets ? '>' : '"');
    }

  read_text_file_p = true;
  pph_file = query_pph_include_map (name);
  if (pph_file != NULL && !cpp_included_before (reader, name, input_location))
    {
      pph_read_file (pph_file);
      read_text_file_p = false;
    }

  return read_text_file_p;
}


/* The initial order of the size of the lexical lookaside table,
   which will accomodate as many as half of its slots in use.  */

static const unsigned int cpp_lt_order = /* 2 to the power of */ 9;

/* Initialize PPH support.  */

void
pph_init (void)
{
  cpp_callbacks *cb;
  cpp_lookaside *table;

  if (flag_pph_logfile)
    {
      pph_logfile = fopen (flag_pph_logfile, "w");
      if (!pph_logfile)
	fatal_error ("Cannot create %s for writing: %m", flag_pph_logfile);
    }
  else
    pph_logfile = stdout;

  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Initializing.\n");

  cb = cpp_get_callbacks (parse_in);
  cb->include = pph_include_handler;
  /* FIXME pph: Use file change instead.
  state->file_change_prev = cb->file_change;
  cb->file_change = pph_file_change;
  */

  table = cpp_lt_exchange (parse_in,
                           cpp_lt_create (cpp_lt_order, flag_pph_debug));
  gcc_assert (table == NULL);
}


/* Finalize PPH support.  */

void
pph_finish (void)
{
  if (pph_out_file != NULL)
    {
      const char *offending_file = cpp_main_missing_guard (parse_in);
      if (offending_file == NULL)
        pph_write_file ();
      else
        error ("header lacks guard for PPH: %s", offending_file);
    }

  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Finishing.\n");

  if (flag_pph_logfile)
    fclose (pph_logfile);
}
