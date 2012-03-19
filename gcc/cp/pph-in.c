/* Routines for reading PPH data.

   Copyright (C) 2012 Free Software Foundation, Inc.
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
#include "pph.h"
#include "tree.h"
#include "langhooks.h"
#include "tree-iterator.h"
#include "tree-pretty-print.h"
#include "lto-streamer.h"
#include "pph-streamer.h"
#include "tree-pass.h"
#include "version.h"
#include "cppbuiltin.h"
#include "toplev.h"
#include "parser.h"
#include "pointer-set.h"


/********************************************************* type declarations */


typedef char *char_p;
DEF_VEC_P(char_p);
DEF_VEC_ALLOC_P(char_p,heap);


/****************************************************** forward declarations */


/* Forward declarations to avoid circularity.  */
static tree pph_in_merge_key_decl_on_chain (pph_stream *, tree *);
static tree pph_in_merge_key_type_in_var (pph_stream *, tree *);


/***************************************************** stream initialization */


/* String tables for all input streams.  These are allocated separately
  from streams because they cannot be deallocated after the streams
  have been read (string streaming works by pointing into these
  tables).

  Each stream will create a new entry in this table of tables.  The
  memory will remain allocated until the end of compilation.  */
static VEC(char_p,heap) *string_tables = NULL;


/* Read into memory the contents of the file in STREAM.  Initialize
   internal tables and data structures needed to re-construct the
   ASTs in the file.  */

void
pph_init_read (pph_stream *stream)
{
  struct stat st;
  size_t i, bytes_read, strtab_size, body_size;
  int retcode;
  pph_file_header *header;
  const char *strtab, *body;
  char *new_strtab;

  /* Read STREAM->NAME into the memory buffer stream->encoder.r.file_data.  */
  retcode = fstat (fileno (stream->file), &st);
  gcc_assert (retcode == 0);
  stream->encoder.r.file_size = (size_t) st.st_size;
  stream->encoder.r.file_data = XCNEWVEC (char, stream->encoder.r.file_size);
  bytes_read = fread (stream->encoder.r.file_data, 1,
		      stream->encoder.r.file_size, stream->file);
  gcc_assert (bytes_read == stream->encoder.r.file_size);

  /* Set up the file sections, header, body and string table for the
     low-level streaming routines.  */
  stream->encoder.r.pph_sections = XCNEWVEC (struct lto_file_decl_data *,
					     PPH_NUM_SECTIONS);
  for (i = 0; i < PPH_NUM_SECTIONS; i++)
    stream->encoder.r.pph_sections[i] = XCNEW (struct lto_file_decl_data);

  header = (pph_file_header *) stream->encoder.r.file_data;
  strtab = (const char *) header + sizeof (pph_file_header);
  strtab_size = header->strtab_size;
  body = strtab + strtab_size;
  gcc_assert (stream->encoder.r.file_size
	      >= strtab_size + sizeof (pph_file_header));
  body_size = stream->encoder.r.file_size
	      - strtab_size - sizeof (pph_file_header);

  /* Create a new string table for STREAM.  This table is not part of
     STREAM because it needs to remain around until the end of
     compilation (all the string streaming routines work by pointing
     into the string table, so we cannot deallocate it after reading
     STREAM).  */
  new_strtab = XNEWVEC (char, strtab_size);
  memcpy (new_strtab, strtab, strtab_size);
  VEC_safe_push (char_p, heap, string_tables, new_strtab);

  /* Create an input block structure pointing right after the string
     table.  */
  stream->encoder.r.ib = XCNEW (struct lto_input_block);
  LTO_INIT_INPUT_BLOCK_PTR (stream->encoder.r.ib, body, 0, body_size);
  stream->encoder.r.data_in
      = lto_data_in_create (stream->encoder.r.pph_sections[0],
			    new_strtab, strtab_size, NULL);

  /* Associate STREAM with STREAM->ENCODER.R.DATA_IN so we can recover
     it from the streamer hooks.  */
  stream->encoder.r.data_in->sdata = (void *) stream;
}


/********************************************************** primitive values */


/* Read an unsigned char VALUE to STREAM.  */

static unsigned char
pph_in_uchar (pph_stream *stream)
{
  unsigned char n = streamer_read_uchar (stream->encoder.r.ib);
  return n;
}


/* Read a HOST_WIDE_INT from STREAM.  */

static inline HOST_WIDE_INT
pph_in_hwi (pph_stream *stream)
{
  return streamer_read_hwi (stream->encoder.r.ib);
}


/* Read an int from STREAM.  */

static inline int
pph_in_int (pph_stream *stream)
{
  HOST_WIDE_INT n = streamer_read_hwi (stream->encoder.r.ib);
  gcc_assert (n == (int) n);
  return (int) n;
}


/* Read an unsigned HOST_WIDE_INT from STREAM.  */

static inline unsigned HOST_WIDE_INT
pph_in_uhwi (pph_stream *stream)
{
  return streamer_read_uhwi (stream->encoder.r.ib);
}


/* Read an unsigned integer from STREAM.  */

unsigned int
pph_in_uint (pph_stream *stream)
{
  HOST_WIDE_INT unsigned n = streamer_read_uhwi (stream->encoder.r.ib);
  gcc_assert (n == (unsigned) n);
  return (unsigned) n;
}


/* Read N bytes from STREAM into P.  The caller is responsible for
   allocating a sufficiently large buffer.  */

static void
pph_in_bytes (pph_stream *stream, void *p, size_t n)
{
  lto_input_data_block (stream->encoder.r.ib, p, n);
}


/* Read and return a string from STREAM.  */

const char *
pph_in_string (pph_stream *stream)
{
  const char *s = streamer_read_string (stream->encoder.r.data_in,
                                        stream->encoder.r.ib);
  return s;
}


/* Read a bitpack from STREAM.  */

static struct bitpack_d
pph_in_bitpack (pph_stream *stream)
{
  struct bitpack_d bp = streamer_read_bitpack (stream->encoder.r.ib);
  return bp;
}


/* Read a boolean value from STREAM.  */

static inline bool
pph_in_bool (pph_stream *stream)
{
  unsigned val = pph_in_uint (stream);
  gcc_assert (val <= 1);
  return (bool) val;
}


/******************************************************** source information */


/* Set in pph_in_and_merge_line_table. Represents the source_location offset
   which every streamed in token must add to it's serialized source_location.

   FIXME pph: Ideally this would be in pph_stream.encoder.r, but for that we
   first need to get rid of the dependency to the streamer_hook for locations.
   */
static int pph_loc_offset;


/* Read a linenum_type from STREAM.  */

static inline linenum_type
pph_in_linenum_type (pph_stream *stream)
{
  return (linenum_type) pph_in_uint (stream);
}


/* Read a source_location from STREAM.  */

static inline source_location
pph_in_source_location (pph_stream *stream)
{
  return (source_location) pph_in_uint (stream);
}


/* Read a line table marker from STREAM.  */

static inline enum pph_linetable_marker
pph_in_linetable_marker (pph_stream *stream)
{
  enum pph_linetable_marker m =
    (enum pph_linetable_marker) pph_in_uchar (stream);
  gcc_assert (m == PPH_LINETABLE_ENTRY
	      || m == PPH_LINETABLE_REFERENCE
	      || m == PPH_LINETABLE_END);
  return m;
}



/* Read all the fields of struct line_map LM from STREAM.  LM is assumed
   to be an ordinary line map.  */

static void
pph_in_line_map_ordinary (pph_stream *stream, struct line_map *lm)
{
  struct bitpack_d bp;

  ORDINARY_MAP_FILE_NAME (lm) = pph_in_string (stream);
  ORDINARY_MAP_STARTING_LINE_NUMBER (lm) = pph_in_linenum_type (stream);

  /* Note that this index is an offset indicating the distance from LM
     to the line map entry for LM's includer.  It needs to be adjusted
     while reading the line table in pph_in_line_table_and_includes.  */
  ORDINARY_MAP_INCLUDER_FILE_INDEX (lm) = pph_in_int (stream);
  bp = pph_in_bitpack (stream);
  ORDINARY_MAP_IN_SYSTEM_HEADER_P (lm)
      = (unsigned char) bp_unpack_value (&bp, CHAR_BIT);
  ORDINARY_MAP_NUMBER_OF_COLUMN_BITS (lm) = bp_unpack_value (&bp,
							     COLUMN_BITS_BIT);
}


/* Read a line_map from STREAM into LM.  */

static void
pph_in_line_map (pph_stream *stream, struct line_map *lm)
{
  struct lto_input_block *ib = stream->encoder.r.ib;

  lm->start_location = pph_in_source_location (stream);
  lm->reason = streamer_read_enum (ib, lc_reason, LC_ENTER_MACRO);

  /* FIXME pph.  We currently do not support location tracking for
     macros in PPH images.  */
  gcc_assert (lm->reason != LC_ENTER_MACRO);
  pph_in_line_map_ordinary (stream, lm);
}


/* Read in from STREAM and merge a referenced include into the current parsing
   context.  */

static void
pph_in_include (pph_stream *stream)
{
  int old_loc_offset;
  const char *include_name;
  source_location prev_start_loc = pph_in_source_location (stream);

  /* Simulate highest_location to be as it would be at this point in a non-pph
     compilation.  */
  line_table->highest_location = (prev_start_loc - 1) + pph_loc_offset;

  /* FIXME pph: If we move pph_loc_offset to pph_stream.encoder.r, we could
     have an independent offset for each stream and not have to save and
     restore the state of a global pph_loc_offset as we are doing here.  */
  old_loc_offset = pph_loc_offset;

  include_name = pph_in_string (stream);

  /* We should not be trying to include STREAM again.  */
  gcc_assert (strcmp (include_name, stream->name) != 0);
  pph_read_file (include_name, stream);

  pph_loc_offset = old_loc_offset;
}


/* Read the line_table from STREAM and merge it in the current
   line_table.  At the same time load includes in the order they were
   originally included by loading them at the point they were
   referenced in the line_table.

   Returns the source_location of line 1 / col 0 for this include.  */

static source_location
pph_in_line_table_and_includes (pph_stream *stream)
{
  unsigned int used_before, old_depth;
  bool first;
  enum pph_linetable_marker next_lt_marker;
  int top_includer_ix;

  used_before = LINEMAPS_ORDINARY_USED (line_table);
  first = true;

  /* Read the path name of the original text header file that was
     used to generate STREAM.  */
  pph_stream_set_header_name (stream, pph_in_string (stream));

  /* All line map entries that have -1 as the includer, will now be
     relocated to the current last line map entry in the line table.  */
  top_includer_ix = used_before - 1;

  for (next_lt_marker = pph_in_linetable_marker (stream);
       next_lt_marker != PPH_LINETABLE_END;
       next_lt_marker = pph_in_linetable_marker (stream))
    {
      if (next_lt_marker == PPH_LINETABLE_REFERENCE)
	{
	  gcc_assert (!first);
	  pph_in_include (stream);
	}
      else
	{
	  struct line_map *lm;
	  int last_entry_ix;

	  lm = linemap_new_map (line_table, LC_ENTER);
	  pph_in_line_map (stream, lm);

	  /* All the entries that we read from STREAM will be appended
	     to the end of line_table.  Calculate the index for the
	     last entry so that we can resolve the relative indices
	     for all the includer file index entries we read from
	     STREAM.  Note that LAST_ENTRY_IX is the index of the line
	     map LM that we have just read.  */
	  last_entry_ix = LINEMAPS_ORDINARY_USED (line_table) - 1;

	  if (first)
	    {
	      first = false;
	      pph_loc_offset = (line_table->highest_location + 1)
		               - lm->start_location;
	      gcc_assert (ORDINARY_MAP_INCLUDER_FILE_INDEX (lm) == -1);
	    }

	  /* Relocate the includer file index.  For most entries, the
	     writer wrote ORDINARY_MAP_INCLUDER_FILE_INDEX as a
	     relative offset between this entry and the original
	     includer file index (see pph_out_line_map_ordinary).
	     Convert that relative index into an absolute index in the
	     current line_table.

	     A relocation value of -1 means that line map LM is
	     included by the current top includer (i.e., STREAM), so
	     we use the value TOP_INCLUDER_IX for it.  */
	  if (ORDINARY_MAP_INCLUDER_FILE_INDEX (lm) == -1)
	    ORDINARY_MAP_INCLUDER_FILE_INDEX (lm) = top_includer_ix;
	  else
	    {
	      gcc_assert (last_entry_ix
			  > ORDINARY_MAP_INCLUDER_FILE_INDEX (lm));
	      ORDINARY_MAP_INCLUDER_FILE_INDEX (lm)
		= last_entry_ix - ORDINARY_MAP_INCLUDER_FILE_INDEX (lm);
	    }

	  lm->start_location += pph_loc_offset;
	}
    }

  /* We used to expect exactly the same number of entries, but files
     included from this PPH file may sometimes not be needed.  For
     example,

	#include "2.pph"
	#include "foo.pph"
	  +-->	#include "1.pph"
		#include "2.pph"
		#include "3.pph"

     When foo.pph was originally created, the line table was built
     with inclusions of 1.pph, 2.pph and 3.pph.  But when compiling
     the main translation unit, we include 2.pph before foo.pph, so
     the inclusion of 2.pph from foo.pph does nothing.  Leaving the
     line table in a different shape than the original compilation.

     Instead of insisting on getting EXPECTED_IN entries, we expect at
     most EXPECTED_IN entries.  */
  {
    unsigned int expected = pph_in_uint (stream);
    gcc_assert (LINEMAPS_ORDINARY_USED (line_table) - used_before <= expected);
  }

  line_table->highest_location = pph_loc_offset + pph_in_uint (stream);
  line_table->highest_line = pph_loc_offset + pph_in_uint (stream);

  /* The MAX_COLUMN_HINT can be directly overwritten.  */
  line_table->max_column_hint = pph_in_uint (stream);

  /* The line_table doesn't store the last LC_LEAVE in any given compilation;
     thus we need to replay the LC_LEAVE for the header now.  For that same
     reason, the line_table should currently be in a state representing a depth
     one include deeper then the depth at which this pph was included.  The
     LC_LEAVE replay will then bring the depth back to what it was before
     calling this function.  */
  old_depth = line_table->depth++;
  linemap_add (line_table, LC_LEAVE, 0, NULL, 0);
  gcc_assert (line_table->depth == old_depth);

  return MAP_START_LOCATION (LINEMAPS_ORDINARY_MAP_AT (line_table, used_before));
}


