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
#include "tree-inline.h"
#include "tree-pretty-print.h"
#include "parser.h"
#include "pph.h"
#include "pph-streamer.h"

/* Statistics collected for PTH/PPH.  */
struct pth_stats_d pth_stats;
struct pph_stats_d pph_stats;

/* Forward declarations to solve cyclical references.  */
static void pph_print_trees_tokens (VEC(tree,heap) *, cp_token *, cp_token *);

/* Map of decl dependencies.  */
static struct pph_decl_deps_d *pph_decl_deps = NULL;

/* Tree catcher for the incremental compiler.  Whenever this array is
   allocated, make_node_stat() will push certain trees into this array.  */
static VEC(tree,heap) *pph_tree_catcher = NULL;

/* Catcher for name lookups.  This stores every name lookup performed
   on identifiers while we are catching ASTs in the parser.  */
static VEC(tree,heap) *pph_name_lookups = NULL;

/* Since identifiers may be looked up more than once during parsing,
   this set prevents duplicate symbols from being placed in
   pph_name_lookups.  */
static struct pointer_set_t *pph_name_lookups_set = NULL;

/* This map stores the token locations where a given symbol was looked
   up.  When an identifier is looked up and resolved to symbol S, we
   check where the current token pointer is and save it in a vector
   associated with S.  */
static struct pointer_map_t *pph_nl_token_map = NULL;

/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
FILE *pph_logfile = NULL;

/* Cache of token ranges for head of symbol declarations.  For each
   *_DECL tree intercepted during parsing, we store the vector of
   tokens that make up the head of the declaration for the node.  */
static struct pointer_map_t *pph_decl_head_token_cache = NULL;

/* Cache of token ranges for body of symbol declarations.  For each
   *_DECL tree intercepted during parsing, we store the vector of
   tokens that make up the body of the declaration for the node.  */
static struct pointer_map_t *pph_decl_body_token_cache = NULL;


/* Return true if path P1 and path P2 point to the same file.  */

static inline bool
pathnames_equal_p (const char *p1, const char *p2)
{
  return strcmp (lrealpath (p1), lrealpath (p2)) == 0;
}

/* Expand and print location LOC to FILE.  If FILE is NULL, pph_logfile
   is used.  */

static void
pph_debug_location (FILE *file, location_t loc)
{
  expanded_location xloc;
  file = (file) ? file : pph_logfile;
  xloc = expand_location (loc);
  fprintf (file, "%s:%d:%d", lrealpath (xloc.file), xloc.line, xloc.column);
}


/* Expand and print the location of tree T to FILE.  If FILE is NULL,
   pph_logfile is used.  */

static void
pph_debug_loc_of_tree (FILE *file, tree t)
{
  pph_debug_location (file, DECL_SOURCE_LOCATION (t));
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

  pph_output_uint (f, type_idx);
  pph_output_uint (f, type_kind);

  if (type_kind == CPP_N_INTEGER)
    {
      HOST_WIDE_INT v[2];

      v[0] = TREE_INT_CST_LOW (val);
      v[1] = TREE_INT_CST_HIGH (val);
      pph_output_bytes (f, v, 2 * sizeof (HOST_WIDE_INT));
    }
  else if (type_kind == CPP_N_FLOATING)
    {
      REAL_VALUE_TYPE r = TREE_REAL_CST (val);
      pph_output_bytes (f, &r, sizeof (REAL_VALUE_TYPE));
    }
  else if (type_kind == CPP_N_FRACT)
    {
      FIXED_VALUE_TYPE fv = TREE_FIXED_CST (val);
      pph_output_bytes (f, &fv, sizeof (FIXED_VALUE_TYPE));
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
	pph_output_string_with_length (f, str, len);
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
	pph_output_string_with_length (f, str, len);
	break;

      case CPP_PRAGMA:
	/* Nothing to do.  Field pragma_kind has already been written.  */
	break;

      default:
	gcc_assert (token->u.value == NULL);
	pph_output_bytes (f, &token->u.value, sizeof (token->u.value));
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
  pph_output_bytes (f, token, sizeof (cp_token) - sizeof (void *));
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
      pph_output_uint (f, 0);
      return;
    }

  for (num = 0, tok = cache->first; tok != cache->last; tok++)
    num++;

  pph_output_uint (f, num);
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

  pph_output_bytes (stream, id, strlen (id));
  pph_output_bytes (stream, image->digest, DIGEST_LEN);
}


/* Dump a table of IDENTIFIERS to the STREAM. */

