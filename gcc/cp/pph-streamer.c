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
#include "streamer-hooks.h"

/* List of PPH images opened for reading.  Images opened during #include
   processing and opened from pph_in_includes cannot be closed
   immediately after reading, because the pickle cache contained in
   them may be referenced from other images.  We delay closing all of
   them until the end of parsing (when pph_streamer_finish is called).  */
static VEC(pph_stream_ptr, heap) *pph_read_images = NULL;

/* A cache of pre-loaded common tree nodes.  */
static pph_cache *pph_preloaded_cache;

/* Pre-load common tree nodes into CACHE.  These nodes are always built by the
   front end, so there is no need to pickle them.  */

static void
pph_cache_preload (pph_cache *cache)
{
  unsigned i;

  for (i = itk_char; i < itk_none; i++)
    pph_cache_add (cache, integer_types[i], NULL,
                   pph_tree_code_to_tag (integer_types[i]));

  for (i = 0; i < TYPE_KIND_LAST; i++)
    pph_cache_add (cache, sizetype_tab[i], NULL,
                   pph_tree_code_to_tag (sizetype_tab[i]));

  /* global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < TI_MAX; i++)
    if (global_trees[i])
      pph_cache_add (cache, global_trees[i], NULL,
                     pph_tree_code_to_tag (global_trees[i]));

  /* c_global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < CTI_MAX; i++)
    if (c_global_trees[i])
      pph_cache_add (cache, c_global_trees[i], NULL,
                     pph_tree_code_to_tag (c_global_trees[i]));

  /* cp_global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < CPTI_MAX; i++)
    {
      /* Also skip trees which are generated while parsing.  */
      if (i == CPTI_KEYED_CLASSES)
	continue;

      if (cp_global_trees[i])
	pph_cache_add (cache, cp_global_trees[i], NULL,
                       pph_tree_code_to_tag (cp_global_trees[i]));
    }

  /* Add other well-known nodes that should always be taken from the
     current compilation context.  */
  pph_cache_add (cache, global_namespace, NULL,
                 pph_tree_code_to_tag (global_namespace));
  pph_cache_add (cache, DECL_CONTEXT (global_namespace), NULL,
                 pph_tree_code_to_tag (DECL_CONTEXT (global_namespace)));
}


/* Callback for writing ASTs to a stream.  Write EXPR to the PPH stream
   in OB.  */

static void
pph_write_tree (struct output_block *ob, tree expr, bool ref_p ATTRIBUTE_UNUSED)
{
  pph_out_tree ((pph_stream *) ob->sdata, expr);
}


/* Callback for reading ASTs from a stream.  Instantiate and return a
   new tree from the PPH stream in DATA_IN.  */

static tree
pph_read_tree (struct lto_input_block *ib ATTRIBUTE_UNUSED,
	       struct data_in *data_in)
{
  return pph_in_tree ((pph_stream *) data_in->sdata);
}


/* Callback for streamer_hooks.input_location.  An offset is applied to
   the location_t read in according to the properties of the merged
   line_table.  IB and DATA_IN are as in lto_input_location.  This function
   should only be called after pph_in_and_merge_line_table was called as
   we expect pph_loc_offset to be set.  */

static location_t
pph_input_location (struct lto_input_block *ib ATTRIBUTE_UNUSED,
                    struct data_in *data_in)
{
  return pph_in_location ((pph_stream *) data_in->sdata);
}


/* Callback for streamer_hooks.output_location.  Output the LOC directly,
   an offset will be applied on input after rebuilding the line_table.
   OB and LOC are as in lto_output_location.  */

static void
pph_output_location (struct output_block *ob, location_t loc)
{
  pph_out_location ((pph_stream *) ob->sdata, loc);
}


/* Initialize all the streamer hooks used for streaming ASTs.  */

static void
pph_hooks_init (void)
{
  streamer_hooks_init ();
  streamer_hooks.write_tree = pph_write_tree;
  streamer_hooks.read_tree = pph_read_tree;
  streamer_hooks.input_location = pph_input_location;
  streamer_hooks.output_location = pph_output_location;
}


/* Initialize an empty pickle CACHE.  */