/*********************************************************** record handling */


/* Wrapper for memory allocation calls that should have their results
   registered in the PPH streamer cache.  DATA is the pointer returned
   by the memory allocation call in ALLOC_EXPR.  IX is the cache slot 
   in CACHE where the newly allocated DATA should be registered at.  */
#define ALLOC_AND_REGISTER(CACHE, IX, TAG, DATA, ALLOC_EXPR)	\
    do {							\
      (DATA) = (ALLOC_EXPR);					\
      pph_cache_insert_at (CACHE, DATA, IX, TAG);		\
    } while (0)


/* Read and return a record marker from STREAM.  On return, *TAG_P will
   contain the tag for the data type stored in this record.  */
enum pph_record_marker
pph_in_record_marker (pph_stream *stream, enum pph_tag *tag_p)
{
  enum pph_record_marker m = (enum pph_record_marker) pph_in_uchar (stream);
  gcc_assert (m == PPH_RECORD_START
              || m == PPH_RECORD_START_NO_CACHE
              || m == PPH_RECORD_START_MUTATED
	      || m == PPH_RECORD_START_MERGE_KEY
	      || m == PPH_RECORD_START_MERGE_BODY
              || m == PPH_RECORD_END
              || m == PPH_RECORD_IREF
              || m == PPH_RECORD_XREF
              || m == PPH_RECORD_PREF);

  *tag_p = (enum pph_tag) pph_in_uint (stream);
  gcc_assert ((unsigned) *tag_p < (unsigned) PPH_NUM_TAGS);

  if (flag_pph_tracer >= 5)
    pph_trace_marker (m, *tag_p);

  return m;
}


/* Read and return a record header from STREAM.  EXPECTED_TAG indicates
   the data type that should be stored in this record.  When a
   PPH_RECORD_START marker is read, the next word read is an index
   into the streamer cache where the rematerialized data structure
   should be stored. When the writer stored this data structure for
   the first time, it added it to its own streamer cache at slot
   number *CACHE_IX_P.

   This way, if the same data structure was written a second time to
   the stream, instead of writing the whole structure again, only the
   index *CACHE_IX_P is written as a PPH_RECORD_IREF record.

   Therefore, when reading a PPH_RECORD_START marker, *CACHE_IX_P will
   contain the slot number where the materialized data should be
   cached at.  When reading a PPH_RECORD_IREF marker, *CACHE_IX_P will
   contain the slot number the reader can find the previously
   materialized structure.

   If the record starts with PPH_RECORD_XREF, this means that the data
   we are about to read is located in the pickle cache of one of
   STREAM's included images.  In this case, the record consists of two
   indices: the first one (*INCLUDE_IX_P) indicates which included
   image contains the data (it is an index into STREAM->INCLUDES), the
   second one indicates which slot in that image's pickle cache we can
   find the data.  */

static inline enum pph_record_marker
pph_in_start_record (pph_stream *stream, unsigned *include_ix_p,
		     unsigned *cache_ix_p, enum pph_tag expected_tag)
{
  enum pph_tag read_tag;
  enum pph_record_marker marker = pph_in_record_marker (stream, &read_tag);

  /* If the caller expects any tree, make sure we get a valid tree code.  */
  if (expected_tag == PPH_any_tree)
    gcc_assert (read_tag < PPH_any_tree);
  else
    gcc_assert (read_tag == expected_tag);

  *include_ix_p = (unsigned) -1;
  *cache_ix_p = (unsigned) -1;

  /* For PPH_RECORD_START and PPH_RECORD_IREF markers, read the
     streamer cache slot where we should store or find the
     rematerialized data structure (see description above).
     Also read the preloaded cache slot in IX for PPH_RECORD_PREF.  */
  if (marker == PPH_RECORD_START
      || marker == PPH_RECORD_IREF
      || marker == PPH_RECORD_PREF
      || marker == PPH_RECORD_START_MERGE_BODY
      || marker == PPH_RECORD_START_MERGE_KEY)
    *cache_ix_p = pph_in_uint (stream);
  else if (marker == PPH_RECORD_XREF
           || marker == PPH_RECORD_START_MUTATED)
    {
      *include_ix_p = pph_in_uint (stream);
      *cache_ix_p = pph_in_uint (stream);
    }
  else if (marker == PPH_RECORD_END || marker == PPH_RECORD_START_NO_CACHE)
    ; /* Nothing to do.  This record will not need cache updates.  */
  else
    gcc_unreachable ();

  return marker;
}


/********************************************************** lexical elements */


/* Read and return a location_t from STREAM.  */

location_t
pph_in_location (pph_stream *stream)
{
  struct bitpack_d bp;
  bool is_builtin;
  unsigned HOST_WIDE_INT n;
  location_t old_loc;

  bp = pph_in_bitpack (stream);
  is_builtin = bp_unpack_value (&bp, 1);

  n = pph_in_uhwi (stream);
  old_loc = (location_t) n;
  gcc_assert (old_loc == n);

  return is_builtin ? old_loc : old_loc + pph_loc_offset;
}


/* Load the tree value associated with TOKEN from STREAM.  */

static void
pph_in_token_value (pph_stream *stream, cp_token *token)
{
  switch (token->type)
    {
      case CPP_TEMPLATE_ID:
      case CPP_NESTED_NAME_SPECIFIER:
	/* FIXME pph - Need to handle struct tree_check.  */
	break;

      case CPP_KEYWORD:
	token->u.value = ridpointers[token->keyword];
	break;

      case CPP_NAME:
      case CPP_CHAR:
      case CPP_WCHAR:
      case CPP_CHAR16:
      case CPP_CHAR32:
      case CPP_NUMBER:
      case CPP_STRING:
      case CPP_WSTRING:
      case CPP_STRING16:
      case CPP_STRING32:
	token->u.value = pph_in_tree (stream);
	break;

      case CPP_PRAGMA:
	/* Nothing to do.  Field pragma_kind has already been loaded.  */
	break;

      default:
	pph_in_bytes (stream, &token->u.value, sizeof (token->u.value));
	gcc_assert (token->u.value == NULL);
    }
}


/* Read and return a token from STREAM.  */

static cp_token *
pph_in_token (pph_stream *stream)
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

  /* FIXME pph: verify that pph_in_token_value works with no tokens.  */
  pph_in_token_value (stream, token);

  return token;
}


/* Read and return a cp_token_cache instance from STREAM.  */

static cp_token_cache *
pph_in_token_cache (pph_stream *stream)
{
  unsigned i, num;
  cp_token *first, *last;

  num = pph_in_uint (stream);
  for (last = first = NULL, i = 0; i < num; i++)
    {
      last = pph_in_token (stream);
      if (first == NULL)
	first = last;
    }

  return cp_token_cache_new (first, last);
}


/******************************************************************* vectors */


/* Read and return a gc VEC of trees from STREAM.  */

VEC(tree,gc) *
pph_in_tree_vec (pph_stream *stream)
{
  HOST_WIDE_INT i, num;
  VEC(tree,gc) *v;

  num = pph_in_hwi (stream);
  v = NULL;
  for (i = 0; i < num; i++)
    {
      tree t = pph_in_tree (stream);
      VEC_safe_push (tree, gc, v, t);
    }

  return v;
}


/* Read and return a gc VEC of qualified_typedef_usage_t from STREAM.  */

static VEC(qualified_typedef_usage_t,gc) *
pph_in_qual_use_vec (pph_stream *stream)
{
  unsigned i, num;
  VEC(qualified_typedef_usage_t,gc) *v;

  num = pph_in_uint (stream);
  v = NULL;
  for (i = 0; i < num; i++)
    {
      qualified_typedef_usage_t q;
      q.typedef_decl = pph_in_tree (stream);
      q.context = pph_in_tree (stream);
      q.locus = pph_in_location (stream);
      VEC_safe_push (qualified_typedef_usage_t, gc, v, &q);
    }

  return v;
}


/* Read the vector V of tree_pair_s instances from STREAM.  */

static VEC(tree_pair_s,gc) *
pph_in_tree_pair_vec (pph_stream *stream)
{
  unsigned i, num;
  VEC(tree_pair_s,gc) *v;

  num = pph_in_uint (stream);
  for (i = 0, v = NULL; i < num; i++)
    {
      tree_pair_s p;
      p.purpose = pph_in_tree (stream);
      p.value = pph_in_tree (stream);
      VEC_safe_push (tree_pair_s, gc, v, &p);
    }

  return v;
}


/* Test whether tree T is an element of vector V.  */

static bool
pph_is_tree_element_of_vec (tree t, VEC(tree,gc) *v)
{
  unsigned i;
  tree s;
  FOR_EACH_VEC_ELT (tree, v, i, s)
    if (s == t)
      return true;
  return false;
}


/* Return the union of two tree vecs.  The argument vectors are unmodified.  */

static VEC(tree,gc) *
pph_union_two_tree_vecs (VEC(tree,gc) *left, VEC(tree,gc) *right)
{
  /* FIXME pph: This O(left)+O(left*right) union may become a problem.
     In the long run, we probably want to copy both into a hash table
     and then copy the table into the result.  */
  unsigned i;
  tree t;
  VEC(tree,gc) *unioned = VEC_copy (tree, gc, left);
  FOR_EACH_VEC_ELT (tree, right, i, t)
    {
      if (!pph_is_tree_element_of_vec (t, left))
	VEC_safe_push (tree, gc, unioned, t);
    }
  return unioned;
}


/* Union FROM one tree vec with and INTO a tree vec.  The INTO argument will
   have an updated value.  The FROM argument is no longer valid.  */

void
pph_union_into_tree_vec (VEC(tree,gc) **into, VEC(tree,gc) *from)
{
  if (!VEC_empty (tree, from))
    {
      if (*into == NULL)
	*into = from;
      else if (VEC_empty (tree, *into))
	{
	  VEC_free (tree, gc, *into);
	  *into = from;
	}
      else
	{
	  VEC(tree,gc) *unioned = pph_union_two_tree_vecs (*into, from);
	  VEC_free (tree, gc, *into);
	  VEC_free (tree, gc, from);
	  *into = unioned;
	}
    }
}


/******************************************************************** chains */

/* Test whether or not a tree T is on a CHAIN.  */

static bool
pph_is_on_chain (tree chain, tree t)
{
  for (; chain; chain = TREE_CHAIN (chain))
    {
      if (chain == t)
        return true;
    }
  return false;
}

/* Union a NEWER chain into an EXISTING chain.  */

static void
pph_union_into_chain (tree *existing, tree newer)
{
  /* FIXME pph: A better algorithm would be nice.  */
  if (newer == NULL)
    return;
  if (pph_is_on_chain (*existing, newer))
      pph_union_into_chain (existing, TREE_CHAIN (newer));
  else
    {
      pph_union_into_chain (existing, TREE_CHAIN (newer));
      TREE_CHAIN (newer) = *existing;
      *existing = newer;
    }
}


/* Read a chain of ASTs from STREAM.  */

static tree
pph_in_chain (pph_stream *stream)
{
  return streamer_read_chain (stream->encoder.r.ib, stream->encoder.r.data_in);
}


/* Read a chain of AST merge keys from STREAM.  Merge each tree
   into *CHAIN.  */