static void
pth_dump_identifiers (FILE *stream, cpp_idents_used *identifiers)
{
  unsigned int idx, col = 1;

  fprintf (stream, "%u identifiers up to %u chars\n",
           identifiers->num_entries, identifiers->max_length);
  for (idx = 0; idx < identifiers->num_entries; ++idx)
    {
      cpp_ident_use *ident = identifiers->entries + idx;

      if (col + ident->ident_len >= 80)
        {
          fprintf (stream, "\n");
          col = 1;
        }
      if (ident->before_str || ident->after_str)
        {
          if (col > 1)
            fprintf (stream, "\n");
          fprintf (stream, " %s = %s -> %s\n", ident->ident_str,
                   ident->before_str, ident->after_str);
          col = 1;
        }
      else
        {
          fprintf (stream, " %s", ident->ident_str);
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
  cp_lexer_dump_tokens (stream, hunk->buffer, 0);
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
      cp_lexer_dump_tokens (f, state->lexer->buffer, 0);
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
  unsigned int num_entries, id;

  num_entries = identifiers->num_entries;
  pph_output_uint (stream, identifiers->max_length);
  pph_output_uint (stream, num_entries);

  for ( id = 0; id < num_entries; ++id )
    {
      cpp_ident_use *entry = identifiers->entries + id;

      gcc_assert (entry->ident_len <= identifiers->max_length);
      pph_output_string_with_length (stream, entry->ident_str,
				     entry->ident_len);

      gcc_assert (entry->before_len <= identifiers->max_length);
      pph_output_string_with_length (stream, entry->before_str,
				     entry->before_len);

      gcc_assert (entry->after_len <= identifiers->max_length);
      pph_output_string_with_length (stream, entry->after_str,
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
  pph_output_uint (stream, VEC_length (cp_token, hunk->buffer));

  /* Write the tokens.  */
  for (j = 0; VEC_iterate (cp_token, hunk->buffer, j, token); j++)
    pth_save_token (token, stream);
}


/* Save the #include directive INCLUDE to STREAM.  */

static void
pth_save_include (pth_include *include, pph_stream *stream)
{
  pph_output_string (stream, include->image->fname);
  pph_output_uint (stream, (unsigned int) include->itype);
  pph_output_uint (stream, include->angle_brackets);
  pph_output_string (stream, include->iname);
  pph_output_string (stream, include->dname);
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
  pph_output_uint (stream, num);
  if (num > 0)
    pph_output_bytes (stream, VEC_address (char, image->ih_sequence), num);
  
  /* Write the number of #include commands.  */
  pph_output_uint (stream, VEC_length (pth_include_ptr, image->includes));

  /* Write all the #include commands used by IMAGE.  */
  for (i = 0; VEC_iterate (pth_include_ptr, image->includes, i, include); i++)
    pth_save_include (include, stream);

  /* Write the number of token caches in the cache.  */
  pph_output_uint (stream, VEC_length (cp_token_hunk_ptr, image->token_hunks));

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

  type_idx = pph_input_uint (f);
  type_kind = pph_input_uint (f);

  type = pth_get_type_from_index (type_idx, type_kind);

  if (type_kind == CPP_N_INTEGER)
    {
      HOST_WIDE_INT v[2];
      pph_input_bytes (f, v, 2 * sizeof (HOST_WIDE_INT));
      val = build_int_cst_wide (type, v[0], v[1]);
    }
  else if (type_kind == CPP_N_FLOATING)
    {
      REAL_VALUE_TYPE r;
      pph_input_bytes (f, &r, sizeof (REAL_VALUE_TYPE));
      val = build_real (type, r);
    }
  else if (type_kind == CPP_N_FRACT)
    {
      FIXED_VALUE_TYPE fv;
      pph_input_bytes (f, &fv, sizeof (FIXED_VALUE_TYPE));
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
	str = pph_input_string (f);
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
	str = pph_input_string (f);
	token->u.value = build_string (strlen (str), str);
	break;

      case CPP_PRAGMA:
	/* Nothing to do.  Field pragma_kind has already been loaded.  */
	break;

      default:
	pph_input_bytes (f, &token->u.value, sizeof (token->u.value));
	gcc_assert (token->u.value == NULL);
    }
}


/* Load the IDENTIFERS for a hunk from a STREAM.  */

static void
pth_load_identifiers (cpp_idents_used *identifiers, pph_stream *stream)
{
  unsigned int j;
  unsigned int max_length, num_entries;
  unsigned int ident_len, before_len, after_len;

  max_length = pph_input_uint (stream);
  identifiers->max_length = max_length;
  num_entries = pph_input_uint (stream);
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
      const char *s = pph_input_string (stream);
      gcc_assert (s);
      ident_len = strlen (s);
      identifiers->entries[j].ident_len = ident_len;
      identifiers->entries[j].ident_str =
        (const char *) obstack_copy0 (identifiers->strings, s, ident_len);

      s = pph_input_string (stream);
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

      s = pph_input_string (stream);
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
  num_tokens = pph_input_uint (stream);

  /* Read the tokens in the HUNK. */
  hunk->buffer = VEC_alloc (cp_token, gc, num_tokens);
  for (j = 0; j < num_tokens; j++)
    {
      cp_token *token = VEC_quick_push (cp_token, hunk->buffer, NULL);

      /* Do not read the whole structure, the token value has
         dynamic size as it contains swizzled pointers.
         FIXME pph, restructure to allow bulk reads of the whole
         section.  */
      pph_input_bytes (stream, token, sizeof (cp_token) - sizeof (void *));

      /* FIXME pph.  Use an arbitrary (but valid) location to avoid
         confusing the rest of the compiler for now.  */
      token->location = input_location;

      /* FIXME pph: verify that pth_load_token_value works with no tokens.  */
      pth_load_token_value (token, stream);
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

  s = pph_input_string (stream);
  include->image = pth_image_lookup (state, s, reader);

  tmp = pph_input_uint (stream);
  include->itype = (enum include_type) tmp;

  tmp = pph_input_uint (stream);
  include->angle_brackets = (tmp != 0);

  include->iname = pph_input_string (stream);
  include->dname = pph_input_string (stream);

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
  pph_input_bytes (stream, buf, pth_header_len ());
  free (buf);

  /* Read the include-hunk (IH) sequencing vector.  */
  num = pph_input_uint (stream);
  if (num > 0)
    {
      image->ih_sequence = VEC_alloc (char, gc, num);
      VEC_safe_grow (char, gc, image->ih_sequence, num);
      pph_input_bytes (stream, VEC_address (char, image->ih_sequence), num);
    }

  /* Read the number path names of all the files #included by
     IMAGE->FNAME.  */
  num = pph_input_uint (stream);
  image->includes = VEC_alloc (pth_include_ptr, gc, num);

  /* Now read all the path names #included by IMAGE->FNAME.  */
  for (i = 0; i < num; i++)
    pth_load_include (state, image, reader, stream);

  /* Read how many token hunks are contained in this image.  */
  num = pph_input_uint (stream);
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
  pph_input_bytes (f, id, strlen (good_id));
  if (strcmp (id, good_id) != 0)
    goto invalid_img;

  /* Now check if the MD5 digest stored in the image file matches the
     digest for FNAME.  */
  pph_input_bytes (f, saved_digest, DIGEST_LEN);
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


/* Write PPH output symbols and IDENTS_USED to STREAM as an object.  */

static void
pph_write_file_contents (pph_stream *stream, cpp_idents_used *idents_used)
{ 
  pth_save_identifiers (idents_used, stream);
  pph_output_tree (stream, global_namespace);
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


/* Read contents of PPH file in STREAM.  */

static void
pph_read_file_contents (pph_stream *stream)
{
  bool verified;
  cpp_ident_use *bad_use;
  const char *cur_def;
  cpp_idents_used idents_used;

  pth_load_identifiers (&idents_used, stream);

  /*FIXME pph: This validation is weak.  */
  verified = cpp_lt_verify_1 (parse_in, &idents_used, &bad_use, &cur_def, true);
  if (!verified)
    report_validation_error (stream->name, bad_use->ident_str, cur_def,
                             bad_use->before_str, bad_use->after_str);

  /* FIXME pph: We cannot replay the macro definitions
     as long as we are still reading the actual file.
  cpp_lt_replay (parse_in, &idents_used);
  */

  /* FIXME pph: Also read decls.  */
}


/* Read PPH file.  */

static void
pph_read_file (const char *filename)
{
  pph_stream *stream;

  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Reading %s\n", filename);

  stream = pph_stream_open (filename, "rb");
  if (!stream)
    fatal_error ("Cannot open PPH file for reading: %s: %m", filename);

  pph_read_file_contents (stream);

  pph_stream_close (stream);
}

/* Record a #include or #include_next for PTH.  */

static void
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
}

/* Record a #include or #include_next for PPH.  */

static void
pph_include_handler (cpp_reader *reader,
                     location_t loc ATTRIBUTE_UNUSED,
                     const unsigned char *dname,
                     const char *name,
                     int angle_brackets,
                     const cpp_token **tok_p ATTRIBUTE_UNUSED)
{
  const char *pph_file;

  if (flag_pph_debug >= 1)
    {
      fprintf (pph_logfile, "PPH: #%s", dname);
      fprintf (pph_logfile, " %c", angle_brackets ? '<' : '"');
      fprintf (pph_logfile, "%s", name);
      fprintf (pph_logfile, "%c\n", angle_brackets ? '>' : '"');
    }

  pph_file = query_pph_include_map (name);
  if (pph_file != NULL && !cpp_included_before (reader, name, input_location))
    pph_read_file (pph_file);
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
    {
      if (flag_pph_debug > 1)
	fprintf (stderr, "*** WARNING: Not saving PTH images because PPH "
		 "is enabled\n");
    }
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


static void
pph_log_exposed (cp_parser *parser, const char *end)
{
  if (flag_pph_debug >= 2)
    {
      cp_token *pos = cp_lexer_token_position (parser->lexer, false);
      fprintf (pph_logfile, "PPH: %s exposed declaration at ", end);
      pph_debug_location (pph_logfile, pos->location);
      fprintf (pph_logfile, "\n");
    }
}


/* Allocate the various arrays, maps and sets used to collect ASTs and
   their dependencies during parsing.  This memory is allocated and
   freed for every grammar rule intercepted by pph_start_exposed() and
   pph_stop_exposed().  */

static void
pph_allocate_catcher_memory (void)
{
  /* Note.  pph_tree_catcher *must* be instantiated to indicate that
     we are going to be catching trees during parsing.  */
  pph_tree_catcher = VEC_alloc (tree, heap, 5);
  pph_decl_head_token_cache = pointer_map_create ();
  pph_decl_body_token_cache = pointer_map_create ();
  pph_decl_deps = XCNEW (struct pph_decl_deps_d);
  pph_decl_deps->header = pointer_map_create ();
  pph_decl_deps->body = pointer_map_create ();
  pph_name_lookups = NULL;
  pph_name_lookups_set = pointer_set_create ();
  pph_nl_token_map = pointer_map_create ();
}


/* Free all the memory allocated by pph_allocate_catcher_memory.  */

static void
pph_free_catcher_memory (void)
{
  VEC_free (tree, heap, pph_tree_catcher);

  pointer_map_destroy (pph_decl_head_token_cache);
  pph_decl_head_token_cache = NULL;

  pointer_map_destroy (pph_decl_body_token_cache);
  pph_decl_body_token_cache = NULL;

  pointer_map_destroy (pph_decl_deps->header);
  pointer_map_destroy (pph_decl_deps->body);
  free (pph_decl_deps);
  pph_decl_deps = NULL;

  VEC_free (tree, heap, pph_name_lookups);

  pointer_set_destroy (pph_name_lookups_set);
  pph_name_lookups_set = NULL;

  pointer_map_destroy (pph_nl_token_map);
  pph_nl_token_map = NULL;
}


/* Start collecting ASTs and dependencies.  */

cp_token *
pph_start_exposed (cp_parser *parser)
{
  if (flag_pph_debug >= 2)
    {
      timevar_push (TV_PPH_MANAGE);

      if (flag_pph_debug >= 4)
        fprintf (pph_logfile, "\n--------------------------------------------------------------------------\n");
      pph_log_exposed (parser, "start");
      pph_allocate_catcher_memory ();
      timevar_pop (TV_PPH_MANAGE);

      return parser->lexer->next_token; /* the first token */
    }
  else
    return NULL;
}

/* Return the token cache associated with tree node T.  */

static VEC(cp_token, heap) *
pph_lookup_head_token_cache_for (tree t)
{
  void **slot = pointer_map_contains (pph_decl_head_token_cache, t);
  if (slot)
    return ((VEC(cp_token, heap) *) *slot);

  return NULL;
}

static VEC(cp_token, heap) *
pph_lookup_body_token_cache_for (tree t)
{
  void **slot = pointer_map_contains (pph_decl_body_token_cache, t);
  if (slot)
    return ((VEC(cp_token, heap) *) *slot);

  return NULL;
}


/* Set the token cache associated with tree node T to CACHE.  */

static void
pph_set_head_token_cache_for (tree t, VEC(cp_token, heap) *cache)
{
  void **slot = pointer_map_insert (pph_decl_head_token_cache, t);
  *slot = (void *) cache;
}

static void
pph_set_body_token_cache_for (tree t, VEC(cp_token, heap) *cache)
{
  void **slot = pointer_map_insert (pph_decl_body_token_cache, t);
  *slot = (void *) cache;
}

/* Emulate the copying of declarations into the parser cache.  Deep
   copy all the declarations in V.  */

static void
pph_copy_decls_into_cache (VEC(tree, heap) *v)
{
  unsigned i;
  static tree t_copy, type_copy, t;
  void **slot;
  VEC(tree, heap) *ast_cache, *old_pph_tree_catcher, *old_pph_name_lookups;
  static struct pointer_map_t *hunk_to_decls_map = NULL;
  static htab_t cache_dir = NULL;
  const char *fname;

  timevar_push (TV_PPH_CACHE_IN);

  if (hunk_to_decls_map == NULL)
    hunk_to_decls_map = pointer_map_create ();

  if (cache_dir == NULL)
    cache_dir = htab_create (10, htab_hash_pointer, htab_eq_pointer, 0);

  /* We will be copying trees, which will call into the tree catching
     routines.  Prevent that.  */
  old_pph_tree_catcher = pph_tree_catcher;
  old_pph_name_lookups = pph_name_lookups;
  pph_tree_catcher = pph_name_lookups = NULL;

  /* Copy every declaration in V into the cache.  */
  for (i = 0; VEC_iterate (tree, v, i, t); i++)
    {
      /* 1- Determine the token hunk H that owns T.  We first determine
	 the PTH image and then which hunk inside that PTH image.  For
	 choosing the hunk within the image, we simulate a second
	 hash table lookup hashing the location to the token hunk.  */
      pth_image *image, *hunk;
      VEC(cp_token, heap) *head_tokens, *body_tokens;
      cp_token *first;

      head_tokens = pph_lookup_head_token_cache_for (t);
      body_tokens = pph_lookup_body_token_cache_for (t);
      if (VEC_empty (cp_token, head_tokens))
	continue;
      first = VEC_index (cp_token, head_tokens, 0);
      fname = LOCATION_FILE (first->location);
      slot = htab_find_slot (cache_dir, fname, INSERT);
      if (*slot == NULL)
	{
	  image = XCNEW (pth_image);
	  *slot = CONST_CAST (char *, fname);
	}
      else
	image = *((pth_image **) slot);

      /* For now, re-do the pth_image_lookup to simulate the lookup of
	 the hunk within the image.  */
      slot = htab_find_slot (cache_dir, fname, NO_INSERT);
      hunk = *((pth_image **) slot);

      /* Insert dummy uses for head_tokens, body_tokens and image.  */
      if (i > VEC_length (tree, v))
	{
	  free (head_tokens);
	  free (body_tokens);
	  free (image);
	}

      /* 2- Find the tree cache associated with HUNK.  */
      slot = pointer_map_insert (hunk_to_decls_map, hunk);
      ast_cache = (VEC(tree, heap) *) *slot;

      /* 3- Copy T and its type into the cache associated with HUNK.
	 If T has a body (a FUNCTION_DECL), copy the body.  FIXME pph,
	 copying ASTs will need new copying code to be implemented,
	 the current routines do not handle everything that can be
	 generated by the C++ FE.  */
      t_copy = copy_decl (t);
      if (!type_copy || TREE_TYPE (t))
	type_copy = copy_type (TREE_TYPE (t));
      if (TREE_CODE (t) == FUNCTION_DECL)
	walk_tree (&DECL_SAVED_TREE (t_copy), copy_tree_r, (void *)1, NULL);
      VEC_safe_push (tree, heap, ast_cache, t_copy);
      *slot = (void *) ast_cache;
    }

  PPH_STATS_INCR (cached_decls, VEC_length (tree, v));

  /* Restore tree and lookup catchers.  */
  pph_tree_catcher = old_pph_tree_catcher;
  pph_name_lookups = old_pph_name_lookups;

  timevar_pop (TV_PPH_CACHE_IN);
}


/* Emulate the cache actions needed to get a declaration out of the
   parser cache and instantiate it into the current compilation context.  */

static void
pph_copy_decls_outof_cache (VEC(tree, heap) *v)
{
  unsigned i;
  tree t;
  static VEC(tree, heap) *compilation_context = NULL;
  VEC(tree, heap) *old_pph_tree_catcher, *old_pph_name_lookups;

  /* Conceptually, this will be called with a token hunk that contains
     all the declarations that we want to instantiate.  */
  timevar_push (TV_PPH_CACHE_OUT);

  /* We will be copying trees, which will call into the tree catching
     routines.  Prevent that.  */
  old_pph_tree_catcher = pph_tree_catcher;
  old_pph_name_lookups = pph_name_lookups;
  pph_tree_catcher = pph_name_lookups = NULL;

  /* 1- Verify that the hunk is valid.  Traverse all the declarations
     checking that none have been tainted.  */
  for (i = 0; VEC_iterate (tree, v, i, t); i++)
    {
      /* If T is not valid, none of its users is valid.  */
      if (1 || TREE_VISITED (t))
	{
	  unsigned j;
	  tree r;
	  for (j = 0; VEC_iterate (tree, v, j, r); j++)
	    TREE_VISITED (r) = TREE_VISITED (r);
	}
    }

  /* 2- Copy all the trees in the hunk to the current compilation context.  */
  for (i = 0; VEC_iterate (tree, v, i, t); i++)
    {
      static tree t_copy, type_copy;
      
      t_copy = copy_decl (t);
      if (!type_copy || TREE_TYPE (t))
	type_copy = copy_type (TREE_TYPE (t));
      if (TREE_CODE (t) == FUNCTION_DECL)
	walk_tree (&DECL_SAVED_TREE (t_copy), copy_tree_r, (void *)1, NULL);

      /* Emulate restoration into compilation context.  FIXME pph, this is
	 missing the name lookups that may be required.  Estimate this
	 separately from the number of name lookup operations and the
	 time spent doing name lookups.  */
      VEC_safe_push (tree, heap, compilation_context, t_copy);
    }

  PPH_STATS_INCR (restored_decls, VEC_length (tree, v));

  /* Restore tree and lookup catchers.  */
  pph_tree_catcher = old_pph_tree_catcher;
  pph_name_lookups = old_pph_name_lookups;

  timevar_pop (TV_PPH_CACHE_OUT);
}

void
pph_stop_exposed (cp_parser *parser, cp_token *first_token)
{
  if (flag_pph_debug >= 2 && !VEC_empty (tree, pph_tree_catcher))
    {
      cp_token *last_token;

      timevar_push (TV_PPH_MANAGE);

      last_token = parser->lexer->next_token;
      pph_print_trees_tokens (pph_tree_catcher, first_token, last_token);
      pph_copy_decls_into_cache (pph_tree_catcher);
      pph_copy_decls_outof_cache (pph_tree_catcher);
      PPH_STATS_INCR (cached_refs, VEC_length(tree, pph_name_lookups));
      pph_free_catcher_memory ();
      if (flag_pph_debug >= 4)
        pph_log_exposed (parser, "stop");

      timevar_pop (TV_PPH_MANAGE);
    }
}


/* PPH printing help.  */

static void
pph_debug_tree (tree t, bool body)
{
  if (t == NULL)
    {
      fprintf (pph_logfile, "nulldecl");
      return;
    }

  if (!DECL_P (t))
    {
      fprintf (pph_logfile, "__%s__", tree_code_name[TREE_CODE (t)]);
      return;
    }

  if (flag_pph_decls >= 2)
    fprintf (pph_logfile, "%s ", tree_code_name[TREE_CODE (t)]);
  fprintf (pph_logfile, "%d", (DECL_UID (t) << 1) + body);
  if (flag_pph_decls >= 1)
    fprintf (pph_logfile, " '%s'", get_name (t));
  if (flag_pph_decls >= 3)
    {
      fprintf (pph_logfile, " ");
      pph_debug_loc_of_tree (pph_logfile, t);
      if (flag_pph_decls >= 4)
        fprintf (pph_logfile, " @%p", (void *) t);
    }
}

static void
pph_debug_type (tree t, bool body)
{
  tree t_decl;
  if (t == NULL)
    {
      fprintf (pph_logfile, "nulltype");
      return;
    }
  t_decl = get_type_decl (t);
  if (t_decl == NULL)
    fprintf (pph_logfile, "nameless");
  else
    pph_debug_tree (t_decl, body);
}

/* Return true if tree T has been caught already.  */

static bool
pph_tree_caught_p (tree t)
{
  return (pph_lookup_head_token_cache_for (t) != NULL);
}

/* Collect the tokens needed for the head of DECL.
   This assumes that the current token is positioned right after
   the end of the declarator expression for DECL (i.e., it should
   be called from grokdeclarator).  */

static VEC(cp_token, heap) *
pph_catch_head_tokens_for (tree t)
{
  cp_token *tok, *last, *first;
  cp_lexer *lexer;
  VEC(cp_token, heap) *tokens;

  gcc_assert (t != error_mark_node);

  tokens = pph_lookup_head_token_cache_for (t);
  if (tokens)
    {
      fprintf (stderr, "*** ");
      pph_debug_location (stderr, input_location);
      fprintf (stderr, ": Tried to catch head tokens more than once for: ");
      print_generic_stmt (stderr, t, 0);
      gcc_unreachable ();
    }

  lexer = the_parser->lexer;

  /* Look for the tokens backwards until the first brace or semicolon.  */
  first = last = lexer->next_token;
  for (tok = last - 1; tok >= VEC_address (cp_token, lexer->buffer); tok--)
    {
      if (tok->type == CPP_OPEN_BRACE
	  || tok->type == CPP_CLOSE_BRACE
	  || tok->type == CPP_SEMICOLON)
	break;

      first = tok;
    }

  /* Now include any trailing semicolon.  */
  if (last->type == CPP_SEMICOLON)
    last++;

  /* Add all the tokens in [FIRST, LAST) to TOKENS.  */
  for (tok = first; tok != last; tok++)
    VEC_safe_push (cp_token, heap, tokens, tok);

  pph_set_head_token_cache_for (t, tokens);

  return tokens;
}


/* Collect the tokens needed for the body of DECL.
   This assumes that the current token is positioned right after
   the end of the declarator expression for DECL (i.e., it should
   be called from grokdeclarator).  */

static VEC(cp_token, heap) *
pph_catch_body_tokens_for (tree t)
{
  cp_token *tok, *last, *first;
  cp_lexer *lexer;
  VEC(cp_token, heap) *tokens;

  gcc_assert (t != error_mark_node);

  tokens = pph_lookup_body_token_cache_for (t);
  if (tokens)
    {
      fprintf (stderr, "*** ");
      pph_debug_location (stderr, input_location);
      fprintf (stderr, ": Tried to catch body tokens more than once for: ");
      print_generic_stmt (stderr, t, 0);
      gcc_unreachable ();
    }

  lexer = the_parser->lexer;

  /* Look for the tokens forwards until the closing brace or semicolon.  */
  first = last = lexer->next_token;
  tok = first;
  if (tok->type == CPP_EQ || tok->type == CPP_OPEN_PAREN)
    {
      /* Skip a variable-like definition.  Find the semicolon.  */
      /* FIXME pph - This code changes with C++0x.  */
      for (; tok <= VEC_last (cp_token, lexer->buffer); tok++)
        if (tok->type == CPP_SEMICOLON)
          break;
      last = tok;
    }
  else if (tok->type == CPP_OPEN_BRACE || tok->type == CPP_COLON)
    {
      /* Skip a class-like or function-like definition.
         Skip to a left brace, then skip to the matching right brace.  */
      /* FIXME pph - This code changes with C++0x.  */
      int nesting = 0;
      for (; tok <= VEC_last (cp_token, lexer->buffer); tok++)
        if (tok->type == CPP_OPEN_BRACE)
          nesting++;
        else if (tok->type == CPP_CLOSE_BRACE)
          {
            if ( nesting <= 1)
              break;
            else
              nesting--;
          }
      last = tok;
    }
  else
    return NULL; /* no body */

  /* Add all the tokens in [FIRST, LAST) to TOKENS.  */
  for (tok = first; tok <= last; tok++)
    VEC_safe_push (cp_token, heap, tokens, tok);

  pph_set_body_token_cache_for (t, tokens);

  return tokens;
}


/* Return the dependencies for tree node T.  If HEADER_P is true, it
   returns the dependencies for the header of T's declaration.
   Otherwise, it returns dependencies for T's body.  */

static VEC(tree,gc) *
pph_lookup_dependencies_for (tree t, bool header_p)
{
  struct pointer_map_t *map;
  void **slot;

  map = (header_p) ? pph_decl_deps->header : pph_decl_deps->body;
  slot = pointer_map_contains (map, t);
  if (slot)
    return ((VEC(tree,gc) *) *slot);

  return NULL;
}


/* Set the dependencies for tree node T to DEPS.  If HEADER_P is true,
   DEPS are the dependencies for T's header.  Otherwise, DEPS are the
   dependencies for T's body.  */

static void
pph_set_dependencies_for (tree t, VEC(tree,gc) *deps, bool header_p)
{
  void **slot;
  struct pointer_map_t *map;

  map = (header_p) ? pph_decl_deps->header : pph_decl_deps->body;
  slot = pointer_map_insert (map, t);
  *slot = (void *) deps;
}

#define PPH_ARTIFICIAL(t) \
(DECL_ARTIFICIAL (t) && !DECL_IMPLICIT_TYPEDEF_P (t))

static bool
is_namespace (tree container)
{
  enum tree_code code;

  if (container == NULL)
    return true;

  code = TREE_CODE (container);
  if (code == NAMESPACE_DECL)
    return true;

  return false;
}

/* Find the exposed declaration containing a symbol lookup.  */

static tree
pph_null_exposed (const char *reason)
{
  if (flag_pph_debug >= 3)
    fprintf (pph_logfile, "%s\n", reason);
  return NULL;
}

static tree
pph_live_exposed (tree t, bool body)
{
  if (PPH_ARTIFICIAL (t))
    return pph_null_exposed ("Artificial symbols are not exposed.");

  if (flag_pph_debug >= 3)
    {
      if (t == NULL)
        fprintf (pph_logfile, "(null)");
      else
        pph_debug_tree (t, body);
      fprintf (pph_logfile, "\n");
    }
  return t;
}

static tree
pph_find_exposed_for (tree t, bool *body)
{
  tree container;
  enum tree_code code, t_code;
  bool in_class;
  *body = false; /* By default, we only depend on heads. */
  for ( ; ; t = container, *body = true)
    {
      reclassify:
      if (t == NULL)
        {
          PPH_STATS_INCR (bad_lookups, 1);
          return pph_null_exposed ("NULLOID"); /* FIXME pph */
        }
      if (flag_pph_debug >= 3)
        {
          fprintf (pph_logfile, "      exposed for ");
          pph_debug_tree (t, false);
          fprintf (pph_logfile, " is ");
        }
      code = TREE_CODE (t);
      switch (code)
        {
	  /* Types common to C and C++.  */
	  case ARRAY_TYPE:
	  case BOOLEAN_TYPE:
	  case COMPLEX_TYPE:
	  case ENUMERAL_TYPE:
	  case FIXED_POINT_TYPE:
	  case FUNCTION_TYPE:
	  case INTEGER_TYPE:
	  case LANG_TYPE:
	  case METHOD_TYPE:
	  case OFFSET_TYPE:
	  case POINTER_TYPE:
	  case QUAL_UNION_TYPE:
	  case REAL_TYPE:
	  case RECORD_TYPE:
	  case REFERENCE_TYPE:
	  case UNION_TYPE:
	  case VECTOR_TYPE:
	  case VOID_TYPE:
	  /* C++-specific types.  */
	  case BOUND_TEMPLATE_TEMPLATE_PARM:
	  case TEMPLATE_TEMPLATE_PARM:
	  case TEMPLATE_TYPE_PARM:
	  case TYPENAME_TYPE:
	  case TYPEOF_TYPE:
	  case UNBOUND_CLASS_TEMPLATE:
	  case TYPE_ARGUMENT_PACK:
	  case TYPE_PACK_EXPANSION:
	  case DECLTYPE_TYPE:
            {
              if (TYPE_NAME (t) == NULL)
                return pph_null_exposed ("Anonymous Type");
              else
                t = TYPE_NAME (t);
              /* FALLTHRU */
            }

          case TYPE_DECL:
            {
              container = DECL_CONTEXT (t);
              in_class = container && CLASS_TYPE_P (container);
              t_code = TREE_CODE (TREE_TYPE (t));
              /* FIXME pph: Why DECL_TEMPLATE_TEMPLATE_PARM_P does not apply
                 here?  It is a template template parameter, but the tree code
                 is inconsistent.  */
              if (DECL_TEMPLATE_PARM_P (t)
                  || t_code == TEMPLATE_TEMPLATE_PARM
                  || t_code == BOUND_TEMPLATE_TEMPLATE_PARM)
                return pph_null_exposed ("TMPLPARM");

              if (is_namespace (container))
                return pph_live_exposed (t, *body);
              break;
            }

          case VAR_DECL:
            {
              /* If var is lazy, depend on its body, not its head.  */
              tree enclass = DECL_CONTEXT (t);
              bool in_class = enclass && CLASS_TYPE_P (enclass);
              bool defined = DECL_INITIAL (t) != NULL;
                             /* FIXME pph: DECL_INITIALIZED_P (t)  */
              if (defined && (in_class || !DECL_THIS_EXTERN (t))
                  && decl_constant_var_p (t))
                *body = true;

              container = DECL_CONTEXT (t);
              in_class = container && CLASS_TYPE_P (container);
              if (in_class && DECL_THIS_STATIC (t))
                container = TYPE_CONTEXT (container);
              if (is_namespace (container))
                return pph_live_exposed (t, *body);
	      break;
            }

          case FUNCTION_DECL:
            {
              /* If function is lazy, depend on body.  */
              bool defined = DECL_INITIAL (t) != NULL;
              if (defined && (DECL_DECLARED_INLINE_P (t)
                              || DECL_USE_TEMPLATE (t) != 2))
                *body = true;

              container = DECL_CONTEXT (t);
              in_class = container && CLASS_TYPE_P (container);
              if (in_class)
                container = TYPE_CONTEXT (container);
              if (is_namespace (container))
                return pph_live_exposed (t, *body);;
              break;
            }

          case TEMPLATE_DECL:
            {
              int generic = DECL_USE_TEMPLATE (t);
              if (generic != 2)
                {
                  t = DECL_TEMPLATE_RESULT (t);
                  if (flag_pph_debug >= 3)
                    fprintf (pph_logfile, "template redirected\n");
                  goto reclassify;
                }
            }

          case SCOPE_REF:
            return pph_null_exposed ("SCOPE_REF"); /* FIXME pph */

          case OVERLOAD:
            return pph_null_exposed ("OVERLOAD"); /* FIXME pph */

	  case BASELINK:
	    container = BASELINK_BINFO (t);
	    break;

	  case TREE_BINFO:
	    container = BINFO_TYPE (t);
	    break;

	  case TREE_LIST:
	    t = TREE_VALUE (t);
	    /* Fallthru  */

          default:
            {
              if (t == NULL)
                {
                  PPH_STATS_INCR (bad_lookups, 1);
                  return pph_null_exposed ("NULLOID"); /* FIXME pph */
                }
	      else if (!DECL_P (t))
                {
                  PPH_STATS_INCR (bad_lookups, 1);
                  return pph_null_exposed ("BOZOID"); /* FIXME pph */
                  /*FIXME pph:
		  fatal_error ("Expecting a *_decl node.  Got %s",
			       tree_code_name[TREE_CODE (t)]);
                  */
                }

              container = DECL_CONTEXT (t);
              in_class = container && CLASS_TYPE_P (container);
              if (is_namespace (container))
                return pph_null_exposed ("UNKNOWN");
              break;
            }
        }
      if (flag_pph_debug >= 3)
        {
          pph_debug_tree (container, *body);
          fprintf (pph_logfile, "\n");
        }
    }
}


/* Collect the AST nodes that node T depends on.  HEADER_P is true if
   we should collect ASTs from T's header.  Otherwise, we collect
   ASTs from T's body.  */

static VEC(tree,gc) *
pph_catch_dependencies_for (tree t, bool header_p)
{
  VEC(cp_token, heap) *tokens;
  unsigned i;
  cp_token *tok;
  VEC(tree,gc) *deps;

  tokens = (header_p)
	   ? pph_lookup_head_token_cache_for (t)
	   : pph_lookup_body_token_cache_for (t);

  if (tokens == NULL)
    tokens = (header_p)
	     ? pph_catch_head_tokens_for (t)
	     : pph_catch_body_tokens_for (t);

  deps = pph_lookup_dependencies_for (t, header_p);

  for (i = 0; VEC_iterate (cp_token, tokens, i, tok); i++)
    if (tok->type == CPP_NAME)
      VEC_safe_push (tree, gc, deps, tok->u.value);

  pph_set_dependencies_for (t, deps, header_p);

  return deps;
}


/* Intercept tree node T by storing it in pph_tree_catcher and collecting
   the tokens used in its instantiation.  */

void
pph_catch_tree (tree t)
{
  /* Only collect trees if the parser instantiated pph_tree_catcher
     and we are currently parsing from the main lexer.  */
  if (pph_tree_catcher && the_parser->lexer->buffer && !pph_tree_caught_p (t))
    {
      timevar_push (TV_PPH_MANAGE);

      VEC_safe_push (tree, heap, pph_tree_catcher, t);
      pph_catch_head_tokens_for (t);
      pph_catch_body_tokens_for (t);
      pph_catch_dependencies_for (t, true);
      pph_catch_dependencies_for (t, false);

      timevar_pop (TV_PPH_MANAGE);
    }
}


/* Retract a caught tree.  */

void
pph_uncatch_tree (tree t)
{
  /* Only uncollect trees if the parser instantiated pph_tree_catcher
     and we are currently parsing from the main lexer.  */
  if (pph_tree_catcher && the_parser->lexer->buffer)
    {
      int i;
      tree u;

      timevar_push (TV_PPH_MANAGE);

      /* Find the index; if present, remove it.  */
      for (i = 0; VEC_iterate (tree, pph_tree_catcher, i, u); i++)
        {
          if (u == t)
            {
              VEC_ordered_remove (tree, pph_tree_catcher, i);
              break;
            }
        }

      timevar_pop (TV_PPH_MANAGE);
    }
}


/* Given a set of tokens TOKENS, return the symbols from pph_name_lookups
   that occur in TOKENS.  The returned vector is, then, the set of all
   symbols that were resolved via name lookups during parsing.  This set
   is a subset of all the CPP_NAME tokens in TOKENS.  */

static void
pph_locate_name_lookups_in (VEC(cp_token, heap) *tokens,
                            VEC(tree,gc) **heads_found,
                            VEC(tree,gc) **bodies_found)
{
  unsigned i;
  tree t;
  cp_token *first, *last;

  *heads_found = NULL;
  *bodies_found = NULL;

  if (tokens == NULL || VEC_empty (cp_token, tokens))
      return;

  first = VEC_index (cp_token, tokens, 0);
  last = VEC_last (cp_token, tokens);

  for (i = 0; VEC_iterate (tree, pph_name_lookups, i, t); i++)
    {
      unsigned j;
      cp_token *tok;
      VEC(cp_token, heap) *lookup_locations;
      void **slot;
      bool pushed = false;

      slot = pointer_map_contains (pph_nl_token_map, t);
      gcc_assert (slot && *slot);
      lookup_locations = (VEC(cp_token, heap) *) *slot;
      for (j = 0; VEC_iterate (cp_token, lookup_locations, j, tok); j++)
	{
	  if (tok->location >= first->location
	      && tok->location <= last->location)
	    {
              if (!pushed)
                {
                  bool body;
                  tree exposed = pph_find_exposed_for (t, &body);
                  if (exposed)
                    {
                      if (body)
                        VEC_safe_push (tree, gc, *bodies_found, exposed);
                      else
                        VEC_safe_push (tree, gc, *heads_found, exposed);
                      pushed = true;
                    }
                }

              /* Avoid double-counting lookups by removing the lookup
                 location after a class member declaration has found it.
                 To make that work, we must remove all redundant entries.  */
              if (flag_pph_debug >= 4)
                {
                  fprintf (pph_logfile, "      lookup in %p for ",
                           (void*)lookup_locations);
                  pph_debug_tree (t, false);
                  fprintf (pph_logfile, " found at ");
                  pph_debug_location (pph_logfile, tok->location);
                  fprintf (pph_logfile, "\n");
                  fprintf (pph_logfile, "        vector length from %d",
                           VEC_length (cp_token, lookup_locations));
                  
                }
              /* This code is slow, but VEC won't let me null entires.  */
              VEC_ordered_remove (cp_token, lookup_locations, j);
              /* We have just shifted down all later entries,
                 and need to counteract the upcoming index increment.  */
              j--;
              if (flag_pph_debug >= 4)
                {
                  fprintf (pph_logfile, " to %d\n",
                           VEC_length (cp_token, lookup_locations));
                }
	    }
	}
    }
}


/* Print all the trees in V and the tokens in the token range
   [TOK1, TOK2).  */

static VEC(cp_token, heap) *
pph_print_copy_tokens (cp_token *tok1, cp_token *tok2)
{
  cp_token *tok;
  VEC(cp_token, heap) *vtok;

  /* If TOK2 is CPP_EOF, it will have the address of eof_token, which
     will make the loop below go off the deep end.  Detect this and
     make TOK2 the last token in the lexer buffer instead.  */
  if (tok2 == &eof_token)
    tok2 = VEC_last (cp_token, the_parser->lexer->buffer);

  vtok = NULL;
  for (tok = tok1; tok != tok2; tok++)
    VEC_safe_push (cp_token, heap, vtok, tok);

  return vtok;
}

static void
pph_print_token_range (VEC(tree,heap) *v, VEC(cp_token, heap) *vtok)
{
  unsigned i;
  tree t;

  if (flag_pph_debug >= 4)
    {
      fprintf (pph_logfile, "PPH: hunk location ");
      pph_debug_location (pph_logfile, VEC_index (cp_token, vtok, 0)->location);
      fprintf (pph_logfile, " to ");
      pph_debug_location (pph_logfile, VEC_last (cp_token, vtok)->location);
      fprintf (pph_logfile, "\n");
      fprintf (pph_logfile, "PPH: hunk tokens ");
      cp_lexer_dump_tokens (stderr, (VEC(cp_token, gc) *)vtok, 0);
      fprintf (pph_logfile, "PPH: hunk ASTs:\n");
      for (i = 0; VEC_iterate (tree, v, i, t); i++)
        {
          pph_debug_tree (t, true);
          /* FIXME pph: this may not be right; we may not care.  */
          print_generic_stmt (stderr, t, 0);
        }
      fprintf (pph_logfile, "PPH: hunk decls:\n");
    }
}

static void
pph_print_dependence (bool user_body, bool used_body,
                      tree t, tree d)
{
  static bool prior_user_body = false;
  static bool prior_used_body = false;
  static tree prior_t = NULL;
  static tree prior_d = NULL;
  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    pd_base ");
      pph_debug_tree (t, user_body);
      fprintf (pph_logfile, " on ");
      pph_debug_tree (d, used_body);
      fprintf (pph_logfile, "\n");
    }
  if (t && d && DECL_P (t) && DECL_P (d) && (t != d || user_body != used_body))
    {
      if (PPH_ARTIFICIAL (t))
        {
          /* Okay, find the real symbol this articial one belongs to.  */
          d = pph_find_exposed_for (d, &used_body);
          if (d == NULL)
            return;
          used_body = true;
        }
      if (user_body != prior_user_body
          || used_body != prior_used_body
          || t != prior_t
          || d != prior_d)
        {
          fprintf (pph_logfile, "depend ");
          pph_debug_tree (t, user_body);
          fprintf (pph_logfile, " uses ");
          pph_debug_tree (d, used_body);
          fprintf (pph_logfile, "\n");
          prior_user_body = user_body;
          prior_used_body = used_body;
          prior_t = t;
          prior_d = d;
        }
    }
}

static void
pph_print_depend_template (tree tmpl_info, tree t)
{
  tree tmpl_decl;
  tree tmpl_ptrn;
  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    pd_template ");
      pph_debug_tree (t, true);
      fprintf (pph_logfile, " %p", (void*)tmpl_info);
      fprintf (pph_logfile, "\n");
    }
  tmpl_decl = TI_TEMPLATE (tmpl_info);
  if (TREE_CODE (tmpl_decl) == OVERLOAD)
    tmpl_decl = OVL_CURRENT (tmpl_decl);
  tmpl_ptrn = DECL_TEMPLATE_RESULT (tmpl_decl);
  if (tmpl_ptrn && t != tmpl_ptrn)
    {
      /* This is a template, but not the pattern.  */
      pph_print_dependence (true, true, t, tmpl_ptrn);
    }
}

/* Print the dependence of a head of declaration 
   on the body of a type that the head uses directly.
   If either of these is not exposed,
   find the body of the exposed declaration that contains it.  */

static void
pph_print_depend_decl (tree user, tree used)
{
  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    pd_decl ");
      pph_debug_tree (user, false);
      fprintf (pph_logfile, " on ");
      pph_debug_tree (used, false);
      fprintf (pph_logfile, "\n");
    }
  if (user != NULL)
    {
      if (used != NULL)
        {
          bool body;
          tree exp_for_user = pph_find_exposed_for (user, &body);
          tree exp_for_used = pph_find_exposed_for (used, &body);
          if (exp_for_user && exp_for_used && exp_for_user != exp_for_used)
            pph_print_dependence (exp_for_user != user, true,
                                  exp_for_user, exp_for_used);
        }
    }
}

static void
pph_print_depend_type (tree decl, tree type)
{
  tree type_decl;
  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    pd_type ");
      pph_debug_tree (decl, false);
      fprintf (pph_logfile, " on ");
      pph_debug_type (type, false);
      fprintf (pph_logfile, "\n");
    }
  if (type != NULL)
    {
      type_decl = get_type_decl (type);
      pph_print_depend_decl (decl, type_decl);
    }
}

static void
pph_print_depend_type_type (tree t)
{
  tree t_type;
  tree field;

  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    depending on typedecl type ");
      pph_debug_tree (t, false);
      fprintf (pph_logfile, "\n");
    }

  t_type = TREE_TYPE (t);
  field = TYPE_FIELDS (t_type); 
  for (; field; field = TREE_CHAIN(field))
    {
      if (flag_pph_debug >= 2)
        {
          fprintf (pph_logfile, "    field ");
          pph_debug_tree (field, false);
        }
      if (TREE_CODE (field) == FIELD_DECL)
        {
          tree f_type = TREE_TYPE (field);
          if (flag_pph_debug >= 2)
            {
              fprintf (pph_logfile, " of type ");
              pph_debug_type (f_type, false);
              if (DECL_FIELD_IS_BASE (field))
                fprintf (pph_logfile, " is a base field!!\n");
              else
                fprintf (pph_logfile, " is a plain field\n");
            }
          pph_print_depend_type (t, f_type);
        }
      else if (TREE_CODE (field) == TYPE_DECL)
        {
        tree f_type = TREE_TYPE (field);
        if (flag_pph_debug >= 2)
          {
            fprintf (pph_logfile, " of type ");
            pph_debug_type (f_type, false);
            fprintf (pph_logfile, " is a type field\n");
          }
        pph_print_depend_type (t, f_type);
        }
    }

  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    end of fields\n");
    }
}

static void
pph_print_depend_func_type (tree t)
{
  /* We must print a dependence of the head of the function
     on the body of the types of its signature.  */

  tree args;
  tree func_type;

  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    depending on function type ");
      pph_debug_tree (t, false);
      fprintf (pph_logfile, "\n");
    }

  func_type = TREE_TYPE (t);
  pph_print_depend_type (t, TREE_TYPE (func_type)); /* return type */
  for (args = TYPE_ARG_TYPES (func_type); args; args = TREE_CHAIN (args))
    pph_print_depend_type (t, TREE_VALUE (args)); /* parameter */

  if (DECL_VIRTUAL_P (t))
    {
      tree ctx_type = DECL_CONTEXT (t);
      tree ctx_decl = get_type_decl (ctx_type);
      /* Virtual functions depend on containing class's body.*/
      pph_print_depend_type (t, ctx_type);
      /* The virtual class's body also depends on the function
         for construction of the vtable. */
      pph_print_dependence (true, true, ctx_decl, t);
    }
}

