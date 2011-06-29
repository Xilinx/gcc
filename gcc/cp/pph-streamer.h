/* Support routines and data structures for streaming PPH data.
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

#ifndef GCC_CP_PPH_STREAMER_H
#define GCC_CP_PPH_STREAMER_H

#include "lto-streamer.h"
#include "tree.h"

/* Record markers.  */
enum pph_record_marker {
  PPH_RECORD_START = 0xfd,
  PPH_RECORD_END,
  PPH_RECORD_SHARED
};

/* Number of sections in a PPH file.  FIXME, currently only one section
   is supported.  To add more, it will also be necessary to handle
   section names in pph_get_section_data and pph_free_section_data.  */
#define PPH_NUM_SECTIONS	1

/* String to identify PPH files.  Keep it to 7 characters, so it takes
   exactly 8 bytes in the file.  */
static const char pph_id_str[] = "PPH0x42";

/* Structure of the header of a PPH file.  */
typedef struct pph_file_header {
  /* Identification string.  */
  char id_str[sizeof(pph_id_str)];

  /* Version information.  */
  char major_version;
  char minor_version;
  char patchlevel;

  /* Size of the string table in bytes.  */
  unsigned int strtab_size;
} pph_file_header;


typedef void *void_p;
DEF_VEC_P(void_p);
DEF_VEC_ALLOC_P(void_p,heap);

/* A cache for storing pickled data structures.  This is used to implement
   pointer sharing.

   When a data structure is initially pickled for writing, a pointer
   to it is stored in this cache.  If the same data structure is
   streamed again, instead of pickling it, the compiler will write
   the index into the cache.

   The same mechanism is used when reading. When the data structure is
   first materialized, its address is saved into the same cache slot
   used when writing.  Subsequent reads will simply get the
   materialized pointer from that slot.  */
typedef struct pph_pickle_cache {
  /* Array of entries.  */
  VEC(void_p,heap) *v;

  /* Map between slots in the array and pointers.  */
  struct pointer_map_t *m;
} pph_pickle_cache;


/* A PPH stream contains all the data and attributes needed to
   write symbols, declarations and other parsing products to disk.  */
typedef struct pph_stream {
  /* Path name of the PPH file.  */
  const char *name;

  /* FILE object associated with it.  */
  FILE *file;

  /* LTO output block to hold pickled ASTs and references.  This is
      NULL when the file is opened for reading.  */
  struct output_block *ob;
  struct lto_out_decl_state *out_state;
  struct lto_output_stream *decl_state_stream;

  /* LTO input block to read ASTs and references from.  This is NULL
      when the file is opened for writing.  */
  struct lto_input_block *ib;

  /* String tables and other descriptors used by the LTO reading
     routines.  NULL when the file is opened for reading.  */
  struct data_in *data_in;

  /* Array of sections in the PPH file.  */
  struct lto_file_decl_data **pph_sections;

  /* Buffer holding the file contents.  FIXME pph, we are bringing
     the whole file in memory at once.  This seems wasteful.  */
  char *file_data;
  size_t file_size;

  /* Cache of pickled data structures.  */
  pph_pickle_cache cache;

  /* Nonzero if the stream was opened for writing.  */
  unsigned int write_p : 1;

  /* List of functions with bodies that need to be expanded after
     reading the PPH file.  */
  VEC(tree,gc) *fns_to_expand;
} pph_stream;

/* Filter values for pph_out_chain_filtered.  */
enum chain_filter { NONE, NO_BUILTINS };

/* In pph-streamer.c.  */
pph_stream *pph_stream_open (const char *, const char *);
void pph_stream_close (pph_stream *);
void pph_trace_tree (pph_stream *, tree, bool ref_p);
void pph_trace_uint (pph_stream *, unsigned int);
void pph_trace_bytes (pph_stream *, const void *, size_t);
void pph_trace_string (pph_stream *, const char *);
void pph_trace_string_with_length (pph_stream *, const char *, unsigned);
void pph_trace_chain (pph_stream *, tree);
void pph_trace_bitpack (pph_stream *, struct bitpack_d *);
void pph_cache_insert_at (pph_stream *, void *, unsigned);
bool pph_cache_add (pph_stream *, void *, unsigned *);
void *pph_cache_get (pph_stream *, unsigned);

/* In pph-streamer-out.c.  */
void pph_flush_buffers (pph_stream *);
void pph_init_write (pph_stream *);
void pph_write_tree (struct output_block *, tree, bool ref_p);
void pph_pack_value_fields (struct bitpack_d *, tree);
void pph_out_tree_header (struct output_block *, tree);
void pph_write_file (void);

