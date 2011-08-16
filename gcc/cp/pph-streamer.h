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
#include "data-streamer.h"
#include "tree-streamer.h"
#include "tree.h"

/* Record markers.  */
enum pph_record_marker {
  /* This record contains the physical representation of the memory data.  */
  PPH_RECORD_START = 0x23,

  /* End of record marker.  If a record starts with PPH_RECORD_END, the
     reader should return a NULL pointer.  */
  PPH_RECORD_END,

  /* Internal reference.  This marker indicates that this data has
     been written before and it resides in the pickle cache for the
     current image.  Following this marker, the reader will find the
     cache slot where the data has been stored.  */
  PPH_RECORD_IREF,

  /* External reference.  This marker indicates that this data has
     been written before and it resides in the pickle cache for
     another image.  Following this marker, the reader will find two
     indices: (1) the index into the include table where the other
     image lives, and (2) the cache slot into that image's pickle
     cache where the data resides.  */
  PPH_RECORD_XREF
};

/* Symbol table markers.  These are all the symbols that need to be 
   registered with the middle end.  */
enum pph_symtab_marker {
  /* A FUNCTION_DECL with DECL_STRUCT_FUNCTION but no body.  */
  PPH_SYMTAB_FUNCTION = 0x01,

  /* A FUNCTION_DECL with DECL_STRUCT_FUNCTION and a body.  */
  PPH_SYMTAB_FUNCTION_BODY,

  /* All other symbols.  */
  PPH_SYMTAB_DECL
};

/* Number of sections in a PPH file.  FIXME, currently only one section
   is supported.  To add more, it will also be necessary to handle
   section names in pph_get_section_data and pph_free_section_data.  */
#define PPH_NUM_SECTIONS	1

/* String to identify PPH files.  Keep it to 7 characters, so it takes
   exactly 8 bytes in the file.  */
static const char pph_id_str[] = "PPH0x42";

/* When streaming out the line_table we will ignore the first 3 entries.
   The first one is the entrance in the header, the second one is for
   builtins, the third one is the command line, the fourth one is finally
   the LC_RENAME back to the header file, we want to stream out starting at
   that one, changing it's reason to LC_ENTER (as we ignored the original
   entrance), and then streaming every other entry as is from that point on.  */
#define PPH_NUM_IGNORED_LINE_TABLE_ENTRIES 3

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

/* Symbol table for a PPH stream.  */
typedef struct pph_symtab
{
  /* Table of all the declarations to register in declaration order.  */
  VEC(tree,heap) *v;

  /* Set of declarations to register used to avoid adding duplicate
     entries to the table.  */
  struct pointer_set_t *m;
} pph_symtab;

/* Vector of pph_stream pointers.  */
struct pph_stream;
typedef struct pph_stream *pph_stream_ptr;
DEF_VEC_P(pph_stream_ptr);
DEF_VEC_ALLOC_P(pph_stream_ptr,heap);

/* Data structures used to encode and decode trees.  */

/* A PPH stream contains all the data and attributes needed to
   write symbols, declarations and other parsing products to disk.  */
typedef struct pph_stream {
  /* Path name of the PPH file.  */
  const char *name;

  /* FILE object associated with it.  */
  FILE *file;

  /* Data structures used to encode/decode trees.  */
  union {
    /* Encoding tables and buffers used to write trees to a file.  */
    struct {
	struct output_block *ob;
	struct lto_out_decl_state *out_state;
	struct lto_output_stream *decl_state_stream;
    } w;

    /* Decoding tables and buffers used to read trees from a file.  */
    struct {
	struct lto_input_block *ib;
	struct data_in *data_in;
	struct lto_file_decl_data **pph_sections;
	char *file_data;
	size_t file_size;
    } r;
  } encoder;

  /* Cache of pickled data structures.  */
  pph_pickle_cache cache;

  /* Nonzero if the stream was opened for writing.  */
  unsigned int write_p : 1;

  /* Symbol table.  This is collected as the compiler instantiates
    symbols and functions.  Once we finish parsing the header file,
    this array is written out to the PPH image.  This way, the reader
    will be able to instantiate these symbols in the same order that
    they were instantiated originally.  */
  pph_symtab symtab;

  /* List of PPH files included by the PPH file that we are currently
     generating.  Note that this list only contains PPH files, not
     regular text headers.  Those are embedded in this stream.  */
  VEC(pph_stream_ptr,heap) *includes;
} pph_stream;

/* Filter values to avoid emitting certain objects to a PPH file.  */
#define PPHF_NONE		0
#define PPHF_NO_BUILTINS	(1 << 0)
#define PPHF_NO_XREFS		(1 << 1)