static void
pph_print_depend_var_type (tree t)
{
  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    depending on var/field type ");
      pph_debug_tree (t, false);
      fprintf (pph_logfile, "\n");
    }

  pph_print_depend_type (t, TREE_TYPE (t));
}

enum decl_exposure { HIDDEN, EXPOSED, NEEDED };

static enum decl_exposure
pph_get_decl_exposure (tree t)
{
  tree container;
  tree type;
  tree tmpl_info;
  bool defined = false;
  bool inlined = false;
  bool needed = false;
  int generic = 0;
  enum tree_code code = TREE_CODE (t);

  if (flag_pph_debug >= 2)
    {
      fprintf (pph_logfile, "    get_exposure for ");
      pph_debug_tree (t, false);
      fprintf (pph_logfile, "\n");
    }

  /* For DECL_USE_TEMPLATE and CLASSTYPE_USE_TEMPLATE,
      1=implicit instantiation
      2=partial or explicit specialization, e.g.: 
	  template <> int min<int> (int, int),
      3=explicit instantiation, e.g.:
	  template int min<int> (int, int);
  */

  if (code == TYPE_DECL)
    {
      pph_print_depend_type_type (t);

      container = DECL_CONTEXT (t);
      if (!is_namespace (container))
        return HIDDEN;
      type = TREE_TYPE (t);
      defined = COMPLETE_TYPE_P (type);

      /* FIXME pph: Why DECL_TEMPLATE_TEMPLATE_PARM_P does not apply
         here?  It is a template template parameter, but the tree code
         is inconsistent.  */
      if (DECL_TEMPLATE_PARM_P (t)
          || TREE_CODE (type) == TEMPLATE_TEMPLATE_PARM
          || TREE_CODE (type) == BOUND_TEMPLATE_TEMPLATE_PARM)
        return HIDDEN;
      if (CLASS_TYPE_P (t))
        {
          tmpl_info = CLASSTYPE_TEMPLATE_INFO (type);
          generic = CLASSTYPE_USE_TEMPLATE (type);
        }
      else
        {
          tmpl_info = NULL;
          generic = 0;
        }
      if (generic == 1)
        return HIDDEN;
      if (tmpl_info != NULL)
        {
          pph_print_depend_template (tmpl_info, t);
          needed = defined && generic == 3;
        }
      else
        {
          needed = false;
        }
    }
  else if (code == VAR_DECL)
    {
      tree enclass = DECL_CONTEXT (t);
      bool in_class = enclass && CLASS_TYPE_P (enclass);
      /* If the VAR_DECL is in a class, it must be a static member.  */
      container = enclass;
      if (in_class)
        container = TYPE_CONTEXT (enclass);

      pph_print_depend_var_type (t);

      if (!is_namespace (container))
          return HIDDEN;
      defined = DECL_INITIAL (t) != NULL /* FIXME pph: DECL_INITIALIZED_P (t)  */;
      type = TREE_TYPE (t);
      needed = !((!defined && (in_class || DECL_THIS_EXTERN (t)))
                 || decl_constant_var_p (t));
      if (in_class)
        {
          tmpl_info = DECL_TEMPLATE_INFO (t);
          generic = DECL_USE_TEMPLATE (t);
          if (generic == 1)
            return HIDDEN;
        }
      else
        {
          tmpl_info = NULL;
          generic = 0;
        }
      if (tmpl_info != NULL)
        {
          pph_print_depend_template (tmpl_info, t);
          needed = needed && generic == 3;
        }
    }
  else if (code == FUNCTION_DECL)
    {
      if (flag_pph_debug >= 2)
        {
          fprintf (pph_logfile, "    depending on function ");
          pph_debug_tree (t, false);
          fprintf (pph_logfile, "\n");
        }

      container = DECL_CONTEXT (t);
      if (container && CLASS_TYPE_P (container))
          container = TYPE_CONTEXT (DECL_CONTEXT (t));

      pph_print_depend_func_type (t);

      if (!is_namespace (container))
        return HIDDEN;
      inlined = DECL_DECLARED_INLINE_P (t);
      defined = DECL_INITIAL (t) != NULL;
      tmpl_info = DECL_TEMPLATE_INFO (t);
      generic = DECL_USE_TEMPLATE (t);
      if (tmpl_info != NULL)
        {
          if (generic == 2)
              needed = defined && !inlined;
          else
            {
              pph_print_depend_template (tmpl_info, t);
              needed = defined && !inlined && generic == 3;
            }
        }
      else
        {
          needed = defined && !inlined;
        }
    }
  else
    {
      gcc_assert (code < MAX_TREE_CODES);
      return HIDDEN;
    }

  if (needed)
    return NEEDED;
  else
    return EXPOSED;
}