static void
pph_in_merge_key_chain (pph_stream *stream, tree *chain)
{
  unsigned i;
  HOST_WIDE_INT count;

  count = pph_in_hwi (stream);
  for (i = 0; i < count; i++)
    pph_in_merge_key_decl_on_chain (stream, chain);
}


/* Read a chain of AST merge bodies from STREAM.  */

static void
pph_in_merge_body_chain (pph_stream *stream)
{
  unsigned i;
  HOST_WIDE_INT count;

  count = pph_in_hwi (stream);
  for (i = 0; i < count; i++)
    pph_in_tree (stream);
}


/* Merge table of contents.  This TOC is used to decide whether a
   symbol has already been merged into a given compilation context.
   Compilation contexts are usually tree chains (e.g.,
   scope_chain->bindings->names), but they can be any stable memory
   address.

   This TOC is indexed by two values: the merge key read by
   pph_in_merge_key_decl_on_chain and the context in which we are doing this
   merge.  */
static htab_t merge_toc = NULL;

/* Table of contents entry.  */
typedef struct {
  /* Tree being matched.  */
  tree expr;

  /* Context where this tree should be inserted into.  */
  void *context;

  /* Name of the tree (from pph_merge_name).  */
  const char *name;
} merge_toc_entry;


/* Hash and equivalence functions for the merge TOC.  */

static hashval_t
htab_merge_key_hash (const void *p)
{
  const merge_toc_entry *key = (const merge_toc_entry *) p;
  hashval_t context_val = htab_hash_pointer (key->context);
  hashval_t name_val = htab_hash_string (key->name);
  hashval_t id_val = iterative_hash_hashval_t (name_val, TREE_CODE (key->expr));
  return iterative_hash_hashval_t (context_val, id_val);
}

static int
htab_merge_key_eq (const void *p1, const void *p2)
{
  const merge_toc_entry *key1 = (const merge_toc_entry *) p1;
  const merge_toc_entry *key2 = (const merge_toc_entry *) p2;

  if (key1->context != key2->context)
    return false;

  if (TREE_CODE (key1->expr) != TREE_CODE (key2->expr))
    return false;

  if (key1->name == NULL || key2->name == NULL)
    return false;

  return strcmp (key1->name, key2->name) == 0;
}


/* Look in TOC for an existing tree matching KEY.  */

static tree
pph_toc_lookup (htab_t toc, merge_toc_entry *key)
{
  void *slot = htab_find (toc, key);
  tree expr = NULL;

  if (slot)
    {
      merge_toc_entry *e = (merge_toc_entry *) slot;
      expr = e->expr;
    }

  return expr;
}


/* Insert KEY into TOC.  */

static void
pph_toc_add (htab_t toc, merge_toc_entry *key)
{
  void **slot;
  merge_toc_entry *entry;

  slot = htab_find_slot (toc, key, INSERT);
  gcc_assert (*slot == NULL);

  entry = XCNEW (merge_toc_entry);
  memcpy (entry, key, sizeof (*key));
  *slot = (void *) entry;
}


/* Prepend a tree EXPR to a CHAIN.  */

static tree
pph_prepend_to_chain (tree expr, tree *chain)
{
  DECL_CHAIN (expr) = *chain;
  *chain = expr;
  return expr;
}


/* Lookup and possibly add an expr, name, and context tuple in the TOC.
   Return the existing expr found.  If none was found, add an entry
   and return the argument expr. */

static tree
pph_toc_lookup_add (tree expr, const char *name, void *context)
{
  merge_toc_entry key;
  tree found;

  key.expr = expr;
  key.context = context;
  key.name = name;
  found = pph_toc_lookup (merge_toc, &key);
  if (!found)
    {
      pph_toc_add (merge_toc, &key);

      if (flag_pph_debug >= 3)
        fprintf (pph_logfile, "PPH: %s %p NOT found in TOC\n", name, context);

      return expr;
    }

  if (flag_pph_debug >= 3)
    fprintf (pph_logfile, "PPH: %s %p FOUND in TOC\n", name, context);

  gcc_assert (TREE_CODE (found) == TREE_CODE (expr));

  return found;
}


/* Merge the just-read header for tree EXPR with NAME onto the CHAIN.  */

static tree
pph_merge_into_chain (tree expr, const char *name, tree *chain)
{
  tree found = pph_toc_lookup_add (expr, name, (void*)chain);
  if (found == expr)
    {
      gcc_assert (chain);
      return pph_prepend_to_chain (expr, chain);
    }
  return found;
}


/****************************************************************** bindings */


/* Forward declaration to break cyclic dependencies.  */
static void pph_in_merge_body_namespace_decl (pph_stream *stream);
static void pph_in_merge_key_namespace_decl (pph_stream *stream, tree *chain);


/* Read the start of a binding level from STREAM.  If the binding
   level had already been read, *EXISTED_P will be set to true,
   otherwise it will be set to false.

   If EXISTING_BL is given, it means that the caller already has a
   binding level instance that it would like to use.  In that case,
   this function will:

   1- If the record read from STREAM is a reference, the binding level
      in that reference must be identical to EXISTING_BL.

   2- If the record read from STREAM is a new instance, the binding
      level given in EXISTING_BL is registered in the cache at the
      slot location given by this record.  This way, subsesequent
      internal references to EXISTING_BL will resolve to EXISTING_BL.
      This is used for binding levels that are already set in the
      compilation (e.g., scope_chain->bindings).

   If EXISTING_BL is NULL, a new one will be allocated, registered and
   returned.  */

static cp_binding_level *
pph_in_binding_level_start (pph_stream *stream, cp_binding_level *existing_bl,
                            bool *existed_p)
{
  unsigned ix, image_ix;
  enum pph_record_marker marker;
  cp_binding_level *new_bl;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_cp_binding_level);
  if (marker == PPH_RECORD_END)
    {
      *existed_p = true;
      gcc_assert (existing_bl == NULL);
      return NULL;
    }
  else if (pph_is_reference_marker (marker))
    {
      *existed_p = true;
      new_bl = (cp_binding_level *) pph_cache_find (stream, marker, image_ix,
						    ix, PPH_cp_binding_level);
      gcc_assert (existing_bl == NULL || new_bl == existing_bl);
      return new_bl;
    }

  /* This is the first time we try to read this binding level, allocate a new
     instance, if the caller did not provide one.  If *BLP is NULL, it
     means that the caller does not have a binding level and wants to
     allocate a new one.  */
  *existed_p = false;
  new_bl = (existing_bl) ? existing_bl : ggc_alloc_cleared_cp_binding_level ();
  pph_cache_insert_at (&stream->cache, new_bl, ix, PPH_cp_binding_level);
  return new_bl;
}


/* Read and return a cp_binding_level from STREAM.  This function
   expects to find a reference to a binding level, as it is only
   called after all the binding levels in STREAM have been
   instantiated and merged.  */

static cp_binding_level *
pph_in_binding_level_ref (pph_stream *stream)
{
  cp_binding_level *bl;
  bool existed_p;

  bl = pph_in_binding_level_start (stream, NULL, &existed_p);
  gcc_assert (existed_p);
  return bl;
}


/* Helper for pph_in_cxx_binding.  Read and return a cxx_binding
   instance from STREAM.  */

static cxx_binding *
pph_in_cxx_binding_1 (pph_stream *stream)
{
  struct bitpack_d bp;
  cxx_binding *cb;
  tree value, type;
  enum pph_record_marker marker;
  unsigned ix, image_ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_cxx_binding);
  if (marker == PPH_RECORD_END)
    return NULL;
  else if (pph_is_reference_marker (marker))
    return (cxx_binding *) pph_cache_find (stream, marker, image_ix, ix,
					   PPH_cxx_binding);

  /* Remove if we start emitting merge keys for this structure.  */
  gcc_assert (marker == PPH_RECORD_START);

  value = pph_in_tree (stream);
  type = pph_in_tree (stream);
  ALLOC_AND_REGISTER (&stream->cache, ix, PPH_cxx_binding, cb,
                      cxx_binding_make (value, type));
  cb->scope = pph_in_binding_level_ref (stream);
  bp = pph_in_bitpack (stream);
  cb->value_is_inherited = bp_unpack_value (&bp, 1);
  cb->is_local = bp_unpack_value (&bp, 1);

  return cb;
}


/* Read and return an instance of cxx_binding from STREAM.  */

static cxx_binding *
pph_in_cxx_binding (pph_stream *stream)
{
  cxx_binding *curr, *prev, *cb;

  /* Read the current binding first.  */
  cb = pph_in_cxx_binding_1 (stream);

  /* Read the list of previous bindings.  */
  for (curr = cb; curr; curr = prev)
    {
      prev = pph_in_cxx_binding_1 (stream);
      curr->previous = prev;
    }

  return cb;
}


/* Read all the fields of cp_class_binding instance CB to STREAM.  */

static cp_class_binding *
pph_in_class_binding (pph_stream *stream)
{
  cp_class_binding *cb;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_cp_class_binding);
  if (marker == PPH_RECORD_END)
    return NULL;
  else if (pph_is_reference_marker (marker))
    return (cp_class_binding *) pph_cache_find (stream, marker, image_ix, ix,
						PPH_cp_class_binding);

  /* Remove if we start emitting merge keys for this structure.  */
  gcc_assert (marker == PPH_RECORD_START);

  ALLOC_AND_REGISTER (&stream->cache, ix, PPH_cp_class_binding, cb,
                      ggc_alloc_cleared_cp_class_binding ());
  cb->base = pph_in_cxx_binding (stream);
  cb->identifier = pph_in_tree (stream);

  return cb;
}


/* Read and return an instance of cp_label_binding from STREAM.  */

static cp_label_binding *
pph_in_label_binding (pph_stream *stream)
{
  cp_label_binding *lb;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_cp_label_binding);
  if (marker == PPH_RECORD_END)
    return NULL;
  else if (pph_is_reference_marker (marker))
    return (cp_label_binding *) pph_cache_find (stream, marker, image_ix, ix,
						PPH_cp_label_binding);

  /* Remove if we start emitting merge keys for this structure.  */
  gcc_assert (marker == PPH_RECORD_START);

  ALLOC_AND_REGISTER (&stream->cache, ix, PPH_cp_label_binding, lb,
                      ggc_alloc_cleared_cp_label_binding ());
  lb->label = pph_in_tree (stream);
  lb->prev_value = pph_in_tree (stream);

  return lb;
}


/* Read all the merge keys from STREAM into the cp_binding_level *BLP.  */

static void
pph_in_merge_key_binding_level (pph_stream *stream, cp_binding_level **blp)
{
  unsigned count, num;
  bool existed_p;
  cp_binding_level *bl;

  /* Get a new binding level instance.  If *BLP was not given,
     a new one will be created.  */
  bl = pph_in_binding_level_start (stream, *blp, &existed_p);
  if (!existed_p && *blp == NULL)
    *blp = bl;

  gcc_assert (*blp);

  num = pph_in_hwi (stream);
  for (count = 0; count < num; ++count)
    pph_in_merge_key_namespace_decl (stream, &bl->namespaces);
  pph_in_merge_key_chain (stream, &bl->names);
  pph_in_merge_key_chain (stream, &bl->usings);
  pph_in_merge_key_chain (stream, &bl->using_directives);
}


/* Read all the merge bodies from STREAM into the cp_binding_level BL.  */

static void
pph_in_merge_body_binding_level (pph_stream *stream, cp_binding_level *bl)
{
  unsigned i, count, num;
  cp_binding_level *new_bl;
  struct bitpack_d bp;
  bool existed_p;

  /* We should have already registered BL.  Make sure we are reading a
     reference to it.  */
  new_bl = pph_in_binding_level_start (stream, bl, &existed_p);
  gcc_assert (existed_p && new_bl == bl);

  /* Merge the bodies of BL->NAMESPACES, BL->NAMES, BL->USINGS and
     BL->USING_DIRECTIVES.  */
  num = pph_in_hwi (stream);
  for (count = 0; count < num; ++count)
    pph_in_merge_body_namespace_decl (stream);
  pph_in_merge_body_chain (stream);
  pph_in_merge_body_chain (stream);
  pph_in_merge_body_chain (stream);
  pph_union_into_tree_vec (&bl->static_decls, pph_in_tree_vec (stream));

  /* Read the remaining fields in BL.  FIXME pph: The following is probably too
     aggressive in overwriting.  */
  bl->this_entity = pph_in_tree (stream);

  num = pph_in_uint (stream);
  bl->class_shadowed = NULL;
  for (i = 0; i < num; i++)
    {
      cp_class_binding *cb = pph_in_class_binding (stream);
      VEC_safe_push (cp_class_binding, gc, bl->class_shadowed, cb);
    }

  bl->type_shadowed = pph_in_tree (stream);

  num = pph_in_uint (stream);
  bl->shadowed_labels = NULL;
  for (i = 0; i < num; i++)
    {
      cp_label_binding *sl = pph_in_label_binding (stream);
      VEC_safe_push (cp_label_binding, gc, bl->shadowed_labels, sl);
    }

  bl->blocks = pph_in_tree (stream);
  bl->level_chain = pph_in_binding_level_ref (stream);
  bl->dead_vars_from_for = pph_in_tree_vec (stream);
  bl->statement_list = pph_in_chain (stream);
  bl->binding_depth = pph_in_uint (stream);

  bp = pph_in_bitpack (stream);
  bl->kind = (enum scope_kind) bp_unpack_value (&bp, 4);
  bl->keep = bp_unpack_value (&bp, 1);
  bl->more_cleanups_ok = bp_unpack_value (&bp, 1);
  bl->have_cleanups = bp_unpack_value (&bp, 1);
}