static void
pph_cache_init (pph_cache *cache)
{
  cache->v = NULL;
  cache->m = pointer_map_create ();
}


/* Initialize the pre-loaded cache.  This contains all the common
   tree nodes built by the compiler on startup.  */

static void
pph_init_preloaded_cache (void)
{
  pph_preloaded_cache = XCNEW (pph_cache);
  pph_cache_init (pph_preloaded_cache);
  pph_cache_preload (pph_preloaded_cache);
}


/* Initialize the streamer.  */

void
pph_streamer_init (void)
{
  pph_hooks_init ();
  pph_init_preloaded_cache ();
}


/* Finalize the streamer.  */

void
pph_streamer_finish (void)
{
  unsigned i;
  pph_stream *image;

  /* Finalize the writer.  */
  pph_writer_finish ();

  /* Close any images read during parsing.  */
  FOR_EACH_VEC_ELT (pph_stream_ptr, pph_read_images, i, image)
    pph_stream_close (image);

  VEC_free (pph_stream_ptr, heap, pph_read_images);
}


/* If FILENAME has already been read, return the stream associated with it.  */

static pph_stream *
pph_find_stream_for (const char *filename)
{
  pph_stream *include;
  unsigned i;

  /* FIXME pph, implement a hash map to avoid this linear search.  */
  FOR_EACH_VEC_ELT (pph_stream_ptr, pph_read_images, i, include)
    if (strcmp (include->name, filename) == 0)
      return include;

  return NULL;
}


/* Add STREAM to the list of read images.  */

void
pph_mark_stream_read (pph_stream *stream)
{
  stream->in_memory_p = true;
  VEC_safe_push (pph_stream_ptr, heap, pph_read_images, stream);
}


/* Create a new PPH stream to be stored on the file called NAME.
   MODE is passed to fopen directly.  */

pph_stream *
pph_stream_open (const char *name, const char *mode)
{
  pph_stream *stream;
  FILE *f;

  /* If we have already opened a PPH stream named NAME, just return
     its associated stream.  */
  stream = pph_find_stream_for (name);
  if (stream)
    {
      gcc_assert (stream->in_memory_p);
      return stream;
    }

  f = fopen (name, mode);
  if (!f)
    return NULL;

  stream = XCNEW (pph_stream);
  stream->file = f;
  stream->name = xstrdup (name);
  stream->write_p = (strchr (mode, 'w') != NULL);
  pph_cache_init (&stream->cache);
  stream->preloaded_cache = pph_preloaded_cache;
  if (stream->write_p)
    pph_init_write (stream);
  else
    pph_init_read (stream);

  return stream;
}



/* Close PPH stream STREAM.  */

void
pph_stream_close (pph_stream *stream)
{
  /* STREAM can be NULL if it could not be properly opened.  An error
     has already been emitted, so avoid crashing here.  */
  if (stream == NULL)
    return;

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Closing %s\n", stream->name);

  /* If we were writing to STREAM, flush all the memory buffers.  This
     does the actual writing of all the pickled data structures.  */
  if (stream->write_p)
    pph_flush_buffers (stream);

  fclose (stream->file);

  /* Deallocate all memory used.  */
  stream->file = NULL;
  VEC_free (pph_cache_entry, heap, stream->cache.v);
  pointer_map_destroy (stream->cache.m);
  VEC_free (pph_symtab_entry, heap, stream->symtab.v);
  VEC_free (pph_stream_ptr, heap, stream->includes);

  if (stream->write_p)
    {
      destroy_output_block (stream->encoder.w.ob);
      free (stream->encoder.w.decl_state_stream);
      lto_delete_out_decl_state (stream->encoder.w.out_state);
    }
  else
    {
      unsigned i;

      free (stream->encoder.r.ib);
      lto_data_in_delete (stream->encoder.r.data_in);
      for (i = 0; i < PPH_NUM_SECTIONS; i++)
	free (stream->encoder.r.pph_sections[i]);
      free (stream->encoder.r.pph_sections);
      free (stream->encoder.r.file_data);
    }

  free (stream);
}


/* Add INCLUDE, and the images included by it, to the list of files
   included by STREAM.  */