static void
pph_print_dependences (bool user_body, bool used_body,
                        tree t, VEC(tree,gc) *deps)
{
  unsigned j;
  tree d;
  for (j = 0; VEC_iterate (tree, deps, j, d); j++)
      pph_print_dependence (user_body, used_body, t, d);
}

/* Print the head of declaration T and its dependencies. N_HEAD_TOKENS
   is the number of tokens taken by T's head.  N_HEAD_ITOKENS is the
   number of invisible tokens.
   
   HEAD_TOKENS is the array of tokens in the head (note that the
   length of this array may be different than N_HEAD_TOKENS, due to
   adjustments made by the caller).

   If CONTAINER is set, then T is a member of it.  */

static void
pph_print_declaration_head (tree t, bool artificial, tree container,
                            unsigned n_head_tokens, unsigned n_head_invis,
			    VEC(cp_token, heap) *head_tokens)
{
  VEC(tree,gc) *sym_head_deps, *sym_body_deps;
  enum tree_code code = TREE_CODE (t);

  fprintf (pph_logfile, "declaration ");
  pph_debug_tree (t, false);

  fprintf (pph_logfile, " htok %u,%u", n_head_tokens, n_head_invis);

  /*FIXME pph: We want to get rid of most artificial tokens;
    this is temporary to find them.  */
  if (artificial)
    fprintf (pph_logfile, " artificial");

  if (container)
    {
      fprintf (pph_logfile, " mbrof ");
      pph_debug_tree (get_type_decl (container), true);
    }

  fprintf (pph_logfile, "\n");

  /* Template instances should depend on their pattern body.  */
  if (artificial)
    {
      if (code == TYPE_DECL)
        {
          tree t_type = TREE_TYPE (t);
          if (CLASS_TYPE_P (t_type))
            {
              tree tmpl_info = CLASSTYPE_TEMPLATE_INFO (t_type);
              if (tmpl_info != NULL)
                pph_print_depend_template (tmpl_info, t);
            }
        }
      else if (code == VAR_DECL || code == FUNCTION_DECL)
        {
          tree tmpl_info;
          tmpl_info = (DECL_LANG_SPECIFIC (t)) ? DECL_TEMPLATE_INFO (t) : NULL;
          if (tmpl_info != NULL)
            pph_print_depend_template (tmpl_info, t);
        }
    }
  else
    {
      /* From the name dependencies, determine symbol dependencies
	 by correlating the location of the looked-up symbols with
	 the tokens in HEAD_TOKENS and BODY_TOKENS.  */
      if (flag_pph_debug >= 2)
        fprintf (pph_logfile, "  begin normal dependences\n");
      pph_locate_name_lookups_in (head_tokens, &sym_head_deps, &sym_body_deps);
      pph_print_dependences (false, false, t, sym_head_deps);
      pph_print_dependences (true, false, t, sym_body_deps);
      if (flag_pph_debug >= 2)
        fprintf (pph_logfile, "  end normal dependences\n");
    }
}