/* In name-lookup.c.  */
struct binding_table_s;
void pph_out_binding_table (pph_stream *, struct binding_table_s *,
				     bool);
struct binding_table_s *pph_in_binding_table (pph_stream *);

/* In pph-streamer-in.c.  */
void pph_init_read (pph_stream *);
void pph_read_tree (struct lto_input_block *, struct data_in *, tree);
void pph_unpack_value_fields (struct bitpack_d *, tree);
tree pph_alloc_tree (enum tree_code, struct lto_input_block *,
			    struct data_in *);
void pph_read_file (const char *filename);

/* Inline functions.  */

/* Output AST T to STREAM.  If REF_P is true, output all the leaves of T
   as references.  This function is the primary interface.  */
static inline void
pph_out_tree (pph_stream *stream, tree t, bool ref_p)
{
  if (flag_pph_tracer >= 1)
    pph_trace_tree (stream, t, ref_p);
  lto_output_tree (stream->ob, t, ref_p);
}

/* Output array A of cardinality C of ASTs to STREAM.
   If REF_P is true, output all the leaves of T as references.  */
/* FIXME pph: hold for alternate routine. */
#if 0
static inline void
pph_out_tree_array (pph_stream *stream, tree *a, size_t c, bool ref_p)
{
  size_t i;
  for (i = 0; i < c; ++i)
    {
      if (flag_pph_tracer >= 1)
        pph_trace_tree (stream, a[i], ref_p);
      lto_output_tree (stream->ob, a[i], ref_p);
    }
}
#endif

/* Output AST T to STREAM.  If REF_P is true, output a reference to T.
   If -fpph-tracer is set to TLEVEL or higher, T is sent to
   pph_trace_tree.  */
static inline void
pph_out_tree_or_ref_1 (pph_stream *stream, tree t, bool ref_p, int tlevel)
{
  if (flag_pph_tracer >= tlevel)
    pph_trace_tree (stream, t, ref_p);
  lto_output_tree_or_ref (stream->ob, t, ref_p);
}

/* Output AST T to STREAM.  If REF_P is true, output a reference to T.
   Trigger tracing at -fpph-tracer=2.  */
static inline void
pph_out_tree_or_ref (pph_stream *stream, tree t, bool ref_p)
{
  pph_out_tree_or_ref_1 (stream, t, ref_p, 2);
}

/* Write an unsigned int VALUE to STREAM.  */
static inline void
pph_out_uint (pph_stream *stream, unsigned int value)
{
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, value);
  lto_output_sleb128_stream (stream->ob->main_stream, value);
}

/* Write an unsigned char VALUE to STREAM.  */
static inline void
pph_out_uchar (pph_stream *stream, unsigned char value)
{
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, value);
  lto_output_1_stream (stream->ob->main_stream, value);
}

/* Write N bytes from P to STREAM.  */
static inline void
pph_out_bytes (pph_stream *stream, const void *p, size_t n)
{
  if (flag_pph_tracer >= 4)
    pph_trace_bytes (stream, p, n);
  lto_output_data_stream (stream->ob->main_stream, p, n);
}

/* Write string STR to STREAM.  */
static inline void
pph_out_string (pph_stream *stream, const char *str)
{
  if (flag_pph_tracer >= 4)
    pph_trace_string (stream, str);
  lto_output_string (stream->ob, stream->ob->main_stream, str, false);
}

/* Write string STR of length LEN to STREAM.  */
static inline void
pph_out_string_with_length (pph_stream *stream, const char *str,
			       unsigned int len)
{
  if (str)
    {
      if (flag_pph_tracer >= 4)
	pph_trace_string_with_length (stream, str, len);
      lto_output_string_with_length (stream->ob, stream->ob->main_stream,
				     str, len + 1, false);
    }
  else
    {
      /* lto_output_string_with_length does not handle NULL strings,
	 but lto_output_string does.  */
      if (flag_pph_tracer >= 4)
	pph_trace_string (stream, str);
      pph_out_string (stream, NULL);
    }
}

/* Output VEC V of ASTs to STREAM.
   If REF_P is true, output all the leaves of T as references.  */
