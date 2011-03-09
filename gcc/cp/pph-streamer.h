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

/* FIXME pph - Workaround incomplete PPH streamer.  Use regular FILE I/O.  */
#define PPH_USE_FILE_IO	1

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
      routines.  */
  struct data_in *data_in;

  /* Nonzero if the stream was open for writing.  */
  unsigned int write_p : 1;
} pph_stream;

/* In pph-streamer.c.  */
pph_stream *pph_stream_open (const char *, bool);
void pph_stream_close (pph_stream *);


/* Inline functions.  */

/* Output AST T to STREAM.  */
static inline void
pph_output_tree (pph_stream *stream ATTRIBUTE_UNUSED, tree t ATTRIBUTE_UNUSED)
{
#if defined PPH_USE_FILE_IO
  gcc_unreachable ();
#else
  lto_output_tree (stream->ob, t, true);
#endif
}

/* Write a uint VALUE to STREAM.  */
static inline void
pph_output_uint (pph_stream *stream, unsigned int value)
{
#if defined PPH_USE_FILE_IO
  fwrite (&value, 1, sizeof (value), stream->file);
#else
  lto_output_sleb128_stream (stream->ob->main_stream, value);
#endif
}

/* Write N bytes from P to STREAM.  */
static inline void
pph_output_bytes (pph_stream *stream, const void *p, size_t n)
{
#if defined PPH_USE_FILE_IO
  fwrite (p, 1, n, stream->file);
#else
  lto_output_data_stream (stream->ob->main_stream, p, n);
#endif
}

/* Write string STR to STREAM.  */
static inline void
pph_output_string (pph_stream *stream, const char *str)
{
#if defined PPH_USE_FILE_IO
  if (str == NULL)
    pph_output_uint (stream, -1U);
  else
    {
      unsigned length = strlen (str);
      pph_output_uint (stream, length);
      if (length > 0)
	pph_output_bytes (stream, str, length);
    }
#else
  lto_output_string (stream->ob, stream->ob->main_stream, str);
#endif
}

/* Write string STR of length LEN to STREAM.  */
static inline void
pph_output_string_with_length (pph_stream *stream, const char *str,
			       unsigned int len)
{
#if defined PPH_USE_FILE_IO
  if (str == NULL)
    pph_output_uint (stream, -1U);
  else
    {
      pph_output_uint (stream, len);
      if (len > 0)
	pph_output_bytes (stream, str, len);
    }
#else
  lto_output_string_with_length (stream->ob, stream->ob->main_stream, str, len);
#endif
}

/* Read an unsigned HOST_WIDE_INT integer from STREAM.  */
static inline unsigned
pph_input_uint (pph_stream *stream)
{
#if defined PPH_USE_FILE_IO
  unsigned num;
  size_t received;
  received = fread (&num, sizeof num, 1, stream->file);
  gcc_assert (received == 1);
  return num;
#else
  HOST_WIDE_INT unsigned n = lto_input_uleb128 (stream->ib);
  gcc_assert (n == (unsigned) n);
  return (unsigned) n;
#endif
}

/* Read N bytes from STREAM into P.  The caller is responsible for 
   allocating a sufficiently large buffer.  */
static inline void
pph_input_bytes (pph_stream *stream, void *p, size_t n)
{
#if defined PPH_USE_FILE_IO
  size_t received = fread (p, 1, n, stream->file);
  gcc_assert (received == n);
#else
  lto_input_data_block (stream->ib, p, n);
#endif
}

/* Read and return a string of up to MAX characters from STREAM.
   The caller is responsible for freeing the memory allocated
   for the string.  */

static inline const char *
pph_input_string (pph_stream *stream)
{
#if defined PPH_USE_FILE_IO
  char *buf = NULL;
  unsigned len;
  size_t received = fread (&len, sizeof len, 1, stream->file);
  gcc_assert (received == 1);
  if (len > 0 && len != -1U)
    {
      buf = XCNEWVEC (char, len + 1);
      received = fread (buf, 1, len, stream->file);
    }
  return (const char *) buf;
#else
  return lto_input_string (stream->data_in, stream->ib);
#endif
}

/* Load an AST from STREAM.  Return the corresponding tree.  */

static inline tree
pph_input_tree (pph_stream *stream ATTRIBUTE_UNUSED)
{
#if defined PPH_USE_FILE_IO
  gcc_unreachable ();
#else
  return lto_input_tree (stream->ib, stream->data_in);
#endif
}

#endif  /* GCC_CP_PPH_STREAMER_H  */