/* Print the head of declaration T and its dependencies. N_BODY_TOKENS
   is the number of tokens taken by T's head.  N_BODY_ITOKENS is the
   number of invisible tokens.

   BODY_TOKENS is the array of tokens in the head (note that the
   length of this array may be different than N_BODY_TOKENS, due to
   adjustments made by the caller).

   EXPOSURE indicates the exposure of T.

   N_SUBTOKENS is the number of tokens that declarations inside T's
   body have used up, those should be subtracted from the total number
   of tokens in T to avoid double counting. */

static void
pph_print_declaration_body (tree t, bool artificial,
                            enum decl_exposure exposure,
                            unsigned n_body_tokens, unsigned n_body_invis,
                            VEC(cp_token, heap) *body_tokens)
{
  VEC(tree,gc) *sym_head_deps, *sym_body_deps;
  const char* msg;

  fprintf (pph_logfile, "declaration ");
  pph_debug_tree (t, true);

  fprintf (pph_logfile, " btok %u,%u", n_body_tokens, n_body_invis);

  /* FIXME pph: We want to get rid of most artificial tokens;
     this is temporary to find them.  */
  if (artificial)
    msg = "artificial";
  else if (exposure == NEEDED)
    msg = "needed";
  else
    msg = "lazy";
  fprintf (pph_logfile, " %s", msg);

  fprintf (pph_logfile, "\n");

  pph_print_dependence (true, false, t, t); /* body depends on its head */

  if (flag_pph_debug >= 2)
    fprintf (pph_logfile, "  begin normal dependences\n");
  pph_locate_name_lookups_in (body_tokens, &sym_head_deps, &sym_body_deps);
  pph_print_dependences (true, false, t, sym_head_deps);
  pph_print_dependences (true, false, t, sym_body_deps);
  if (flag_pph_debug >= 2)
    fprintf (pph_logfile, "  end normal dependences\n");
}


