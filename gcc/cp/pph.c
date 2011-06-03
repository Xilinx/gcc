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

/* Statistics collected for PTH.  */
struct pth_stats_d pth_stats;

/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
FILE *pph_logfile = NULL;


/* Return true if path P1 and path P2 point to the same file.  */

static inline bool
pathnames_equal_p (const char *p1, const char *p2)
{
  return strcmp (lrealpath (p1), lrealpath (p2)) == 0;
}

/* Hash and comparison functions for the directory of cached images.  */

static hashval_t
pth_image_dir_hash (const void *p)
{
  const char *s = lrealpath (((const pth_image *)p)->fname);
  return htab_hash_string (s);
}


static int
pth_image_dir_eq (const void *p1, const void *p2)
{
  const char *s1 = ((const pth_image *)p1)->fname;
  const char *s2 = ((const pth_image *)p2)->fname;
  return pathnames_equal_p (s1, s2);
}


static GTY(()) pth_state *pth_global_state = NULL;

/* Return the global PTH state where the cache and its directory
   are stored.  */

pth_state *
pth_get_state (void)
{
  if (pth_global_state == NULL)
    {
      pth_global_state = ggc_alloc_cleared_pth_state ();
      pth_global_state->cache_dir = htab_create_ggc (10, pth_image_dir_hash,
				                     pth_image_dir_eq, NULL);
    }

  return pth_global_state;
}


/* Return an identification string for a PTH image.  */

static const char *
pth_id_str (void)
{
  /* FIXME pph - Build a better identification string.  */
  return "PTH0x42";
}


/* Return the number of bytes taken by the header of a PTH image.  */

static size_t
pth_header_len (void)
{
  /* The header of a PTH image contains:
	- An identification string (pth_id_str ())
	- The MD5 digest for the source file associated with the image.
   */
  return strlen (pth_id_str ()) + DIGEST_LEN;
}


#define PTH_EXTENSION ".pth"

/* Return a new string with the extension PTH_EXTENSION appended to NAME.  The
   caller is responsible for freeing the returned string.  */

static char *
pth_name_for (const char *name)
{
  size_t i, len;
  char *s;
  
  len = strlen (name) + sizeof (PTH_EXTENSION) + 1;
  s = XCNEWVEC (char, len);
  sprintf (s, "%s" PTH_EXTENSION, name);

  /* Make the file name unique and store it in the current directory.  */
  for (i = 0; i < len - sizeof (PTH_EXTENSION) - 1; i++)
    if (!ISALNUM (s[i]))
      s[i] = '_';

  return s;
}


/* Open an image file for path NAME.  MODE is as in fopen.  */

static pph_stream *
pth_file_for (const char *name, const char *mode)
{
  char *s;
  pph_stream *f;

  s = pth_name_for (name);
  f = pph_stream_open (s, mode);
  if (!f)
    fatal_error ("can%'t open token stream file %s: %m", s);
  free (s);

  return f;
}


/* Compute the MD5 digest for FNAME.  Store it in DIGEST.  */

static void
pth_get_md5_digest (const char *fname, unsigned char digest[DIGEST_LEN])
{
  if (flag_pth_md5)
    {
      FILE *f;

      timevar_push (TV_PTH_MD5);

      f = fopen (fname, "rb");
      if (f == NULL)
	fatal_error ("Cannot open %s for computing its digest: %m", fname);

      md5_stream (f, digest);

      fclose (f);

      timevar_pop (TV_PTH_MD5);
    }
  else
    {
      struct stat buf;

      if (stat (fname,&buf) != 0)
	fatal_error ("Cannot stat %s: %m", fname);

      memset (digest, 0, DIGEST_LEN);
      memcpy (digest, &buf.st_mtime, sizeof (buf.st_mtime));
    }
}


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


/* Write header information for IMAGE to STREAM.  */

static void
pth_write_header (pth_image *image, pph_stream *stream)
{
  const char *id = pth_id_str ();

  if (!image->digest_computed_p)
    {
      pth_get_md5_digest (image->fname, image->digest);
      image->digest_computed_p = true;
    }

  pph_out_bytes (stream, id, strlen (id));
  pph_out_bytes (stream, image->digest, DIGEST_LEN);
}


/* Dump a table of IDENTIFIERS to the STREAM. */

static void
pth_dump_identifiers (FILE *stream, cpp_idents_used *identifiers)
{
  unsigned int idx, col = 1;
  char use, exp;

  fprintf (stream, "%u identifiers up to %u chars, vals to %u chars\n",
           identifiers->num_entries, identifiers->max_ident_len,
           identifiers->max_value_len);
  for (idx = 0; idx < identifiers->num_entries; ++idx)
    {
      cpp_ident_use *ident = identifiers->entries + idx;

      if (col + ident->ident_len >= 80)
        {
          fprintf (stream, "\n");
          col = 1;
        }
      use = ident->used_by_directive ? 'U' : '-';
      exp = ident->expanded_to_text ? 'E' : '-';
      if (ident->before_str || ident->after_str)
        {
          if (col > 1)
            fprintf (stream, "\n");
          fprintf (stream, " %s %c%c = %s -> %s\n", ident->ident_str, use, exp,
                   ident->before_str, ident->after_str);
          col = 1;
        }
      else
        {
          fprintf (stream, " %s %c%c", ident->ident_str, use, exp);
          col += ident->ident_len;
        }
    }
  fprintf (stream, "\n");
}

/* Dump a debug log of the IDENTIFIERS. */

void
pth_debug_identifiers (cpp_idents_used *identifiers);

void
pth_debug_identifiers (cpp_idents_used *identifiers)
{
  pth_dump_identifiers (stderr, identifiers);
}

/* Dump a HUNK to the STREAM. */

static void
pth_dump_hunk (FILE *stream, cp_token_hunk *hunk)
{
  pth_dump_identifiers (stream, &hunk->identifiers);
  cp_lexer_dump_tokens (stream, hunk->buffer, NULL, 0, NULL);
}

/* Dump a debug log of the HUNK. */

static void
pth_debug_hunk (cp_token_hunk *hunk)
{
  pth_dump_hunk (stderr, hunk);
}


/* Dump #include command INCLUDE to FILE.  */