/* In pph-streamer.c.  */
pph_stream *pph_stream_open (const char *, const char *);
void pph_stream_close (pph_stream *);
void pph_trace_tree (pph_stream *, tree);
void pph_trace_uint (pph_stream *, unsigned int);
void pph_trace_bytes (pph_stream *, const void *, size_t);
void pph_trace_string (pph_stream *, const char *);
void pph_trace_string_with_length (pph_stream *, const char *, unsigned);
void pph_trace_location (pph_stream *, location_t);
void pph_trace_chain (pph_stream *, tree);
void pph_trace_bitpack (pph_stream *, struct bitpack_d *);
void pph_cache_insert_at (pph_stream *, void *, unsigned);
bool pph_cache_lookup (pph_stream *, void *, unsigned *);
bool pph_cache_lookup_in_includes (pph_stream *, void *, unsigned *,
				   unsigned *);
bool pph_cache_add (pph_stream *, void *, unsigned *);
void *pph_cache_get (pph_stream *, unsigned, unsigned);

/* In pph-streamer-out.c.  */
void pph_flush_buffers (pph_stream *);
void pph_init_write (pph_stream *);
void pph_write_tree (struct output_block *, tree, bool);
void pph_add_decl_to_symtab (tree);
void pph_add_include (pph_stream *, pph_stream *);
void pph_writer_init (void);
void pph_writer_finish (void);
void pph_write_location (struct output_block *, location_t);

/* In name-lookup.c.  */
struct binding_table_s;
void pph_out_binding_table (pph_stream *, struct binding_table_s *);
struct binding_table_s *pph_in_binding_table (pph_stream *);

/* In pph-streamer-in.c.  */
void pph_init_read (pph_stream *);
tree pph_read_tree (struct lto_input_block *, struct data_in *);
location_t pph_read_location (struct lto_input_block *, struct data_in *);
pph_stream *pph_read_file (const char *);
void pph_reader_finish (void);

/* In pt.c.  */
extern void pph_out_pending_templates_list (pph_stream *stream);
extern void pph_in_pending_templates_list (pph_stream *stream);
extern void pph_out_spec_entry_tables (pph_stream *stream);
extern void pph_in_spec_entry_tables (pph_stream *stream);


/* Inline functions.  */

/* Output AST T to STREAM.  This function is the primary interface.  */
static inline void
pph_out_tree (pph_stream *stream, tree t)
{
  if (flag_pph_tracer >= 1)
    pph_trace_tree (stream, t);
  pph_write_tree (stream->encoder.w.ob, t, false);
}

/* Output array A of cardinality C of ASTs to STREAM.  */
/* FIXME pph: hold for alternate routine. */
#if 0
static inline void
pph_out_tree_array (pph_stream *stream, tree *a, size_t c)
{
  size_t i;
  for (i = 0; i < c; ++i)
    {
      if (flag_pph_tracer >= 1)
        pph_trace_tree (stream, a[i]);
      pph_write_tree (stream->encoder.w.ob, a[i]);
    }
}
#endif

/* Output AST T to STREAM.  If -fpph-tracer is set to TLEVEL or
   higher, T is sent to pph_trace_tree.  */
static inline void
pph_out_tree_or_ref_1 (pph_stream *stream, tree t, int tlevel)
{
  if (flag_pph_tracer >= tlevel)
    pph_trace_tree (stream, t);
  pph_write_tree (stream->encoder.w.ob, t, false);
}

/* Output AST T to STREAM.  Trigger tracing at -fpph-tracer=2.  */
static inline void
pph_out_tree_or_ref (pph_stream *stream, tree t)
{
  pph_out_tree_or_ref_1 (stream, t, 2);
}

/* Write an unsigned int VALUE to STREAM.  */
static inline void
pph_out_uint (pph_stream *stream, unsigned int value)
{
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, value);
  streamer_write_hwi (stream->encoder.w.ob, value);
}

/* Write an unsigned char VALUE to STREAM.  */
static inline void
pph_out_uchar (pph_stream *stream, unsigned char value)
{
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, value);
  streamer_write_char_stream (stream->encoder.w.ob->main_stream, value);
}

/* Write N bytes from P to STREAM.  */
static inline void
pph_out_bytes (pph_stream *stream, const void *p, size_t n)
{
  if (flag_pph_tracer >= 4)
    pph_trace_bytes (stream, p, n);
  lto_output_data_stream (stream->encoder.w.ob->main_stream, p, n);
}

/* Write string STR to STREAM.  */
static inline void
pph_out_string (pph_stream *stream, const char *str)
{
  if (flag_pph_tracer >= 4)
    pph_trace_string (stream, str);
  streamer_write_string (stream->encoder.w.ob,
			 stream->encoder.w.ob->main_stream, str, false);
}

/* Write string STR of length LEN to STREAM.  */
static inline void
pph_out_string_with_length (pph_stream *stream, const char *str,
			    unsigned int len)
{
  if (flag_pph_tracer >= 4)
    pph_trace_string_with_length (stream, str, len);
  streamer_write_string_with_length (stream->encoder.w.ob,
				     stream->encoder.w.ob->main_stream,
				     str, len + 1, false);
}

