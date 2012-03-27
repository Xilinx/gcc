/* Routines for writing PPH data.

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
#include "cgraph.h"
#include "parser.h"


/***************************************************** stream initialization */


/* PPH stream that we are currently generating.  FIXME pph, this
   global is needed because we call back from various parts of the
   compiler that do not know about PPH (e.g., some LTO callbacks,
   cp_rest_of_decl_compilation).  */
static pph_stream *pph_out_stream = NULL;


/* Return the stream we are currently generating.  We declare this function
   with external linkage so it can be accessed from pph_include_handler,
   but this really should be private to this file.  */

pph_stream *pph_writer_get_stream (void);

pph_stream *
pph_writer_get_stream (void)
{
  return pph_out_stream;
}


/* Initialize buffers and tables in STREAM for writing.  */

void
pph_init_write (pph_stream *stream)
{
  stream->encoder.w.out_state = lto_new_out_decl_state ();
  lto_push_out_decl_state (stream->encoder.w.out_state);
  stream->encoder.w.decl_state_stream = XCNEW (struct lto_output_stream);
  stream->encoder.w.ob = create_output_block (LTO_section_decls);

  /* Associate STREAM with stream->encoder.w.ob so we can recover it from the
     streamer hooks.  */
  stream->encoder.w.ob->sdata = (void *) stream;

  /* Since we are about to generate STREAM, its header name is the
     name of the header file that we are currently parsing.  */
  pph_stream_set_header_name (stream, xstrdup (input_filename));
}


/* Initialize the PPH writer.  */

void
pph_writer_init (void)
{
  timevar_start (TV_PPH_OUT);

  gcc_assert (pph_out_stream == NULL);
  pph_out_stream = pph_stream_open (pph_out_file, "wb");
  if (pph_out_stream == NULL)
    fatal_error ("Cannot open PPH file %s for writing: %m", pph_out_file);

  timevar_stop (TV_PPH_OUT);
}


/********************************************************** primitive values */


/* Write an unsigned char VALUE to STREAM.  */

static void
pph_out_uchar (pph_stream *stream, unsigned char value)
{
  streamer_write_char_stream (stream->encoder.w.ob->main_stream, value);
}


/* Write a HOST_WIDE_INT VALUE to stream.  */

static inline void
pph_out_hwi (pph_stream *stream, HOST_WIDE_INT value)
{
  streamer_write_hwi (stream->encoder.w.ob, value);
}


/* Write an int VALUE to stream.  */

static inline void
pph_out_int (pph_stream *stream, int value)
{
  streamer_write_hwi (stream->encoder.w.ob, value);
}


/* Write an unsigned HOST_WIDE_INT VALUE to STREAM.  */

static inline void
pph_out_uhwi (pph_stream *stream, unsigned HOST_WIDE_INT value)
{
  streamer_write_uhwi (stream->encoder.w.ob, value);
}


/* Write an unsigned int VALUE to STREAM.  */

void
pph_out_uint (pph_stream *stream, unsigned int value)
{
  streamer_write_uhwi (stream->encoder.w.ob, value);
}


/* Write N bytes from P to STREAM.  */

static void
pph_out_bytes (pph_stream *stream, const void *p, size_t n)
{
  lto_output_data_stream (stream->encoder.w.ob->main_stream, p, n);
}


/* Write string STR to STREAM.  */

static inline void
pph_out_string (pph_stream *stream, const char *str)
{
  streamer_write_string (stream->encoder.w.ob,
                         stream->encoder.w.ob->main_stream, str, false);
}


/* Write string STR of length LEN to STREAM.  */
static inline void
pph_out_string_with_length (pph_stream *stream, const char *str,
                            unsigned int len)
{
  streamer_write_string_with_length (stream->encoder.w.ob,
                                     stream->encoder.w.ob->main_stream,
                                     str, len + 1, false);
}


/* Write a bitpack BP to STREAM.  */

static inline void
pph_out_bitpack (pph_stream *stream, struct bitpack_d *bp)
{
  gcc_assert (stream->encoder.w.ob->main_stream == bp->stream);
  streamer_write_bitpack (bp);
}


/* Write a boolean value VAL to STREAM.  */

static inline void
pph_out_bool (pph_stream *stream, bool val)
{
  pph_out_uint (stream, val ? 1 : 0);
}


/******************************************************** source information */


/* Emit linenum_type LN to STREAM.  */

static inline void
pph_out_linenum_type (pph_stream *stream, linenum_type ln)
{
  pph_out_uint (stream, ln);
}


/* Emit source_location SL to STREAM.  */

static inline void
pph_out_source_location (pph_stream *stream, source_location sl)
{
  pph_out_uint (stream, sl);
}


/* Emit line table MARKER to STREAM.  */

static inline void
pph_out_linetable_marker (pph_stream *stream, enum pph_linetable_marker marker)
{
  gcc_assert (marker == (enum pph_linetable_marker)(unsigned char) marker);
  pph_out_uchar (stream, marker);
}


/* Emit all the fields of struct line_map_ordinary LM to STREAM.  IX
   is the slot number in the line table where LM is stored.  */

static void
pph_out_line_map_ordinary (pph_stream *stream, struct line_map *lm, int ix)
{
  struct bitpack_d bp;
  int rel_includer_ix, includer_ix;

  pph_out_string (stream, ORDINARY_MAP_FILE_NAME (lm));
  pph_out_linenum_type (stream, ORDINARY_MAP_STARTING_LINE_NUMBER (lm));

  /* To support relocating this table into other translation units,
     emit a relative index to LM's includer.  All the relative indices
     are positive values indicating the distance from LM *back* to the
     line map for its includer.

     So, if we had header top.h including [1234].h, the line table
     entries look something like this:

	Map #10	LC_ENTER "top.h" FROM: -1 (top file).
	Map #11	LC_ENTER "1.h" FROM: 10 (top.h) -> saved as offset 1 (11 - 10).
	Map #12	LC_ENTER "2.h" FROM: 10 (top.h) -> saved as offset 2 (12 - 10).
	Map #13	LC_ENTER "3.h" FROM: 10 (top.h) -> saved as offset 3 (13 - 10).
	Map #14	LC_ENTER "4.h" FROM: 10 (top.h) -> saved as offset 4 (14 - 10).

     When saving the map entry for #13 (3.h), we do not want to save
     the absolute index value '10', since top.pph may be included from
     different TUs.  Instead, we save the offset 3, so that the reader
     knows that the includer for 3.h is 3 slots before 3.h's entry.  */
  includer_ix = ORDINARY_MAP_INCLUDER_FILE_INDEX (lm);
  if (includer_ix >= 0)
    {
      gcc_assert (includer_ix < ix);
      rel_includer_ix = ix - includer_ix;
    }
  else
    {
      /* If LM is included by index -1, it means that this is a line map
	 entry for the top level file being compiled (i.e., the PPH that
	 we are currently generating).  The first time we find this index,
	 the reader will remember the index of the current parent file
	 and replace all the -1 entries with it.  */
      gcc_assert (includer_ix == -1);
      rel_includer_ix = includer_ix;
    }

  pph_out_int (stream, rel_includer_ix);

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, ORDINARY_MAP_IN_SYSTEM_HEADER_P (lm), CHAR_BIT);
  bp_pack_value (&bp, ORDINARY_MAP_NUMBER_OF_COLUMN_BITS (lm), COLUMN_BITS_BIT);
  pph_out_bitpack (stream, &bp);
}


/* Emit all information contained in LM to STREAM.  IX is the slot number
   in the line table where LM is stored.  */

static void
pph_out_line_map (pph_stream *stream, struct line_map *lm, int ix)
{
  struct output_block *ob = stream->encoder.w.ob;

  pph_out_source_location (stream, lm->start_location);
  streamer_write_enum (ob->main_stream, lc_reason, LC_ENTER_MACRO, lm->reason);

  /* FIXME pph.  We currently do not support location tracking for
     macros in PPH images.  */
  gcc_assert (lm->reason != LC_ENTER_MACRO);
  pph_out_line_map_ordinary (stream, lm, ix);
}


/* Write a reference of INCLUDE to STREAM.  Also write the START_LOCATION of
   this include in the current line_table.  */

static void
pph_out_include (pph_stream *stream, pph_stream *include,
                 source_location start_location)
{
  pph_out_source_location (stream, start_location);
  pph_out_string (stream, include->name);
}


/* Emit the required line_map entry (those directly related to this include)
   and some properties in the line_table to STREAM, ignoring builtin and
   command-line entries.  We will write references to our direct includes
   children and skip their actual line_map entries (unless they are non-pph
   children in which case we have to write out their line_map entries as well).
   We assume stream->encoder.w.includes contains the pph headers included in the
   same order they are seen in the line_table.  */

