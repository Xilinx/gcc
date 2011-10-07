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

  /* Like PPH_RECORD_START, but the reconstructed data should not be
     added to the pickle cache (see pph_cache_should_handle).  */
  PPH_RECORD_START_NO_CACHE,

  /* Start a mutated reference.  This marker indicates that this data
     already existed in the cache for another PPH image, but it has
     mutated since it was inserted into the cache:

     - The writer will pickle the object again as if it had not
       been pickled before.

     - The reader uses this as an indication that it should not
       allocate a new object, it should simply unpickle the object on
       top of the already allocated object.  */
  PPH_RECORD_START_MUTATED,

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
  PPH_RECORD_XREF,

  /* Preloaded reference. This marker indicates that this data is a preloaded
     node created by the front-end at the beginning of compilation, which we
     do not need to stream out as it will already exist on the way in.  */
  PPH_RECORD_PREF
};

/* Record type tags.  Every record saved on a PPH image contains a data
   tag to identify the data structure saved in that record.  */
enum pph_tag {
  PPH_null = 0,

  /* The tags below are named after the data types they represent.

     Note that to simplify tag management, we reserve enough
     values to fit all the tree codes.  This guarantees that for
     every tree code C and PPH tag T, (unsigned) C == (unsigned) T.

     The value PPH_any_tree is used when reading tree records.  Since
     the reader does not have enough context, it will generally not
     know what kind of tree is about to read, all it knows is that it
     should be a valid tree code.  */
  PPH_any_tree = MAX_TREE_CODES,

  /* Maintain the tags below in alphabetical order.  */
  PPH_binding_entry,
  PPH_binding_table,
  PPH_cgraph_node,
  PPH_cp_binding_level,
  PPH_cp_class_binding,
  PPH_cp_label_binding,
  PPH_cxx_binding,
  PPH_function,
  PPH_lang_decl,
  PPH_lang_type,
  PPH_language_function,
  PPH_sorted_fields_type,

  /* This tag must always be last.  */
  PPH_NUM_TAGS
};


/* Line table markers. We only stream line table entries from the parent header
   file, other entries are referred to by the name of the file which is then
   loaded as an include at the correct point in time.  */
enum pph_linetable_marker {
  /* A regular line_map entry in the line_table.  */
  PPH_LINETABLE_ENTRY = 0x01,

  /* A reference to another header to be loaded at this point.  */
  PPH_LINETABLE_REFERENCE,

  /* Marks the end of the line_map entries.  */
  PPH_LINETABLE_END
};

/* Number of sections in a PPH file.  FIXME, currently only one section
   is supported.  To add more, it will also be necessary to handle
   section names in pph_get_section_data and pph_free_section_data.  */
#define PPH_NUM_SECTIONS	1

/* String to identify PPH files.  Keep it to 7 characters, so it takes
   exactly 8 bytes in the file.  */
static const char pph_id_str[] = "PPH0x42";

/* When streaming out the line_table we will ignore the first 2 entries.
   The first one is the entrance in the header, the second one is the command
   line, the third one is the LC_RENAME back to the header file: we want to
   stream out starting at that one, changing it's reason to LC_ENTER (as we
   ignored the original entrance), and then streaming every other entry as is
   from that point on.  */
#define PPH_NUM_IGNORED_LINE_TABLE_ENTRIES 2

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


/* An entry in the pickle cache.  Each entry contains a pointer to
   the cached data and checksum information.  This checksum is used to
   detect mutated states.  This is common, for instance, in decl trees
   that are first parsed as declarations and later on they are
   converted into their definition.

   When the cache notices a cache hit on a mutated data, it writes a
   PPH_RECORD_START_MUTATED to indicate to the reader that it is about
   to read an already instantiated tree.  */