/* Output VEC V of ASTs to STREAM.  */
/* FIXME pph: hold for alternate routine. */
#if 0
static inline void
pph_out_tree_VEC (pph_stream *stream, VEC(tree,gc) *v)
{
  tree t;
  size_t i;
  size_t c = VEC_length (tree, v);
  pph_out_uint (stream, c);
  FOR_EACH_VEC_ELT (tree, v, i, t)
    {
      if (flag_pph_tracer >= 1)
        pph_trace_tree (stream, t);
      pph_write_tree (stream->encoder.w.ob, t);
    }
}
#endif

/* Write location LOC of length to STREAM.  */
static inline void
pph_out_location (pph_stream *stream, location_t loc)
{
  if (flag_pph_tracer >= 4)
    pph_trace_location (stream, loc);
  pph_write_location (stream->encoder.w.ob, loc);
}

/* Write a chain of ASTs to STREAM starting with FIRST.  */
static inline void
pph_out_chain (pph_stream *stream, tree first)
{
  if (flag_pph_tracer >= 2)
    pph_trace_chain (stream, first);
  streamer_write_chain (stream->encoder.w.ob, first, false);
}

/* Write a bitpack BP to STREAM.  */
static inline void
pph_out_bitpack (pph_stream *stream, struct bitpack_d *bp)
{
  gcc_assert (stream->encoder.w.ob->main_stream == bp->stream);
  if (flag_pph_tracer >= 4)
    pph_trace_bitpack (stream, bp);
  streamer_write_bitpack (bp);
}

/* Read an unsigned HOST_WIDE_INT integer from STREAM.  */
static inline unsigned int
pph_in_uint (pph_stream *stream)
{
  HOST_WIDE_INT unsigned n = streamer_read_uhwi (stream->encoder.r.ib);
  gcc_assert (n == (unsigned) n);
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, n);
  return (unsigned) n;
}

/* Read an unsigned char VALUE to STREAM.  */
static inline unsigned char
pph_in_uchar (pph_stream *stream)
{
  unsigned char n = streamer_read_uchar (stream->encoder.r.ib);
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, n);
  return n;
}

/* Read N bytes from STREAM into P.  The caller is responsible for 
   allocating a sufficiently large buffer.  */
static inline void
pph_in_bytes (pph_stream *stream, void *p, size_t n)
{
  lto_input_data_block (stream->encoder.r.ib, p, n);
  if (flag_pph_tracer >= 4)
    pph_trace_bytes (stream, p, n);
}

/* Read and return a string from STREAM.  */

static inline const char *
pph_in_string (pph_stream *stream)
{
  const char *s = streamer_read_string (stream->encoder.r.data_in,
				        stream->encoder.r.ib);
  if (flag_pph_tracer >= 4)
    pph_trace_string (stream, s);
  return s;
}

/* Read and return a location_t from STREAM.
   FIXME pph: If pph_trace didn't depend on STREAM, we could avoid having to
   call this function, only for it to call lto_input_location, which calls the
   streamer hook back to pph_read_location.  */

static inline location_t
pph_in_location (pph_stream *stream)
{
  location_t loc = pph_read_location (stream->encoder.r.ib,
				       stream->encoder.r.data_in);
  if (flag_pph_tracer >= 4)
    pph_trace_location (stream, loc);
  return loc;
}

/* Load an AST from STREAM.  Return the corresponding tree.  */
static inline tree
pph_in_tree (pph_stream *stream)
{
  tree t = pph_read_tree (stream->encoder.r.ib, stream->encoder.r.data_in);
  if (flag_pph_tracer >= 4)
    pph_trace_tree (stream, t);
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
      tree t = pph_read_tree (stream->encoder.r.ib, stream->encoder.r.data_in);
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
      tree t = pph_read_tree (stream->encoder.r.ib, stream->encoder.r.data_in);
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
  tree t = streamer_read_chain (stream->encoder.r.ib,
                                stream->encoder.r.data_in);
  if (flag_pph_tracer >= 2)
    pph_trace_chain (stream, t);
  return t;
}

/* Read a bitpack from STREAM.  */
static inline struct bitpack_d
pph_in_bitpack (pph_stream *stream)
{
  struct bitpack_d bp = streamer_read_bitpack (stream->encoder.r.ib);
  if (flag_pph_tracer >= 4)
    pph_trace_bitpack (stream, &bp);
  return bp;
}

/* Write record MARKER to STREAM.  */
static inline void
pph_out_record_marker (pph_stream *stream, enum pph_record_marker marker)
{
  gcc_assert (marker == (enum pph_record_marker)(unsigned char) marker);
  pph_out_uchar (stream, marker);
}

/* Read and return a record marker from STREAM.  */
static inline enum pph_record_marker
pph_in_record_marker (pph_stream *stream)
{
  enum pph_record_marker m = (enum pph_record_marker) pph_in_uchar (stream);
  gcc_assert (m == PPH_RECORD_START
	      || m == PPH_RECORD_END
	      || m == PPH_RECORD_IREF
	      || m == PPH_RECORD_XREF);
  return m;
}

#endif  /* GCC_CP_PPH_STREAMER_H  */