/********************************************************** tree aux classes */


/* Read and return an instance of struct language_function from STREAM.  */

static struct language_function *
pph_in_language_function (pph_stream *stream)
{
  struct bitpack_d bp;
  struct language_function *lf;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_language_function);
  if (marker == PPH_RECORD_END)
    return NULL;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  lf = ggc_alloc_cleared_language_function ();
  lf->base.x_stmt_tree.x_cur_stmt_list = pph_in_tree_vec (stream);
  lf->base.x_stmt_tree.stmts_are_full_exprs_p = pph_in_uint (stream);
  lf->x_cdtor_label = pph_in_tree (stream);
  lf->x_current_class_ptr = pph_in_tree (stream);
  lf->x_current_class_ref = pph_in_tree (stream);
  lf->x_eh_spec_block = pph_in_tree (stream);
  lf->x_in_charge_parm = pph_in_tree (stream);
  lf->x_vtt_parm = pph_in_tree (stream);
  lf->x_return_value = pph_in_tree (stream);
  bp = pph_in_bitpack (stream);
  lf->returns_value = bp_unpack_value (&bp, 1);
  lf->returns_null = bp_unpack_value (&bp, 1);
  lf->returns_abnormally = bp_unpack_value (&bp, 1);
  lf->x_in_function_try_handler = bp_unpack_value (&bp, 1);
  lf->x_in_base_initializer = bp_unpack_value (&bp, 1);
  lf->can_throw = bp_unpack_value (&bp, 1);

  /* FIXME pph.  We are not reading lf->x_named_labels.  */

  lf->bindings = pph_in_binding_level_ref (stream);
  lf->x_local_names = pph_in_tree_vec (stream);

  /* FIXME pph.  We are not reading lf->extern_decl_map.  */

  return lf;
}


/* Read applicable fields of struct function from STREAM.  Associate
   the read structure to DECL.  */

static void
pph_in_struct_function (pph_stream *stream, tree decl)
{
  size_t count, i;
  unsigned image_ix, ix;
  enum pph_record_marker marker;
  struct function *fn;
  tree t;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_function);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  /* Possibly allocate a new DECL_STRUCT_FUNCTION for DECL.  */
  t = pph_in_tree (stream);
  gcc_assert (t == decl);
  fn = DECL_STRUCT_FUNCTION (decl);
  if (fn == NULL)
    {
      /* The DECL_STRUCT_FUNCTION does not already already exists,
         which implies that we are reading an entirely new function
         and not merging into an existing function.  */
      allocate_struct_function (decl, false);
      fn = DECL_STRUCT_FUNCTION (decl);
    }

  /* FIXME pph: For now, accept the new version of the fields when merging.  */
  input_struct_function_base (fn, stream->encoder.r.data_in,
			      stream->encoder.r.ib);

  /* struct eh_status *eh;					-- zero init */
  /* struct control_flow_graph *cfg;				-- zero init */
  /* struct gimple_seq_d *gimple_body;				-- zero init */
  /* struct gimple_df *gimple_df;				-- zero init */
  /* struct loops *x_current_loops;				-- zero init */
  /* struct stack_usage *su;					-- zero init */
  /* htab_t value_histograms;					-- zero init */
  /* tree decl;							-- zero init */
  /* tree static_chain_decl;					-- in base */
  /* tree nonlocal_goto_save_area;				-- in base */
  /* tree local_decls;						-- in base */
  /* struct machine_function * machine;				-- zero init */

  fn->language = pph_in_language_function (stream);

  count = pph_in_uint (stream);
  if ( count > 0 )
    {
      fn->used_types_hash = htab_create_ggc (37, htab_hash_pointer,
					     htab_eq_pointer, NULL);
      for (i = 0; i < count;  i++)
	{
	  void **slot;
	  tree type = pph_in_tree (stream);
	  slot = htab_find_slot (fn->used_types_hash, type, INSERT);
	  if (*slot == NULL)
	    *slot = type;
	}
    }
  /* else zero initialized */

  /* int last_stmt_uid;						-- zero init */
  /* int funcdef_no;						-- zero init */
  /* location_t function_start_locus;				-- in base */
  /* location_t function_end_locus;				-- in base */
  /* unsigned int curr_properties;				-- in base */
  /* unsigned int last_verified;				-- zero init */
  /* const char *cannot_be_copied_reason;			-- zero init */

  /* unsigned int va_list_gpr_size : 8;				-- in base */
  /* unsigned int va_list_fpr_size : 8;				-- in base */
  /* unsigned int calls_setjmp : 1;				-- in base */
  /* unsigned int calls_alloca : 1;				-- in base */
  /* unsigned int has_nonlocal_label : 1;			-- in base */
  /* unsigned int cannot_be_copied_set : 1;			-- zero init */
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


/* Read all fields in lang_decl_base instance LDB from STREAM.  */

static void
pph_in_ld_base (pph_stream *stream, struct lang_decl_base *ldb)
{
  struct bitpack_d bp;

  bp = pph_in_bitpack (stream);
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


/* Merge all fields into lang_decl_base instance LDB from STREAM.  */

static void
pph_in_merge_ld_base (pph_stream *stream, struct lang_decl_base *ldb)
{
  struct bitpack_d bp;

  bp = pph_in_bitpack (stream);
  /* FIXME pph: At present, we are only merging the anticipated and odr_used
     bits.  The rest are simply overwritten.  */
  ldb->selector = bp_unpack_value (&bp, 16);
  ldb->language = (enum languages) bp_unpack_value (&bp, 4);
  ldb->use_template = bp_unpack_value (&bp, 2);
  ldb->not_really_extern |= bp_unpack_value (&bp, 1);
  ldb->initialized_in_class = bp_unpack_value (&bp, 1);
  ldb->repo_available_p = bp_unpack_value (&bp, 1);
  ldb->threadprivate_or_deleted_p = bp_unpack_value (&bp, 1);
  ldb->anticipated_p &= bp_unpack_value (&bp, 1);
  ldb->friend_attr = bp_unpack_value (&bp, 1);
  ldb->template_conv_p = bp_unpack_value (&bp, 1);
  ldb->odr_used |= bp_unpack_value (&bp, 1);
  ldb->u2sel = bp_unpack_value (&bp, 1);
}


/* Read all the fields in lang_decl_min instance LDM from STREAM.  */

static void
pph_in_ld_min (pph_stream *stream, struct lang_decl_min *ldm)
{
  ldm->template_info = pph_in_tree (stream);
  if (ldm->base.u2sel == 0)
    ldm->u2.access = pph_in_tree (stream);
  else if (ldm->base.u2sel == 1)
    ldm->u2.discriminator = pph_in_uint (stream);
  else
    gcc_unreachable ();
}


/* Read all the fields of lang_decl_fn instance LDF from STREAM.  */

static void
pph_in_ld_fn (pph_stream *stream, struct lang_decl_fn *ldf)
{
  struct bitpack_d bp;

  /* Read all the fields in lang_decl_min.  */
  pph_in_ld_min (stream, &ldf->min);

  bp = pph_in_bitpack (stream);
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

  ldf->befriending_classes = pph_in_tree (stream);
  ldf->context = pph_in_tree (stream);

  if (ldf->thunk_p == 0)
    ldf->u5.cloned_function = pph_in_tree (stream);
  else if (ldf->thunk_p == 1)
    ldf->u5.fixed_offset = pph_in_uint (stream);
  else
    gcc_unreachable ();

  if (ldf->pending_inline_p == 1)
    ldf->u.pending_inline_info = pph_in_token_cache (stream);
  else if (ldf->pending_inline_p == 0)
    ldf->u.saved_language_function = pph_in_language_function (stream);
}


/* Read all the fields of lang_decl_ns instance LDNS from STREAM.  */

static void
pph_in_ld_ns (pph_stream *stream, struct lang_decl_ns *ldns)
{
  ldns->level = pph_in_binding_level_ref (stream);
}


/* Read all the fields of lang_decl_parm instance LDP from STREAM.  */

static void
pph_in_ld_parm (pph_stream *stream, struct lang_decl_parm *ldp)
{
  ldp->level = pph_in_uint (stream);
  ldp->index = pph_in_uint (stream);
}


/* Read language specific data in DECL from STREAM.  */

static void
pph_in_lang_decl (pph_stream *stream, tree decl)
{
  struct lang_decl *ld;
  struct lang_decl_base *ldb;
  bool is_merge;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_lang_decl);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);


  is_merge = true;
  if (DECL_LANG_SPECIFIC (decl) == NULL)
    {
      is_merge = false;
      retrofit_lang_decl (decl);
    }

  ld = DECL_LANG_SPECIFIC (decl);

  /* Read all the fields in lang_decl_base.  */
  ldb = &ld->u.base;
  if (is_merge)
    pph_in_merge_ld_base (stream, ldb);
  else
    pph_in_ld_base (stream, ldb);

  if (ldb->selector == 0)
    {
      /* Read all the fields in lang_decl_min.  */
      pph_in_ld_min (stream, &ld->u.min);
    }
  else if (ldb->selector == 1)
    {
      /* Read all the fields in lang_decl_fn.  */
      pph_in_ld_fn (stream, &ld->u.fn);
    }
  else if (ldb->selector == 2)
    {
      /* Read all the fields in lang_decl_ns.  */
      pph_in_ld_ns (stream, &ld->u.ns);
    }
  else if (ldb->selector == 3)
    {
      /* Read all the fields in lang_decl_parm.  */
      pph_in_ld_parm (stream, &ld->u.parm);
    }
  else
    gcc_unreachable ();
}


/********************************************************* tree base classes */


/* Stream in the language-independent parts of EXPR's body.  */

static void
pph_in_lang_indep_tree_body (pph_stream *stream, tree expr)
{
  struct lto_input_block *ib = stream->encoder.r.ib;
  struct data_in *data_in = stream->encoder.r.data_in;
  streamer_read_tree_body (ib, data_in, expr);
}


/* Merge in from STREAM the language-independent parts of EXPR's body.
   This merging operates in three phases: (1) fields that need to be
   preserved are saved, (2) the new body for EXPR is read-in, (3)
   preserved fields are restored back into EXPR.

   This three stage merging is needed because the generic tree reader
   is not merge-aware.  */

static void
pph_in_merge_lang_indep_tree_body (pph_stream *stream, tree expr)
{
  enum tree_code code = TREE_CODE (expr);
  bool decl_comdat = false;
  tree decl_comdat_group = NULL;
  bool decl_declared_inline = false;
  tree decl_result = NULL;
  tree decl_arguments = NULL;

  /* Preserve some fields which should not be overwritten by the DECL
     that we are merging in.  */
  if (CODE_CONTAINS_STRUCT (code, TS_DECL_WITH_VIS))
    {
      decl_comdat = DECL_COMDAT (expr);
      decl_comdat_group = DECL_COMDAT_GROUP (expr);
      /* FIXME pph: These too?
	 bool decl_common = DECL_COMMON (expr);
	 bool decl_weak = DECL_WEAK (expr);
	 enum symbol_visibility decl_visibility = DECL_VISIBILITY (expr);
	 bool decl_visibility_specified = DECL_VISIBILITY_SPECIFIED (expr);
	 tree decl_section_name = DECL_SECTION_NAME (expr);
      */
    }

  if (CODE_CONTAINS_STRUCT (code, TS_FUNCTION_DECL))
    {
      decl_declared_inline = DECL_DECLARED_INLINE_P (expr);

      /* DECL_RESULT and DECL_ARGUMENTS are used inside the body
	 of a function.  Preserve them only if EXPR already has a
	 body.  */
      if (DECL_SAVED_TREE (expr))
	{
	  decl_result = DECL_RESULT (expr);
	  decl_arguments = DECL_ARGUMENTS (expr);
	}
    }

  /* FIXME pph: Also see the functions below for more potential fields.
	unpack_ts_decl_with_vis_value_fields
	lto_input_ts_decl_with_vis_tree_pointers
	unpack_ts_function_decl_value_fields
	lto_input_ts_decl_non_common_tree_pointers
  */

  /* Read the body of the new decl (this overwrites all the fields of
     EXPR).  */
  pph_in_lang_indep_tree_body (stream, expr);

  /* Restore values that we do not want overwritten.  */
  if (CODE_CONTAINS_STRUCT (code, TS_DECL_WITH_VIS))
    {
      decl_comdat ? DECL_COMDAT (expr) = true : 0;
      decl_comdat_group ? DECL_COMDAT_GROUP (expr) = decl_comdat_group : 0;
      /* FIXME pph: These too?
	 decl_common ? DECL_COMMON (expr) = true : 0;
	 decl_weak ? DECL_WEAK (expr) = true : 0;
	 decl_visibility ? DECL_VISIBILITY (expr) = decl_visibility : 0;
	 decl_visibility_specified ? DECL_VISIBILITY_SPECIFIED (expr) = true : 0;
	 decl_section_name ? DECL_SECTION_NAME (expr) = decl_section_name : 0;
      */
    }

  if (CODE_CONTAINS_STRUCT (code, TS_FUNCTION_DECL))
    {
      decl_declared_inline ? DECL_DECLARED_INLINE_P (expr) = true : 0;

      /* DECL_RESULT and DECL_ARGUMENTS are used inside the body
	 of a function.  Preserve them only if EXPR already has a
	 body.  */
      if (DECL_SAVED_TREE (expr))
	{
	  decl_result ? DECL_RESULT (expr) = decl_result : 0;
	  decl_arguments ? DECL_ARGUMENTS (expr) = decl_arguments : 0;
	}
    }
}