typedef struct pph_cache_entry {
  /* Pointer to cached data.  */
  void *data;

  /* Tag describing the type of the cached data.  */
  enum pph_tag tag;

  /* Checksum information for DATA.  */
  unsigned int crc;

  /* Length in bytes of the checksummed area pointed by DATA.  Note
     that this is *not* the size of the memory area pointed by DATA,
     just the number of bytes in DATA that we have checksummed.  */
  size_t crc_nbytes;
} pph_cache_entry;

DEF_VEC_O(pph_cache_entry);
DEF_VEC_ALLOC_O(pph_cache_entry,heap);

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
typedef struct pph_cache {
  /* Array of entries.  */
  VEC(pph_cache_entry,heap) *v;

  /* Map between slots in the array and pointers.  */
  struct pointer_map_t *m;
} pph_cache;


/* Actions associated with each symbol table entry.  These indicate
   what the reader should do when registering each entry with the
   middle-end.  */
enum pph_symtab_action {
  /* Declare this symbol with rest_of_decl_compilation.  */
  PPH_SYMTAB_DECLARE = 0x23,

  /* Expand this function with expand_or_defer_fn.  */
  PPH_SYMTAB_EXPAND
};


/* Symbol table entry.  */
typedef struct pph_symtab_entry
{
  /* Registration action to perform by the reader.  */
  enum pph_symtab_action action;

  /* VAR_DECL or FUNCTION_DECL to declare.  */
  tree decl;

  /* Values to be passed to rest_of_decl_compilation.  */
  unsigned int top_level : 1;
  unsigned int at_end : 1;
} pph_symtab_entry;

DEF_VEC_O(pph_symtab_entry);
DEF_VEC_ALLOC_O(pph_symtab_entry,heap);

/* Symbol table for a PPH stream.  */
typedef struct pph_symtab
{
  /* Table of all the declarations to register in declaration order.  */
  VEC(pph_symtab_entry,heap) *v;
} pph_symtab;

/* Vector of pph_stream pointers.  */
struct pph_stream;
typedef struct pph_stream *pph_stream_ptr;
DEF_VEC_P(pph_stream_ptr);
DEF_VEC_ALLOC_P(pph_stream_ptr,heap);

/* List of PPH images read during parsing.  Images opened during #include
   processing and opened from pph_in_includes cannot be closed
   immediately after reading, because the pickle cache contained in
   them may be referenced from other images.  We delay closing all of
   them until the end of parsing (when pph_reader_finish is called).  */
extern VEC(pph_stream_ptr, heap) *pph_read_images;

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

      /* List of PPH files included by the PPH file that we are currently
        generating.  Note that this list only contains PPH files, not
        regular text headers.  Those are embedded in this stream.  */
      VEC(pph_stream_ptr,heap) *includes;
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
  pph_cache cache;

  /* Pointer to the pre-loaded cache.  This cache contains all the
     trees that are always built by the compiler on startup (and
     thus need not be pickled).  This cache is shared by all the
     pph_stream objects.  */
  pph_cache *preloaded_cache;

  /* Nonzero if the stream was opened for writing.  */
  unsigned int write_p : 1;

  /* Symbol table.  This is collected as the compiler instantiates
    symbols and functions.  Once we finish parsing the header file,
    this array is written out to the PPH image.  This way, the reader
    will be able to instantiate these symbols in the same order that
    they were instantiated originally.  */
  pph_symtab symtab;
} pph_stream;

/* Filter values to avoid emitting certain objects to a PPH file.  */
#define PPHF_NONE		0
#define PPHF_NO_BUILTINS	(1 << 0)
#define PPHF_NO_XREFS		(1 << 1)
#define PPHF_NO_PREFS		(1 << 2)

/* In pph-streamer.c.  */
void pph_init_preloaded_cache (void);
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
void pph_cache_insert_at (pph_cache *, void *, unsigned, enum pph_tag);
bool pph_cache_lookup (pph_cache *, void *, unsigned *, enum pph_tag);
bool pph_cache_lookup_in_includes (void *, unsigned *, unsigned *,
                                   enum pph_tag);