static void
pph_out_line_table_and_includes (pph_stream *stream)
{
  int ix;

  timevar_start (TV_PPH_OUT_LINE_TABLE);

  /* Any #include should have been fully parsed and exited at this point.  */
  gcc_assert (line_table->depth == 0);

  /* Write the path name of the original text header file that was
     used to generate STREAM.  */
  pph_out_string (stream, stream->header_name);

  for (ix = PPH_NUM_IGNORED_LINE_TABLE_ENTRIES;
       ix < (int) LINEMAPS_ORDINARY_USED (line_table);
       ix++)
    {
      pph_stream *current_include;
      struct line_map *lm = LINEMAPS_ORDINARY_MAP_AT (line_table, ix);

      /* FIXME pph.  We currently do not support location tracking for
	 macros in PPH images.  */
      gcc_assert (lm->reason != LC_ENTER_MACRO);

      if (ix == PPH_NUM_IGNORED_LINE_TABLE_ENTRIES)
        {
	  /* The first non-ignored entry should be an LC_RENAME back
	     in the header after inserting the builtin and
	     command-line entries.  When reading the pph we want this
	     to be a simple LC_ENTER as the builtin and command_line
	     entries will already exist and we are now entering a
             #include.  */
          gcc_assert (lm->reason == LC_RENAME);
          lm->reason = LC_ENTER;
        }

      /* If LM is an entry for an included PPH image, output a line table
	 reference to it, so the reader can load the included image at
	 this point.  Make sure we do not emit self-referential
	 entries (even if a file is properly guarded against double
	 inclusion, there will be linemap entries in the line table
	 for it).  */
      current_include = (lm->reason == LC_ENTER
	                 && ix > PPH_NUM_IGNORED_LINE_TABLE_ENTRIES)
	                ? pph_stream_registry_lookup (LINEMAP_FILE (lm))
			: NULL;
      if (current_include && current_include != stream)
	{
	  struct line_map *included_from;

	  /* Assert that we are entering a new header file from another.  */
	  gcc_assert (lm->reason == LC_ENTER);
	  gcc_assert (ORDINARY_MAP_INCLUDER_FILE_INDEX (lm) != -1);

	  pph_out_linetable_marker (stream, PPH_LINETABLE_REFERENCE);

	  pph_out_include (stream, current_include, lm->start_location);

	  /* Since all the line table entries corresponding to
	     CURRENT_INCLUDE and its #include children are emitted
	     inside CURRENT_INCLUDE, we need to skip them so they do
	     not get emitted in STREAM.

	     To do this, we look for the next line map table that
	     corresponds to an LC_LEAVE event back to the file that
	     includes CURRENT_INCLUDE (which is represented by the
	     line map LM).  */
	  included_from = INCLUDED_FROM (line_table, lm);
	  for (ix++; ix < (int) LINEMAPS_ORDINARY_USED (line_table); ix++)
	    {
	      struct line_map *map = LINEMAPS_ORDINARY_MAP_AT (line_table, ix);

	      /* When we find an LC_LEAVE map, we have two ways of
		 deciding if it is an LC_LEAVE event back to
		 INCLUDED_FROM.  Either MAP goes to the same file name
		 as INCLUDED_FROM or both MAP and INCLUDED_FROM are
		 included by the same line map.  Since the latter is
		 faster, we check it instead of doing a string
		 comparison.  */
	      if (map->reason == LC_LEAVE
		  && ORDINARY_MAP_INCLUDER_FILE_INDEX (included_from)
		     == ORDINARY_MAP_INCLUDER_FILE_INDEX (map))
		break;
	    }

	    /* We should always leave this loop before the end of the
		current line_table entries.  */
	    gcc_assert (ix < (int) LINEMAPS_ORDINARY_USED (line_table));
	}
      else
	{
	  pph_out_linetable_marker (stream, PPH_LINETABLE_ENTRY);
	  pph_out_line_map (stream, lm, ix);
	}

      /* Restore changes made to the first entry above, if needed.  */
      if (ix == PPH_NUM_IGNORED_LINE_TABLE_ENTRIES)
	lm->reason = LC_RENAME;
    }

  pph_out_linetable_marker (stream, PPH_LINETABLE_END);

  /* Output the number of entries written to validate on input.  */
  pph_out_uint (stream, LINEMAPS_ORDINARY_USED (line_table)
                        - PPH_NUM_IGNORED_LINE_TABLE_ENTRIES);

  pph_out_source_location (stream, line_table->highest_location);
  pph_out_source_location (stream, line_table->highest_line);

  pph_out_uint (stream, line_table->max_column_hint);

  timevar_stop (TV_PPH_OUT_LINE_TABLE);
}


/*********************************************************** record handling */


/* Return a PPH record marker according to whether DATA is NULL or
   it can be found in one of the caches associated with STREAM.  TAG
   is the data type for DATA.

   If DATA is in any of the caches, return the corresponding slot in
   *IX_P.  If DATA is in the cache of an image included by STREAM,
   return the image's index in *INCLUDE_IX_P.

   Caches are consulted in order of preference: preloaded, internal
   and external.

   If DATA is not found anywhere, return PPH_RECORD_START and, if
   given, set *IX_P and *INCLUDE_IX_P to -1.  */

static enum pph_record_marker
pph_get_marker_for (pph_stream *stream, void *data, unsigned *include_ix_p,
                    unsigned *ix_p, enum pph_tag tag)
{
  if (ix_p)
    *ix_p = -1u;

  if (include_ix_p)
    *include_ix_p = -1u;

  /* We represent NULL pointers with PPH_RECORD_END.  */
  if (data == NULL)
    return PPH_RECORD_END;

  /* Some structures should never be shared as they are already
     members of shared structures.  If the parent structure is shared,
     the inner structure will be shared too.  Additionally, this is a
     problem when the parent structure has mutated from one PPH to the
     other.  If we allow the inner structure to be shared, we will
     read the mutated state of the outer structure, but we will
     happily re-use the data from the inner one (and miss mutated
     state in it).  */
  switch (tag)
    {
    case PPH_binding_table:
    case PPH_function:
    case PPH_lang_decl:
    case PPH_lang_type:
    case PPH_language_function:
    case PPH_sorted_fields_type:
      return PPH_RECORD_START_NO_CACHE;

    default:
      break;
    }

  /* If DATA is a pre-loaded tree node, return a pre-loaded reference
     marker.  */
  if (pph_cache_lookup (NULL, data, ix_p, tag))
    return PPH_RECORD_PREF;

  /* If DATA is in STREAM's cache, return an internal reference marker.  */
  if (pph_cache_lookup (&stream->cache, data, ix_p, tag))
    return PPH_RECORD_IREF;

  /* If DATA is in the cache of an included image, return an external
     reference marker.  */
  if (pph_cache_lookup_in_includes (stream, data, include_ix_p, ix_p, tag))
    return PPH_RECORD_XREF;

  /* DATA is in none of the caches.  It should be pickled out.  */
  return PPH_RECORD_START;
}


/* Write record MARKER for data type TAG to STREAM.  */
void
pph_out_record_marker (pph_stream *stream, enum pph_record_marker marker,
                       enum pph_tag tag)
{
  gcc_assert (marker == (enum pph_record_marker)(unsigned char) marker);
  pph_out_uchar (stream, marker);

  gcc_assert (tag == (enum pph_tag)(unsigned) tag);
  pph_out_uint (stream, tag);

  if (flag_pph_tracer >= 5)
    pph_trace_marker (marker, tag);

  pph_stats.num_records_by_marker[marker]++;
  pph_stats.num_records_by_tag[tag]++;
  pph_stats.num_records++;
}


/* Write a reference record on STREAM.  MARKER is the tag indicating
   what kind of reference to write.  TAG indicates the data type to be
   stored in this record.  IX is the cache slot index to write.
   INCLUDE_IX is used for PPH_RECORD_XREF records.  */

static inline void
pph_out_reference_record (pph_stream *stream, enum pph_record_marker marker,
                          unsigned include_ix, unsigned ix, enum pph_tag tag)
{
  gcc_assert (marker == PPH_RECORD_END || pph_is_reference_marker (marker));

  pph_out_record_marker (stream, marker, tag);

  if (pph_is_reference_marker (marker))
    {
      if (marker == PPH_RECORD_XREF)
        {
          gcc_assert (include_ix != -1u);
          pph_out_uint (stream, include_ix);
        }

      gcc_assert (ix != -1u);
      pph_out_uint (stream, ix);
    }
  else
    gcc_assert (marker == PPH_RECORD_END);
}


/* Start a new record in STREAM for DATA with data type TAG.  If DATA
   is NULL write an end-of-record marker.

   If DATA is not NULL and did not exist in the pickle cache, add it,
   write a start-of-record marker and return true.  This means that we
   could not write a reference marker to DATA and the caller should
   pickle out DATA's physical representation.

   If DATA existed in the cache, one of two things may happen:

   - If a merge key had been emitted for DATA, it means that DATA has
     only been partially written.  The rest of it needs to be emitted
     now.  In these cases, a PPH_RECORD_START_MERGE_BODY marker is
     written.

   - If DATA had been written before, we do only need to write a
     reference to it.  In this case, a reference-record marker is
     written.

   The emitted marker is returned.  */