/* Read in the tree_common fields.  */

static void
pph_in_tree_common (pph_stream *stream, tree t)
{
  /* The 'struct tree_typed typed' base class is handled in LTO.  */
  TREE_CHAIN (t) = pph_in_tree (stream);
}


/* Read all the fields in lang_type_header instance LTH from STREAM.  */

static void
pph_in_lang_type_header (pph_stream *stream, struct lang_type_header *lth)
{
  struct bitpack_d bp;

  bp = pph_in_bitpack (stream);
  lth->is_lang_type_class = bp_unpack_value (&bp, 1);
  lth->has_type_conversion = bp_unpack_value (&bp, 1);
  lth->has_copy_ctor = bp_unpack_value (&bp, 1);
  lth->has_default_ctor = bp_unpack_value (&bp, 1);
  lth->const_needs_init = bp_unpack_value (&bp, 1);
  lth->ref_needs_init = bp_unpack_value (&bp, 1);
  lth->has_const_copy_assign = bp_unpack_value (&bp, 1);
}


/* Read a struct sorted_fields_type instance SFT to STREAM.  */

static struct sorted_fields_type *
pph_in_sorted_fields_type (pph_stream *stream)
{
  unsigned i, num_fields;
  struct sorted_fields_type *v;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_sorted_fields_type);
  if (marker == PPH_RECORD_END)
    return NULL;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  num_fields = pph_in_uint (stream);
  v = sorted_fields_type_new (num_fields);
  for (i = 0; i < num_fields; i++)
    v->elts[i] = pph_in_tree (stream);

  return v;
}


/* Read and merge the bits in lang_type_class instance LTC to STREAM.  */

static void
pph_in_merge_lang_type_class_bits (pph_stream *stream,
				   struct lang_type_class *ltc)
{
  struct bitpack_d bp;

  ltc->align = pph_in_uchar (stream);

  bp = pph_in_bitpack (stream);
  ltc->has_mutable |= bp_unpack_value (&bp, 1);
  ltc->com_interface |= bp_unpack_value (&bp, 1);
  ltc->non_pod_class |= bp_unpack_value (&bp, 1);
  ltc->nearly_empty_p |= bp_unpack_value (&bp, 1);
  ltc->user_align |= bp_unpack_value (&bp, 1);
  ltc->has_copy_assign |= bp_unpack_value (&bp, 1);
  ltc->has_new |= bp_unpack_value (&bp, 1);
  ltc->has_array_new |= bp_unpack_value (&bp, 1);
  ltc->gets_delete |= bp_unpack_value (&bp, 2);
  ltc->interface_only |= bp_unpack_value (&bp, 1);
  ltc->interface_unknown |= bp_unpack_value (&bp, 1);
  ltc->contains_empty_class_p |= bp_unpack_value (&bp, 1);
  ltc->anon_aggr |= bp_unpack_value (&bp, 1);
  ltc->non_zero_init |= bp_unpack_value (&bp, 1);
  ltc->empty_p |= bp_unpack_value (&bp, 1);
  ltc->vec_new_uses_cookie |= bp_unpack_value (&bp, 1);
  ltc->declared_class |= bp_unpack_value (&bp, 1);
  ltc->diamond_shaped |= bp_unpack_value (&bp, 1);
  ltc->repeated_base |= bp_unpack_value (&bp, 1);
  ltc->being_defined |= bp_unpack_value (&bp, 1);
  ltc->java_interface |= bp_unpack_value (&bp, 1);
  ltc->debug_requested |= bp_unpack_value (&bp, 1);
  ltc->fields_readonly |= bp_unpack_value (&bp, 1);
  ltc->use_template |= bp_unpack_value (&bp, 2);
  ltc->ptrmemfunc_flag |= bp_unpack_value (&bp, 1);
  ltc->was_anonymous |= bp_unpack_value (&bp, 1);
  ltc->lazy_default_ctor |= bp_unpack_value (&bp, 1);
  ltc->lazy_copy_ctor |= bp_unpack_value (&bp, 1);
  ltc->lazy_copy_assign |= bp_unpack_value (&bp, 1);
  ltc->lazy_destructor |= bp_unpack_value (&bp, 1);
  ltc->has_const_copy_ctor |= bp_unpack_value (&bp, 1);
  ltc->has_complex_copy_ctor |= bp_unpack_value (&bp, 1);
  ltc->has_complex_copy_assign |= bp_unpack_value (&bp, 1);
  ltc->non_aggregate |= bp_unpack_value (&bp, 1);
  ltc->has_complex_dflt |= bp_unpack_value (&bp, 1);
  ltc->has_list_ctor |= bp_unpack_value (&bp, 1);
  ltc->non_std_layout |= bp_unpack_value (&bp, 1);
  ltc->is_literal |= bp_unpack_value (&bp, 1);
  ltc->lazy_move_ctor |= bp_unpack_value (&bp, 1);
  ltc->lazy_move_assign |= bp_unpack_value (&bp, 1);
  ltc->has_complex_move_ctor |= bp_unpack_value (&bp, 1);
  ltc->has_complex_move_assign |= bp_unpack_value (&bp, 1);
  ltc->has_constexpr_ctor |= bp_unpack_value (&bp, 1);
}

/* Read all the fields in lang_type_class instance LTC from STREAM.  */

static void
pph_in_lang_type_class (pph_stream *stream, struct lang_type_class *ltc)
{
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  pph_in_merge_lang_type_class_bits (stream, ltc);
  /* We are merging even for new classes,
     because they have been allocated to zero.  */

  ltc->primary_base = pph_in_tree (stream);
  ltc->vcall_indices = pph_in_tree_pair_vec (stream);
  ltc->vtables = pph_in_tree (stream);
  ltc->typeinfo_var = pph_in_tree (stream);
  ltc->vbases = pph_in_tree_vec (stream);

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_binding_table);
  if (marker == PPH_RECORD_START_NO_CACHE)
    ltc->nested_udts = pph_in_binding_table (stream);
  else
    {
      gcc_assert (marker == PPH_RECORD_END);
      ltc->nested_udts = NULL;
    }

  ltc->as_base = pph_in_tree (stream);
  ltc->pure_virtuals = pph_in_tree_vec (stream);
  ltc->friend_classes = pph_in_tree (stream);
  ltc->methods = pph_in_tree_vec (stream);
  ltc->key_method = pph_in_tree (stream);
  ltc->decl_list = pph_in_tree (stream);
  ltc->template_info = pph_in_tree (stream);
  ltc->befriending_classes = pph_in_tree (stream);
  ltc->objc_info = pph_in_tree (stream);
  ltc->sorted_fields = pph_in_sorted_fields_type (stream);
  ltc->lambda_expr = pph_in_tree (stream);
}


/* Merge all the fields in lang_type_class instance LTC from STREAM.  */

static void
pph_in_merge_key_lang_type_class (pph_stream *stream,
				  struct lang_type_class *ltc)
{
  pph_in_merge_lang_type_class_bits (stream, ltc);
}


/* Read all fields of struct lang_type_ptrmem instance LTP from STREAM.  */

static void
pph_in_lang_type_ptrmem (pph_stream *stream,
				  struct lang_type_ptrmem *ltp)
{
  ltp->record = pph_in_tree (stream);
}


/* Read all the fields in struct lang_type from STREAM.  */

static struct lang_type *
pph_in_lang_type (pph_stream *stream)
{
  struct lang_type *lt;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_lang_type);
  if (marker == PPH_RECORD_END)
    return NULL;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  lt = ggc_alloc_cleared_lang_type (sizeof (struct lang_type));

  pph_in_lang_type_header (stream, &lt->u.h);
  if (lt->u.h.is_lang_type_class)
    pph_in_lang_type_class (stream, &lt->u.c);
  else
    pph_in_lang_type_ptrmem (stream, &lt->u.ptrmem);

  return lt;
}


/* Read merge keys in struct lang_type from STREAM.  */

static void
pph_in_merge_key_lang_type (pph_stream *stream, struct lang_type **lt_p)
{
  enum pph_record_marker marker;
  unsigned image_ix, ix;
  struct lang_type *lt = *lt_p;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_lang_type);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  if (! lt)
    {
      /* There is no prior lang_type, hence no merging, so allocate it.  */
      lt = ggc_alloc_cleared_lang_type (sizeof (struct lang_type));
      *lt_p = lt;
    }

  pph_in_lang_type_header (stream, &lt->u.h);
  gcc_assert (lt->u.h.is_lang_type_class);
  pph_in_merge_key_lang_type_class (stream, &lt->u.c);
}


/* Read from STREAM an enum machine_mode value.  */

static enum machine_mode
pph_in_machine_mode (pph_stream *stream)
{
  struct lto_input_block *ib = stream->encoder.r.ib;
  return streamer_read_enum (ib, machine_mode, NUM_MACHINE_MODES);
}


/* Read from STREAM the body of tcc_type node TYPE.  */

static void
pph_in_tcc_type (pph_stream *stream, tree type)
{
  TYPE_LANG_SPECIFIC (type) = pph_in_lang_type (stream);
  TYPE_POINTER_TO (type) = pph_in_tree (stream);
  if (TREE_CODE (type) == POINTER_TYPE)
    TYPE_NEXT_PTR_TO (type) = pph_in_tree (stream);
  TYPE_REFERENCE_TO (type) = pph_in_tree (stream);
  TYPE_NEXT_VARIANT (type) = pph_in_tree (stream);
  SET_TYPE_MODE (type, pph_in_machine_mode (stream));
  /* We do not read TYPE_CANONICAL.  Instead, we emit the table of
     canonical types and re-instantiate it on read.  */
  TREE_CHAIN (type) = pph_in_tree (stream);

  /* The type values cache is built as constants are instantiated,
     so we only stream it on the nodes that use it for
     other purposes.  */
  switch (TREE_CODE (type))
    {
    case BOUND_TEMPLATE_TEMPLATE_PARM:
    case DECLTYPE_TYPE:
    case TEMPLATE_TEMPLATE_PARM:
    case TEMPLATE_TYPE_PARM:
    case TYPENAME_TYPE:
    case TYPEOF_TYPE:
      TYPE_VALUES_RAW (type) = pph_in_tree (stream);
      break;

    default:
      break;
    }
}


/* Read from STREAM the body of tcc_declaration tree DECL.  */

static void
pph_in_tcc_declaration (pph_stream *stream, tree decl)
{
  pph_in_lang_decl (stream, decl);
  DECL_INITIAL (decl) = pph_in_tree (stream);
  DECL_ABSTRACT_ORIGIN (decl) = pph_in_tree (stream);

  /* Handle some individual decl nodes.  */
  switch (TREE_CODE (decl))
    {
      /* The tree streamer only writes TREE_CHAIN for PARM_DECL nodes.
         We need to read TREE_CHAIN for variables and functions because
         they are sometimes chained together in places other than regular
         tree chains.  For example in BINFO_VTABLEs, the decls are chained
         together).  */

    case VAR_DECL:
      TREE_CHAIN (decl) = pph_in_tree (stream);
      break;

    case FUNCTION_DECL:
      TREE_CHAIN (decl) = pph_in_tree (stream);
      DECL_SAVED_TREE (decl) = pph_in_tree (stream);
      break;

    case TYPE_DECL:
      DECL_ORIGINAL_TYPE (decl) = pph_in_tree (stream);
      break;

    case TEMPLATE_DECL:
      DECL_TEMPLATE_RESULT (decl) = pph_in_tree (stream);
      DECL_TEMPLATE_PARMS (decl) = pph_in_tree (stream);
      break;

    default:
      break;
    }
}