bool pph_cache_add (pph_cache *, void *, unsigned *, enum pph_tag);
void pph_cache_sign (pph_cache *, unsigned, unsigned, size_t);
unsigned pph_get_signature (tree, size_t *);

/* In pph-streamer-out.c.  */
void pph_flush_buffers (pph_stream *);
void pph_init_write (pph_stream *);
void pph_write_tree (struct output_block *, tree, bool);
#if 0
void pph_write_mergeable_tree (pph_stream *, tree);
#endif
void pph_write_mergeable_chain (pph_stream *, tree);
void pph_add_decl_to_symtab (tree, enum pph_symtab_action, bool, bool);
void pph_add_include (pph_stream *);
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
#if 0
tree pph_read_mergeable_tree (pph_stream *, tree *);
void pph_read_mergeable_chain (pph_stream *, tree *);
#endif
location_t pph_read_location (struct lto_input_block *, struct data_in *);
void pph_read_file (const char *);
void pph_reader_finish (void);

/* In pt.c.  */
extern void pph_out_pending_templates_list (pph_stream *stream);
extern void pph_in_pending_templates_list (pph_stream *stream);
extern void pph_out_spec_entry_tables (pph_stream *stream);
extern void pph_in_spec_entry_tables (pph_stream *stream);


/* Inline functions.  */

/* Return true if we are generating a PPH image.  */
static inline bool
pph_writer_enabled_p (void)
{
  return pph_out_file != NULL;
}

/* Return true if PPH has been enabled.  */
static inline bool
pph_enabled_p (void)
{
  return pph_writer_enabled_p () || pph_reader_enabled_p ();
}

/* Return the pickle cache in STREAM corresponding to MARKER.
   if MARKER is PPH_RECORD_IREF, it returns the cache in STREAM itself.
   If MARKER is PPH_RECORD_XREF, it returns the cache in
   pph_read_images[INCLUDE_IX].
   If MARKER is a PREF, it returns the preloaded cache.  */
static inline pph_cache *
pph_cache_select (pph_stream *stream, enum pph_record_marker marker,
                  unsigned include_ix)
{
  switch (marker)
    {
    case PPH_RECORD_IREF:
      return &stream->cache;
      break;
    case PPH_RECORD_XREF:
      return &VEC_index (pph_stream_ptr, pph_read_images, include_ix)->cache;
      break;
    case PPH_RECORD_PREF:
      return stream->preloaded_cache;
      break;
    default:
      gcc_unreachable ();
    }
}

/* Return entry IX in CACHE.  */
static inline pph_cache_entry *
pph_cache_get_entry (pph_cache *cache, unsigned ix)
{
  return VEC_index (pph_cache_entry, cache->v, ix);
}

/* Return the data pointer at slot IX in CACHE  */
static inline void *
pph_cache_get (pph_cache *cache, unsigned ix)
{
  pph_cache_entry *e = pph_cache_get_entry (cache, ix);
  return e->data;
}

/* Return the data pointer in one of STREAM's caches (determined by
   MARKER, IMAGE_IX and IX.  Expect data to have type tag TAG.  */
static inline void *
pph_cache_find (pph_stream *stream, enum pph_record_marker marker,
		unsigned image_ix, unsigned ix, enum pph_tag tag)
{
  pph_cache *cache = pph_cache_select (stream, marker, image_ix);
  pph_cache_entry *e = pph_cache_get_entry (cache, ix);

  /* If the caller expects any tree, make sure we get a valid tree code.  */
  if (tag == PPH_any_tree)
    gcc_assert (e->tag < PPH_any_tree);
  else
    gcc_assert (e->tag == tag);

  return e->data;
}


#if 0
/* Output AST T to STREAM.  If -fpph-tracer is set to TLEVEL or
   higher, T is sent to pph_trace_tree.  */
static inline void
pph_out_tree_1 (pph_stream *stream, tree t, int tlevel)
{
  if (flag_pph_tracer >= tlevel)
    pph_trace_tree (stream, t);
  pph_write_tree (stream->encoder.w.ob, t, false);
}