static inline enum pph_record_marker
pph_out_start_record (pph_stream *stream, void *data, enum pph_tag tag)
{
  unsigned include_ix, ix;
  enum pph_record_marker marker;

  /* Try to write a reference record first.  */
  marker = pph_get_marker_for (stream, data, &include_ix, &ix, tag);
  if (marker == PPH_RECORD_END)
    {
      pph_out_reference_record (stream, marker, include_ix, ix, tag);
      return marker;
    }
  else if (pph_is_reference_marker (marker))
    {
      /* If we have already emitted DATA, check whether we need
	 to write DATA's body.  This happens when DATA is written in
	 two parts so it can be merged across PPH files.  The first
	 part is the merge key, which only writes enough data to
	 identify DATA so it can b merged, the second part consists
	 of all the remaining fields in DATA (the merge body).  */
      pph_cache *cache = pph_cache_select (stream, marker, include_ix);
      pph_cache_entry *e = pph_cache_get_entry (cache, ix);
      if (e->needs_merge_body)
	{
	  marker = PPH_RECORD_START_MERGE_BODY;
	  e->needs_merge_body = false;
	}
      else
	{
	  /* This is a regular reference, emit it and tell the caller
	     that we are done.  */
	  pph_out_reference_record (stream, marker, include_ix, ix, tag);
	  return marker;
	}
    }
  else if (marker == PPH_RECORD_START)
    {
      /* DATA is in none of the pickle caches.  Add DATA to STREAM's
	 pickle cache and write the slot where we stored it in.  */
      pph_cache_add (&stream->cache, data, &ix, tag);
    }

  /* The caller will have to write a physical representation for DATA.  */
  gcc_assert (marker == PPH_RECORD_START
	      || marker == PPH_RECORD_START_NO_CACHE
	      || marker == PPH_RECORD_START_MERGE_BODY);
  pph_out_record_marker (stream, marker, tag);
  if (marker != PPH_RECORD_START_NO_CACHE)
    pph_out_uint (stream, ix);

  return marker;
}


/* Return true if tree node T should be added to the pickle cache.  */

static inline bool
pph_cache_should_handle (tree t)
{
  if (t)
    {
      if (TREE_CODE (t) == INTEGER_CST)
        {
          /* With the exception of some special constants that are
            pointer-compared everywhere (e.g., integer_zero_node), we
            do not want constants in the cache.  Their physical
            representation is smaller than a cache reference record
            and they can also trick the cache in similar ways to
            builtins (read below).  */
          return false;
        }
      else if (streamer_handle_as_builtin_p (t))
        {
          /* We do not want builtins in the cache for two reasons:

            - They never need pickling.  Much like pre-loaded tree
              nodes, builtins are pre-built by the compiler and
              accessible with their class and code.

            - They can trick the cache.  When possible, user-provided
              functions are generally replaced by their builtin
              counterparts (e.g., strcmp, malloc, etc).  When this
              happens, the writer cache will store in its cache two
              different expressions, one for the user provided
              function and another for the builtin itself.  However,
              when written out to the PPH image, they both get
              emitted as a reference to the builtin.  Therefore, when
              the reader tries to instantiate the second copy, it
              tries to store the same tree in two different cache
              slots (and proceeds to ICE in pph_cache_insert_at).  */
          return false;
        }
    }

  return true;
}


/* Start a record for tree node T in STREAM.  This is like
   pph_out_start_record, but it filters certain special trees that
   should never be added to the cache.  Return the marker used
   to start the record.  */

static inline enum pph_record_marker
pph_out_start_tree_record (pph_stream *stream, tree t)
{
  unsigned include_ix, ix;
  enum pph_record_marker marker;
  enum pph_tag tag;

  /* Determine what kind of record we will be writing.  */
  tag = pph_tree_code_to_tag (t);
  marker = pph_get_marker_for (stream, t, &include_ix, &ix, tag);

  /* Signed tree nodes that have been read from an external PPH image
     may have mutated while parsing this header.  In that case,
     we need to write a mutated reference record and re-pickle the
     tree.  */
  if (marker == PPH_RECORD_XREF && tree_needs_signature (t))
    {
      pph_cache *cache = pph_cache_select (stream, marker, include_ix);
      pph_cache_entry *e = pph_cache_get_entry (cache, ix);
      unsigned crc = pph_get_signature (t, NULL);
      if (crc != e->crc)
        marker = PPH_RECORD_START_MUTATED;
    }

  /* If we are about to write an internal reference for a mergeable
     tree, it means that we have already written the merge key for it.
     Check whether we still need to write its merge body.  If so,
     MARKER should be PPH_RECORD_START_MERGE_BODY.  */
  if (marker == PPH_RECORD_IREF && pph_tree_is_mergeable (t))
    {
      pph_cache_entry *e = pph_cache_get_entry (&stream->cache, ix);
      if (e->needs_merge_body)
	{
	  marker = PPH_RECORD_START_MERGE_BODY;
	  e->needs_merge_body = false;
	}
    }

  /* Write a record header according to the value of MARKER.  */
  if (marker == PPH_RECORD_END || pph_is_reference_marker (marker))
    pph_out_reference_record (stream, marker, include_ix, ix, tag);
  else if (marker == PPH_RECORD_START || marker == PPH_RECORD_START_MERGE_BODY)
    {
      /* We want to prevent some trees from hitting the cache.
         For example, we do not want to cache regular constants (as
         their representation is actually smaller than a cache
         reference), but some constants are special and need to
         always be shared (e.g., integer_zero_node).  Those special
         constants are pre-loaded in STREAM->PRELOADED_CACHE.  */
      if (!pph_cache_should_handle (t))
        marker = PPH_RECORD_START_NO_CACHE;

      pph_out_record_marker (stream, marker, tag);

      if (marker == PPH_RECORD_START || marker == PPH_RECORD_START_MERGE_BODY)
        {
          unsigned ix;
          pph_cache_add (&stream->cache, t, &ix, tag);
          pph_out_uint (stream, ix);
        }
    }
  else if (marker == PPH_RECORD_START_MUTATED)
    {
      unsigned new_crc;
      size_t nbytes;
      pph_cache *ext_cache;

      /* We found T in an external PPH file, but it has mutated since
         we originally read it.  We are going to write out T again,
         but the reader should not re-allocate T.  Rather, it should
         read the contents of T on top of the existing address.

	 We also re-compute T's CRC and update its cache entry.  This
	 way, further references to T will become regular external
	 references.  */
      pph_out_record_marker (stream, marker, tag);

      /* Update the CRC for T in the external cache, so we don't
	 continue to consider it mutated.  */
      new_crc = pph_get_signature (t, &nbytes);
      ext_cache = pph_cache_select (stream, PPH_RECORD_XREF, include_ix);
      pph_cache_sign (ext_cache, ix, new_crc, nbytes);

      /* Write the location of T in the external cache.  */
      gcc_assert (include_ix != -1u);
      pph_out_uint (stream, include_ix);

      gcc_assert (ix != -1u);
      pph_out_uint (stream, ix);
    }
  else
    gcc_unreachable ();

  return marker;
}


/* Start a merge key record for DATA on STREAM.  TAG identifies the
   data type of DATA.  Return true if a merge key for DATA had already
   been emitted.  */

static bool
pph_out_start_merge_key_record (pph_stream *stream, void *data,
				enum pph_tag tag)
{
  enum pph_record_marker marker;
  pph_cache_entry *e;
  unsigned include_ix, ix;

  marker = pph_get_marker_for (stream, data, &include_ix, &ix, tag);
  if (marker == PPH_RECORD_END || pph_is_reference_marker (marker))
    {
      /* We had already emitted a merge key for DATA, write a
	 reference and return true.  */
      pph_out_reference_record (stream, marker, include_ix, ix, tag);
      return true;
    }

  /* This should be the first time that we try to write DATA.  */
  gcc_assert (marker == PPH_RECORD_START);

  pph_out_record_marker (stream, PPH_RECORD_START_MERGE_KEY, tag);
  e = pph_cache_add (&stream->cache, data, &ix, tag);
  e->needs_merge_body = true;
  pph_out_uint (stream, ix);

  /* Tell the caller that this is the first time this merge key is
     emitted.  */
  return false;
}


/* Start a merge key record for EXPR on STREAM.  Return true if a
   merge key for EXPR had already been emitted.  */

static bool
pph_out_start_merge_key_tree_record (pph_stream *stream, tree expr)
{
  enum pph_tag tag = pph_tree_code_to_tag (expr);
  return pph_out_start_merge_key_record (stream, expr, tag);
}



/********************************************************** lexical elements */


/* Write location LOC of length to STREAM.  */

void
pph_out_location (pph_stream *stream, location_t loc)
{
  /* FIXME pph: we are streaming builtin locations, which implies that we are
     streaming some builtins, we probably want to figure out what those are and
     simply add them to the cache in the preload.  */
  struct bitpack_d bp;
  struct line_map *map;
  location_t first_non_builtin_loc;

  map = LINEMAPS_ORDINARY_MAP_AT (line_table,
				  PPH_NUM_IGNORED_LINE_TABLE_ENTRIES);
  first_non_builtin_loc = MAP_START_LOCATION (map);

  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  if (loc < first_non_builtin_loc)
    {
      /* We should never stream out trees with locations between builtins
	 and user locations (e.g. <command-line>).  */
      gcc_assert (loc <= BUILTINS_LOCATION);
      bp_pack_value (&bp, true, 1);
    }
  else
    bp_pack_value (&bp, false, 1);

  pph_out_bitpack (stream, &bp);
  pph_out_uhwi (stream, loc);
}


/* Save the tree associated with TOKEN to STREAM.  */

