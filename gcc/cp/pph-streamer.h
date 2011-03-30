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

  /* Nonzero if the stream was opened for writing.  */
  unsigned int write_p : 1;
} pph_stream;

/* In pph-streamer.c.  */
pph_stream *pph_stream_open (const char *, const char *);
void pph_stream_close (pph_stream *);
void pph_stream_trace_tree (pph_stream *, tree);
void pph_stream_trace_uint (pph_stream *, unsigned int);
void pph_stream_trace_bytes (pph_stream *, const void *, size_t);
void pph_stream_trace_string (pph_stream *, const char *);
void pph_stream_trace_string_with_length (pph_stream *, const char *, unsigned);

/* In pph.c.  FIXME move these to pph-streamer.c.  */
struct cp_token_cache;
extern void pth_save_token_cache (struct cp_token_cache *, pph_stream *);

/* Inline functions.  */

/* Output AST T to STREAM.  */
static inline void
pph_output_tree (pph_stream *stream ATTRIBUTE_UNUSED, tree t ATTRIBUTE_UNUSED)
{
  if (flag_pph_tracer)
    pph_stream_trace_tree (stream, t);
  lto_output_tree (stream->ob, t, true);
}

/* Write a uint VALUE to STREAM.  */
static inline void
pph_output_uint (pph_stream *stream, unsigned int value)
{
  if (flag_pph_tracer)
    pph_stream_trace_uint (stream, value);
  lto_output_sleb128_stream (stream->ob->main_stream, value);
}

/* Write N bytes from P to STREAM.  */
static inline void
pph_output_bytes (pph_stream *stream, const void *p, size_t n)
{
  if (flag_pph_tracer)
    pph_stream_trace_bytes (stream, p, n);
  lto_output_data_stream (stream->ob->main_stream, p, n);
}

/* Write string STR to STREAM.  */
static inline void
pph_output_string (pph_stream *stream, const char *str)
{
  lto_output_string (stream->ob, stream->ob->main_stream, str);
  if (flag_pph_tracer)
    pph_stream_trace_string (stream, str);
}

/* Write string STR of length LEN to STREAM.  */
static inline void
pph_output_string_with_length (pph_stream *stream, const char *str,
			       unsigned int len)
{
  if (str)
    lto_output_string_with_length (stream->ob, stream->ob->main_stream,
				   str, len + 1);
  else
    {
      /* lto_output_string_with_length does not handle NULL strings,
	 but lto_output_string does.  */
      pph_output_string (stream, NULL);
    }

  if (flag_pph_tracer)
    pph_stream_trace_string_with_length (stream, str, len);
}

/* Read an unsigned HOST_WIDE_INT integer from STREAM.  */
static inline unsigned int
pph_input_uint (pph_stream *stream)
{
  HOST_WIDE_INT unsigned n = lto_input_uleb128 (stream->ib);
  gcc_assert (n == (unsigned) n);
  if (flag_pph_tracer)
    pph_stream_trace_uint (stream, n);
  return (unsigned) n;
}

/* Read N bytes from STREAM into P.  The caller is responsible for 
   allocating a sufficiently large buffer.  */
static inline void
pph_input_bytes (pph_stream *stream, void *p, size_t n)
{
  lto_input_data_block (stream->ib, p, n);
  if (flag_pph_tracer)
    pph_stream_trace_bytes (stream, p, n);
}

/* Read and return a string of up to MAX characters from STREAM.
   The caller is responsible for freeing the memory allocated
   for the string.  */

static inline const char *
pph_input_string (pph_stream *stream)
{
  const char *s = lto_input_string (stream->data_in, stream->ib);
  if (flag_pph_tracer)
    pph_stream_trace_string (stream, s);
  return s;
}

/* Load an AST from STREAM.  Return the corresponding tree.  */

static inline tree
pph_input_tree (pph_stream *stream ATTRIBUTE_UNUSED)
{
  tree t = lto_input_tree (stream->ib, stream->data_in);
  if (flag_pph_tracer)
    pph_stream_trace_tree (stream, t);
  return t;
}

/* Return the PPH stream object associated with output block OB.  */

static inline pph_stream *
pph_get_ob_stream (struct output_block *ob)
{
  /* FIXME pph - Do not overload OB fields this way.  */
  return ((pph_stream *) ob->cfg_stream);
}

/* Set the PPH stream object F associated with output block OB.  */

static inline void
pph_set_ob_stream (struct output_block *ob, pph_stream *f)
{
  /* FIXME pph - Do not overload OB fields this way.  */
  ob->cfg_stream = (struct lto_output_stream *) f;
}

#endif  /* GCC_CP_PPH_STREAMER_H  */