/* Compute the implicit cost of a method F.  */

static unsigned
pph_find_special_methods (tree f)
{
  unsigned found = 0;
  tree o;
  if (TREE_CODE (f) == OVERLOAD)
    {
      for (o = f; o; o = OVL_NEXT (o))
        found |= pph_find_special_methods (OVL_CURRENT (o));
    }
  else if (TREE_CODE (f) == TEMPLATE_DECL)
    found |= pph_find_special_methods (DECL_TEMPLATE_RESULT (f));
  else
    {
      gcc_assert (TREE_CODE (f) == FUNCTION_DECL);
      if (DECL_ARTIFICIAL (f))
        return found;
      if (DECL_CONSTRUCTOR_P (f))
        if (DECL_COPY_CONSTRUCTOR_P (f))
          found |= (1<<2); /* copy constructor */
        else
          found |= (1<<1); /* default constructor */
      else if (DECL_DESTRUCTOR_P (f))
        found |= (1<<0); /* destructor */
      else if (DECL_ASSIGNMENT_OPERATOR_P (f))
        found |= (1<<3); /* copy assign op */
    }
  return found;
}

/* Compute the implicit cost of a class type T_TYPE.  */

static unsigned
pph_implicit_class_cost (tree t_type)
{
  VEC(tree,gc) *methods;
  unsigned idx;
  unsigned mbrs;
  unsigned cost = 0;
  unsigned found = 0;

  /* Gather general statistics.  */
  unsigned fields = fields_length (t_type); /* also direct bases */
  unsigned vptr = (TYPE_POLYMORPHIC_P (t_type) ? 1 : 0);
  unsigned slots = fields + vptr;
  unsigned vbases = VEC_length (tree, CLASSTYPE_VBASECLASSES (t_type));
  unsigned vtables = list_length (CLASSTYPE_VTABLES (t_type));

  /* Assign cost of implicit special member variables.  */
  /* These costs are somewhat arbitrary.  */
  cost += 20 * (CLASSTYPE_TYPEINFO_VAR (t_type) != NULL); /* typeinfo */
  cost += 4 * vbases * vtables; /* virtual tables */

  /* Assign cost of implicit special member functions.  */
  /* First find them.  */
  methods = CLASSTYPE_METHOD_VEC (t_type);
  if (methods != NULL)
    for (idx = 0;  idx < VEC_length (tree, methods);  idx++)
      {
        tree ovld = VEC_index (tree, methods, idx);
        if (ovld)
          found |= pph_find_special_methods (ovld);
      }
  /* These costs are somewhat arbitrary.  */
  /* FIXME pph: These queries seem to not work for templates.
     We can accept the inaccuracy for now.  */
  mbrs =  slots * 2 + vbases * 4;
  if (!(found & (1<<2))) /* copy constructor */
    {
      if (TYPE_HAS_TRIVIAL_COPY_ASSIGN (t_type))
        cost += 4;
      else
        cost += (8 + 2*mbrs) * (vbases > 0 ? 2 : 1);
    }
  if (!(found & (1<<1))) /* default constructor */
    if (!TYPE_HAS_TRIVIAL_DFLT (t_type))
      cost += 4 + mbrs;
  if (!(found & (1<<0))) /* destructor */
    if (!TYPE_HAS_TRIVIAL_DESTRUCTOR (t_type))
      cost += (8 + 2*mbrs) * (vbases > 0 ? 2 : 1);
  if (!(found & (1<<3))) /* copy assign op */
    {
      if (TYPE_HAS_TRIVIAL_COPY_ASSIGN (t_type))
        cost += 4;
      else
        cost += (8 + 2*mbrs);
    }

  return cost;
}