/* Output AST T to STREAM.  Trigger tracing at -fpph-tracer=2.  */
static inline void
pph_out_tree (pph_stream *stream, tree t)
{
  pph_out_tree_1 (stream, t, 2);
}

/* Output AST T from ENCLOSING_NAMESPACE to STREAM.
   Trigger tracing at -fpph-tracer=2.  */
static inline void
pph_out_mergeable_tree (pph_stream *stream, tree t)
{
  if (flag_pph_tracer >= 2)
    pph_trace_tree (stream, t);
  pph_write_mergeable_tree (stream, t);
}
#else
extern void pph_out_tree (pph_stream *stream, tree t);
#endif

#if 0
/* Write an unsigned int VALUE to STREAM.  */
static inline void
pph_out_uint (pph_stream *stream, unsigned int value)
{
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, value);
  streamer_write_uhwi (stream->encoder.w.ob, value);
}

/* Write an unsigned HOST_WIDE_INT VALUE to STREAM.  */
static inline void
pph_out_uhwi (pph_stream *stream, unsigned HOST_WIDE_INT value)
{
  streamer_write_uhwi (stream->encoder.w.ob, value);
}

/* Write a HOST_WIDE_INT VALUE to stream.  */
static inline void
pph_out_hwi (pph_stream *stream, HOST_WIDE_INT value)
{
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
#else
extern void pph_out_uint (pph_stream *stream, unsigned int value);
#endif

#if 0
/* Write location LOC of length to STREAM.  */
static inline void
pph_out_location (pph_stream *stream, location_t loc)
{
  if (flag_pph_tracer >= 4)
    pph_trace_location (stream, loc);
  pph_write_location (stream->encoder.w.ob, loc);
}
#else
extern void pph_out_location (pph_stream *stream, location_t loc);
#endif

#if 0
/* Write a chain of ASTs to STREAM starting with FIRST.  */
static inline void
pph_out_chain (pph_stream *stream, tree first)
{
  if (flag_pph_tracer >= 2)
    pph_trace_chain (stream, first);
  streamer_write_chain (stream->encoder.w.ob, first, false);
}

/* Write a chain of ASTs to STREAM starting with FIRST.  */
static inline void
pph_out_mergeable_chain (pph_stream *stream, tree first)
{
  if (flag_pph_tracer >= 2)
    pph_trace_chain (stream, first);
  pph_write_mergeable_chain (stream, first);
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
#endif

#if 0
/* Read an unsigned integer from STREAM.  */
static inline unsigned int
pph_in_uint (pph_stream *stream)
{
  HOST_WIDE_INT unsigned n = streamer_read_uhwi (stream->encoder.r.ib);
  gcc_assert (n == (unsigned) n);
  if (flag_pph_tracer >= 4)
    pph_trace_uint (stream, n);
  return (unsigned) n;
}

/* Read an unsigned HOST_WIDE_INT from STREAM.  */
static inline unsigned HOST_WIDE_INT
pph_in_uhwi (pph_stream *stream)
{
  return streamer_read_uhwi (stream->encoder.r.ib);
}

/* Read a HOST_WIDE_INT from STREAM.  */
static inline HOST_WIDE_INT
pph_in_hwi (pph_stream *stream)
{
  return streamer_read_hwi (stream->encoder.r.ib);
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
#else
extern unsigned int pph_in_uint (pph_stream *stream);
extern location_t pph_in_location (pph_stream *stream);
extern tree pph_in_tree (pph_stream *stream);
#endif

#if 0
/* Load an AST in an ENCLOSING_NAMESPACE from STREAM.
   Return the corresponding tree.  */
static inline void
pph_in_mergeable_tree (pph_stream *stream, tree *chain)
{
  tree t = pph_read_mergeable_tree (stream, chain);
  if (flag_pph_tracer >= 3)
    pph_trace_tree (stream, t);
}

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

/* Read and merge a chain of ASTs from STREAM into an existing CHAIN.  */
static inline void
pph_in_mergeable_chain (pph_stream *stream, tree* chain)
{
  pph_read_mergeable_chain (stream, chain);
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
#endif

#if 0
/* Write record MARKER for data type TAG to STREAM.  */
static inline void
pph_out_record_marker (pph_stream *stream, enum pph_record_marker marker,
                       enum pph_tag tag)
{
  gcc_assert (marker == (enum pph_record_marker)(unsigned char) marker);
  pph_out_uchar (stream, marker);

  gcc_assert (tag == (enum pph_tag)(unsigned) tag);
  pph_out_uint (stream, tag);
}


/* Read and return a record marker from STREAM.  On return, *TAG_P will
   contain the tag for the data type stored in this record.  */
static inline enum pph_record_marker
pph_in_record_marker (pph_stream *stream, enum pph_tag *tag_p)
{
  enum pph_record_marker m = (enum pph_record_marker) pph_in_uchar (stream);
  gcc_assert (m == PPH_RECORD_START
              || m == PPH_RECORD_START_NO_CACHE
              || m == PPH_RECORD_START_MUTATED
	      || m == PPH_RECORD_END
	      || m == PPH_RECORD_IREF
	      || m == PPH_RECORD_XREF
	      || m == PPH_RECORD_PREF);

  *tag_p = (enum pph_tag) pph_in_uint (stream);
  gcc_assert ((unsigned) *tag_p < (unsigned) PPH_NUM_TAGS);

  return m;
}
#else
extern void pph_out_record_marker (pph_stream *stream,
		enum pph_record_marker marker, enum pph_tag tag);
extern enum pph_record_marker pph_in_record_marker (pph_stream *stream,
		enum pph_tag *tag_p);
#endif


/* Return true if MARKER is PPH_RECORD_IREF, PPH_RECORD_XREF,
   or PPH_RECORD_PREF.  */

static inline bool
pph_is_reference_marker (enum pph_record_marker marker)
{
  return marker == PPH_RECORD_IREF
         || marker == PPH_RECORD_XREF
         || marker == PPH_RECORD_PREF;
}


/* Return true if tree T needs to be signed to detect state mutations.
   This is used when multiple PPH images contain different versions of
   the same tree node (e.g., decl.pph contains the declaration of
   function F while impl.pph contains its definition).

   When generating the image for impl.pph, we will read F's
   declaration from decl.pph.  This becomes an external reference.
   When we go to write F in impl.pph, the cache will find the external
   reference to F in decl.pph and write it.

   This causes us to lose all the information added to F's node in
   impl.h (its body, return value, etc).  So, a translation unit
   reading impl.pph will never get that data and compilation will
   fail.

   We notice state mutations by computing CRC signatures on the body
   of trees.  The first signature is computed when the tree is read
   from an image.  The second signature is computed when we go to write
   the tree again (pph_out_start_tree_record).  */
static inline bool
tree_needs_signature (tree t)
{
  return DECL_P (t) || TYPE_P (t);
}

/* Return true if PPH tag TAG corresponds to a valid tree code.  */
static inline bool
pph_tag_is_tree_code (enum pph_tag tag)
{
  return (unsigned) tag < (unsigned) MAX_TREE_CODES;
}

/* Return the PPH tag associated with tree node T.  */
/* FIXME pph: apparently unused, except just below.  */
static inline enum pph_tag
pph_tree_code_to_tag (tree t)
{
  return t ? (enum pph_tag) TREE_CODE (t) : PPH_null;
}

/* Return the tree code associated with PPH tag TAG.  */
/* FIXME pph: apparently unused.  */
static inline enum tree_code
pph_tag_to_tree_code (enum pph_tag tag)
{
  gcc_assert (pph_tag_is_tree_code (tag));
  return (enum tree_code) tag;
}

#endif  /* GCC_CP_PPH_STREAMER_H  */