/* Read a tree from STREAM, and if non-null, assign to *FIELD.  */

static void
pph_in_nonnull_tree (tree *field, pph_stream *stream)
{
  tree candidate;
  candidate = pph_in_tree (stream);
  if (candidate && *field == NULL)
    *field = candidate;
}


/* Read from STREAM and merge the body of tcc_declaration tree DECL.  */

static void
pph_in_merge_tcc_declaration (pph_stream *stream, tree decl)
{
  pph_in_merge_lang_indep_tree_body (stream, decl);
  pph_in_lang_decl (stream, decl);

  /* FIXME pph: Some of the following may not be necessary.  */
  pph_in_nonnull_tree (&DECL_INITIAL (decl), stream);
  pph_in_nonnull_tree (&DECL_ABSTRACT_ORIGIN (decl), stream);

  /* Handle some individual decl nodes.  */
  switch (TREE_CODE (decl))
    {
      /* The tree streamer only writes TREE_CHAIN for PARM_DECL nodes.
         We need to read TREE_CHAIN for variables and functions because
         they are sometimes chained together in places other than regular
         tree chains.  For example in BINFO_VTABLEs, the decls are chained
         together).  However, in merging, we will actually ignore the new
	 chain, and thus not overwrite the previous one.  */

    case VAR_DECL:
      /* ignore TREE_CHAIN (decl) = */ pph_in_tree (stream);
      break;

    case FUNCTION_DECL:
      /* ignore TREE_CHAIN (decl) = */ pph_in_tree (stream);
      pph_in_nonnull_tree (&DECL_SAVED_TREE (decl), stream);
      break;

    case TYPE_DECL:
      DECL_ORIGINAL_TYPE (decl) = pph_in_tree (stream);
      break;

    case TEMPLATE_DECL:
      DECL_TEMPLATE_RESULT (decl) = pph_in_tree (stream);
      DECL_TEMPLATE_PARMS (decl) = pph_in_tree (stream);
      break;

    default:
      break;
    }
}


/******************************************************** tree head and body */


/* Stream in identifier bindings.  */

static void
pph_in_identifier_bindings (pph_stream *stream, tree expr)
{
  if (flag_pph_debug >= 3)
    fprintf (pph_logfile, "PPH: in identifier %s\n", IDENTIFIER_POINTER (expr));
/* FIXME pph: Writing bindings is causing trouble,
   but not writing them is not yet working.  */
  REAL_IDENTIFIER_TYPE_VALUE (expr) = pph_in_tree (stream);
}


/* Read the body fields of EXPR from STREAM.  */

static void
pph_in_tree_body (pph_stream *stream, tree expr)
{
  bool handled_p;

  pph_in_lang_indep_tree_body (stream, expr);

  /* Handle common tree code classes first.  */
  handled_p = true;
  switch (TREE_CODE_CLASS (TREE_CODE (expr)))
    {
      case tcc_declaration:
	pph_in_tcc_declaration (stream, expr);
	break;

      case tcc_type:
	pph_in_tcc_type (stream, expr);
	break;

      case tcc_constant:
	if (TREE_CODE (expr) == PTRMEM_CST)
	  {
	    pph_in_tree_common (stream, expr);
	    PTRMEM_CST_MEMBER (expr) = pph_in_tree (stream);
	  }
	break;

      case tcc_expression:
      case tcc_unary:
      case tcc_binary:
      case tcc_vl_exp:
      case tcc_reference:
      case tcc_comparison:
      case tcc_statement:
	/* These tree classes are completely handled by the tree streamer.  */
	break;

      default:
	handled_p = false;
	break;
    }

  /* If we've already handled the tree, we are done.  */
  if (handled_p)
    return;

  /* Only tcc_exceptional tree codes are left to handle.  */
  gcc_assert (TREE_CODE_CLASS (TREE_CODE (expr)) == tcc_exceptional);

  switch (TREE_CODE (expr))
    {
    case STATEMENT_LIST:
      {
        HOST_WIDE_INT i, num_trees = pph_in_uint (stream);
        for (i = 0; i < num_trees; i++)
	  {
	    tree stmt = pph_in_tree (stream);
	    append_to_statement_list_force (stmt, &expr);
	  }
      }
      break;

    case OVERLOAD:
      pph_in_tree_common (stream, expr);
      OVL_FUNCTION (expr) = pph_in_tree (stream);
      break;

    case IDENTIFIER_NODE:
      pph_in_identifier_bindings (stream, expr);
      break;

    case BASELINK:
      pph_in_tree_common (stream, expr);
      BASELINK_BINFO (expr) = pph_in_tree (stream);
      BASELINK_FUNCTIONS (expr) = pph_in_tree (stream);
      BASELINK_ACCESS_BINFO (expr) = pph_in_tree (stream);
      break;

    case TEMPLATE_INFO:
      pph_in_tree_common (stream, expr);
      TI_TYPEDEFS_NEEDING_ACCESS_CHECKING (expr)
          = pph_in_qual_use_vec (stream);
      break;

    case DEFAULT_ARG:
      pph_in_tree_common (stream, expr);
      DEFARG_TOKENS (expr) = pph_in_token_cache (stream);
      DEFARG_INSTANTIATIONS (expr) = pph_in_tree_vec (stream);
      break;

    case STATIC_ASSERT:
      pph_in_tree_common (stream, expr);
      STATIC_ASSERT_CONDITION (expr) = pph_in_tree (stream);
      STATIC_ASSERT_MESSAGE (expr) = pph_in_tree (stream);
      STATIC_ASSERT_SOURCE_LOCATION (expr) = pph_in_location (stream);
      break;

    case ARGUMENT_PACK_SELECT:
      pph_in_tree_common (stream, expr);
      ARGUMENT_PACK_SELECT_FROM_PACK (expr) = pph_in_tree (stream);
      ARGUMENT_PACK_SELECT_INDEX (expr) = pph_in_uint (stream);
      break;

    case TRAIT_EXPR:
      pph_in_tree_common (stream, expr);
      TRAIT_EXPR_TYPE1 (expr) = pph_in_tree (stream);
      TRAIT_EXPR_TYPE2 (expr) = pph_in_tree (stream);
      TRAIT_EXPR_KIND (expr) = (enum cp_trait_kind) pph_in_uint (stream);
      break;

    case LAMBDA_EXPR:
      pph_in_tree_common (stream, expr);
      LAMBDA_EXPR_LOCATION (expr) = pph_in_location (stream);
      LAMBDA_EXPR_CAPTURE_LIST (expr) = pph_in_tree (stream);
      LAMBDA_EXPR_THIS_CAPTURE (expr) = pph_in_tree (stream);
      LAMBDA_EXPR_RETURN_TYPE (expr) = pph_in_tree (stream);
      LAMBDA_EXPR_EXTRA_SCOPE (expr) = pph_in_tree (stream);
      LAMBDA_EXPR_DISCRIMINATOR (expr) = pph_in_uint (stream);
      break;

    case TREE_VEC:
      /* TREE_VECs hold template argument lists.  */
      NON_DEFAULT_TEMPLATE_ARGS_COUNT (expr) = pph_in_tree (stream);
      break;

    case PLACEHOLDER_EXPR:
      TREE_TYPE (expr) = pph_in_tree (stream);
      break;

    case TEMPLATE_PARM_INDEX:
      pph_in_tree_common (stream, expr);
      TEMPLATE_PARM_IDX (expr) = pph_in_uint (stream);
      TEMPLATE_PARM_LEVEL (expr) = pph_in_uint (stream);
      TEMPLATE_PARM_ORIG_LEVEL (expr) = pph_in_uint (stream);
      TEMPLATE_PARM_NUM_SIBLINGS (expr) = pph_in_uint (stream);
      TEMPLATE_PARM_DECL (expr) = pph_in_tree (stream);
      break;

    case DEFERRED_NOEXCEPT:
      DEFERRED_NOEXCEPT_PATTERN (expr) = pph_in_tree (stream);
      DEFERRED_NOEXCEPT_ARGS (expr) = pph_in_tree (stream);
      break;

    /* TREES ALREADY HANDLED */
    case ERROR_MARK:
    case TREE_LIST:
    case BLOCK:
    case CONSTRUCTOR:
    case SSA_NAME:
    case TREE_BINFO:
      break;

    /* TREES UNIMPLEMENTED */
    case OMP_CLAUSE:
    case OPTIMIZATION_NODE:
    case TARGET_OPTION_NODE:
      fatal_error ("PPH: unimplemented tree node '%s'",
		   pph_tree_code_text (TREE_CODE (expr)));
      break;

    /* TREES UNRECOGNIZED */
    default:
      fatal_error ("PPH: unrecognized tree node '%s'",
                   pph_tree_code_text (TREE_CODE (expr)));
    }
}


/* Read the body fields of EXPR for merging from STREAM.  */

static void
pph_in_merge_tree_body (pph_stream *stream, tree expr)
{
  /* Handle common tree code classes first.  */
  switch (TREE_CODE_CLASS (TREE_CODE (expr)))
    {
      case tcc_declaration:
	pph_in_merge_tcc_declaration (stream, expr);
	break;

      case tcc_type:
	pph_in_lang_indep_tree_body (stream, expr);
	pph_in_tcc_type (stream, expr);
	break;

      default:
	fatal_error ("PPH: unrecognized tree node '%s'",
		     pph_tree_code_text (TREE_CODE (expr)));
	break;
    }
}


/* Unpack language-dependent bitfields from BP into EXPR.  */

static void
pph_unpack_value_fields (struct bitpack_d *bp, tree expr)
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


/* Read a tree header from STREAM and allocate a memory instance for it.
   Return the new tree.  If the whole tree fits in the header, the
   caller does not need to read anything else.  In that case,
   *FULLY_READ_P will be set to true on return from this function.
   Otherwise, it will be set to false.  */

static tree
pph_in_tree_header (pph_stream *stream, bool *fully_read_p)
{
  struct lto_input_block *ib = stream->encoder.r.ib;
  struct data_in *data_in = stream->encoder.r.data_in;
  struct bitpack_d bp;
  tree expr;
  enum tree_code code;
  enum LTO_tags tag;

  tag = streamer_read_record_start (ib);
  gcc_assert ((unsigned) tag < (unsigned) LTO_NUM_TAGS);

  /* Some trees are completely contained in the header,
     deal with them first.  */
  if (tag == LTO_builtin_decl)
    {
      /* If we are going to read a built-in function, all we need is
	  the code and class.  */
      expr = streamer_get_builtin_tree (ib, data_in);
      *fully_read_p = true;
      return expr;
    }
  else if (tag == lto_tree_code_to_tag (INTEGER_CST))
    {
      /* For integer constants we only need the type and its hi/low
	  words.  */
      *fully_read_p = true;
      return streamer_read_integer_cst (ib, data_in);
    }

  /* Allocate the tree.  Handle C++-specific codes first.  */
  code = lto_tag_to_tree_code (tag);
  if (code == AGGR_INIT_EXPR)
    {
      unsigned nargs = pph_in_uint (stream);
      expr = build_vl_exp (AGGR_INIT_EXPR, nargs + 3);
    }
  else
    expr = streamer_alloc_tree (ib, data_in, tag);

  /* Read the language-independent bitfields for EXPR.  */
  bp = streamer_read_tree_bitfields (ib, expr);

  /* Unpack all language-dependent bitfields.  */
  pph_unpack_value_fields (&bp, expr);

  /* The reader should continue reading the body of EXPR.  */
  *fully_read_p = false;

  return expr;
}


/* Read all the merge keys for the names under namespace DECL from
   STREAM.  */