static void
pph_out_token_value (pph_stream *stream, cp_token *token)
{
  tree val;

  val = token->u.value;
  switch (token->type)
    {
      case CPP_TEMPLATE_ID:
      case CPP_NESTED_NAME_SPECIFIER:
	/* FIXME pph - Need to handle struct tree_check.  */
	break;

      case CPP_KEYWORD:
	/* Nothing to do.  We will reconstruct the keyword from
	   ridpointers[token->keyword] at load time.  */
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
	pph_out_tree (stream, val);
	break;

      case CPP_PRAGMA:
	/* Nothing to do.  Field pragma_kind has already been written.  */
	break;

      default:
	gcc_assert (token->u.value == NULL);
	pph_out_bytes (stream, &token->u.value, sizeof (token->u.value));
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


/******************************************************************* vectors */

/* Note that we use the same format used by streamer_write_chain.
   Since the reader always reads chains using streamer_read_chain, we
   have to write VECs in exactly the same way as tree chains.  */

/* Return true if T matches FILTER for STREAM.  */

static inline bool
pph_tree_matches (pph_stream *stream, tree t, unsigned filter)
{
  if ((filter & PPHF_NO_BUILTINS)
      && DECL_P (t)
      && DECL_IS_BUILTIN (t))
    return false;

  if ((filter & PPHF_NO_PREFS)
      && pph_cache_lookup (NULL, t, NULL, pph_tree_code_to_tag (t)))
    return false;

  if ((filter & PPHF_NO_XREFS)
      && pph_cache_lookup_in_includes (stream, t, NULL, NULL,
	                               pph_tree_code_to_tag (t)))
    return false;

  return true;
}


/* Return a heap vector with all the trees in V that match FILTER.
   The caller is responsible for freeing the returned vector.  */

static inline VEC(tree,heap) *
vec2vec_filter (pph_stream *stream, VEC(tree,gc) *v, unsigned filter)
{
  unsigned i;
  tree t;
  VEC(tree, heap) *filtered_v = NULL;

  /* Do not accept the nil filter.  The caller is responsible for
     freeing the returned vector and they may inadvertently free
     a vector they assumed to be allocated by this function.  */
  gcc_assert (filter != PPHF_NONE);

  /* Collect all the nodes that match the filter.  */
  FOR_EACH_VEC_ELT (tree, v, i, t)
    if (pph_tree_matches (stream, t, filter))
      VEC_safe_push (tree, heap, filtered_v, t);

  return filtered_v;
}


/* Write all the trees in VEC V to STREAM.  */

void
pph_out_tree_vec (pph_stream *stream, VEC(tree,gc) *v)
{
  unsigned i;
  tree t;
  pph_out_hwi (stream, VEC_length (tree, v));
  FOR_EACH_VEC_ELT (tree, v, i, t)
    pph_out_tree (stream, t);
}


/* Write all the merge keys for trees in VEC V to STREAM.  The keys
   must go out in declaration order, i.e. reversed.  */

static void
pph_out_merge_key_vec (pph_stream *stream, VEC(tree,gc) *v)
{
  unsigned i;
  tree t;
  pph_out_hwi (stream, VEC_length (tree, v));
  FOR_EACH_VEC_ELT_REVERSE (tree, v, i, t)
    pph_out_merge_key_tree (stream, t, false);
}


/* Write all the merge bodies for trees in VEC V to STREAM.  The bodies
   should/must go out in declaration order, i.e. reversed.  */

static void
pph_out_merge_body_vec (pph_stream *stream, VEC(tree,gc) *v)
{
  unsigned i;
  tree t;
  pph_out_hwi (stream, VEC_length (tree, v));
  FOR_EACH_VEC_ELT_REVERSE (tree, v, i, t)
    pph_out_tree (stream, t);
}


/* Write all the trees in VEC V that match FILTER to STREAM.  */

static void
pph_out_tree_vec_filtered (pph_stream *stream, VEC(tree,gc) *v, unsigned filter)
{
  if (filter == PPHF_NONE)
    pph_out_tree_vec (stream, v);
  else
    {
      VEC(tree,heap) *w = vec2vec_filter (stream, v, filter);
      pph_out_tree_vec (stream, (VEC(tree,gc) *)w);
      VEC_free (tree, heap, w);
    }
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
      pph_out_tree (stream, q->typedef_decl);
      pph_out_tree (stream, q->context);
      pph_out_location (stream, q->locus);
    }
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
      pph_out_tree (stream, p->purpose);
      pph_out_tree (stream, p->value);
    }
}


/******************************************************************** chains */

/* Convert a CHAIN to a VEC by copying only the nodes that match FILTER
   for STREAM.  */

static VEC(tree,heap) *
chain2vec_filter (pph_stream *stream, tree chain, unsigned filter)
{
  tree t;
  VEC(tree,heap) *v = NULL;

  /* Do not accept the nil filter.  The caller is responsible for
     freeing the returned vector and they may inadvertently free
     a vector they assumed to be allocated by this function.  */
  /* FIXME crowl: gcc_assert (filter != PPHF_NONE); */

  for (t = chain; t; t = TREE_CHAIN (t))
    if (pph_tree_matches (stream, t, filter))
      VEC_safe_push (tree, heap, v, t);

  return v;
}


/* Convert a CHAIN to a VEC by copying the nodes.  */

VEC(tree,heap) *
chain2vec (tree chain)
{
  tree t;
  VEC(tree,heap) *v = NULL;

  for (t = chain; t; t = TREE_CHAIN (t))
    VEC_safe_push (tree, heap, v, t);

  return v;
}


/* Write a chain of trees to STREAM starting with FIRST.  */

static void
pph_out_chain (pph_stream *stream, tree first)
{
  streamer_write_chain (stream->encoder.w.ob, first, false);
}


/* Write, in reverse, a chain of merge keys to STREAM starting
   with the last element of CHAIN.  Only write the trees that match
   FILTER.  */

static void
pph_out_merge_key_chain (pph_stream *stream, tree chain, unsigned filter)
{
  VEC(tree,heap) *w;
  if (filter == PPHF_NONE)
    w = chain2vec (chain);
  else
    w = chain2vec_filter (stream, chain, filter);
  pph_out_merge_key_vec (stream, (VEC(tree,gc) *)w);
  VEC_free (tree, heap, w);
}


/* Write, in reverse, a chain of merge bodies to STREAM starting
   with the last element of CHAIN.  Only write the trees that match
   FILTER.  */

static void
pph_out_merge_body_chain (pph_stream *stream, tree chain, unsigned filter)
{
  VEC(tree,heap) *w;
  if (filter == PPHF_NONE)
    w = chain2vec (chain);
  else
    w = chain2vec_filter (stream, chain, filter);
  pph_out_merge_body_vec (stream, (VEC(tree,gc) *)w);
  VEC_free (tree, heap, w);
}


/****************************************************************** bindings */


/* Forward declaration to break cyclic dependencies.  */
static void pph_out_merge_key_namespace_decl (pph_stream *stream, tree ns);
static void pph_out_merge_body_namespace_decl (pph_stream *stream, tree ns);


/* Write a reference to binding level BL to STREAM.  */

static void
pph_out_binding_level_ref (pph_stream *stream, cp_binding_level *bl)
{
  enum pph_record_marker marker;
  marker = pph_out_start_record (stream, bl, PPH_cp_binding_level);
  gcc_assert (pph_is_reference_or_end_marker (marker));
  return;
}


/* Helper for pph_out_cxx_binding.  STREAM and CB are as in
   pph_out_cxx_binding.  */

static void
pph_out_cxx_binding_1 (pph_stream *stream, cxx_binding *cb)
{
  struct bitpack_d bp;
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, cb, PPH_cxx_binding);
  if (pph_is_reference_or_end_marker (marker))
    return;

  /* Remove if we start emitting merge keys for this structure.  */
  gcc_assert (marker == PPH_RECORD_START);

  pph_out_tree (stream, cb->value);
  pph_out_tree (stream, cb->type);
  pph_out_binding_level_ref (stream, cb->scope);
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
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, cb, PPH_cp_class_binding);
  if (pph_is_reference_or_end_marker (marker))
    return;

  /* Remove if we start emitting merge keys for this structure.  */
  gcc_assert (marker == PPH_RECORD_START);

  pph_out_cxx_binding (stream, cb->base);
  pph_out_tree (stream, cb->identifier);
}


/* Write all the fields of cp_label_binding instance LB to STREAM.  */

static void
pph_out_label_binding (pph_stream *stream, cp_label_binding *lb)
{
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, lb, PPH_cp_label_binding);
  if (pph_is_reference_or_end_marker (marker))
    return;

  /* Remove if we start emitting merge keys for this structure.  */
  gcc_assert (marker == PPH_RECORD_START);

  pph_out_tree (stream, lb->label);
  pph_out_tree (stream, lb->prev_value);
}


/* Iterate over a chain FIRST, apply FILTER, and call output FUNCTION.  */

static void
pph_foreach_out_chain (pph_stream *stream, tree first, unsigned filter,
		       void (*function)(pph_stream *, tree))
{
  unsigned i;
  tree ns;
  VEC(tree,heap) *v;
  v = chain2vec_filter (stream, first, filter);
  pph_out_hwi (stream, VEC_length (tree, v));
  FOR_EACH_VEC_ELT_REVERSE (tree, v, i, ns)
    (*function) (stream, ns);
  VEC_free (tree, heap, v);
}


/* Write an index of mergeable namespaces from cp_binding_level BL to
   STREAM. */

static void
pph_out_merge_key_binding_level (pph_stream *stream, cp_binding_level *bl)
{
  unsigned filter;
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, bl, PPH_cp_binding_level);
  gcc_assert (marker != PPH_RECORD_END);

  pph_foreach_out_chain (stream, bl->namespaces, PPHF_NONE,
			 pph_out_merge_key_namespace_decl);
  filter = PPHF_NO_XREFS | PPHF_NO_PREFS | PPHF_NO_BUILTINS;
  pph_out_merge_key_chain (stream, bl->names, filter);
  pph_out_merge_key_chain (stream, bl->usings, filter);
  pph_out_merge_key_chain (stream, bl->using_directives, filter);
}


/* Write bodies of mergeable namespaces from from cp_binding_level BL
   to STREAM. */

