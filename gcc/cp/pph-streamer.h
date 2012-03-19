/* Support routines and data structures for streaming PPH data.

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

#ifndef GCC_CP_PPH_STREAMER_H
#define GCC_CP_PPH_STREAMER_H

#include "lto-streamer.h"
#include "data-streamer.h"
#include "tree-streamer.h"
#include "tree.h"
#include "pph.h"

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

  /* Non-zero if this entry has been emitted as a merge key.  This means
     that the next time this entry is written out, it should be written
     as a merge body record (see pph_out_start_tree_record for details).  */
  unsigned int needs_merge_body : 1;

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


/* Replay table entry.  */
typedef struct pph_replay_entry
{
  /* Registration action to perform by the reader.  */
  enum pph_replay_action action;

  /* Symbol or type to process.  */
  tree to_replay;

  /* Values to be passed to rest_of_decl_compilation.  */
  unsigned int top_level : 1;
  unsigned int at_end : 1;

  /* Values captured from global state used by expand_or_defer_fn and
     expand_or_defer_fn_1.  FIXME pph, all this global state should
     disappear.  */
  unsigned int at_eof : 1;
  int x_processing_template_decl;
  int function_depth;
} pph_replay_entry;

DEF_VEC_O(pph_replay_entry);
DEF_VEC_ALLOC_O(pph_replay_entry,heap);

/* Replay table for a PPH stream.  This table represents parsing
   actions that need to be done to finalize the parsing of a
   symbol declaration or a type.  These actions transfer the parsed
   tree to the middle end for code generation and/or layout.

   They need to be done in the PPH reader, since none of the
   state produced by them are saved in the PPH image (e.g., call graph
   nodes, method vector layout, etc).  */
typedef struct pph_replay
{
  /* Table of all the declarations to register in declaration order.  */
  VEC(pph_replay_entry,heap) *v;
} pph_replay;

/* Vector of pph_stream pointers.  */
typedef struct pph_stream *pph_stream_ptr;
DEF_VEC_P(pph_stream_ptr);
DEF_VEC_ALLOC_P(pph_stream_ptr,heap);

/* Data structures used to encode and decode trees.  */

/* A PPH stream contains all the data and attributes needed to
   write symbols, declarations and other parsing products to disk.  */
struct pph_stream {
  /* Path name of the PPH file.  */
  const char *name;

  /* Path name of the original text header file.  This is the full
     path name of the header as found by the pre-processor after
     doing include path expansion.  Note that this may not necessarily
     be an absolute path name.

     We rely on this path name to identify the corresponding PPH name
     when deciding whether to load external PPH files in
     pph_in_line_table_and_includes.  */
  const char *header_name;

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
  pph_cache cache;

  /* Pointer to the pre-loaded cache.  This cache contains all the
     trees that are always built by the compiler on startup (and
     thus need not be pickled).  This cache is shared by all the
     pph_stream objects.  */
  pph_cache *preloaded_cache;

  /* Nonzero if the stream was opened for writing.  */
  unsigned int write_p : 1;

  /* Nonzero if the stream has been read and it is available for
     resolving external references.  */
  unsigned int in_memory_p : 1;

  /* Replay table.  This is collected as the compiler instantiates
     symbols and functions.  Once we finish parsing the header file,
     this array is written out to the PPH image.  This way, the reader
     will be able to replay these actions in the same order that
     they were executed originally.  */
  pph_replay replay;

  /* Transitive closure list of all the images included directly and
     indirectly by this image.  Note that this list only contains PPH
     files, not regular text headers.  Regular text headers are embedded
     in this stream.  */
  struct {
    /* Vector to hold all the images.  */
    VEC(pph_stream_ptr,heap) *v;

    /* Set to prevent adding the same image more than once.  */
    struct pointer_set_t *m;
  } includes;
};

/* Filter values to avoid emitting certain objects to a PPH file.  */
#define PPHF_NONE		0
#define PPHF_NO_BUILTINS	(1 << 0)
#define PPHF_NO_XREFS		(1 << 1)
#define PPHF_NO_PREFS		(1 << 2)

enum pph_trace_kind
{
  pph_trace_key_out, pph_trace_unmerged_key, pph_trace_merged_key,
  pph_trace_merge_body, pph_trace_mutate, pph_trace_normal
};

enum pph_trace_end
{
  pph_trace_front, pph_trace_back
};