static void
pph_in_merge_key_namespace_decl (pph_stream *stream, tree *chain)
{
  bool is_namespace_alias, fully_read_p;
  unsigned image_ix, ix;
  enum pph_record_marker marker;
  tree read_decl = NULL, decl = NULL;
  const char *name;
  tree name_id;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_any_tree);
  gcc_assert (marker != PPH_RECORD_END);
  if (pph_is_reference_marker (marker))
    decl = (tree) pph_cache_find (stream, marker, image_ix, ix, PPH_any_tree);
  else
    gcc_assert (marker == PPH_RECORD_START_MERGE_KEY);

  gcc_assert (decl == NULL || TREE_CODE (decl) == NAMESPACE_DECL);

  read_decl = pph_in_tree_header (stream, &fully_read_p);
  gcc_assert (!fully_read_p && TREE_CODE (read_decl) == NAMESPACE_DECL);
  name = pph_in_string (stream);

  if (!decl)
    {
      /* The record is new, so we need to link it in.  */

      /* If we are merging into an existing CHAIN.  Look for a match in
         CHAIN to READ_DECL's header.  If we found a match, DECL will be
         the existing tree that matches READ_DECL. Otherwise, DECL is the
         newly allocated READ_DECL.  */
      decl = (chain) ? pph_merge_into_chain (read_decl, name, chain)
		     : read_decl;
      gcc_assert (decl != NULL);

      pph_cache_insert_at (&stream->cache, decl, ix,
			   pph_tree_code_to_tag (decl));
    }

  if (flag_pph_tracer)
    pph_trace_tree (decl, name, pph_trace_front, pph_trace_unmerged_key);

  name_id = pph_in_tree (stream);
  if (decl == read_decl)
    {
      DECL_NAME (decl) = name_id;
      TREE_TYPE (decl) = void_type_node;
    }

  /* If EXPR is a namespace alias, we do not need to merge
     its binding level (namespaces aliases do not have a
     binding level, they use the one from the namespace they
     alias).  */
  is_namespace_alias = pph_in_bool (stream);
  if (!is_namespace_alias)
    {
      /* If this is the first time we see DECL, it will not have a decl_lang
	 structure associated with it.  Create it, if needed.  */
      if (!DECL_LANG_SPECIFIC (decl))
	retrofit_lang_decl (decl);
      pph_in_merge_key_binding_level (stream, &NAMESPACE_LEVEL (decl));
    }

  if (flag_pph_tracer)
    pph_trace_tree (decl, name, pph_trace_back, pph_trace_unmerged_key);
}


/* Read all the merge bodies for the names under namespace DECL from
   STREAM.  */

static void
pph_in_merge_body_namespace_decl (pph_stream *stream)
{
  bool is_namespace_alias, fully_read_p;
  unsigned image_ix, ix;
  enum pph_record_marker marker;
  tree read_decl = NULL, decl = NULL;

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_any_tree);
  gcc_assert (marker != PPH_RECORD_END);
  if (pph_is_reference_marker (marker))
    decl = (tree) pph_cache_find (stream, marker, image_ix, ix, PPH_any_tree);
  else
    gcc_assert (marker == PPH_RECORD_START_MERGE_BODY
		|| marker == PPH_RECORD_START_MUTATED);

  /* If we read a DECL, it must be a NAMESPACE_DECL.  */
  gcc_assert (decl == NULL || TREE_CODE (decl) == NAMESPACE_DECL);

  read_decl = pph_in_tree_header (stream, &fully_read_p);
  gcc_assert (!fully_read_p && TREE_CODE (read_decl) == NAMESPACE_DECL);
  if (!decl)
    {
      if (marker == PPH_RECORD_START_MUTATED)
        {
          /* When reading a mutated tree, we only need to re-read its
             body, the tree itself is already in the cache for another
             PPH image.  */
          decl = (tree) pph_cache_find (stream, PPH_RECORD_XREF, image_ix, ix,
                                        PPH_any_tree);
        }
      else if (marker == PPH_RECORD_START_MERGE_BODY)
        {
          /* When reading a merge body, the tree has already been allocated
             and added to STREAM's cache.  All we have to do now is read
             its body.  */
          decl = (tree) pph_cache_get (&stream->cache, ix);
        }
    }

  if (flag_pph_tracer)
    pph_trace_tree (decl, "?", pph_trace_front, pph_trace_merge_body);

  gcc_assert (DECL_NAME (decl));

  /* If EXPR is a namespace alias, we do not need to merge
     its binding level (namespaces aliases do not have a
     binding level, they use the one from the namespace they
     alias).  */
  is_namespace_alias = pph_in_bool (stream);
  if (!is_namespace_alias)
    {
      gcc_assert (DECL_LANG_SPECIFIC (decl));
      gcc_assert (NAMESPACE_LEVEL (decl));
      pph_in_merge_body_binding_level (stream, NAMESPACE_LEVEL (decl));
    }

  if (flag_pph_tracer)
    pph_trace_tree (decl, NULL, pph_trace_back, pph_trace_merge_body);
}


/* Merge the attributes of a READ_EXPR into an existing EXPR.  */

static void
pph_merge_tree_attributes (tree expr, tree read_expr)
{
  enum tree_code code = TREE_CODE (read_expr);
  /* Collected from print_node.  */
  TREE_USED (expr) |= TREE_USED (read_expr);
  TREE_NOTHROW (expr) |= TREE_NOTHROW (read_expr);
  TREE_PUBLIC (expr) |= TREE_PUBLIC (read_expr);
  TREE_PRIVATE (expr) |= TREE_PRIVATE (read_expr);
  TREE_PROTECTED (expr) |= TREE_PROTECTED (read_expr);
  TREE_STATIC (expr) |= TREE_STATIC (read_expr);
  TREE_DEPRECATED (expr) |= TREE_DEPRECATED (read_expr);
  TREE_VISITED (expr) |= TREE_VISITED (read_expr);
  if (TREE_CODE_CLASS (code) == tcc_declaration)
    {
      if (CODE_CONTAINS_STRUCT (code, TS_DECL_COMMON))
	{
	  DECL_INTERFACE_KNOWN (expr) |= DECL_INTERFACE_KNOWN (read_expr);
	}
      if (CODE_CONTAINS_STRUCT (code, TS_DECL_WITH_VIS))
	{
	  DECL_WEAK (expr) |= DECL_WEAK (read_expr);
	}
      if (code == FUNCTION_DECL)
	{
	  DECL_DECLARED_INLINE_P (expr) |= DECL_DECLARED_INLINE_P (read_expr);
	}
    }
}


/* Read a merge key from STREAM,
   using the SEARCHER to find a matching existing tree within HOLDER.
   If the search succeeds, merge into the existing tree and use that.
   In any event, the resulting tree is added to the pickle cache.  */

tree
pph_in_merge_key_tree_with_searcher (pph_stream *stream, void *holder,
				     pph_merge_searcher searcher)
{
  enum pph_record_marker marker;
  unsigned image_ix, ix;
  tree read_expr, expr;
  bool fully_read_p;
  const char *name = "?";

  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_any_tree);
  if (marker == PPH_RECORD_END)
    return NULL;
  else if (pph_is_reference_marker (marker))
    return (tree) pph_cache_find (stream, marker, image_ix, ix, PPH_any_tree);
  gcc_assert (marker == PPH_RECORD_START_MERGE_KEY);

  /* Materialize a new node from STREAM.  This will also read all the
     language-independent bitfields for the new tree.  */
  read_expr = pph_in_tree_header (stream, &fully_read_p);
  gcc_assert (!fully_read_p);
  gcc_assert (holder);

  expr = searcher (stream, read_expr, &name, holder);
  gcc_assert (expr != NULL);

  if (expr != read_expr)
    {
      /* When STREAM was generated, EXPR and READ_EXPR were saved as
	 distinct trees (otherwise, we would have gotten READ_EXPR as
	 an internal reference). That's why we are reading them now as
	 distinct pointers.

	 So, if we find that EXPR has already been registered in
	 STREAM's cache, it means that we should not be trying to
	 merge them.  After all, the writer considered them different
	 objects.  */
      if (pph_cache_lookup (&stream->cache, expr, NULL,
			    pph_tree_code_to_tag (expr)))
	fatal_error ("Trying to merge distinct trees from the same "
		     "PPH image %s", stream->name);

      pph_merge_tree_attributes (expr, read_expr);
    }

  pph_cache_insert_at (&stream->cache, expr, ix,
		       pph_tree_code_to_tag (expr));

  if (flag_pph_tracer)
    pph_trace_tree (expr, name, pph_trace_front,
		    expr == read_expr ? pph_trace_unmerged_key
				      : pph_trace_merged_key);

  if (DECL_P (read_expr))
    {
      if (TREE_CODE (expr) == TYPE_DECL)
	{
	  bool is_implicit = pph_in_bool (stream);
	  if (is_implicit)
	    pph_in_merge_key_type_in_var (stream, &(TREE_TYPE (expr)));
	}
    }
  else
    {
      if (CLASS_TYPE_P (expr))
	{
	  pph_in_merge_key_chain (stream, &TYPE_FIELDS (expr));
	  pph_in_merge_key_chain (stream, &TYPE_METHODS (expr));
	  pph_in_merge_key_lang_type (stream, &TYPE_LANG_SPECIFIC (expr));
	}
    }

  if (flag_pph_tracer)
    pph_trace_tree (expr, name, pph_trace_back,
		    expr == read_expr ? pph_trace_unmerged_key
				      : pph_trace_merged_key);

  return expr;
}


/* Search in HOLDER for an existing decl that matches READ_EXPR.
   Read the match string from STREAM and assign to *NAME_P.  */

static tree
pph_in_search_key_decl_on_chain (pph_stream *stream, tree read_expr,
				const char **name_p, void *holder)
{
  gcc_assert (DECL_P (read_expr));
  *name_p = pph_in_string (stream);
  /* If we are merging into an existing CHAIN.  Look for a match in
     CHAIN to READ_EXPR's header.  If we found a match, EXPR will be
     the existing tree that matches READ_EXPR. Otherwise, EXPR is the
     newly allocated READ_EXPR.  */
  return pph_merge_into_chain (read_expr, *name_p, (tree*)holder);
}


/* Read a decl merge key from STREAM and search for matches on the CHAIN.  */

static tree
pph_in_merge_key_decl_on_chain (pph_stream *stream, tree *chain)
{
  return pph_in_merge_key_tree_with_searcher (stream, (void*) chain,
					      pph_in_search_key_decl_on_chain);
}


/* Search in HOLDER for an existing type that matches READ_EXPR.
   The HOLDER is its decl's type field.  */

static tree
pph_in_search_key_type_in_field (pph_stream *stream ATTRIBUTE_UNUSED,
	tree read_expr, const char **name_p ATTRIBUTE_UNUSED, void *holder)
{
  tree *field = (tree*)holder;
  gcc_assert (TYPE_P (read_expr));
  if (*field)
    return *field;
  else
    return read_expr;
}


/* Read a type merge key from STREAM and search for matches in its decl's
   type field.  */

static tree
pph_in_merge_key_type_in_var (pph_stream *stream, tree *field)
{
  return pph_in_merge_key_tree_with_searcher (stream, (void*) field,
					      pph_in_search_key_type_in_field);
}


/* Read and return a tree from STREAM.  */

tree
pph_in_tree (pph_stream *stream)
{
  tree expr = NULL;
  enum pph_record_marker marker;
  unsigned image_ix, ix;

  /* Read record start and test cache.  */
  marker = pph_in_start_record (stream, &image_ix, &ix, PPH_any_tree);
  if (marker == PPH_RECORD_END)
    return NULL;
  else if (pph_is_reference_marker (marker))
    return (tree) pph_cache_find (stream, marker, image_ix, ix, PPH_any_tree);
  else if (marker == PPH_RECORD_START || marker == PPH_RECORD_START_NO_CACHE)
    {
      bool fully_read_p;

      /* Materialize a new node from STREAM.  This will also read all the
         language-independent bitfields for the new tree.  */
      expr = pph_in_tree_header (stream, &fully_read_p);
      if (fully_read_p)
	return expr;
    }

  gcc_assert (marker == PPH_RECORD_START
              || marker == PPH_RECORD_START_MUTATED
	      || marker == PPH_RECORD_START_MERGE_BODY);

  if (marker == PPH_RECORD_START_MUTATED)
    {
      tree read_expr;
      bool fully_read_p;
      /* When reading a mutated tree, the tree itself is already in the
	 cache for another PPH image.  */
      expr = (tree) pph_cache_find (stream, marker, image_ix, ix, PPH_any_tree);
      read_expr = pph_in_tree_header (stream, &fully_read_p);
      /* FIXME pph: We should really merge into the existing tree,
	 but that requires significant changes to the generic streamer.
	 Here, for mutated records, overwriting would be simpler.  */
      pph_merge_tree_attributes (expr, read_expr);
      ggc_free (read_expr);
      if (fully_read_p)
	return expr;
    }
  else if (marker == PPH_RECORD_START_MERGE_BODY)
    {
      /* When reading a merge body, the tree has already been allocated
	 and added to STREAM's cache.  All we have to do now is read
	 its body.  */
      expr = (tree) pph_cache_get (&stream->cache, ix);
    }

  /* If we are starting to read a full tree, add its pointer to the
     cache and read its body.  The tree is added to the cache before
     we read its body to handle circular references and references
     from children nodes.  If we are reading a merge body, then the
     tree is already in the cache (it was added by
     pph_in_merge_key_tree).  */
  if (marker == PPH_RECORD_START)
    pph_cache_insert_at (&stream->cache, expr, ix, pph_tree_code_to_tag (expr));

  if (flag_pph_tracer)
    pph_trace_tree (expr, "?", pph_trace_front,
	marker == PPH_RECORD_START_MERGE_BODY ? pph_trace_merge_body
	: marker == PPH_RECORD_START_MUTATED ? pph_trace_mutate
	: pph_trace_normal );

  if (marker == PPH_RECORD_START_MERGE_BODY)
    pph_in_merge_tree_body (stream, expr);
  else
    pph_in_tree_body (stream, expr);

  if (flag_pph_tracer)
    pph_trace_tree (expr, NULL, pph_trace_back,
	marker == PPH_RECORD_START_MERGE_BODY ? pph_trace_merge_body
	: marker == PPH_RECORD_START_MUTATED ? pph_trace_mutate
	: pph_trace_normal);

  /* If needed, sign the recently materialized tree to detect
     mutations.  Note that we only need to compute signatures
     if we are generating a PPH image.  That is the only time
     where we need to determine whether a tree read from PPH
     was updated while parsing the header file that we are
     currently generating.  */
  if (pph_writer_enabled_p () && tree_needs_signature (expr))
    {
      unsigned crc;
      size_t nbytes;
      pph_cache *cache;

      /* Retrieve the cache to sign based on where we materialized
	 EXPR from.  */
      cache = pph_cache_select (stream, marker, image_ix);
      crc = pph_get_signature (expr, &nbytes);
      pph_cache_sign (cache, ix, crc, nbytes);
    }

  return expr;
}


