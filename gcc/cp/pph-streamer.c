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
#include "tree.h"
#include "langhooks.h"
#include "tree-iterator.h"
#include "tree-pretty-print.h"
#include "lto-streamer.h"
#include "pph-streamer.h"
#include "pph.h"
#include "tree-pass.h"
#include "version.h"
#include "cppbuiltin.h"

/* Return true if the given tree T is streamable.  */

static bool
pph_is_streamable (tree t ATTRIBUTE_UNUSED)
{
  /* We accept most trees.  */
  return TREE_CODE (t) != SSA_NAME
	 && (TREE_CODE (t) < OMP_PARALLEL
	     || TREE_CODE (t) > OMP_CRITICAL);
}


/* Return true if T can be emitted in the decls table as a reference.
   This should only handle C++ specific declarations.  All others are
   handled by the LTO streamer directly.  */

static bool
pph_indexable_with_decls_p (tree t)
{
  return TREE_CODE (t) == TEMPLATE_DECL;
}


/* Initialize all the streamer hooks used for streaming ASTs.  */

static void
pph_stream_hooks_init (void)
{
  lto_streamer_hooks *h = streamer_hooks_init ();
  h->name = "C++ AST";
  h->is_streamable = pph_is_streamable;
  h->write_tree = pph_stream_write_tree;
  h->read_tree = pph_stream_read_tree;
  h->pack_value_fields = pph_stream_pack_value_fields;
  h->indexable_with_decls_p = pph_indexable_with_decls_p;
  h->unpack_value_fields = pph_stream_unpack_value_fields;
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
  if (f)
    {
      pph_stream_hooks_init ();
      stream = XCNEW (pph_stream);
      stream->file = f;
      stream->name = xstrdup (name);
      stream->write_p = (strchr (mode, 'w') != NULL);
      if (stream->write_p)
	pph_stream_init_write (stream);
      else
	pph_stream_init_read (stream);
    }

  return stream;
}


/* Close PPH stream STREAM.  Write all the ASTs to disk and deallocate
   all memory used by it.  */

void
pph_stream_close (pph_stream *stream)
{
  if (stream->write_p)
    pph_stream_flush_buffers (stream);
  fclose (stream->file);
  stream->file = NULL;
}


/* Data types supported by the PPH tracer.  */
enum pph_trace_type
{
    PPH_TRACE_TREE,
    PPH_TRACE_UINT,
    PPH_TRACE_BYTES,
    PPH_TRACE_STRING
};


/* Print tracing information for STREAM on pph_logfile.  DATA is the
   memory area to display, SIZE is the number of bytes to print, TYPE
   is the kind of data to print.  */

static void
pph_stream_trace (pph_stream *stream, const void *data, unsigned int nbytes,
		  enum pph_trace_type type)
{
  const char *op = (stream->write_p) ? "write" : "read";
  const char *type_s[] = { "tree", "uint", "bytes", "string" };

  fprintf (pph_logfile, "*** %s: op=%s, type=%s, size=%u, value=",
	   stream->name, op, type_s[type], (unsigned) nbytes);

  switch (type)
    {
    case PPH_TRACE_TREE:
      {
	const_tree t = (const_tree) data;
	print_generic_expr (pph_logfile, CONST_CAST (union tree_node *, t),
			    TDF_SLIM);
      }
      break;

    case PPH_TRACE_UINT:
      {
	unsigned int val = *((const unsigned int *) data);
	fprintf (pph_logfile, "%u (0x%x)", val, val);
      }
      break;

    case PPH_TRACE_BYTES:
      {
	size_t i;
	const char *buffer = (const char *) data;
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
	fprintf (pph_logfile, "%.*s", (int) nbytes, (const char *) data);
      else
	fprintf (pph_logfile, "<nil>");
      break;

    default:
      gcc_unreachable ();
    }

  fputc ('\n', pph_logfile);
}


/* Show tracing information for T on STREAM.  */

void
pph_stream_trace_tree (pph_stream *stream, tree t)
{
  pph_stream_trace (stream, t, tree_code_size (TREE_CODE (t)), PPH_TRACE_TREE);
}


/* Show tracing information for VAL on STREAM.  */

void
pph_stream_trace_uint (pph_stream *stream, unsigned int val)
{
  pph_stream_trace (stream, &val, sizeof (val), PPH_TRACE_UINT);
}


/* Show tracing information for NBYTES bytes of memory area DATA on
   STREAM.  */

void
pph_stream_trace_bytes (pph_stream *stream, const void *data, size_t nbytes)
{
  pph_stream_trace (stream, data, nbytes, PPH_TRACE_BYTES);
}


/* Show tracing information for S on STREAM.  */

void
pph_stream_trace_string (pph_stream *stream, const char *s)
{
  pph_stream_trace (stream, s, s ? strlen (s) : 0, PPH_TRACE_STRING);
}


/* Show tracing information for LEN bytes of S on STREAM.  */

void
pph_stream_trace_string_with_length (pph_stream *stream, const char *s,
				     unsigned int len)
{
  pph_stream_trace (stream, s, len, PPH_TRACE_STRING);
}
