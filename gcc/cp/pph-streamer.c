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

/* List of PPH images read during parsing.  Images opened during #include
   processing and opened from pph_in_includes cannot be closed
   immediately after reading, because the pickle cache contained in
   them may be referenced from other images.  We delay closing all of
   them until the end of parsing (when pph_reader_finish is called).  */
VEC(pph_stream_ptr, heap) *pph_read_images;

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


/* Initialize all the streamer hooks used for streaming ASTs.  */

static void
pph_hooks_init (void)
{
  streamer_hooks_init ();
  streamer_hooks.write_tree = pph_write_tree;
  streamer_hooks.read_tree = pph_read_tree;
  streamer_hooks.input_location = pph_read_location;
  streamer_hooks.output_location = pph_write_location;
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

void
pph_init_preloaded_cache (void)
{
  pph_preloaded_cache = XCNEW (pph_cache);
  pph_cache_init (pph_preloaded_cache);
  pph_cache_preload (pph_preloaded_cache);
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
  if (!f)
    return NULL;

  pph_hooks_init ();
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

  if (flag_pph_debug >= 1)
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

  if (stream->write_p)
    {
      destroy_output_block (stream->encoder.w.ob);
      free (stream->encoder.w.decl_state_stream);
      lto_delete_out_decl_state (stream->encoder.w.out_state);
      VEC_free (pph_stream_ptr, heap, stream->encoder.w.includes);
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


/* Data types supported by the PPH tracer.  */
enum pph_trace_type
{
    PPH_TRACE_TREE,
    PPH_TRACE_UINT,
    PPH_TRACE_BYTES,
    PPH_TRACE_STRING,
    PPH_TRACE_LOCATION,
    PPH_TRACE_CHAIN,
    PPH_TRACE_BITPACK
};


/* Print tracing information for STREAM on pph_logfile.  DATA is the
   memory area to display, SIZE is the number of bytes to print, TYPE
   is the kind of data to print.  */

static void
pph_trace (pph_stream *stream, const void *data, unsigned int nbytes,
		  enum pph_trace_type type)
{
  const char *op = (stream->write_p) ? "<<" : ">>";
  const char *type_s[] = { "tree", "ref", "uint", "bytes", "string", "chain",
                           "bitpack" };

  if ((type == PPH_TRACE_TREE || type == PPH_TRACE_CHAIN)
      && !data
      && flag_pph_tracer <= 3)
    return;

  fprintf (pph_logfile, "PPH: %s %s %s/%u",
	   stream->name, op, type_s[type], (unsigned) nbytes);

  switch (type)
    {
    case PPH_TRACE_TREE:
      {
	const_tree t = (const_tree) data;
	if (t)
          {
            enum tree_code code = TREE_CODE (t);
            fprintf (pph_logfile, ", code=%s", pph_tree_code_text (code));
            if (DECL_P (t))
              {
                fprintf (pph_logfile, ", value=");
                print_generic_decl (pph_logfile,
                                    CONST_CAST (union tree_node *, t), 0);
              }
          }
	else
	  fprintf (pph_logfile, ", NULL_TREE");
      }
      break;

    case PPH_TRACE_UINT:
      {
	unsigned int val = *((const unsigned int *) data);
	fprintf (pph_logfile, ", value=%u (0x%x)", val, val);
      }
      break;

    case PPH_TRACE_BYTES:
      {
	size_t i;
	const char *buffer = (const char *) data;
        fprintf (pph_logfile, ", value=");
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
	fprintf (pph_logfile, ", value=%.*s",
                              (int) nbytes, (const char *) data);
      else
	fprintf (pph_logfile, ", NULL_STRING");
      break;

    case PPH_TRACE_LOCATION:
      if (data)
	fprintf (pph_logfile, ", value=%.*s",
                              (int) nbytes, (const char *) data);
      else
	fprintf (pph_logfile, ", NULL_LOCATION");
      break;

    case PPH_TRACE_CHAIN:
      {
	const_tree t = (const_tree) data;
	fprintf (pph_logfile, ", value=" );
	print_generic_expr (pph_logfile, CONST_CAST (union tree_node *, t),
			    TDF_SLIM);
	fprintf (pph_logfile, " (%d nodes in chain)", list_length (t));
      }
      break;

    case PPH_TRACE_BITPACK:
      {
	const struct bitpack_d *bp = (const struct bitpack_d *) data;
	fprintf (pph_logfile, ", value=0x%lx", bp->word);
      }
    break;

    default:
      gcc_unreachable ();
    }

  fputc ('\n', pph_logfile);
}


/* Show tracing information for T on STREAM.  */

void
pph_trace_tree (pph_stream *stream, tree t)
{
  pph_trace (stream, t, t ? tree_code_size (TREE_CODE (t)) : 0, PPH_TRACE_TREE);
}


/* Show tracing information for VAL on STREAM.  */

void
pph_trace_uint (pph_stream *stream, unsigned int val)
{
  pph_trace (stream, &val, sizeof (val), PPH_TRACE_UINT);
}


/* Show tracing information for NBYTES bytes of memory area DATA on
   STREAM.  */

void
pph_trace_bytes (pph_stream *stream, const void *data, size_t nbytes)
{
  pph_trace (stream, data, nbytes, PPH_TRACE_BYTES);
}


/* Show tracing information for S on STREAM.  */

void
pph_trace_string (pph_stream *stream, const char *s)
{
  pph_trace (stream, s, s ? strlen (s) : 0, PPH_TRACE_STRING);
}


/* Show tracing information for LEN bytes of S on STREAM.  */

void
pph_trace_string_with_length (pph_stream *stream, const char *s,
				     unsigned int len)
{
  pph_trace (stream, s, len, PPH_TRACE_STRING);
}


/* Show tracing information for location_t LOC on STREAM.  */

void
pph_trace_location (pph_stream *stream, location_t loc)
{
  expanded_location xloc = expand_location (loc);
  size_t flen = strlen (xloc.file);
  size_t mlen = flen + 12; /* for : and 10 digits and \n */
  size_t llen;
  char *str = (char *)xmalloc (mlen);

  strcpy (str, xloc.file);
  str[flen] = ':';
  sprintf (str + flen + 1, "%d", xloc.line);
  llen = strlen (str);
  pph_trace (stream, str, llen, PPH_TRACE_LOCATION);
}


/* Show tracing information for a tree chain starting with T on STREAM.  */

void
pph_trace_chain (pph_stream *stream, tree t)
{
  pph_trace (stream, t, t ? tree_code_size (TREE_CODE (t)) : 0,
		    PPH_TRACE_CHAIN);
}


/* Show tracing information for a bitpack BP on STREAM.  */

void
pph_trace_bitpack (pph_stream *stream, struct bitpack_d *bp)
{
  pph_trace (stream, bp, sizeof (*bp), PPH_TRACE_BITPACK);
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


/* Return true if DATA is in the pickle cache of one of the included
   images.  TAG is the expected data type TAG for data.

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
pph_cache_lookup_in_includes (void *data, unsigned *include_ix_p,
                              unsigned *ix_p, enum pph_tag tag)
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
  FOR_EACH_VEC_ELT (pph_stream_ptr, pph_read_images, include_ix, include)
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