/************************************************************* file contents */


/* Read a symbol table marker from STREAM.  */

static inline enum pph_symtab_action
pph_in_symtab_action (pph_stream *stream)
{
  enum pph_symtab_action m = (enum pph_symtab_action) pph_in_uchar (stream);
  gcc_assert (m == PPH_SYMTAB_DECLARE
	      || m == PPH_SYMTAB_EXPAND
	      || m == PPH_SYMTAB_EXPAND_1
	      || m == PPH_SYMTAB_FINISH_STRUCT_METHODS);
  return m;
}


/* Have we already emitted this DECL?  */

static bool
pph_decl_already_emitted (tree decl)
{
  static struct pointer_set_t *emitted_syms = NULL;
  gcc_assert (decl != NULL);
  if (!emitted_syms)
    emitted_syms = pointer_set_create ();
  return pointer_set_insert (emitted_syms, decl) != 0;
}


/* Have we already expanded this FN?  */

static bool
pph_fn_already_expanded (tree fn)
{
  static struct pointer_set_t *expanded_fns = NULL;
  gcc_assert (fn != NULL);
  if (!expanded_fns)
    expanded_fns = pointer_set_create ();
  return pointer_set_insert (expanded_fns, fn) != 0;
}


/* Read the symbol table from STREAM.  When this image is read into
   another translation unit, we want to guarantee that the IL
   instances taken from this image are instantiated in the same order
   that they were instantiated when we generated this image.

   With this, we can generate code in the same order out of the
   original header files and out of PPH images.  */

static void
pph_in_symtab (pph_stream *stream)
{
  unsigned i, num;

  /* Register all the symbols in STREAM in the same order of the
     original compilation for this header file.  */
  num = pph_in_uint (stream);
  for (i = 0; i < num; i++)
    {
      pph_symtab_entry entry;
      struct bitpack_d bp;

      entry.action = pph_in_symtab_action (stream);
      entry.decl = pph_in_tree (stream);
      bp = pph_in_bitpack (stream);
      entry.top_level = bp_unpack_value (&bp, 1);
      entry.at_end = bp_unpack_value (&bp, 1);
      entry.at_eof = bp_unpack_value (&bp, 1);
      entry.x_processing_template_decl = pph_in_int (stream);
      entry.function_depth = pph_in_int (stream);

      if (entry.action == PPH_SYMTAB_DECLARE)
	{
          if (pph_decl_already_emitted (entry.decl))
            continue;
	  cp_rest_of_decl_compilation (entry.decl, entry.top_level,
				       entry.at_end);
	}
      else if (entry.action == PPH_SYMTAB_EXPAND
	       || entry.action == PPH_SYMTAB_EXPAND_1)
	{
	  bool prev_processing_template_decl, prev_at_eof;
	  int prev_function_depth;
	  bool need_cgraph_node_p;
	  tree prev_current_function_decl;

	  pph_in_struct_function (stream, entry.decl);
	  need_cgraph_node_p = pph_in_bool (stream);
	  if (pph_fn_already_expanded (entry.decl))
	    continue;

	  /* ENTRY.DECL was reset to being external by the original
	     expansion code, so we need to restore its value before
	     expanding again.  */
	  DECL_EXTERNAL (entry.decl) = false;

	  /* Re-establish needed global state before calling the
	     expanders.  FIXME pph, get rid of this by fixing the
	     expanders.  */
	  prev_processing_template_decl = processing_template_decl;
	  prev_at_eof = at_eof;
	  prev_function_depth = function_depth;

	  processing_template_decl = entry.x_processing_template_decl;
	  at_eof = entry.at_eof;
	  function_depth = entry.function_depth;

	  /* Do the expansion.  */
	  prev_current_function_decl = current_function_decl;
	  current_function_decl = entry.decl;
	  push_cfun (DECL_STRUCT_FUNCTION (entry.decl));

	  if (entry.action == PPH_SYMTAB_EXPAND)
	    expand_or_defer_fn (entry.decl);
	  else
	    expand_or_defer_fn_1 (entry.decl);

	  if (need_cgraph_node_p)
	    cgraph_get_create_node (entry.decl);

	  current_function_decl = prev_current_function_decl;
	  pop_cfun ();

	  /* Restore global state.  */
	  processing_template_decl = prev_processing_template_decl;
	  at_eof = prev_at_eof;
	  function_depth = prev_function_depth;
	}
      else if (entry.action == PPH_SYMTAB_FINISH_STRUCT_METHODS)
	{
	  tree type = entry.decl;

	  /* If TYPE has a METHOD_VEC, we need to resort it.  Name
	     lookup in classes relies on the specific ordering of the
	     class method pointers.  Since we generally instantiate
	     them in a different order than the original compile, the
	     pointer values will be different.  This will cause name
	     lookups to fail, unless we resort the vector.  */
	  finish_struct_methods (type);
	}
      else
	gcc_unreachable ();
    }
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
  warning (0, "PPH file %s fails macro validation, "
           "%s is %s and should be %s or %s\n",
         filename, ident, quote_found, quote_before, quote_after);
  free (quote_found);
  free (quote_before);
  free (quote_after);
}


/* Load the IDENTIFIERS for a hunk from a STREAM.  */

static void
pph_in_identifiers (pph_stream *stream, cpp_idents_used *identifiers)
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


/* Read keys global bindings from STREAM and merge them into
   scope_chain->bindings.  Bindings are merged at every level starting
   at the global bindings from STREAM.  */

static void
pph_in_global_binding_keys (pph_stream *stream)
{
  cp_binding_level *bl, *other_bl;
  bool existed_p;

  bl = scope_chain->bindings;
  other_bl = pph_in_binding_level_start (stream, bl, &existed_p);

  /* If we found a reference to scope_chain->bindings, it should be
     the same instance.  This happens when we pull BL from a nested
     PPH image.  Otherwise, the call pph_in_binding_level_start should
     have registered scope_chain->bindings in the cache.  */
  gcc_assert (existed_p || other_bl == bl);

  /* Read the merge keys and merge them into the current compilation
     context.  Since we have registered scope_chain->bindings in the
     same slot IX that the writer used, the trees read now will be
     bound to scope_chain->bindings.  */
  pph_in_merge_key_binding_level (stream, &bl);
}


/* Read global bindings from STREAM and merge them into
   scope_chain->bindings.  Bindings are merged at every level starting
   at the global bindings from STREAM.  */

static void
pph_in_global_binding_bodies (pph_stream *stream)
{
  cp_binding_level *bl = scope_chain->bindings;

  /* Once all the symbols and types at every binding level have been
     merged to the corresponding binding levels in the current
     compilation, read all the bodies.  */
  pph_in_merge_body_binding_level (stream, bl);
}


/* Keep track of whether or not we actually read any PPH files.  */

static bool pph_files_were_read = false;


/* Report whether or not we actually read any PPH files.  */

bool
pph_files_read (void)
{
  return pph_files_were_read;
}


/* Helper for pph_read_file.  Read contents of PPH file in STREAM.
   Return true if STREAM was read or false if STREAM was already
   in memory.  */

static void
pph_read_file_1 (pph_stream *stream)
{
  bool verified;
  cpp_ident_use *bad_use;
  const char *cur_def;
  cpp_idents_used idents_used;
  tree file_static_aggregates;
  source_location cpp_token_replay_loc;

  /* If we already have STREAM in memory (or are reading it), ignore
     this request.  */
  if (stream->in_memory_p)
    return;

  /* Mark this file as being in memory.  This prevents multiple reads
     from the same file.  This scenario can happen in #include chains
     where the top header file is also included by one of its children
     (common in system headers).  */
  stream->in_memory_p = true;

  pph_files_were_read = true;

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "\nPPH: Reading Lines and Includes for %s\n",
			  stream->name);

  /* Read in STREAM's line table and merge it in the current line table.
     At the same time, read in includes in the order they were originally
     read.  */
  cpp_token_replay_loc = pph_in_line_table_and_includes (stream);

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "\nPPH: Reading Contents for %s\n", stream->name);

  /* Read all the identifiers and pre-processor symbols in the global
     namespace.  */
  pph_in_identifiers (stream, &idents_used);

  /* FIXME pph: This validation is weak.  */
  verified = cpp_lt_verify_1 (parse_in, &idents_used, &bad_use, &cur_def, true);
  if (!verified)
    report_validation_error (stream->name, bad_use->ident_str, cur_def,
                             bad_use->before_str, bad_use->after_str);

  /* Re-instantiate all the pre-processor symbols defined by STREAM.  Force
     their source_location to line 1 / column 0 of the file they were included
     in.  This avoids shifting all of the line_table's locations as we would by
     adding locations which wouldn't be there in the non-pph compile; thus
     working towards an identical line_table in pph and non-pph.  */
  cpp_lt_replay (parse_in, &idents_used, &cpp_token_replay_loc);

  /* Read the namespace scope bindings and template state from STREAM.  */
  pph_in_global_binding_keys (stream);
  pph_in_merge_key_template_state (stream);
  pph_in_global_binding_bodies (stream);
  pph_in_merge_body_template_state (stream);

  /* Read and merge the other global state collected during parsing of
     the original header.  */
  pph_union_into_chain (&keyed_classes, pph_in_tree (stream));
  pph_union_into_tree_vec (&unemitted_tinfo_decls, pph_in_tree_vec (stream));
  file_static_aggregates = pph_in_tree (stream);
  static_aggregates = chainon (file_static_aggregates, static_aggregates);
  pph_in_decl2_hidden_state (stream);

  pph_in_canonical_template_parms (stream);

  /* Read and process the symbol table.  This must be done at the end
     because we have symbols coming in from children PPH images which
     must be instantiated in the same order they were instantiated by
     the original parser.  */
  pph_in_symtab (stream);

  if (flag_pph_dump_tree)
    pph_dump_global_state (pph_logfile, "after pph read");
}

/* Helper for pph_add_include.  Add INCLUDE to the list of included
   images in PARENT.  Do nothing if INCLUDE was already in the list.  */

static void
pph_add_include_1 (pph_stream *parent, pph_stream *include)
{
  if (pointer_set_insert (parent->includes.m, include) == 0)
    VEC_safe_push (pph_stream_ptr, heap, parent->includes.v, include);
}


/* Add INCLUDE, and the images included by it, to the list of files
   included by PARENT.  */

static void
pph_add_include (pph_stream *parent, pph_stream *include)
{
  pph_stream *include_child;
  unsigned i;

  pph_add_include_1 (parent, include);
  FOR_EACH_VEC_ELT (pph_stream_ptr, include->includes.v, i, include_child)
    pph_add_include_1 (parent, include_child);
}


/* Read PPH file FILENAME.  Return the in-memory pph_stream instance.
   PARENT is the PPH file including FILENAME.  If PARENT is not NULL,
   the new PPH image is added to the list of images included by
   PARENT.  */

pph_stream *
pph_read_file (const char *filename, pph_stream *parent)
{
  pph_stream *stream = pph_stream_open (filename, "rb");
  if (stream)
    {
      /* FIXME pph.  We do this to mimic what the non-pph compiler
	 does in _cpp_stack_include as our goal is to have identical
	 line_tables.  */
      line_table->highest_location--;

      /* Read STREAM and add it to PARENT.  If PARENT is NULL, it
	 means that STREAM is being read from the toplevel translation
	 unit and we are not generating a PPH image.  */
      pph_read_file_1 (stream);
      if (parent)
	pph_add_include (parent, stream);
    }

  return stream;
}


/********************************************************* stream operations */


/* Initialize the reader.  */

void
pph_reader_init (void)
{
  merge_toc = htab_create (551, htab_merge_key_hash, htab_merge_key_eq, free);
}


/* Finalize the reader.  */

void
pph_reader_finish (void)
{
  htab_delete (merge_toc);
}