/* In pph-core.c.  */
const char *pph_tree_code_text (enum tree_code code);
void pph_dump_global_state (FILE *, const char *msg);
pph_stream *pph_stream_registry_lookup (const char *);
void pph_stream_set_header_name (pph_stream *, const char *);
pph_stream *pph_stream_open (const char *, const char *);
void pph_stream_close (pph_stream *);
void pph_stream_close_no_flush (pph_stream *);
void pph_trace_marker (enum pph_record_marker marker, enum pph_tag tag);
void pph_trace_tree (tree, const char *,
		     enum pph_trace_end, enum pph_trace_kind);
void pph_trace_note (const char *kind, const char *value);
pph_cache_entry *pph_cache_insert_at (pph_cache *, void *, unsigned,
				      enum pph_tag);
pph_cache_entry *pph_cache_lookup (pph_cache *, void *, unsigned *,
				   enum pph_tag);
pph_cache_entry *pph_cache_lookup_in_includes (pph_stream *, void *,
					       unsigned *, unsigned *,
					       enum pph_tag);
pph_cache_entry *pph_cache_add (pph_cache *, void *, unsigned *, enum pph_tag);
void pph_cache_sign (pph_cache *, unsigned, unsigned, size_t);
unsigned pph_get_signature (tree, size_t *);

/* In pph-out.c.  */
void pph_flush_buffers (pph_stream *);
void pph_init_write (pph_stream *);
void pph_write_mergeable_chain (pph_stream *, tree);
void pph_writer_init (void);
void pph_writer_finish (void);
void pph_out_location (pph_stream *, location_t);
void pph_out_tree (pph_stream *, tree);
void pph_disable_output (void);

/* In pph-in.c.  */
void pph_init_read (pph_stream *);
location_t pph_in_location (pph_stream *);
pph_stream *pph_read_file (const char *, pph_stream *);
tree pph_in_tree (pph_stream *stream);
void pph_reader_init (void);
void pph_reader_finish (void);


/* Inline functions.  */


/* Return the pickle cache in STREAM corresponding to MARKER.
   INCLUDE_IX is only used for MARKER values PPH_RECORD_XREF or
   PPH_RECORD_START_MUTATED.

   If MARKER is one of {PPH_RECORD_IREF, PPH_RECORD_START,
   PPH_RECORD_START_MERGE_BODY, PPH_RECORD_START_MERGE_KEY}, it
   returns the cache in STREAM itself.

   If MARKER is one of {PPH_RECORD_XREF, PPH_RECORD_START_MUTATED}, it
   returns the cache in STREAM->INCLUDES[INCLUDE_IX].

   If MARKER is a PREF, it returns the preloaded cache.  */
static inline pph_cache *
pph_cache_select (pph_stream *stream, enum pph_record_marker marker,
                  unsigned include_ix)
{
  switch (marker)
    {
    case PPH_RECORD_IREF:
    case PPH_RECORD_START:
    case PPH_RECORD_START_MERGE_BODY:
    case PPH_RECORD_START_MERGE_KEY:
      return &stream->cache;
      break;
    case PPH_RECORD_XREF:
    case PPH_RECORD_START_MUTATED:
      return &VEC_index (pph_stream_ptr, stream->includes.v, include_ix)->cache;
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


/* Return true if MARKER is PPH_RECORD_IREF, PPH_RECORD_XREF,
   or PPH_RECORD_PREF.  */

static inline bool
pph_is_reference_marker (enum pph_record_marker marker)
{
  return marker == PPH_RECORD_IREF
         || marker == PPH_RECORD_XREF
         || marker == PPH_RECORD_PREF;
}

/* Return true if MARKER is a reference or an END marker.  */

static inline bool
pph_is_reference_or_end_marker (enum pph_record_marker marker)
{
  return marker == PPH_RECORD_END || pph_is_reference_marker (marker);
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
static inline enum pph_tag
pph_tree_code_to_tag (tree t)
{
  return t ? (enum pph_tag) TREE_CODE (t) : PPH_null;
}

/* Return true if EXPR can be emitted in two parts: a merge key and
   a merge body.  This is used to support merging ASTs read from
   multiple PPH images.  */
static inline bool
pph_tree_is_mergeable (tree expr)
{
  return DECL_P (expr) || TYPE_P (expr);
}

#endif  /* GCC_CP_PPH_STREAMER_H  */