static void
pph_out_merge_body_binding_level (pph_stream *stream, cp_binding_level *bl)
{
  unsigned i, filter;
  cp_class_binding *cs;
  cp_label_binding *sl;
  struct bitpack_d bp;

  pph_out_binding_level_ref (stream, bl);

  /* Write the fields that need merge reads.  */
  pph_foreach_out_chain (stream, bl->namespaces, PPHF_NONE,
			 pph_out_merge_body_namespace_decl);
  filter = PPHF_NO_XREFS | PPHF_NO_PREFS | PPHF_NO_BUILTINS;
  pph_out_merge_body_chain (stream, bl->names, filter);
  pph_out_merge_body_chain (stream, bl->usings, filter);
  pph_out_merge_body_chain (stream, bl->using_directives, filter);
  pph_out_tree_vec_filtered (stream, bl->static_decls, filter);

  /* Write the remaining fields.  */
  pph_out_tree (stream, bl->this_entity);

  pph_out_uint (stream, VEC_length (cp_class_binding, bl->class_shadowed));
  FOR_EACH_VEC_ELT (cp_class_binding, bl->class_shadowed, i, cs)
    pph_out_class_binding (stream, cs);

  pph_out_tree (stream, bl->type_shadowed);

  pph_out_uint (stream, VEC_length (cp_label_binding, bl->shadowed_labels));
  FOR_EACH_VEC_ELT (cp_label_binding, bl->shadowed_labels, i, sl)
    pph_out_label_binding (stream, sl);

  pph_out_tree (stream, bl->blocks);
  pph_out_binding_level_ref (stream, bl->level_chain);
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


/********************************************************** tree aux classes */


/* Write all the fields of language_function instance LF to STREAM.  */

static void
pph_out_language_function (pph_stream *stream, struct language_function *lf)
{
  struct bitpack_d bp;
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, lf, PPH_language_function);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  pph_out_tree_vec (stream, lf->base.x_stmt_tree.x_cur_stmt_list);
  pph_out_uint (stream, lf->base.x_stmt_tree.stmts_are_full_exprs_p);
  pph_out_tree (stream, lf->x_cdtor_label);
  pph_out_tree (stream, lf->x_current_class_ptr);
  pph_out_tree (stream, lf->x_current_class_ref);
  pph_out_tree (stream, lf->x_eh_spec_block);
  pph_out_tree (stream, lf->x_in_charge_parm);
  pph_out_tree (stream, lf->x_vtt_parm);
  pph_out_tree (stream, lf->x_return_value);
  bp = bitpack_create (stream->encoder.w.ob->main_stream);
  bp_pack_value (&bp, lf->returns_value, 1);
  bp_pack_value (&bp, lf->returns_null, 1);
  bp_pack_value (&bp, lf->returns_abnormally, 1);
  bp_pack_value (&bp, lf->x_in_function_try_handler, 1);
  bp_pack_value (&bp, lf->x_in_base_initializer, 1);
  bp_pack_value (&bp, lf->can_throw, 1);
  pph_out_bitpack (stream, &bp);

  /* FIXME pph.  We are not writing lf->x_named_labels.  */

  pph_out_binding_level_ref (stream, lf->bindings);
  pph_out_tree_vec (stream, lf->x_local_names);

  /* FIXME pph.  We are not writing lf->extern_decl_map.  */
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
  pph_out_tree (pti->stream, (tree) *slot);
  return 1;
}


/* Write applicable fields of struct function instance FN to STREAM.  */

static void
pph_out_struct_function (pph_stream *stream, struct function *fn)
{
  struct pph_tree_info pti;
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, fn, PPH_function);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

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
  pph_out_tree (stream, ldm->template_info);
  if (ldm->base.u2sel == 0)
    pph_out_tree (stream, ldm->u2.access);
  else if (ldm->base.u2sel == 1)
    pph_out_uint (stream, ldm->u2.discriminator);
  else
    gcc_unreachable ();
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

  pph_out_tree (stream, ldf->befriending_classes);
  pph_out_tree (stream, ldf->context);

  if (ldf->thunk_p == 0)
    pph_out_tree (stream, ldf->u5.cloned_function);
  else if (ldf->thunk_p == 1)
    pph_out_uint (stream, ldf->u5.fixed_offset);
  else
    gcc_unreachable ();

  if (ldf->pending_inline_p == 1)
    pph_out_token_cache (stream, ldf->u.pending_inline_info);
  else if (ldf->pending_inline_p == 0)
    pph_out_language_function (stream, ldf->u.saved_language_function);
}


/* Write all the fields of lang_decl_ns instance LDNS to STREAM.  */