/* Print declaration T with the given EXPOSURE.  If T has a body with
   N tokens, subtract N_SUBTOKENS from it before printing them.  
   This is used when printing class declarations.  The caller first
   prints all the declarations inside the class, followed by the 
   class declaration itself, to avoid double counting tokens in the class
   body, they are subtracted from the total count.

   PRIMARY is true when T is the very first declaration captured
   during a pph_start_exposed/pph_stop_exposed region.  If T is a
   member of a class, and it happens to be the first declaration
   captured, it means that T is an out-of-line definition.
   
   If T is a member of a class, and PARENT is the TYPE_DECL for that
   class, it means that we are printing the in-class declaration of T.
   In that case, when we print the parent, we should subtract the
   tokens attributed to T.  So, in these cases return the total
   number of tokens printed in T's head and body.  Otherwise,
   return 0.  */

static unsigned
pph_print_declaration (tree t, enum decl_exposure exposure,
                       unsigned n_subtokens, bool primary, tree parent)
{
  tree container = NULL;
  enum tree_code code = TREE_CODE (t);
  bool is_member = false;
  bool artificial = PPH_ARTIFICIAL (t);
  bool print_head = true, print_body = true;

  /* The cost of a declaration is proportional to the number of tokens.
     Artificial symbols are not represented in the file, so they do
     not have tokens.  We represent their cost as a number of
     invisible tokens.  */
  VEC(cp_token, heap) *head_tokens = pph_lookup_head_token_cache_for (t);
  unsigned n_head_tokens = VEC_length (cp_token, head_tokens);
  unsigned n_head_invis = 0;
  VEC(cp_token, heap) *body_tokens = pph_lookup_body_token_cache_for (t);
  unsigned n_body_tokens = VEC_length (cp_token, body_tokens);
  unsigned n_body_invis = 0;
  unsigned n_member_tokens = 0;

  /* If this is a member of a class, count the number of tokens in the
     member that overlap with the containing class. */
  if (code == FUNCTION_DECL || code == VAR_DECL)
    {
      container = DECL_CONTEXT (t);
      if (container && CLASS_TYPE_P (container))
	{
	  is_member = true;
	  if (parent && container == TREE_TYPE (parent))
	    n_member_tokens = n_head_tokens + n_body_tokens;
	}
    }

  /* Now we need to adjust costs, and head/body printing.  */

  if (code == VAR_DECL)
    {
      if (artificial)
        /* Artificial static member variables get their token
           counts from the calling expression, which isn't helpful.
           Build the cost into the class instead.  All of which
           means suppress this decl.  */
        return 0;

      if (DECL_NONTRIVIALLY_INITIALIZED_P (t)
          && !DECL_INITIALIZED_BY_CONSTANT_EXPRESSION_P (t))
        n_body_invis += 10;

      if (is_member)
        {
          if (primary) /* Must be an out-of-line declaration/definition.  */
            {
              /* Merge out-of-line member definitions into the body.
                 This merge prevents two heads for the same symbol.  */
              n_body_tokens += n_head_tokens;
              n_body_invis += n_head_invis;
              n_head_tokens = 0;
              n_head_invis = 0;
              print_head = false;
              /* Out-of-line member variable declarations are definitions,
                 and hence need to be generated.  */
              exposure = NEEDED;
            }
          else /* Not primary; must be an in-line declaration/definition.  */
            {
              /* The only var bodies in the class are manifest constants;
                 merge them into the head.  */
              n_head_tokens += n_body_tokens;
              n_head_invis += n_body_invis;
              n_body_tokens = 0;
              n_body_invis = 0;
              print_body = false;
	    }
        }
      else /* not is_member */
        {
          if (exposure == EXPOSED)
            {
              /* Merge manifest constants into the head.
                 This code sweeps up extern declarations with no bodies,
                 but that's okay.  */
              n_head_tokens += n_body_tokens;
              n_head_invis += n_body_invis;
              n_body_tokens = 0;
              n_body_invis = 0;
              print_body = false;
            }
        }
      if (exposure == NEEDED)
        n_body_invis += 4; /* For emitting the actual variable declaration. */
    }
  else if (code == FUNCTION_DECL)
    {
      /* Pure function declarations get no body.  */
      if (exposure == EXPOSED && !artificial
          && n_body_tokens == 0 && n_body_invis == 0)
        print_body = false;
      if (artificial)
        {
          if (is_member)
            {
              /* Artificial special member functions get their token
                 counts from the calling expression, which isn't helpful.
                 Build the cost into the class instead.  All of which
                 means suppress this decl.  */
              return 0;
            }
        }
    }
  else if (code == TYPE_DECL)
    {
      tree t_type = TREE_TYPE (t);

      if (artificial)
        return 0;

      if (DECL_IMPLICIT_TYPEDEF_P (t)
	  && CLASS_TYPE_P (t_type)
          && TYPE_LANG_SPECIFIC (t_type))
        {
          tree tmpl_info;
          int generic;

          n_body_invis += pph_implicit_class_cost (t_type);

          tmpl_info = CLASSTYPE_TEMPLATE_INFO (t_type);
          if (tmpl_info)
            {
              generic = CLASSTYPE_USE_TEMPLATE (t_type);
              if (generic == 1)
                {
                  /* Implicit instantiations have no visibile tokens.  */
                  n_head_invis += n_head_tokens;
                  n_head_tokens = 0;
                  n_body_invis += n_body_tokens;
                  n_body_tokens = 0;
                }
              else if (generic == 3)
                {
                  /* Explicit instantiations have no bodies,
                     but they are work.  This approximation is
                     unjustified, but we are presuming that explicit
                     instantiations are rare in application code. */
                  n_body_invis += 15;
                  exposure = NEEDED;
                }
              if (generic != 2)
                {
                  bool defined;
                  defined = CLASS_TYPE_P (t_type) && COMPLETE_TYPE_P (t_type);
                  if (defined)
                    pph_print_depend_template (tmpl_info, t);
                }
            }
          else if (primary)
            {
              /* Now subtract the sum of members from the body of the class.
                 This prevents double counting when emitting the parent.
                 For non-primary class symbols, this value will be zero.  */
              gcc_assert (n_body_tokens >= n_subtokens);
	      n_body_tokens -= n_subtokens;
            }
        }
      else if (TREE_CODE (t_type) == ENUMERAL_TYPE)
        {
          /* No additional work for enum.  */
        }
      else
        {
          /* Not artificial, not a C++ class, not an enum;
             so must be a pure typedef.  They have no body.  */
          gcc_assert (n_body_tokens == 0 && n_body_invis == 0);
          print_body = false;
        }
    }

  if (print_head)
    pph_print_declaration_head (t, artificial, (is_member) ? container : NULL,
                                n_head_tokens, n_head_invis, head_tokens);

  if (print_body)
    pph_print_declaration_body (t, artificial, exposure,
                                n_body_tokens, n_body_invis, body_tokens);

  if (flag_pph_debug >= 4)
    {
      fprintf (pph_logfile, "    Declarator head tokens: ");
      cp_lexer_debug_tokens ((VEC(cp_token, gc) *)head_tokens);
      fprintf (pph_logfile, "    Declarator body tokens: ");
      cp_lexer_debug_tokens ((VEC(cp_token, gc) *)body_tokens);
      fprintf (pph_logfile, "\n");
    }

  return n_member_tokens;
}