static void
pth_dump_include (FILE *f, pth_include *include)
{
  if (include == NULL)
    return;

  if (include->itype == IT_INCLUDE)
    fprintf (f, "#include ");
  else if (include->itype == IT_INCLUDE_NEXT)
    fprintf (f, "#include_next ");
  else if (include->itype == IT_IMPORT)
    fprintf (f, "#import ");
  else
    fprintf (f, "#??? ");

  fprintf (f, "%c%s%c",
	   (include->angle_brackets) ? '<' : '"',
	   include->iname,
	   (include->angle_brackets) ? '>' : '"');

  fprintf (f, " (found in %s)\n", include->dname);
}


/* Dump #include command INCLUDE to stderr.  */

static void
pth_debug_include (pth_include *include)
{
  pth_dump_include (stderr, include);
}


/* Recursive helper for pth_dump_token_hunks_1.  VISITED keeps track of
   images that have already been dumped to avoid infinite recursion.  */

static void
pth_dump_token_hunks_1 (FILE *f, pth_image *image,
		        struct pointer_set_t *visited)
{
  unsigned i, h_ix, i_ix;
  char s;

  if (pointer_set_insert (visited, image))
    return;

  fprintf (f, "LC_ENTER: %s {\n", image->fname);

  for (i = 0, h_ix = 0, i_ix = 0;
       VEC_iterate (char, image->ih_sequence, i, s);
       i++)
    {
      if (s == 'H')
	{
	  cp_token_hunk *hunk;
	  hunk = VEC_index (cp_token_hunk_ptr, image->token_hunks, h_ix++);
	  pth_dump_hunk (f, hunk);
	}
      else if (s == 'I')
	{
	  pth_include *include;
	  include = VEC_index (pth_include_ptr, image->includes, i_ix++);
	  pth_dump_include (f, include);
	  pth_dump_token_hunks_1 (f, include->image, visited);
	}
    }

  fprintf (f, "LC_LEAVE: %s }\n", image->fname);
}


/* Dump all the tokens in IMAGE and the files included by it to F.  */

static void
pth_dump_token_hunks (FILE *f, pth_image *image)
{
  struct pointer_set_t *visited = pointer_set_create ();
  pth_dump_token_hunks_1 (f, image, visited);
  pointer_set_destroy (visited);
}


/* Dump all the tokens in IMAGE and the files included by it to stderr.  */

void
pth_debug_token_hunks (pth_image *image)
{
  pth_dump_token_hunks (stderr, image);
}


/* Dump a debugging representation of IMAGE to F.  */

static void
pth_dump_image (FILE *f, pth_image *image)
{
  unsigned i;
  cp_token_hunk *hunk;
  pth_include *include;
  char s;

  if (image == NULL)
    return;

  fprintf (f, "Image for: %s\n", image->fname);

  fprintf (f, "  MD5 digest: ");
  if (image->digest_computed_p)
    {
      for (i = 0; i < DIGEST_LEN; i++)
	fprintf (f, "%02x", image->digest[i]);
      fprintf (f, "\n");
    }
  else
    fprintf (f, "NOT COMPUTED\n");

  fprintf (f, "  %u token hunks: { ",
	   VEC_length (cp_token_hunk_ptr, image->token_hunks));
  for (i = 0; VEC_iterate (cp_token_hunk_ptr, image->token_hunks, i, hunk); i++)
    fprintf (f, "%u ", VEC_length (cp_token, hunk->buffer));
  fprintf (f, "}\n");

  fprintf (f, "  %u includes:\n",
	   VEC_length (pth_include_ptr, image->includes));
  for (i = 0; VEC_iterate (pth_include_ptr, image->includes, i, include); i++)
    {
      fprintf (f, "\t");
      pth_dump_include (f, include);
    }

  fprintf (f, "  Include-Hunk (IH) sequence:          ");
  for (i = 0; VEC_iterate (char, image->ih_sequence, i, s); i++)
    fputc (s, f);
  fputc ('\n', f);

  if (image->loaded_p)
    fprintf (f, "  Instantiated from image: %s\n", pth_name_for (image->fname));
  else
    {
      fprintf (f, "  Instantiated from character stream: %s\n", image->fname);

      if (image->save_p)
	fprintf (f, "  Will be saved to image: %s\n",
		 pth_name_for (image->fname));
      else
	fprintf (f, "  Will NOT be saved to an image (not include-guarded)\n");
    }

  if (image->used_p)
    fprintf (f, "  Image already applied to current compilation context\n");

  if (flag_pth_debug >= 4)
    pth_dump_token_hunks (f, image);
}


/* Dump a debugging representation of IMAGE to stderr.  */

static void
pth_debug_image (pth_image *image)
{
  pth_dump_image (stderr, image);
}


/* Show statistics for PTH image IMAGE on FILE.  if FILE is NULL, use
   pph_logfile.  */

static void
pth_show_image_stats (FILE *file, pth_image *image)
{
  unsigned i, num_tokens;
  cp_token_hunk *hunk;

  if (image == NULL)
    return;

  if (file == NULL)
    file = pph_logfile;

  num_tokens = 0;
  for (i = 0; VEC_iterate (cp_token_hunk_ptr, image->token_hunks, i, hunk); i++)
    num_tokens += VEC_length (cp_token, hunk->buffer);

  fprintf (file, "%s: %u tokens, %u token hunks\n", image->fname, num_tokens,
	   VEC_length (cp_token_hunk_ptr, image->token_hunks));
}


/* Dump the current PTH state to F.  */

static void
pth_dump_state (FILE *f)
{
  unsigned i;
  pth_state *state;
  pth_image *image;
  
  state = pth_get_state ();
  fprintf (f, "Incremental compilation state\n\n");
  fprintf (f, "%u file images\n", VEC_length (pth_image_ptr, state->cache));
  for (i = 0; VEC_iterate (pth_image_ptr, state->cache, i, image); i++)
    pth_dump_image (f, image);

  fprintf (f, "\nCurrent image being processed: %s\n",
	   (state->cur_image) ? state->cur_image->fname : "NONE");

  if (state->lexer)
    {
      fprintf (f, "Tokens in main lexer:\n");
      cp_lexer_dump_tokens (f, state->lexer->buffer, NULL, 0, NULL);
    }
}


/* Dump the current PTH state to stderr.  */

void
pth_debug_state (void)
{
  pth_dump_state (stderr);
}


/* Save the IDENTIFIERS to the STREAM.  */

static void
pth_save_identifiers (cpp_idents_used *identifiers, pph_stream *stream)
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

/* Save the HUNK to the STREAM.  */