static void
pph_out_ld_ns (pph_stream *stream, struct lang_decl_ns *ldns)
{
  pph_out_binding_level_ref (stream, ldns->level);
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
pph_out_lang_decl (pph_stream *stream, tree decl)
{
  struct lang_decl *ld;
  struct lang_decl_base *ldb;
  enum pph_record_marker marker;

  ld = DECL_LANG_SPECIFIC (decl);
  marker = pph_out_start_record (stream, ld, PPH_lang_decl);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

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


/********************************************************* tree base classes */


/* Write out the tree_common fields from T to STREAM.  */

static void
pph_out_tree_common (pph_stream *stream, tree t)
{
  /* The chain field in DECLs is handled separately.  Make sure this
     is never called with a DECL.  */
  gcc_assert (!DECL_P (t));

  /* The 'struct tree_typed typed' base class is handled in LTO.  */
  pph_out_tree (stream, TREE_CHAIN (t));
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


/* Write a struct sorted_fields_type instance SFT to STREAM.  */

static void
pph_out_sorted_fields_type (pph_stream *stream, struct sorted_fields_type *sft)
{
  int i;
  enum pph_record_marker marker;

  marker = pph_out_start_record (stream, sft, PPH_sorted_fields_type);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  pph_out_uint (stream, sft->len);
  for (i = 0; i < sft->len; i++)
    pph_out_tree (stream, sft->elts[i]);
}


/* Write all the packed bits in lang_type_class instance LTC to STREAM.  */

static void
pph_out_lang_type_class_bits (pph_stream *stream, struct lang_type_class *ltc)
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
}


/* Write all the fields in lang_type_class instance LTC to STREAM.  */

static void
pph_out_lang_type_class (pph_stream *stream, struct lang_type_class *ltc)
{
  pph_out_lang_type_class_bits (stream, ltc);
  pph_out_tree (stream, ltc->primary_base);
  pph_out_tree_pair_vec (stream, ltc->vcall_indices);
  pph_out_tree (stream, ltc->vtables);
  pph_out_tree (stream, ltc->typeinfo_var);
  pph_out_tree_vec (stream, ltc->vbases);
  {
    enum pph_record_marker marker;
    marker = pph_out_start_record (stream, ltc->nested_udts, PPH_binding_table);
    if (marker != PPH_RECORD_END)
      {
	gcc_assert (marker == PPH_RECORD_START_NO_CACHE);
	pph_out_binding_table (stream, ltc->nested_udts);
      }
  }
  pph_out_tree (stream, ltc->as_base);
  pph_out_tree_vec (stream, ltc->pure_virtuals);
  pph_out_tree (stream, ltc->friend_classes);
  pph_out_tree_vec (stream, ltc->methods);
  pph_out_tree (stream, ltc->key_method);
  pph_out_tree (stream, ltc->decl_list);
  pph_out_tree (stream, ltc->template_info);
  pph_out_tree (stream, ltc->befriending_classes);
  pph_out_tree (stream, ltc->objc_info);
  pph_out_sorted_fields_type (stream, ltc->sorted_fields);
  pph_out_tree (stream, ltc->lambda_expr);
}


/* Write all the merge key fields in lang_type_class instance LTC to STREAM.  */

static void
pph_out_merge_key_lang_type_class (pph_stream *stream,
                                  struct lang_type_class *ltc)
{
  pph_out_lang_type_class_bits (stream, ltc);
}


/* Write struct lang_type_ptrmem instance LTP to STREAM.  */

static void
pph_out_lang_type_ptrmem (pph_stream *stream, struct lang_type_ptrmem *ltp)
{
  pph_out_tree (stream, ltp->record);
}


/* Write all the lang-specific fields of TYPE to STREAM.  */

static void
pph_out_lang_type (pph_stream *stream, tree type)
{
  struct lang_type *lt;
  enum pph_record_marker marker;

  lt = TYPE_LANG_SPECIFIC (type);
  marker = pph_out_start_record (stream, lt, PPH_lang_type);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  pph_out_lang_type_header (stream, &lt->u.h);
  if (lt->u.h.is_lang_type_class)
    pph_out_lang_type_class (stream, &lt->u.c);
  else
    pph_out_lang_type_ptrmem (stream, &lt->u.ptrmem);
}


/* Write the merge keys the lang-specific fields of TYPE to STREAM.  */

static void
pph_out_merge_key_lang_type (pph_stream *stream, struct lang_type *lt)
{
  enum pph_record_marker marker;
  marker = pph_out_start_record (stream, lt, PPH_lang_type);
  if (marker == PPH_RECORD_END)
    return;

  gcc_assert (marker == PPH_RECORD_START_NO_CACHE);

  pph_out_lang_type_header (stream, &lt->u.h);
  gcc_assert (lt->u.h.is_lang_type_class);
  pph_out_merge_key_lang_type_class (stream, &lt->u.c);
}


/* Write machine_mode value MODE to STREAM.  */

static void
pph_out_machine_mode (pph_stream *stream, enum machine_mode mode)
{
  struct output_block *ob = stream->encoder.w.ob;
  streamer_write_enum (ob->main_stream, machine_mode, NUM_MACHINE_MODES, mode);
}


/* Write to STREAM the body of tcc_type node TYPE.  */

static void
pph_out_tcc_type (pph_stream *stream, tree type)
{
  pph_out_lang_type (stream, type);
  pph_out_tree (stream, TYPE_POINTER_TO (type));
  if (TREE_CODE (type) == POINTER_TYPE)
    pph_out_tree (stream, TYPE_NEXT_PTR_TO (type));
  pph_out_tree (stream, TYPE_REFERENCE_TO (type));
  pph_out_tree (stream, TYPE_NEXT_VARIANT (type));
  pph_out_machine_mode (stream, TYPE_MODE (type));
  /* We do not write TYPE_CANONICAL.  Instead, we emit the table of
     canonical types and re-instantiate it on read.  */
  pph_out_tree (stream, TREE_CHAIN (type));

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
      pph_out_tree (stream, TYPE_VALUES_RAW (type));
      break;

    default:
      break;
    }
}


/* Write to STREAM the body of tcc_declaration tree DECL.  */

static void
pph_out_tcc_declaration (pph_stream *stream, tree decl)
{
  pph_out_lang_decl (stream, decl);
  pph_out_tree (stream, DECL_INITIAL (decl));
  pph_out_tree (stream, DECL_ABSTRACT_ORIGIN (decl));

  /* The tree streamer only writes DECL_CHAIN for PARM_DECL nodes.
     We need to write DECL_CHAIN for variables and functions because
     they are sometimes chained together in places other than regular
     tree chains.  For example in BINFO_VTABLEs, the decls are chained
     together).  */
  if (TREE_CODE (decl) == VAR_DECL
      || TREE_CODE (decl) == FUNCTION_DECL)
    pph_out_tree (stream, DECL_CHAIN (decl));

  /* Handle some individual decl nodes.  */
  switch (TREE_CODE (decl))
    {
    case FUNCTION_DECL:
      /* Note that for FUNCTION_DECLs we do not output
	 DECL_STRUCT_FUNCTION here.  This is emitted at the end of the
	 PPH file in pph_out_replay. This way, we will be able to
	 re-instantiate them in the same order when reading the image
	 (the allocation of DECL_STRUCT_FUNCTION has the side effect
	 of generating function sequence numbers
	 (function.funcdef_no).  */
      pph_out_tree (stream, DECL_SAVED_TREE (decl));
      break;

    case TYPE_DECL:
      pph_out_tree (stream, DECL_ORIGINAL_TYPE (decl));
      break;

    case TEMPLATE_DECL:
      pph_out_tree (stream, DECL_TEMPLATE_RESULT (decl));
      pph_out_tree (stream, DECL_TEMPLATE_PARMS (decl));
      break;

    default:
      break;
    }
}


/******************************************************** tree head and body */


/* Emit the bindings for an identifier expr. */

static void
pph_out_identifier_bindings (pph_stream *stream, tree expr)
{
  pph_out_tree (stream, REAL_IDENTIFIER_TYPE_VALUE (expr));
}


/* Write the body of EXPR to STREAM.  This writes out all fields not
   written by the generic tree streaming routines.  */

static void
pph_out_tree_body (pph_stream *stream, tree expr)
{
  bool handled_p;

  /* Write the language-independent parts of EXPR's body.  */
  streamer_write_tree_body (stream->encoder.w.ob, expr, false);

  /* Handle common tree code classes first.  */
  handled_p = true;
  switch (TREE_CODE_CLASS (TREE_CODE (expr)))
    {
      case tcc_declaration:
	pph_out_tcc_declaration (stream, expr);
	break;

      case tcc_type:
	pph_out_tcc_type (stream, expr);
	break;

      case tcc_constant:
	if (TREE_CODE (expr) == PTRMEM_CST)
	  {
	    pph_out_tree_common (stream, expr);
	    pph_out_tree (stream, PTRMEM_CST_MEMBER (expr));
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
        tree_stmt_iterator i;
        unsigned num_stmts;

        /* Compute and write the number of statements in the list.  */
        for (num_stmts = 0, i = tsi_start (expr); !tsi_end_p (i); tsi_next (&i))
	  num_stmts++;

        pph_out_uint (stream, num_stmts);

        /* Write the statements.  */
        for (i = tsi_start (expr); !tsi_end_p (i); tsi_next (&i))
	  pph_out_tree (stream, tsi_stmt (i));
      }
      break;

    case OVERLOAD:
      pph_out_tree_common (stream, expr);
      pph_out_tree (stream, OVL_CURRENT (expr));
      break;

    case IDENTIFIER_NODE:
      pph_out_identifier_bindings (stream, expr);
      break;

    case BASELINK:
      pph_out_tree_common (stream, expr);
      pph_out_tree (stream, BASELINK_BINFO (expr));
      pph_out_tree (stream, BASELINK_FUNCTIONS (expr));
      pph_out_tree (stream, BASELINK_ACCESS_BINFO (expr));
      break;

    case TEMPLATE_INFO:
      pph_out_tree_common (stream, expr);
      pph_out_qual_use_vec (stream, TI_TYPEDEFS_NEEDING_ACCESS_CHECKING (expr));
      break;

    case DEFAULT_ARG:
      pph_out_tree_common (stream, expr);
      pph_out_token_cache (stream, DEFARG_TOKENS (expr));
      pph_out_tree_vec (stream, DEFARG_INSTANTIATIONS (expr));
      break;

    case STATIC_ASSERT:
      pph_out_tree_common (stream, expr);
      pph_out_tree (stream, STATIC_ASSERT_CONDITION (expr));
      pph_out_tree (stream, STATIC_ASSERT_MESSAGE (expr));
      pph_out_location (stream, STATIC_ASSERT_SOURCE_LOCATION (expr));
      break;

    case ARGUMENT_PACK_SELECT:
      pph_out_tree_common (stream, expr);
      pph_out_tree (stream, ARGUMENT_PACK_SELECT_FROM_PACK (expr));
      pph_out_uint (stream, ARGUMENT_PACK_SELECT_INDEX (expr));
      break;

    case TRAIT_EXPR:
      pph_out_tree_common (stream, expr);
      pph_out_tree (stream, TRAIT_EXPR_TYPE1 (expr));
      pph_out_tree (stream, TRAIT_EXPR_TYPE2 (expr));
      pph_out_uint (stream, TRAIT_EXPR_KIND (expr));
      break;

    case LAMBDA_EXPR:
      pph_out_tree_common (stream, expr);
      pph_out_location (stream, LAMBDA_EXPR_LOCATION (expr));
      pph_out_tree (stream, LAMBDA_EXPR_CAPTURE_LIST (expr));
      pph_out_tree (stream, LAMBDA_EXPR_THIS_CAPTURE (expr));
      pph_out_tree (stream, LAMBDA_EXPR_RETURN_TYPE (expr));
      pph_out_tree (stream, LAMBDA_EXPR_EXTRA_SCOPE (expr));
      pph_out_uint (stream, LAMBDA_EXPR_DISCRIMINATOR (expr));
      break;

    case TREE_VEC:
      /* TREE_VECs hold template argument lists.  */
      pph_out_tree (stream, NON_DEFAULT_TEMPLATE_ARGS_COUNT (expr));
      break;

    case PLACEHOLDER_EXPR:
      pph_out_tree (stream, TREE_TYPE (expr));
      break;

    case TEMPLATE_PARM_INDEX:
      pph_out_tree_common (stream, expr);
      pph_out_uint (stream, TEMPLATE_PARM_IDX (expr));
      pph_out_uint (stream, TEMPLATE_PARM_LEVEL (expr));
      pph_out_uint (stream, TEMPLATE_PARM_ORIG_LEVEL (expr));
      pph_out_uint (stream, TEMPLATE_PARM_NUM_SIBLINGS (expr));
      pph_out_tree (stream, TEMPLATE_PARM_DECL (expr));
      break;

    case DEFERRED_NOEXCEPT:
      pph_out_tree (stream, DEFERRED_NOEXCEPT_PATTERN (expr));
      pph_out_tree (stream, DEFERRED_NOEXCEPT_ARGS (expr));
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


/* Pack all the bitfields of EXPR into BP.  */

static void
pph_pack_value_fields (struct bitpack_d *bp, tree expr)
{
  /* First pack all the language-independent bitfields.  */
  streamer_pack_tree_bitfields (bp, expr);

  /* Now pack all the bitfields not handled by the generic packer.  */
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


/* Write the header fields for EXPR to STREAM.  This header contains
   all the data needed to rematerialize EXPR on the reader side and
   a bitpack with all the bitfield values in EXPR.  */

static void
pph_out_tree_header (pph_stream *stream, tree expr)
{
  struct bitpack_d bp;
  struct output_block *ob = stream->encoder.w.ob;

  /* Write the header, containing everything needed to materialize EXPR
     on the reading side.  */
  streamer_write_tree_header (ob, expr);

  /* Process C++ specific codes that need more data in the header
     for the reader to allocate them.  */
  if (TREE_CODE (expr) == AGGR_INIT_EXPR)
    pph_out_uint (stream, aggr_init_expr_nargs (expr));

  /* Pack all the non-pointer fields in EXPR into a bitpack and write
     the resulting bitpack.  */
  bp = bitpack_create (ob->main_stream);
  pph_pack_value_fields (&bp, expr);
  pph_out_bitpack (stream, &bp);
}


/* Return the merge name string identifier tree for a decl EXPR.  The
   caller is responsible of freeing the returned string.  */

static char *
pph_merge_name (tree expr)
{
  char *retval;
  size_t len;
  const char *mangled_str, *name_str;
  tree name;

  unsigned flags = TFF_PLAIN_IDENTIFIER
		   | TFF_SCOPE
		   | TFF_DECL_SPECIFIERS
		   | TFF_CLASS_KEY_OR_ENUM
		   | TFF_RETURN_TYPE
		   | TFF_LOC_FOR_TEMPLATE_PARMS;

  if (TYPE_P (expr))
    expr = TYPE_NAME (expr);

  name_str = decl_as_string (expr, flags);

  name = DECL_NAME (expr);
  if (name)
    {
      if (TREE_CODE (expr) == FUNCTION_DECL)
	flags |= TFF_RETURN_TYPE
		 | TFF_FUNCTION_DEFAULT_ARGUMENTS
		 | TFF_EXCEPTION_SPECIFICATION;
      else if (TREE_CODE (expr) == TEMPLATE_DECL)
	flags |= TFF_TEMPLATE_HEADER
		 | TFF_NO_OMIT_DEFAULT_TEMPLATE_ARGUMENTS;
      mangled_str = IDENTIFIER_POINTER (get_mangled_id (expr));
      len = strlen (name_str) + sizeof("|") + strlen (mangled_str);
      retval = XNEWVEC (char, len + 1);
      sprintf (retval, "%s|%s", name_str, mangled_str);
    }
  else
    {
      location_t locus = DECL_SOURCE_LOCATION (expr);
      expanded_location xloc = expand_location (locus);
      /* There are at most 20 digits in size_t.  Add :| for 22 characters.  */
      len = strlen (name_str) + strlen (xloc.file) + 22;
      retval = XNEWVEC (char, len + 1);
      sprintf (retval, "%s|%s:%u", name_str, xloc.file, xloc.line);
    }

  return retval;
}


/* Write the merge name string for a decl EXPR.  */

static void
pph_out_merge_name (pph_stream *stream, tree expr)
{
  char *name = pph_merge_name (expr);
  pph_trace_note ("merge name", name);
  pph_out_string (stream, name);
  free (name);
}


/* Write merge key information for a namespace DECL to STREAM.  */

static void
pph_out_merge_key_namespace_decl (pph_stream *stream, tree decl)
{
  bool is_namespace_alias;

  gcc_assert (TREE_CODE (decl) == NAMESPACE_DECL);

  pph_out_start_merge_key_tree_record (stream, decl);

  pph_out_tree_header (stream, decl);
  pph_out_merge_name (stream, decl);

  if (flag_pph_tracer)
    pph_trace_tree (decl, NULL, pph_trace_front, pph_trace_key_out);

  pph_out_tree (stream, DECL_NAME (decl));

  /* If EXPR is a namespace alias, it will not have an associated
     binding.  In that case, tell the reader not to bother with it.  */
  is_namespace_alias = (DECL_NAMESPACE_ALIAS (decl) != NULL_TREE);
  pph_out_bool (stream, is_namespace_alias);
  if (!is_namespace_alias)
    pph_out_merge_key_binding_level (stream, NAMESPACE_LEVEL (decl));

  if (flag_pph_tracer)
    pph_trace_tree (decl, NULL, pph_trace_back, pph_trace_key_out);
}


/* Write merge body information for a namespace DECL to STREAM.  */

static void
pph_out_merge_body_namespace_decl (pph_stream *stream, tree decl)
{
  bool is_namespace_alias;

  gcc_assert (TREE_CODE (decl) == NAMESPACE_DECL);

  pph_out_start_tree_record (stream, decl);
  pph_out_tree_header (stream, decl);

  if (flag_pph_tracer)
    pph_trace_tree (decl, NULL, pph_trace_front, pph_trace_merge_body);

  /* If EXPR is a namespace alias, it will not have an associated
     binding.  In that case, tell the reader not to bother with it.  */
  is_namespace_alias = (DECL_NAMESPACE_ALIAS (decl) != NULL_TREE);
  pph_out_bool (stream, is_namespace_alias);
  if (!is_namespace_alias)
    pph_out_merge_body_binding_level (stream, NAMESPACE_LEVEL (decl));

  if (flag_pph_tracer)
    pph_trace_tree (decl, NULL, pph_trace_back, pph_trace_merge_body);
}


/* Write the merge key for tree EXPR to STREAM.  */

void
pph_out_merge_key_tree (pph_stream *stream, tree expr, bool name_type)
{
  if (pph_out_start_merge_key_tree_record (stream, expr))
    return;

  if (flag_pph_tracer)
    pph_trace_tree (expr, NULL, pph_trace_front, pph_trace_key_out);

  /* Write merge key information.  This includes EXPR's header (needed
     to re-allocate EXPR in the reader). */
  pph_out_tree_header (stream, expr);

  if (DECL_P (expr))
    {
      /* Write the merge name, used to lookup EXPR in the reader's context
	 and merge if necessary.  */
      pph_out_merge_name (stream, expr);

      if (TREE_CODE (expr) == TYPE_DECL)
	{
	  bool is_implicit = DECL_IMPLICIT_TYPEDEF_P (expr);
	  tree type = TREE_TYPE (expr);
	  pph_out_bool (stream, is_implicit);
	  if (is_implicit)
	    pph_out_merge_key_tree (stream, type, false);
	}
    }
  else
    {
      if (name_type)
	pph_out_merge_name (stream, expr);
      if (CLASS_TYPE_P (expr))
	{
	  unsigned filter = PPHF_NO_XREFS | PPHF_NO_PREFS | PPHF_NO_BUILTINS;
	  pph_out_merge_key_chain (stream, TYPE_FIELDS (expr), filter);
	  pph_out_merge_key_chain (stream, TYPE_METHODS (expr), filter);
	  pph_out_merge_key_lang_type (stream, TYPE_LANG_SPECIFIC (expr));
	}
    }

  if (flag_pph_tracer)
    pph_trace_tree (expr, NULL, pph_trace_back, pph_trace_key_out);
}


/* Write a tree EXPR to STREAM.  */

void
pph_out_tree (pph_stream *stream, tree expr)
{
  enum pph_record_marker marker;

  marker = pph_out_start_tree_record (stream, expr);

  /* If EXPR is NULL or it already existed in the pickle cache,
     nothing else needs to be done.  */
  if (pph_is_reference_or_end_marker (marker))
    return;

  if (streamer_handle_as_builtin_p (expr))
    {
      /* MD and NORMAL builtins do not need to be written out
         completely as they are always instantiated by the compiler on
         startup.  The only builtins that need to be written out are
         BUILT_IN_FRONTEND.  For all other builtins, we simply write
         the class and code.  */
      gcc_assert (marker == PPH_RECORD_START_NO_CACHE);
      streamer_write_builtin (stream->encoder.w.ob, expr);
      return;
    }
  else if (TREE_CODE (expr) == INTEGER_CST)
    {
      /* INTEGER_CST nodes are special because they need their
	 original type to be materialized by the reader (to implement
	 TYPE_CACHED_VALUES).  */
      gcc_assert (marker == PPH_RECORD_START_NO_CACHE);
      streamer_write_integer_cst (stream->encoder.w.ob, expr, false);
      return;
    }

  if (flag_pph_tracer)
    pph_trace_tree (expr, NULL, pph_trace_front,
	marker == PPH_RECORD_START_MERGE_BODY ? pph_trace_merge_body
	: marker == PPH_RECORD_START_MUTATED ? pph_trace_mutate
	: pph_trace_normal);

  if (marker == PPH_RECORD_START || marker == PPH_RECORD_START_MUTATED)
    {
      pph_out_tree_header (stream, expr);
      pph_out_tree_body (stream, expr);
    }
  else if (marker == PPH_RECORD_START_MERGE_BODY)
    {
      gcc_assert (pph_tree_is_mergeable (expr));

      /* When writing a merge body, we do not need to write EXPR's
	 header, since that was written out when we wrote the merge
	 key record for it.  */
      pph_out_tree_body (stream, expr);
    }
  else
    gcc_unreachable ();

  if (flag_pph_tracer)
    pph_trace_tree (expr, NULL, pph_trace_back,
	marker == PPH_RECORD_START_MERGE_BODY ? pph_trace_merge_body
	: marker == PPH_RECORD_START_MUTATED ? pph_trace_mutate
	: pph_trace_normal);
}


/************************************************************* file contents */


/* Emit symbol table ACTION to STREAM.  */

static inline void
pph_out_replay_action (pph_stream *stream, enum pph_replay_action action)
{
  gcc_assert ((action == PPH_REPLAY_DECLARE
	       || action == PPH_REPLAY_EXPAND
	       || action == PPH_REPLAY_EXPAND_1
	       || action == PPH_REPLAY_FINISH_STRUCT_METHODS)
              && action == (enum pph_replay_action)(unsigned char) action);
  pph_out_uchar (stream, action);
}

/* Add DECL to the symbol table for pph_out_stream.  ACTION determines
   how DECL should be presented to the middle-end when reading this
   image.  TOP_LEVEL and AT_END are as in rest_of_decl_compilation.  */

void
pph_add_decl_to_replay (tree decl, enum pph_replay_action action,
			bool top_level, bool at_end)
{
  pph_replay_entry entry;

  if (decl == NULL || pph_out_stream == NULL)
    return;

  gcc_assert (DECL_P (decl));

  entry.action = action;
  entry.to_replay = decl;
  entry.top_level = top_level;
  entry.at_end = at_end;

  /* Capture some global state that is needed when re-playing this
     entry.  FIXME pph, this should not be needed.  There is even
     inconsistencies in the values for AT_END, the values passed on by
     various callers is not necessarily the same as the value for
     ENTRY.AT_EOF below (sigh).  */
  entry.x_processing_template_decl = processing_template_decl;
  entry.function_depth = function_depth;
  gcc_assert (at_end == at_eof && (at_eof == 0 || at_eof == 1));
  entry.at_eof = at_eof;

  VEC_safe_push (pph_replay_entry, heap, pph_out_stream->replay.v, &entry);
}


/* Add TYPE to the symbol table, to be re-played according to ACTION.  */

void
pph_add_type_to_replay (tree type, enum pph_replay_action action)
{
  pph_replay_entry entry;

  if (pph_out_stream == NULL)
    return;

  gcc_assert (TYPE_P (type));

  memset (&entry, 0, sizeof (entry));
  entry.action = action;
  entry.to_replay = type;
  VEC_safe_push (pph_replay_entry, heap, pph_out_stream->replay.v, &entry);
}


/* Emit the symbol table for STREAM.  When this image is read into
   another translation unit, we want to guarantee that the IL
   instances taken from this image are instantiated in the same order
   that they were instantiated when we generated this image.

   With this, we can generate code in the same order out of the
   original header files and out of PPH images.  */

static void
pph_out_replay (pph_stream *stream)
{
  pph_replay_entry *entry;
  unsigned i;

  timevar_start (TV_PPH_OUT_REPLAY);

  pph_out_uint (stream, VEC_length (pph_replay_entry, stream->replay.v));
  FOR_EACH_VEC_ELT (pph_replay_entry, stream->replay.v, i, entry)
    {
      struct bitpack_d bp;

      pph_out_replay_action (stream, entry->action);
      pph_out_tree (stream, entry->to_replay);
      bp = bitpack_create (stream->encoder.w.ob->main_stream);
      bp_pack_value (&bp, entry->top_level, 1);
      bp_pack_value (&bp, entry->at_end, 1);
      bp_pack_value (&bp, entry->at_eof, 1);
      pph_out_bitpack (stream, &bp);
      pph_out_int (stream, entry->x_processing_template_decl);
      pph_out_int (stream, entry->function_depth);
      if (entry->action == PPH_REPLAY_EXPAND
	  || entry->action == PPH_REPLAY_EXPAND_1)
	{
	  pph_out_struct_function (stream,
				   DECL_STRUCT_FUNCTION (entry->to_replay));
	  pph_out_bool (stream, cgraph_get_node (entry->to_replay) != NULL);
	}

      pph_stats.num_replay_actions_by_action[entry->action]++;
    }

  pph_stats.num_replay_actions = VEC_length (pph_replay_entry,
					     stream->replay.v);

  timevar_stop (TV_PPH_OUT_REPLAY);
}


/* Save the IDENTIFIERS to the STREAM.  */

static void
pph_out_identifiers (pph_stream *stream, cpp_idents_used *identifiers)
{
  unsigned int num_entries, active_entries, id;

  timevar_start (TV_PPH_OUT_IDENTIFIERS);

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

  timevar_stop (TV_PPH_OUT_IDENTIFIERS);
}


/* Write the keys for global bindings in scope_chain to STREAM.  */

static void
pph_out_global_binding_keys (pph_stream *stream)
{
  cp_binding_level *bl;

  timevar_start (TV_PPH_OUT_BL_K);

  /* We only need to write out the scope_chain->bindings, everything
     else should be NULL or be some temporary disposable state.
     old_namespace should be global_namespace and all entries listed
     below should be NULL or 0; otherwise the header parsed was
     incomplete.  */
  gcc_assert (scope_chain->old_namespace == global_namespace
	      && !(scope_chain->class_name
		   || scope_chain->class_type
		   || scope_chain->access_specifier
		   || scope_chain->function_decl
		   || scope_chain->template_parms
		   || scope_chain->x_saved_tree
		   || scope_chain->class_bindings
		   || scope_chain->prev
		   || scope_chain->unevaluated_operand
		   || scope_chain->inhibit_evaluation_warnings
		   || scope_chain->x_processing_template_decl
		   || scope_chain->x_processing_specialization
		   || scope_chain->x_processing_explicit_instantiation
		   || scope_chain->need_pop_function_context)
	      && VEC_empty (tree, scope_chain->x_stmt_tree.x_cur_stmt_list));

  /* We need to write a record for BL before emitting the merge keys
     so that the reader can associate the merge keys to it.  */
  bl = scope_chain->bindings;
  pph_out_start_record (stream, bl, PPH_cp_binding_level);

  /* Emit all the merge keys for objects that need to be merged when
     reading multiple PPH images.  */
  pph_out_merge_key_binding_level (stream, bl);

  timevar_stop (TV_PPH_OUT_BL_K);
}


/* Write the bodies for global bindings in scope_chain to STREAM.  */

static void
pph_out_global_binding_bodies (pph_stream *stream)
{
  cp_binding_level *bl = scope_chain->bindings;

  timevar_start (TV_PPH_OUT_BL_B);

  /* Now emit all the bodies.  */
  pph_out_merge_body_binding_level (stream, bl);

  timevar_stop (TV_PPH_OUT_BL_B);
}


/* Write all the contents of STREAM.  */

static void
pph_write_file (pph_stream *stream)
{
  cpp_idents_used idents_used;

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "\nPPH: Writing %s\n", pph_out_file);

  /* Emit the line table entries and references to our direct includes.   */
  pph_out_line_table_and_includes (stream);

  /* Emit all the identifiers and pre-processor symbols in the global
     namespace.  */
  idents_used = cpp_lt_capture (parse_in);
  pph_out_identifiers (stream, &idents_used);

  /* Emit the bindings for namespace scopes and template state.  */
  pph_out_global_binding_keys (stream);
  pph_out_merge_key_template_state (stream);
  pph_out_global_binding_bodies (stream);
  pph_out_merge_body_template_state (stream);

  /* Emit other global state kept by the parser.  FIXME pph, these
     globals should be fields in struct cp_parser.  */
  timevar_start (TV_PPH_OUT_MISC);
  pph_out_tree (stream, keyed_classes);
  pph_out_tree_vec (stream, unemitted_tinfo_decls);
  pph_out_tree (stream, static_aggregates);
  pph_out_decl2_hidden_state (stream);
  pph_out_canonical_template_parms (stream);
  timevar_stop (TV_PPH_OUT_MISC);

  /* Emit the symbol table.  The symbol table must be emitted at the
     end because all the symbols read from children PPH images are not
     known in advance when we start reading this file in the reader.  */
  pph_out_replay (stream);

  if (flag_pph_dump_tree)
    pph_dump_global_state (pph_logfile, "after pph write");
}


/******************************************************* stream finalization */


/* Callback for lang_hooks.lto.begin_section.  Open file NAME.  */

static void
pph_begin_section (const char *name ATTRIBUTE_UNUSED)
{
}


/* Callback for lang_hooks.lto.append_data.  Write LEN bytes from DATA
   into pph_out_stream.  BLOCK is currently unused.  */

static void
pph_out_data (const void *data, size_t len, void *block ATTRIBUTE_UNUSED)
{
  if (data)
    {
      size_t written = fwrite (data, 1, len, pph_out_stream->file);
      gcc_assert (written == len);
    }
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

  /* Write the main stream where we have been pickling the parse trees.  */
  lto_write_stream (stream->encoder.w.ob->main_stream);
}


/* Flush all the in-memory buffers for STREAM to disk.  */

void
pph_flush_buffers (pph_stream *stream)
{
  /* Redirect the LTO basic I/O langhooks.  */
  lang_hooks.lto.begin_section = pph_begin_section;
  lang_hooks.lto.append_data = pph_out_data;
  lang_hooks.lto.end_section = pph_end_section;

  /* Write the state buffers built by pph_out_*() calls.  */
  lto_begin_section (stream->name, false);
  pph_out_header (stream);
  pph_out_body (stream);
  lto_end_section ();
}


/* Finalize the PPH writer.  */

void
pph_writer_finish (void)
{
  if (pph_out_stream == NULL)
    return;

  timevar_start (TV_PPH_OUT);

  if (!pph_check_main_missing_guard || pph_check_main_guarded ())
    pph_write_file (pph_out_stream);

  pph_stream_close (pph_out_stream);
  pph_out_stream = NULL;

  timevar_stop (TV_PPH_OUT);
}


/* Disable PPH generation.  Used when we discover that the file that we
   are currently converting into PPH is not compatible.  This does not
   necessarily stop compilation, so make sure that the PPH file is
   not generated.  */

void
pph_disable_output (void)
{
  /* If we are not generating a PPH image, do nothing.  */
  if (!pph_out_stream)
    return;

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Disabling PPH generation for %s\n",
	     pph_out_stream->header_name);

  pph_stream_close_no_flush (pph_out_stream);
  pph_out_file = NULL;
  pph_out_stream = NULL;

  /* Also disable the reader.  We are not generating a PPH image anymore,
     so it makes no sense to keep reading images.  FIXME pph, instead
     of disabling PPH generation we may want to simply abort compilation.  */
  pph_disable_reader ();
}
