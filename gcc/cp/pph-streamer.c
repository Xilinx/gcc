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

/* Pre-load common tree nodes into the pickle cache in STREAM.  These
   nodes are always built by the front end, so there is no need to
   pickle them.

   FIXME pph - Every stream will have its own pickle cache.  Many
   entries in all those caches will be the same.  Implement a common
   cache for everything we preload here so that we do not have to
   preload every cache we instantiate.  */

static void
pph_cache_preload (pph_stream *stream)
{
  unsigned i;

  for (i = itk_char; i < itk_none; i++)
    pph_cache_add (stream, integer_types[i], NULL);

  for (i = 0; i < TYPE_KIND_LAST; i++)
    pph_cache_add (stream, sizetype_tab[i], NULL);

  /* global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < TI_MAX; i++)
    if (global_trees[i])
      pph_cache_add (stream, global_trees[i], NULL);

  /* c_global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < CTI_MAX; i++)
    if (c_global_trees[i])
      pph_cache_add (stream, c_global_trees[i], NULL);

  /* cp_global_trees[] can have NULL entries in it.  Skip them.  */
  for (i = 0; i < CPTI_MAX; i++)
    {
      /* Also skip trees which are generated while parsing.  */
      if (i == CPTI_KEYED_CLASSES)
	continue;

      if (cp_global_trees[i])
	pph_cache_add (stream, cp_global_trees[i], NULL);
    }

  /* Add other well-known nodes that should always be taken from the
     current compilation context.  */
  pph_cache_add (stream, global_namespace, NULL);
  pph_cache_add (stream, DECL_CONTEXT (global_namespace), NULL);
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
  stream->cache.m = pointer_map_create ();
  stream->symtab.m = pointer_set_create ();
  if (stream->write_p)
    pph_init_write (stream);
  else
    pph_init_read (stream);

  pph_cache_preload (stream);

  return stream;
}



/* Close PPH stream STREAM.  */

void
pph_stream_close (pph_stream *stream)
{
  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Closing %s\n", stream->name);

  /* If we were writing to STREAM, flush all the memory buffers.  This
     does the actual writing of all the pickled data structures.  */
  if (stream->write_p)
    pph_flush_buffers (stream);

  fclose (stream->file);

  /* Deallocate all memory used.  */
  stream->file = NULL;
  VEC_free (void_p, heap, stream->cache.v);
  pointer_map_destroy (stream->cache.m);
  VEC_free (tree, heap, stream->symtab.v);
  pointer_set_destroy (stream->symtab.m);

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
            fprintf (pph_logfile, ", code=%s", tree_code_name[TREE_CODE (t)]);
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


/* Insert DATA in STREAM's pickle cache at slot IX.  If DATA already
   existed in the cache, IX must be the same as the previous entry.  */

void
pph_cache_insert_at (pph_stream *stream, void *data, unsigned ix)
{
  void **map_slot;

  map_slot = pointer_map_insert (stream->cache.m, data);
  if (*map_slot == NULL)
    {
      *map_slot = (void *) (unsigned HOST_WIDE_INT) ix;
      if (ix + 1 > VEC_length (void_p, stream->cache.v))
	VEC_safe_grow_cleared (void_p, heap, stream->cache.v, ix + 1);
      VEC_replace (void_p, stream->cache.v, ix, data);
    }
}


/* Return true if DATA exists in STREAM's pickle cache.  If IX_P is not
   NULL, store the cache slot where DATA resides in *IX_P (or (unsigned)-1
   if DATA is not found).  */

bool
pph_cache_lookup (pph_stream *stream, void *data, unsigned *ix_p)
{
  void **map_slot;
  unsigned ix;
  bool existed_p;

  map_slot = pointer_map_contains (stream->cache.m, data);
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
    }

  if (ix_p)
    *ix_p = ix;

  return existed_p;
}


/* Return true if DATA is in the pickle cache of one of STREAM's
   included images.

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
			      unsigned *include_ix_p, unsigned *ix_p)
{
  unsigned include_ix, ix;
  pph_stream *include;
  bool found_it;

  found_it = false;
  FOR_EACH_VEC_ELT (pph_stream_ptr, stream->includes, include_ix, include)
    if (pph_cache_lookup (include, data, &ix))
      {
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


/* Add pointer DATA to the pickle cache in STREAM.  If IX_P is not
   NULL, on exit *IX_P will contain the slot number where DATA is
   stored.  Return true if DATA already existed in the cache, false
   otherwise.  */

bool
pph_cache_add (pph_stream *stream, void *data, unsigned *ix_p)
{
  unsigned ix;
  bool existed_p;

  if (pph_cache_lookup (stream, data, &ix))
    existed_p = true;
  else
    {
      existed_p = false;
      ix = VEC_length (void_p, stream->cache.v);
      pph_cache_insert_at (stream, data, ix);
    }

  if (ix_p)
    *ix_p = ix;

  return existed_p;
}


/* Return the pointer at slot IX in STREAM's pickle cache.  If INCLUDE_IX
   is not -1U, then instead of looking up in STREAM's pickle cache,
   the pointer is looked up in the pickle cache for
   STREAM->INCLUDES[INCLUDE_IX].  */

void *
pph_cache_get (pph_stream *stream, unsigned include_ix, unsigned ix)
{
  void *data;
  pph_stream *image;

  /* Determine which image's pickle cache to use.  */
  if (include_ix == (unsigned) -1)
    image = stream;
  else
    image = VEC_index (pph_stream_ptr, stream->includes, include_ix);

  data = VEC_index (void_p, image->cache.v, ix);
  gcc_assert (data);
  return data;
}