static void
pth_save_hunk (cp_token_hunk *hunk, pph_stream *stream)
{
  unsigned j;
  cp_token *token;

  if (flag_pth_debug >= 5)
    pth_debug_hunk (hunk);

  /* Write out the identifiers used by HUNK.  */
  pth_save_identifiers (&hunk->identifiers, stream);

  /* Write the number of tokens in HUNK.  */
  pph_out_uint (stream, VEC_length (cp_token, hunk->buffer));

  /* Write the tokens.  */
  for (j = 0; VEC_iterate (cp_token, hunk->buffer, j, token); j++)
    pth_save_token (token, stream);
}


/* Save the #include directive INCLUDE to STREAM.  */

static void
pth_save_include (pth_include *include, pph_stream *stream)
{
  pph_out_string (stream, include->image->fname);
  pph_out_uint (stream, (unsigned int) include->itype);
  pph_out_uint (stream, include->angle_brackets);
  pph_out_string (stream, include->iname);
  pph_out_string (stream, include->dname);
}


/* Save the PTH image IMAGE to a file.  */

static void
pth_save_image (pth_image *image)
{
  pph_stream *stream;
  cp_token_hunk *hunk;
  unsigned i, num;
  pth_include *include;

  timevar_push (TV_PTH_SAVE);

  /* Open the stream in append mode since we have already created
     it in pth_new_image.  */
  stream = pth_file_for (image->fname, "wb");

  /* Write a header to recognize the file later.  */
  pth_write_header (image, stream);

  /* Write the include-hunk (IH) sequencing vector.  */
  num = VEC_length (char, image->ih_sequence);
  pph_out_uint (stream, num);
  if (num > 0)
    pph_out_bytes (stream, VEC_address (char, image->ih_sequence), num);
  
  /* Write the number of #include commands.  */
  pph_out_uint (stream, VEC_length (pth_include_ptr, image->includes));

  /* Write all the #include commands used by IMAGE.  */
  for (i = 0; VEC_iterate (pth_include_ptr, image->includes, i, include); i++)
    pth_save_include (include, stream);

  /* Write the number of token caches in the cache.  */
  pph_out_uint (stream, VEC_length (cp_token_hunk_ptr, image->token_hunks));

  /* Write all the token hunks in image.  */
  for (i = 0; VEC_iterate (cp_token_hunk_ptr, image->token_hunks, i, hunk); i++)
    pth_save_hunk (hunk, stream);

  /* Clean up.  */
  pph_stream_close (stream);
  image->save_p = false;

  if (flag_pth_debug >= 3)
    {
      fprintf (stderr, "\nSaved image for %s:\n", image->fname);
      pth_debug_image (image);
    }

  timevar_pop (TV_PTH_SAVE);
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


/* Load the IDENTIFERS for a hunk from a STREAM.  */

static void
pth_load_identifiers (cpp_idents_used *identifiers, pph_stream *stream)
{
  unsigned int j;
  unsigned int max_ident_len, max_value_len, num_entries;
  unsigned int ident_len, before_len, after_len;

  max_ident_len = pph_in_uint (stream);
  identifiers->max_ident_len = max_ident_len;
  max_value_len = pph_in_uint (stream);
  identifiers->max_value_len = max_value_len;
  num_entries = pph_in_uint (stream);
  identifiers->num_entries = num_entries;
  identifiers->entries = XCNEWVEC (cpp_ident_use, num_entries);
  identifiers->strings = XCNEW (struct obstack);

  /* Strings need no alignment.  */
  _obstack_begin (identifiers->strings, 0, 0,
                  (void *(*) (long)) xmalloc,
                  (void (*) (void *)) free);
  obstack_alignment_mask (identifiers->strings) = 0;
  /* FIXME pph: We probably need to free all these things somewhere.  */

  /* Read the identifiers in HUNK. */
  for (j = 0; j < num_entries; ++j)
    {
      const char *s;
      identifiers->entries[j].used_by_directive = pph_in_uint (stream);
      identifiers->entries[j].expanded_to_text = pph_in_uint (stream);
      s = pph_in_string (stream);
      gcc_assert (s);
      ident_len = strlen (s);
      identifiers->entries[j].ident_len = ident_len;
      identifiers->entries[j].ident_str =
        (const char *) obstack_copy0 (identifiers->strings, s, ident_len);

      s = pph_in_string (stream);
      if (s)
	{
	  before_len = strlen (s);
	  identifiers->entries[j].before_len = before_len;
	  identifiers->entries[j].before_str = (const char *)
	      obstack_copy0 (identifiers->strings, s, before_len);
	}
      else
	{
	  /* The identifier table expects NULL entries to have
	     a length of -1U.  */
	  identifiers->entries[j].before_len = -1U;
	  identifiers->entries[j].before_str = NULL;
	}

      s = pph_in_string (stream);
      if (s)
	{
	  after_len = strlen (s);
	  identifiers->entries[j].after_len = after_len;
	  identifiers->entries[j].after_str = (const char *)
	      obstack_copy0 (identifiers->strings, s, after_len);
	}
      else
	{
	  /* The identifier table expects NULL entries to have
	     a length of -1U.  */
	  identifiers->entries[j].after_len = -1U;
	  identifiers->entries[j].after_str = NULL;
	}
    }
}


/* Load a hunk into the IMAGE from a STREAM.  */

static void
pth_load_hunk (pth_image *image, pph_stream *stream)
{
  unsigned j, num_tokens;
  cp_token_hunk *hunk;
  
  hunk = ggc_alloc_cleared_cp_token_hunk ();

  /* Setup the identifier list.  */
  pth_load_identifiers (&hunk->identifiers, stream);

  /* Read the number of tokens in HUNK. */
  num_tokens = pph_in_uint (stream);

  /* Read the tokens in the HUNK. */
  hunk->buffer = VEC_alloc (cp_token, gc, num_tokens);
  for (j = 0; j < num_tokens; j++)
    {
      cp_token *token = pth_load_token (stream);
      VEC_quick_push (cp_token, hunk->buffer, token);
    }
  gcc_assert (num_tokens == VEC_length (cp_token, hunk->buffer));

  VEC_quick_push (cp_token_hunk_ptr, image->token_hunks, hunk);
}


/* Create a new empty #include directive for NAME.  ITYPE is one of
   the supported include commands.  ANGLE_BRACKETS is true if the
   include used '<>'.  */

static pth_include *
pth_create_include (enum include_type itype, bool angle_brackets,
		    const char *name)
{
  pth_include *include = ggc_alloc_cleared_pth_include ();
  include->itype = itype;
  include->angle_brackets = angle_brackets;
  include->iname = (name) ? xstrdup (name) : name;

  return include;
}


/* Load an #include directive for IMAGE from STREAM.  */

static void
pth_load_include (pth_state *state, pth_image *image, cpp_reader *reader,
		  pph_stream *stream)
{
  const char *s;
  pth_include *include;
  unsigned tmp;

  include = pth_create_include (IT_INCLUDE, false, NULL);

  s = pph_in_string (stream);
  include->image = pth_image_lookup (state, s, reader);

  tmp = pph_in_uint (stream);
  include->itype = (enum include_type) tmp;

  tmp = pph_in_uint (stream);
  include->angle_brackets = (tmp != 0);

  include->iname = pph_in_string (stream);
  include->dname = pph_in_string (stream);

  VEC_safe_push (pth_include_ptr, gc, image->includes, include);
}


/* Load a PTH image for LEXER using the READER.  */

static void
pth_load_image (pth_state *state, pth_image *image, cpp_reader *reader)
{
  pph_stream *stream;
  unsigned i, num;
  char *buf;

  timevar_push (TV_PTH_LOAD);

  stream = pth_file_for (image->fname, "rb");

  /* Skip over the header, as we assume that it has already been
     validated by pth_have_valid_image_for.  */
  buf = XCNEWVEC (char, pth_header_len ());
  pph_in_bytes (stream, buf, pth_header_len ());
  free (buf);

  /* Read the include-hunk (IH) sequencing vector.  */
  num = pph_in_uint (stream);
  if (num > 0)
    {
      image->ih_sequence = VEC_alloc (char, gc, num);
      VEC_safe_grow (char, gc, image->ih_sequence, num);
      pph_in_bytes (stream, VEC_address (char, image->ih_sequence), num);
    }

  /* Read the number path names of all the files #included by
     IMAGE->FNAME.  */
  num = pph_in_uint (stream);
  image->includes = VEC_alloc (pth_include_ptr, gc, num);

  /* Now read all the path names #included by IMAGE->FNAME.  */
  for (i = 0; i < num; i++)
    pth_load_include (state, image, reader, stream);

  /* Read how many token hunks are contained in this image.  */
  num = pph_in_uint (stream);
  image->token_hunks = VEC_alloc (cp_token_hunk_ptr, gc, num);

  PTH_STATS_INCR (hunks, num);

  /* Read all the token hunks.  */
  for (i = 0; i < num; i++)
    pth_load_hunk (image, stream);

  pph_stream_close (stream);

  /* Indicate that we have loaded this image from a file.  */
  image->loaded_p = true;
  image->save_p = false;
  image->used_p = false;

  if (flag_pth_debug >= 3)
    {
      fprintf (stderr, "\nLoaded image for %s:\n", image->fname);
      pth_debug_image (image);
    }

  timevar_pop (TV_PTH_LOAD);
}


/* Return true if FNAME has a PTH image that can be used.  If an image
   already exists, compute the MD5 digest for FNAME and store it
   in IMAGE.  */

static bool
pth_have_valid_image_for (const char *fname, pth_image *image)
{
  pph_stream *f = NULL;
  struct stat s;
  char *img_name, *id;
  const char *good_id;
  char saved_digest[DIGEST_LEN];

  image->digest_computed_p = false;

  img_name = pth_name_for (fname);
  if (stat (img_name, &s) != 0)
    goto invalid_img;

  /* If the file exists, check if it has a valid signature.  */
  f = pph_stream_open (img_name, "rb");
  gcc_assert (f);

  good_id = pth_id_str ();
  id = XCNEWVEC (char, strlen (good_id) + 1);
  pph_in_bytes (f, id, strlen (good_id));
  if (strcmp (id, good_id) != 0)
    goto invalid_img;

  /* Now check if the MD5 digest stored in the image file matches the
     digest for FNAME.  */
  pph_in_bytes (f, saved_digest, DIGEST_LEN);
  pth_get_md5_digest (fname, image->digest);
  image->digest_computed_p = true;
  if (memcmp (image->digest, saved_digest, DIGEST_LEN) != 0)
    goto invalid_img;

  pph_stream_close (f);
  return true;

invalid_img:
  if (f)
    pph_stream_close (f);

  return false;
}


/* Create a new PTH cache object for file FNAME.  */

static pth_image *
pth_new_image (const char *fname)
{
  pth_image *image;

  image = ggc_alloc_cleared_pth_image ();
  image->fname = xstrdup (fname);

  return image;
}


/* Return a cache image associated with file FNAME.  STATE holds
   the PTH cache to use.  */

pth_image *
pth_image_lookup (pth_state *state, const char *fname, cpp_reader *reader)
{
  void **slot;
  pth_image *image, e;

  if (fname == NULL)
    return NULL;

  e.fname = fname;
  slot = htab_find_slot (state->cache_dir, &e, INSERT);
  if (*slot == NULL)
    {
      /* Create a new image and store it in the cache.  */
      image = pth_new_image (fname);
      *slot = image;
      VEC_safe_push (pth_image_ptr, gc, state->cache, image);

      /* If a valid disk image already exists for FNAME, load it.
	 Otherwise, mark the memory image for processing and saving.  */
      if (pth_have_valid_image_for (fname, image))
	pth_load_image (state, image, reader);
      else
	image->save_p = true;
    }
  else
    image = *((pth_image **) slot);

  return image;
}


/* Add all the tokens in HUNK to the end of LEXER->BUFFER.  IMAGE is
   the memory image holding HUNK.  */

static void
pth_append_hunk (cp_lexer *lexer, cp_token_hunk *hunk)
{
  cp_token *lexer_addr, *hunk_addr;
  unsigned lexer_len, hunk_len;

  PTH_STATS_INCR (valid_hunks, 1);

  /* Apply all the identifiers used and defined by HUNK.  */
  cpp_lt_replay (parse_in, &hunk->identifiers);

  hunk_len = VEC_length (cp_token, hunk->buffer);

  /* Some hunks have no tokens and they are only useful for the
     macros defined by them.  This is useful when one or more
     image files are tainted and need to be restored from their
     character stream.  */
  if (hunk_len == 0)
    return;

  /* Determine the last location in LEXER->BUFFER before growing it.  */
  lexer_len = VEC_length (cp_token, lexer->buffer);
  VEC_safe_grow (cp_token, gc, lexer->buffer, lexer_len + hunk_len);

  /* Bulk copy all the tokens in HUNK to the end of LEXER->BUFFER.  */
  lexer_addr = VEC_address (cp_token, lexer->buffer);
  hunk_addr = VEC_address (cp_token, hunk->buffer);
  memcpy (&lexer_addr[lexer_len], hunk_addr, hunk_len * sizeof (cp_token));

  if (flag_pth_debug >= 2)
    {
      fprintf (stderr, "\n=> ADDED TOKEN HUNK TO LEXER BUFFER");
      if (flag_pth_debug >= 5)
        pth_debug_hunk (hunk);
    }
}


/* Return true if HUNK can be used in the current compilation
   context of the cpp READER.  It must validate the identifier state.  */

static bool
pth_hunk_is_valid_p (pth_image *image, cp_token_hunk *hunk, cpp_reader *reader)
{
  bool verified;
  cpp_ident_use *bad_use;
  const char *cur_def;

  timevar_push (TV_PTH_DEPENDENCY);
  verified = cpp_lt_verify (reader, &hunk->identifiers, &bad_use, &cur_def);
  if (!verified && flag_pth_debug >= 1)
    {
      pth_debug_hunk (hunk);
      fprintf (stderr, "PTH: %s failed verification: %s : <%s> -> <%s>\n",
                         pth_name_for (image->fname), bad_use->ident_str,
                         bad_use->before_str, cur_def);
    }
  PTH_STATS_INCR (verified_hunks, 1);
  PTH_STATS_INCR (verified_identifiers, hunk->identifiers.num_entries);
  timevar_pop (TV_PTH_DEPENDENCY);
  return verified;
}


/* Return true if IMAGE can be used in the current compilation context.  */

static bool
pth_image_can_be_used (pth_image *image)
{
  return image && image->loaded_p && !image->used_p;
}


/* Split NAME into its directory and file name components, storing them
   in *DNAME_P and *FNAME_P.  After using it, *DNAME_P should be freed
   by the caller.  */

static void
pth_get_dir_and_name (const char *name, char **dname_p, const char **fname_p)
{
  size_t len;

  *fname_p = lbasename (name);
  *dname_p = NULL;

  len = *fname_p - name;
  if (len > 0)
    {
      *dname_p = XNEWVEC (char, len + 1);
      memcpy (*dname_p, name, len);
      (*dname_p)[len] = '\0';
      gcc_assert (IS_DIR_SEPARATOR ((*dname_p)[len - 1]));
    }
}


/* Read tokens from the text stream in IMAGE->FNAME into LEXER.
   If INCLUDE is not NULL, it describes how IMAGE->FNAME was
   #included originally.  Otherwise, it is assumed to be an
   include command done with the flag -include.

   This is used when we need to process an #include command from 
   a PPH image and the file to be included is a regular text file.  */

static void
pth_process_text_file (cp_lexer *lexer, pth_image *image, pth_include *include)
{
  bool pushed_p;
  cpp_buffer *buffer;

  /* Emulate a #include directive on IMAGE->FNAME, if needed.  Note
     that if we are already inside the CPP buffer for IMAGE->FNAME
     we should not include it again, since this will cause another
     call to pth_file_change which will again register IMAGE->FNAME as
     an include for the parent file.  */
  gcc_assert (image->buffer != cpp_get_buffer (parse_in));

  if (include == NULL || include->itype == IT_INCLUDE_NEXT)
    {
      char *dname;
      const char *fname;
      pth_get_dir_and_name (image->fname, &dname, &fname);
      /* FIXME pph.  We are leaking DNAME here.  libcpp
	 wants the directory name in permanent storage so we
	 cannot free it, but we should put it in an obstack
	 so it can be reclaimed at some point.  */
      pushed_p = cpp_push_include_type (parse_in, dname, fname, false,
	                                IT_INCLUDE);
    }
  else
    pushed_p = cpp_push_include_type (parse_in, include->dname, include->iname,
				      include->angle_brackets, include->itype);

  /* Nothing else to do if libcpp decided it did not need the file.  */
  if (!pushed_p)
    return;

  /* Position the reader at the start of the buffer and request to
     stop reading at the end of it.  */
  buffer = cpp_get_buffer (parse_in);
  cpp_set_pos (buffer, cpp_buffer_start);
  cpp_return_at_eof (buffer, true);

  /* Get tokens from IMAGE->FNAME.  */
  cp_lexer_get_tokens (lexer);

  /* Since we read this file separately, the very last token in LEXER
     will now contain an EOF, which we do not need.  */
  VEC_pop (cp_token, lexer->buffer);
  cpp_return_at_eof (buffer, false);
}


/* Populate LEXER->BUFFER with all the valid token hunks in
   IMAGE.  If possible, try to load token hunks from files
   included by IMAGE as well.

   This means that we try to load the whole transitive closure
   starting at IMAGE until we find the first unloadable file.
   
   Once we find the first unloadable token hunk, we skip the token
   hunks from the character stream so that they don't need to be
   pre-processed again.

   If non-NULL, INCLUDE describes the #include command used to include
   IMAGE->FNAME.  */

static void
pth_image_to_lexer (cp_lexer *lexer, pth_image *image, pth_include *include)
{
  unsigned i, h_ix, i_ix;
  char s;

  /* If we are trying to apply the same image more than once,
     something is wrong.  We never create images for files that 
     are included more than once, so we should never try to apply
     the same image more than once.  */
  gcc_assert (pth_image_can_be_used (image));

  image->used_p = true;

  if (flag_pth_debug >= 2)
    {
      fprintf (stderr, "\n<= INCORPORATING IMAGE %s INTO COMPILATION CONTEXT\n",
	       image->fname);
      if (flag_pth_debug >= 3)
        pth_debug_include (include);
    }

  PTH_STATS_INCR (valid_images, 1);

  for (i = 0, h_ix = 0, i_ix = 0;
       VEC_iterate (char, image->ih_sequence, i, s);
       i++)
    {
      if (s == 'H')
	{
	  cp_token_hunk *hunk;

	  hunk = VEC_index (cp_token_hunk_ptr, image->token_hunks, h_ix++);
	  if (pth_hunk_is_valid_p (image, hunk, parse_in))
	    pth_append_hunk (lexer, hunk);
	  else
	    fatal_error ("Found an invalid hunk in %s.  This header file "
			 "cannot be converted into a pre-parsed image.",
			 image->fname);
	}
      else if (s == 'I')
	{
	  pth_include *incdir;
	  pth_get_state ()->cur_image = image;
	  incdir = VEC_index (pth_include_ptr, image->includes, i_ix++);
	  if (pth_image_can_be_used (incdir->image))
	    pth_image_to_lexer (lexer, incdir->image, incdir);
	  else
	    pth_process_text_file (lexer, incdir->image, incdir);
	}
      else
	gcc_unreachable ();
    }

  /* If we just applied the tokens for the main input filename,
     we need to append a CPP_EOF token, since that one is never
     saved with the token hunks.  */
  if (pathnames_equal_p (image->fname, main_input_filename))
    VEC_safe_push (cp_token, gc, lexer->buffer, &eof_token);

  /* If IMAGE has a libcpp buffer associated with it, it means that a
     file that was being pre-processed from text has #included IMAGE
     and the pre-processor has executed the file change logic.

     In that case, the pre-processor will want to finish processing
     IMAGE's text, and since we have just applied its tokens from
     the image, the result will be duplicate tokens.  To prevent
     this, we tell libcpp to skip over the whole text buffer
     associated with IMAGE.  */
  if (image->buffer)
    cpp_set_pos (image->buffer, cpp_buffer_end);

  return;
}


/* Create a token hunk for IMAGE from the token buffer in
   LEXER->BUFFER.  The hunk will contain all the tokens starting at
   IMAGE->HUNK_START_IX to the end of LEXER->BUFFER.

   The new token hunk will be added to the end of IMAGE->TOKEN_HUNKS.  */

static void
pth_lexer_to_image (pth_image *image, cp_lexer *lexer, cpp_reader *reader)
{
  cp_token *lexer_addr, *hunk_addr;
  cp_token_hunk *hunk;
  unsigned num_tokens, start_ix, end_ix;

  /* Create a new token hunk.  */
  hunk = ggc_alloc_cleared_cp_token_hunk ();
  VEC_safe_push (cp_token_hunk_ptr, gc, image->token_hunks, hunk);
  VEC_safe_push (char, gc, image->ih_sequence, 'H');

  /* The identifiers that may conflict with macros.  */
  hunk->identifiers = cpp_lt_capture (reader);

  /* Compute the bounds for the new token hunk.  */
  start_ix = image->hunk_start_ix;
  end_ix = VEC_length (cp_token, lexer->buffer);
  gcc_assert (end_ix >= start_ix);
  num_tokens = end_ix - start_ix;

  if (num_tokens > 0)
    {
      /* Copy tokens from LEXER->BUFFER into the new hunk.  */
      hunk->buffer = VEC_alloc (cp_token, gc, num_tokens);
      VEC_safe_grow (cp_token, gc, hunk->buffer, num_tokens);
      lexer_addr = VEC_address (cp_token, lexer->buffer);
      hunk_addr = VEC_address (cp_token, hunk->buffer);
      memcpy (hunk_addr, &lexer_addr[start_ix], num_tokens * sizeof (cp_token));
    }

  if (flag_pth_debug >= 3)
    {
      fprintf (stderr, "\n=> SAVED HUNK TO IMAGE: %s\n", image->fname);
      if (flag_pth_debug >= 5)
        pth_debug_hunk (hunk);
    }
}


/* Compute the effects of a file transition given the file change 
   described by MAP.  On exit:

    - *PREV_IMAGE_P will point to the image for the file that we just left,

    - *NEW_IMAGE_P will point to the image for the file that we just
      entered.  If *NEW_IMAGE_P is NULL, it means that we just left
      the last file in the translation unit, so there isn't anything
      else to be done.

    - *REASON_P will have the LC_* reason for the change.  */

static void
pth_get_file_transition (const struct line_map *map, pth_image **prev_image_p,
			 pth_image **new_image_p, enum lc_reason *reason_p)
{
  const char *fname;
  pth_state *state;

  /* MAP is NULL when we leave the main file in this translation unit.  */
  if (map == NULL)
    {
      fname = NULL;
      *reason_p = LC_LEAVE;
    }
  else
    {
      fname = map->to_file;
      *reason_p = map->reason;
    }

  state = pth_get_state ();
  *prev_image_p = state->cur_image;

  /* FIXME pph.  Sanitize use of PARSE_IN.  Stick it in
     pth_state together with lexer.  */
  *new_image_p = pth_image_lookup (state, fname, parse_in);
}


/* Do bookkeeping actions required when the pre-processor is leaving
   file IMAGE->FNAME.  READER is the cpp file reader object we
   are using for lexing.  */

static void
pth_leave_file (cpp_reader *reader, pth_image *image)
{
  pth_state *state;

  /* We are only interested in processing IMAGE if we have decided to
     save its image.  */
  if (!image->save_p)
    return;

  state = pth_get_state ();

  /* If the image for the file we just finished is marked as
     modified, create a new token hunk spanning from the token
     that started the image to the current end of the lexer
     buffer.  */
  pth_lexer_to_image (image, state->lexer, reader);
}


/* Do bookkeeping actions required when the pre-processor is entering
   file IMAGE->FNAME for reason REASON.  READER is the cpp file reader
   object we are using for lexing.  INCLUDE is the #include command
   used to enter IMAGE->FNAME.  It can be NULL in the case of the
   top file in the translation unit.  */

static void
pth_enter_file (cpp_reader *reader, pth_image *image, pth_include *include,
	        enum lc_reason reason)
{
  pth_state *state;

  state = pth_get_state ();

  /* Associate the current buffer with IMAGE.  */
  image->buffer = cpp_get_buffer (reader);

  /* Complete the current #include command with the directory
     and image for the file that we just switched to.  */
  if (include)
    {
      const char *dname = cpp_get_dir (cpp_get_file (image->buffer))->name;
      include->image = image;
      include->dname = (*dname) ? xstrdup (dname) : NULL;
    }

  /* If the file we are about to switch to has been loaded into an
     image, try to get as many tokens as possible from the image
     instead of the character stream.  */
  if (pth_image_can_be_used (image))
    pth_image_to_lexer (state->lexer, image, include);

  /* If IMAGE does not need to be saved, we are done.  */
  if (!image->save_p)
    return;

  /* Detect multiple inclusions of the same header file.  When a file
     is included more than once, each inclusion will usually produce
     different token hunks (e.g., <stddef.h> is typically included
     from different places with "arguments" in the form of #defines
     that determine what the caller wants stddef.h to provide.  See
     <wchar.h> for an example).

     This disrupts the validity of the image, as the hunks saved in it
     no longer correspond to a single pre-processing of the file.  We
     avoid this problem by tainting the image and forcing the file to
     be always processed from its character stream.  */
  if (reason == LC_ENTER && !VEC_empty (cp_token_hunk_ptr, image->token_hunks))
    image->save_p = false;

  /* The starting point for the next token hunk in the new
     file image will be at the current last slot in
     STATE->LEXER->BUFFER.  */
  image->hunk_start_ix = VEC_length (cp_token, state->lexer->buffer);
}


/* Callback from the pre-processor when changing in or out of a file.
   READER is the pre-processor state.  MAP is the line map for the 
   file that we are changing to.  */

static void
pth_file_change (cpp_reader *reader, const struct line_map *map)
{
  enum lc_reason reason;
  pth_state *state;
  pth_image *prev_image, *new_image;

  /* Call the previous file change handler, if it exists.  */
  state = pth_get_state ();
  if (state->file_change_prev)
    state->file_change_prev (reader, map);

  /* When processing pre-processed output, we will see names like
     '<built-in>' and '<command-line>'.  Reject those.
     ??? This rejects real path names that may start with '<', but
         those should be rare.  */
  if (map && map->to_file && map->to_file[0] == '<')
    return;

  /* Ignore LC_RENAME events.  They do not affect the actual image
     that we are processing.  */
  if (map && map->reason == LC_RENAME)
    return;

  timevar_push (TV_PTH_MANAGE);

  /* Get images for the file involved in the transition.  */
  pth_get_file_transition (map, &prev_image, &new_image, &reason);
  gcc_assert (prev_image);

  /* Ignore self-referential file change events.  These can happen
     when mixing token images with text buffers.  */
  if (prev_image == new_image)
    {
      timevar_pop (TV_PTH_MANAGE);
      return;
    }

  /* Process the file we just left (get tokens from lexer buffer,
     etc).  */
  pth_leave_file (reader, prev_image);

  /* Process the file we are about to enter (try to use its tokens if
     the file is valid, etc).  */
  if (new_image)
    {
      pth_include *include = NULL;
      
      if (reason == LC_ENTER)
	include = pth_create_include (state->new_itype,
				      state->new_angle_brackets,
				      state->new_iname);

      pth_enter_file (reader, new_image, include, reason);

      /* If we are LC_ENTERing NEW_IMAGE, it means that PREV_IMAGE has
	 #included NEW_IMAGE.  In that case, add NEW_IMAGE to the list
	 of included files by PREV_IMAGE.  */
      if (reason == LC_ENTER)
	{
	  PTH_STATS_INCR (included_files, 1);

	  if (prev_image->save_p)
	    {
	      gcc_assert (include->image == new_image);
	      VEC_safe_push (pth_include_ptr, gc, prev_image->includes,
			     include);
	      VEC_safe_push (char, gc, prev_image->ih_sequence, 'I');
	    }
	}
    }

  /* Update the current image.  */
  state->cur_image = new_image;

  timevar_pop (TV_PTH_MANAGE);
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

static void
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


/* Write PPH output symbols and IDENTS_USED to STREAM as an object.  */

static void
pph_write_file_contents (pph_stream *stream, cpp_idents_used *idents_used)
{ 
  pth_save_identifiers (idents_used, stream);
  if (flag_pph_dump_tree)
    pph_dump_namespace (pph_logfile, global_namespace);
  pph_out_tree (stream, global_namespace, false);
  pph_out_tree (stream, keyed_classes, false);
  pph_out_tree_vec (stream, unemitted_tinfo_decls, false);
}


/* Write PPH output file.  */

static void
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


/* Wrap a macro DEFINITION for printing in an error.  */

static char *
wrap_macro_def (const char *definition)
{
  char *string;
  if (definition)
    {
      size_t length;
      length = strlen (definition);
      string = (char *) xmalloc (length+3);
      string[0] = '"';
      strcpy (string + 1, definition);
      string[length + 1] = '"';
      string[length + 2] = '\0';
    }
  else
    string = xstrdup ("undefined");
  return string;
}


/* Report a macro validation error in FILENAME for macro IDENT,
   which should have the value EXPECTED but actually had the value FOUND. */

static void
report_validation_error (const char *filename,
			 const char *ident, const char *found,
			 const char *before, const char *after)
{
  char* quote_found = wrap_macro_def (found);
  char* quote_before = wrap_macro_def (before);
  char* quote_after = wrap_macro_def (after);
  error ("PPH file %s fails macro validation, "
         "%s is %s and should be %s or %s\n",
         filename, ident, quote_found, quote_before, quote_after);
  free (quote_found);
  free (quote_before);
  free (quote_after);
}



/* Add all the new names declared in NEW_NS to NS.  */

static void
pph_add_names_to_namespace (tree ns, tree new_ns)
{
  tree t, chain;
  struct cp_binding_level *level = NAMESPACE_LEVEL (new_ns);

  for (t = level->names; t; t = chain)
    {
      /* Pushing a decl into a scope clobbers its DECL_CHAIN.
	 Preserve it.  */
      chain = DECL_CHAIN (t);
      pushdecl_into_namespace (t, ns);
    }

  for (t = level->namespaces; t; t = chain)
    {
      /* Pushing a decl into a scope clobbers its DECL_CHAIN.
	 Preserve it.  */
      /* FIXME pph: we should first check to see if it isn't already there.  */
      chain = DECL_CHAIN (t);
      pushdecl_into_namespace (t, ns);
      pph_add_names_to_namespace (t, t);
    }
}


/* Read contents of PPH file in STREAM.  */

static void
pph_read_file_contents (pph_stream *stream)
{
  bool verified;
  cpp_ident_use *bad_use;
  const char *cur_def;
  cpp_idents_used idents_used;
  tree file_ns;

  pth_load_identifiers (&idents_used, stream);

  /* FIXME pph: This validation is weak.  */
  verified = cpp_lt_verify_1 (parse_in, &idents_used, &bad_use, &cur_def, true);
  if (!verified)
    report_validation_error (stream->name, bad_use->ident_str, cur_def,
                             bad_use->before_str, bad_use->after_str);

  /* Re-instantiate all the pre-processor symbols defined by STREAM.  */
  cpp_lt_replay (parse_in, &idents_used);

  /* Read global_namespace from STREAM and add all the names defined
     there to the current global_namespace.  */
  file_ns = pph_in_tree (stream);
  if (flag_pph_dump_tree)
    pph_dump_namespace (pph_logfile, file_ns);
  pph_add_names_to_namespace (global_namespace, file_ns);
  keyed_classes = pph_in_tree (stream);
  unemitted_tinfo_decls = pph_in_tree_vec (stream);
  /* FIXME pph: This call replaces the tinfo, we should merge instead.
     See pph_in_tree_VEC.  */
}


/* Read PPH file FILENAME.  */

static void
pph_read_file (const char *filename)
{
  pph_stream *stream;

  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Reading %s\n", filename);

  stream = pph_stream_open (filename, "rb");
  if (stream)
    {
      pph_read_file_contents (stream);
      pph_stream_close (stream);

      if (flag_pph_debug >= 1)
        fprintf (pph_logfile, "PPH: Closing %s\n", filename);
    }
  else
    error ("Cannot open PPH file for reading: %s: %m", filename);
}


/* Record a #include or #include_next for PTH.  */

static bool
pth_include_handler (cpp_reader *reader ATTRIBUTE_UNUSED,
	             location_t loc ATTRIBUTE_UNUSED,
	             const unsigned char *dname,
	             const char *name,
	             int angle_brackets,
	             const cpp_token **tok_p ATTRIBUTE_UNUSED)
{
  pth_state *state;

  state = pth_get_state ();

  /* Remember the attributes for this #include command.  This is
     used in pth_file_change to register a new include event for
     the parent file.  */
  if (strcmp ((const char *)dname, "include") == 0)
    state->new_itype = IT_INCLUDE;
  else if (strcmp ((const char *)dname, "include_next") == 0)
    state->new_itype = IT_INCLUDE_NEXT;
  else if (strcmp ((const char *)dname, "import") == 0)
    state->new_itype = IT_IMPORT;
  else
    gcc_unreachable ();

  state->new_angle_brackets = angle_brackets;
  state->new_iname = name;

  return true;
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


/* Initialize PTH support.  LEXER is the main lexer object used for
   pre-processing.  */

void
pth_init (cp_lexer *lexer)
{
  pth_state *state;
  cpp_callbacks *cb;
  cpp_lookaside *table;

  timevar_push (TV_PTH_INIT);

  gcc_assert (flag_pth);

  table = cpp_lt_exchange (parse_in,
                           cpp_lt_create (cpp_lt_order, flag_pth_debug));
  gcc_assert (table == NULL);

  memset (&pth_stats, 0, sizeof (pth_stats));

  state = pth_get_state ();

  /* If not using MD5 signatures, make sure that time stamps given
     by stat() are smaller than DIGEST_LEN bytes.  FIXME pph, this is
     slighly hacky.  */
  if (!flag_pth_md5)
    {
      struct stat tmp;
      gcc_assert (sizeof (tmp.st_mtime) < DIGEST_LEN);
    }

  /* Set an handler for file change events in libcpp.  */
  cb = cpp_get_callbacks (parse_in);
  state->file_change_prev = cb->file_change;
  cb->file_change = pth_file_change;
  cb->include = pth_include_handler;

  state->lexer = lexer;

  /* Make sure that we have not tried to get any tokens yet.  */
  gcc_assert (VEC_empty (cp_token, lexer->buffer));

  /* If we have a valid image for the main input file, populate as
     many tokens from its transitive closure as possible.  */
  state->cur_image = pth_image_lookup (state, main_input_filename, parse_in);
  pth_enter_file (parse_in, state->cur_image, NULL, LC_ENTER);

  timevar_pop (TV_PTH_INIT);
}


/* Show statistics on PTH on FILE.  If FILE is NULL, use pph_logfile.
   LEXER is the lexer we just filled with tokens.  This is usually
   the same as pth_get_state()->lexer, but it may be NULL if PTH is
   not enabled (in cases where we just want stats on pre-processed
   files).  */

void
pth_print_stats (FILE *file, cp_lexer *lexer)
{
  unsigned i, num_tokens, total_tokens;
  pth_state *state;
  pth_image *image;
  const char *prev_fname;
  cp_token *token;

  if (file == NULL)
    file = pph_logfile;

  fprintf (file, "\nPTH statistics\n\n");
  fprintf (file, "#included files:       %lu\n", pth_stats.included_files);
  fprintf (file, "Valid images:          %lu\n", pth_stats.valid_images);
  fprintf (file, "Token hunks:           %lu\n", pth_stats.hunks);
  fprintf (file, "Valid hunks:           %lu\n", pth_stats.valid_hunks);
  fprintf (file, "Invalid hunks:         %lu\n", pth_stats.invalid_hunks);
  fprintf (file, "Verified hunks:        %lu\n", pth_stats.verified_hunks);
  fprintf (file, "Verified identifiers:  %lu\n", pth_stats.verified_identifiers);

  state = pth_get_state ();
  fprintf (file, "\n\nPTH image statistics (%u files)\n\n",
	   VEC_length (pth_image_ptr, state->cache));
  for (i = 0; VEC_iterate (pth_image_ptr, state->cache, i, image); i++)
    pth_show_image_stats (file, image);

  fprintf (file, "\nToken counts per file in #include order:\n");
  num_tokens = total_tokens = 0;
  prev_fname = NULL;
  for (i = 0; VEC_iterate (cp_token, lexer->buffer, i, token); i++)
    {
      const char *fname = LOCATION_FILE (token->location);

      if (prev_fname == NULL)
	prev_fname = fname;

      if ((fname
	   && strcmp (fname, prev_fname) != 0)
	  || i == VEC_length (cp_token, lexer->buffer) - 1)
	{
	  fprintf (file, "tokens: %u %s\n", num_tokens, lrealpath (prev_fname));
	  prev_fname = fname;
	  total_tokens += num_tokens;
	  num_tokens = 0;
	}

      num_tokens++;
    }

  /* The main lexer buffer should have one more token: CPP_EOF.  */
  if (total_tokens != VEC_length (cp_token, lexer->buffer) - 1)
    fprintf (stderr, "*** WARNING: I counted %u, but there are %u\n",
	     total_tokens, VEC_length (cp_token, lexer->buffer));

  fprintf (file, "\n");
}


/* Save all the header images that have been marked modified from
   the incremental state.  */

void
pth_finish (void)
{
  /* If PPH is enabled, do not save PTH images to prevent analysis problems
     due to lack of location information in PTH images.  FIXME pph:
     Unneeded after we start saving proper location information.  */
  if (flag_pph_debug >= 1)
    fprintf (stderr, "*** WARNING: Not saving PTH images because PPH "
	     "is enabled\n");
  else
    {
      pth_state *state;
      pth_image *image;
      size_t i;

      state = pth_get_state ();
      for (i = 0; VEC_iterate (pth_image_ptr, state->cache, i, image); i++)
	if (image->save_p)
	  pth_save_image (image);
    }
}


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
                           cpp_lt_create (cpp_lt_order, flag_pth_debug));
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

#include "gt-cp-pph.h"