void
pph_add_include (pph_stream *stream, pph_stream *include)
{
  pph_stream *include_child;
  unsigned i;

  VEC_safe_push (pph_stream_ptr, heap, stream->includes, include);
  FOR_EACH_VEC_ELT (pph_stream_ptr, include->includes, i, include_child)
    VEC_safe_push (pph_stream_ptr, heap, stream->includes, include_child);
}


/* Print tracing information for a possibly MERGEABLE tree T.  */

void
pph_trace_tree (tree t, bool mergeable, bool merged)
{
  bool emit = false;
  char merging = merged ? '#' : mergeable ? '*' : '.';
  bool is_decl = DECL_P (t);
  char userdef =  is_decl ? '*' : '.';

  if (mergeable && is_decl && flag_pph_tracer >= 2)
    emit = true;
  else if ((mergeable || is_decl) && flag_pph_tracer >= 3)
    emit = true;
  else if (!EXPR_P (t) && flag_pph_tracer >= 4)
    emit = true;

  if (emit)
    {
      enum tree_code code = TREE_CODE (t);
      fprintf (pph_logfile, "PPH: %c%c ", merging, userdef);
      fprintf (pph_logfile, "%-19s ", pph_tree_code_text (code));
      pph_dump_tree_name (pph_logfile, t, 0);
    }
}


/* Insert DATA in CACHE at slot IX.  TAG represents the data structure
   pointed-to by DATA.  As a restriction to prevent stomping on cache
   entries, this will not allow inserting into the same slot more than
   once.  */

void
pph_cache_insert_at (pph_cache *cache, void *data, unsigned ix,
                     enum pph_tag tag)
{
  void **map_slot;
  pph_cache_entry e = { data, tag, 0, 0 };

  map_slot = pointer_map_insert (cache->m, data);

  /* We should not be trying to insert the same data more than once.  */
  gcc_assert (*map_slot == NULL);

  *map_slot = (void *) (intptr_t) ix;
  if (ix + 1 > VEC_length (pph_cache_entry, cache->v))
    VEC_safe_grow_cleared (pph_cache_entry, heap, cache->v, ix + 1);
  VEC_replace (pph_cache_entry, cache->v, ix, &e);
}


/* Return true if DATA exists in CACHE.  If IX_P is not NULL, store
   the cache slot where DATA resides in *IX_P (or (unsigned)-1 if DATA
   is not found). If CACHE is NULL use pph_preloaded_cache.

   If a cache hit is found, the data type tag for the entry must match
   TAG.  */

bool
pph_cache_lookup (pph_cache *cache, void *data, unsigned *ix_p,
                  enum pph_tag tag)
{
  void **map_slot;
  unsigned ix;
  bool existed_p;

  if (cache == NULL)
    cache = pph_preloaded_cache;

  map_slot = pointer_map_contains (cache->m, data);
  if (map_slot == NULL)
    {
      existed_p = false;
      ix = (unsigned) -1;
    }
  else
    {
      intptr_t slot_ix = (intptr_t) *map_slot;
      gcc_assert (slot_ix == (intptr_t)(unsigned) slot_ix);
      ix = (unsigned) slot_ix;
      existed_p = true;

      /* If the caller is looking for a specific tag, make sure
         it matches the tag we pulled from the cache.  */
      if (tag != PPH_null)
        {
          pph_cache_entry *e = pph_cache_get_entry (cache, ix);
          gcc_assert (tag == e->tag);
        }
    }

  if (ix_p)
    *ix_p = ix;

  return existed_p;
}


/* Return true if DATA is in the pickle cache of one of STREAM's
   included images.  TAG is the expected data type TAG for data.

   If DATA is found:
      - the index for INCLUDE_P into IMAGE->INCLUDES is returned in
	*INCLUDE_IX_P (if INCLUDE_IX_P is not NULL),
      - the cache slot index for DATA into *INCLUDE_P's pickle cache
	is returned in *IX_P (if IX_P is not NULL), and,
      - the function returns true.

   If DATA is not found:
      - *INCLUDE_IX_P is set to -1 (if INCLUDE_IX_P is not NULL),
      - *IX_P is set to -1 (if IX_P is not NULL), and,
      - the function returns false.  */