/* FIXME pph: hold for alternate routine. */
#if 0
static inline void
pph_out_tree_VEC (pph_stream *stream, VEC(tree,gc) *v, bool ref_p)
{
  tree t;
  size_t i;
  size_t c = VEC_length (tree, v);
  pph_out_uint (stream, c);
  for (i = 0; VEC_iterate (tree, v, i, t); i++)
    {
      if (flag_pph_tracer >= 1)
        pph_trace_tree (stream, t, ref_p);
      lto_output_tree (stream->ob, t, ref_p);
    }
}
#endif

/* Write a chain of ASTs to STREAM starting with FIRST.  REF_P is true
   if the nodes should be emitted as references.  */
static inline void
pph_out_chain (pph_stream *stream, tree first, bool ref_p)
{
  if (flag_pph_tracer >= 2)
    pph_trace_chain (stream, first);
  lto_output_chain (stream->ob, first, ref_p);
}

/* Write a bitpack BP to STREAM.  */
static inline void
pph_out_bitpack (pph_stream *stream, struct bitpack_d *bp)
{
  gcc_assert (stream->ob->main_stream == bp->stream);
  if (flag_pph_tracer >= 4)
    pph_trace_bitpack (stream, bp);
  lto_output_bitpack (bp);
}

/* Read an unsigned HOST_WIDE_INT integer from STREAM.  */
static inline unsigned int
pph_in_uint (pph_stream *stream)
{
  HOST_WIDE_INT unsigned n = lto_input_uleb128 (stream->ib);
  gcc_assert (n == (unsigned) n);
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, n);
  return (unsigned) n;
}

/* Read an unsigned char VALUE to STREAM.  */
static inline unsigned char
pph_in_uchar (pph_stream *stream)
{
  unsigned char n = lto_input_1_unsigned (stream->ib);
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, n);
  return n;
}

/* Read N bytes from STREAM into P.  The caller is responsible for 
   allocating a sufficiently large buffer.  */
static inline void
pph_in_bytes (pph_stream *stream, void *p, size_t n)
{
  lto_input_data_block (stream->ib, p, n);
  if (flag_pph_tracer >= 4)
    pph_trace_bytes (stream, p, n);
}

/* Read and return a string of up to MAX characters from STREAM.
   The caller is responsible for freeing the memory allocated
   for the string.  */

static inline const char *
pph_in_string (pph_stream *stream)
{
  const char *s = lto_input_string (stream->data_in, stream->ib);
  if (flag_pph_tracer >= 4)
    pph_trace_string (stream, s);
  return s;
}

/* Load an AST from STREAM.  Return the corresponding tree.  */
static inline tree
pph_in_tree (pph_stream *stream)
{
  tree t = lto_input_tree (stream->ib, stream->data_in);
  if (flag_pph_tracer >= 4)
    pph_trace_tree (stream, t, false); /* FIXME pph: always false? */
  return t;
}

/* Load into an array A of cardinality C of AST from STREAM.  */
/* FIXME pph: Hold for later use. */
#if 0
static inline void
pph_in_tree_array (pph_stream *stream, tree *a, size_t c)
{
  size_t i;
  for (i = 0; i < c; ++i)
    {
      tree t = lto_input_tree (stream->ib, stream->data_in);
      if (flag_pph_tracer >= 4)
        pph_trace_tree (stream, t, false); /* FIXME pph: always false? */
      a[i] = t;
    }
}
#endif

/* Load into a VEC V of AST from STREAM.  */
/* FIXME pph: Hold for later use. */
#if 0
static inline void
pph_in_tree_VEC (pph_stream *stream, VEC(tree,gc) *v)
{
  size_t i;
  unsigned int c = pph_in_uint (stream);
  for (i = 0; i < c; ++i)
    {
      tree t = lto_input_tree (stream->ib, stream->data_in);
      if (flag_pph_tracer >= 4)
        pph_trace_tree (stream, t, false); /* FIXME pph: always false? */
      VEC_safe_push (tree, gc, v, t);
    }
}
#endif

/* Read a chain of ASTs from STREAM.  */
static inline tree
pph_in_chain (pph_stream *stream)
{
  tree t = lto_input_chain (stream->ib, stream->data_in);
  if (flag_pph_tracer >= 2)
    pph_trace_chain (stream, t);
  return t;
}

/* Read a bitpack from STREAM.  */
static inline struct bitpack_d
pph_in_bitpack (pph_stream *stream)
{
  struct bitpack_d bp = lto_input_bitpack (stream->ib);
  if (flag_pph_tracer >= 4)
    pph_trace_bitpack (stream, &bp);
  return bp;
}

#endif  /* GCC_CP_PPH_STREAMER_H  */