static void
pph_print_declarations (VEC(tree,heap) *v)
{
  unsigned i, j, n, first_ix;
  tree t, first, parent;
  enum decl_exposure exposure;
  unsigned n_subtokens = 0;

  if (VEC_empty (tree, v))
    return;

  /* If the first AST in V is a class/structure declaration, process
     the sub-declarations first.  This will accumulate the tokens in
     the sub-declarations, so that when we print the class itself
     we don't double count the tokens in its body.  */

  /* Skip over un-exposed declarations, like template parameters. */
  n = VEC_length (tree, v);
  for ( i = 0; VEC_iterate (tree, v, i, t); i++)
    {
      exposure = pph_get_decl_exposure (t);
      if (exposure >= EXPOSED)
        break;
    }
  if (i >= n)
    return; /* No exposed decls. */

  first = VEC_index (tree, v, i);
  if (VEC_length (tree, v) > i+1
      && TREE_CODE (first) == TYPE_DECL
      && CLASS_TYPE_P (TREE_TYPE (first)))
    {
      parent = first;
      first_ix = i+1;
    }
  else
    {
      parent = NULL;
      first_ix = i;
    }

  for (j = first_ix; VEC_iterate (tree, v, j, t); j++)
    {
      exposure = pph_get_decl_exposure (t);
      if (exposure >= EXPOSED)
        n_subtokens += pph_print_declaration (t, exposure, 0U, i == j, parent);
    }

  /* If we didn't print the first declaration, print it now.  */
  if (first_ix > i)
    {
      exposure = pph_get_decl_exposure (first);
      if (exposure >= EXPOSED)
	pph_print_declaration (first, exposure, n_subtokens, true, NULL);
    }
}

static void
pph_print_trees_tokens (VEC(tree,heap) *v, cp_token *tok1, cp_token *tok2)
{
  VEC(cp_token, heap) *vtok;

  vtok = pph_print_copy_tokens (tok1, tok2);
  if (vtok == NULL)
    return;

  pph_print_token_range (v, vtok);
  pph_print_declarations (v);
  fprintf (pph_logfile, "\n");
}


/* Intercept the result of a name lookup operation requested by the
   parser while we are intercepting AST creation.  T is the result
   of a name lookup done by the parser.  If this is the first time
   we see it, store it in pph_name_lookups.  */

void
pph_catch_name_lookup (tree t)
{
  if (t == NULL_TREE || t == error_mark_node || pph_tree_catcher == NULL)
    return;

  timevar_push (TV_PPH_MANAGE);

  PPH_STATS_INCR (name_lookups, 1);

  if (the_parser->lexer)
    {
      /* If we are parsing, we are stopped one token past the identifier
	 that we have just looked up.  Store the token where we have seen
	 this identifier so that we can determine whether the identifier
	 was accessed in a head or a body.

	 Note that we do this for every instance we find for T, so that
	 we can store all the locations where T was accessed from.  */
      VEC(cp_token, heap) *tokens;
      cp_token *tok;
      void **slot;

      slot = pointer_map_insert (pph_nl_token_map, t);
      tokens = (VEC(cp_token, heap) *) *slot;
      tok = the_parser->lexer->next_token - 1;
      VEC_safe_push (cp_token, heap, tokens, tok);
      *slot = tokens;
    }

  /* Make sure we do not store the same decl more than once.  */
  if (pointer_set_insert (pph_name_lookups_set, t))
    {
      timevar_pop (TV_PPH_MANAGE);
      return;
    }

  VEC_safe_push (tree, heap, pph_name_lookups, t);
  timevar_pop (TV_PPH_MANAGE);
}


/* Print statistics for the PPH cache.  */

static void
pph_print_stats (void)
{
  fprintf (stderr, "\nPPH cache statistics\n");
  fprintf (stderr, "Number of tokens in the lexer:           %lu\n",
	   pph_stats.lexed_tokens);
  fprintf (stderr, "Number of tokens consumed by the parser: %lu\n",
	   pph_stats.parsed_tokens);
  fprintf (stderr, "Number of declarations cached in:        %lu\n",
	   pph_stats.cached_decls);
  fprintf (stderr, "Number of declarations restored:         %lu\n",
	   pph_stats.restored_decls);
  fprintf (stderr, "Number of cached decl references:        %lu\n",
	   pph_stats.cached_refs);
  fprintf (stderr, "Number of name lookups:                  %lu\n",
	   pph_stats.name_lookups);
  fprintf (stderr, "Number of bad lookups:                   %lu\n",
	   pph_stats.bad_lookups);
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

  if (flag_pph_stats)
    pph_print_stats ();

  if (flag_pph_logfile)
    fclose (pph_logfile);
}

#include "gt-cp-pph.h"