bool
pph_cache_lookup_in_includes (pph_stream *stream, void *data,
			      unsigned *include_ix_p, unsigned *ix_p,
			      enum pph_tag tag)
{
  unsigned include_ix, ix;
  pph_stream *include;
  bool found_it;

  /* When searching the external caches, do not try to find a match
     for TAG.  Since this is an external cache, the parser may have
     re-allocated the object pointed by DATA (e.g., when merging
     decls).  In this case, TAG will be different from the tag we find
     in the cache, so instead of ICEing, we ignore the match so the
     caller is forced to pickle DATA.  */
  found_it = false;
  FOR_EACH_VEC_ELT (pph_stream_ptr, stream->includes, include_ix, include)
    if (pph_cache_lookup (&include->cache, data, &ix, PPH_null))
      {
        pph_cache_entry *e = pph_cache_get_entry (&include->cache, ix);

        /* Only consider DATA found if its data type matches TAG.  If
           not, it means that the object pointed by DATA has changed,
           so DATA will need to be re-pickled.  */
        if (e->tag == tag)
          found_it = true;
        break;
      }

  if (!found_it)
    {
      include_ix = ix = (unsigned) -1;
      ix = (unsigned) -1;
    }

  if (include_ix_p)
    *include_ix_p = include_ix;

  if (ix_p)
    *ix_p = ix;

  return found_it;
}


/* Add pointer DATA with data type TAG to CACHE.  If IX_P is not NULL,
   on exit *IX_P will contain the slot number where DATA is stored.
   Return true if DATA already existed in the CACHE, false otherwise.  */

bool
pph_cache_add (pph_cache *cache, void *data, unsigned *ix_p, enum pph_tag tag)
{
  unsigned ix;
  bool existed_p;

  if (pph_cache_lookup (cache, data, &ix, tag))
    existed_p = true;
  else
    {
      existed_p = false;
      ix = VEC_length (pph_cache_entry, cache->v);
      pph_cache_insert_at (cache, data, ix, tag);
    }

  if (ix_p)
    *ix_p = ix;

  return existed_p;
}


/* Associate signature CRC with the first NBYTES of the area memory
   pointed to by slot IX of CACHE.  */

void
pph_cache_sign (pph_cache *cache, unsigned ix, unsigned crc, size_t nbytes)
{
  pph_cache_entry *e;

  /* Needed because xcrc32 requires an int to specify the length but
     tree_size returns size_t values.  */
  gcc_assert (nbytes == (size_t) (int) nbytes);

  e = pph_cache_get_entry (cache, ix);
  e->crc = crc;
  e->crc_nbytes = nbytes;
}


/* Return a signature for tree T.  Store the length of the signed area
   in *NBYTES_P.  */

unsigned
pph_get_signature (tree t, size_t *nbytes_p)
{
  tree prev_chain = NULL;
  rtx prev_rtl = NULL;
  int prev_used;
  size_t nbytes;
  unsigned crc;

  nbytes = tree_size (t);
  if (nbytes_p)
    *nbytes_p = nbytes;

  /* Preserve the value of the fields not included in the signature.  */
  prev_chain = (DECL_P (t)) ? DECL_CHAIN (t) : NULL;
  prev_rtl = (HAS_RTL_P (t)) ? DECL_RTL_IF_SET (t) : NULL;
  prev_used = TREE_USED (t);

  /* Clear the fields not included in the signature.  */
  if (DECL_P (t))
    DECL_CHAIN (t) = NULL;
  if (HAS_RTL_P (t))
    SET_DECL_RTL (t, NULL);
  TREE_USED (t) = 0;

  crc = xcrc32 ((const unsigned char *) t, nbytes, -1);

  /* Restore fields we did not include in the signature.  */
  if (DECL_P (t))
    DECL_CHAIN (t) = prev_chain;
  if (HAS_RTL_P (t))
    SET_DECL_RTL (t, prev_rtl);
  TREE_USED (t) = prev_used;

  return crc;
}


/* Return the merge name string identifier tree for a decl EXPR.  */

tree
pph_merge_name (tree expr)
{
  if (TREE_CODE (expr) == FUNCTION_DECL && !DECL_BUILT_IN (expr))
    return DECL_ASSEMBLER_NAME (expr);
  else
    return DECL_NAME (expr);
}
