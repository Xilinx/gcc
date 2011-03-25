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
#include "tree-pretty-print.h"
#include "lto-streamer.h"
#include "pph-streamer.h"
#include "pph.h"
#include "tree-pass.h"
#include "version.h"
#include "cppbuiltin.h"

/* FIXME pph.  This holds the FILE handle for the current PPH file
   that we are writing.  It is necessary because the LTO callbacks do
   not allow passing a FILE handle to them.  */
static FILE *current_pph_file = NULL;


/* Get the section with name NAME and type SECTION_TYPE from FILE_DATA.
   Return a pointer to the start of the section contents and store
   the length of the section in *LEN_P.

   FIXME pph, this does not currently handle multiple sections.  It
   assumes that the file has exactly one section.  */

static const char *
pph_get_section_data (struct lto_file_decl_data *file_data,
		      enum lto_section_type section_type ATTRIBUTE_UNUSED,
		      const char *name ATTRIBUTE_UNUSED,
		      size_t *len)
{
  /* FIXME pph - Stop abusing lto_file_decl_data fields.  */
  const pph_stream *stream = (const pph_stream *) file_data->file_name;
  *len = stream->file_size - sizeof (pph_file_header);
  return (const char *) stream->file_data + sizeof (pph_file_header);
}


/* Free the section data from FILE_DATA of SECTION_TYPE and NAME that
   starts at OFFSET and has LEN bytes.  */

static void
pph_free_section_data (struct lto_file_decl_data *file_data,
		   enum lto_section_type section_type ATTRIBUTE_UNUSED,
		   const char *name ATTRIBUTE_UNUSED,
		   const char *offset ATTRIBUTE_UNUSED,
		   size_t len ATTRIBUTE_UNUSED)
{
  /* FIXME pph - Stop abusing lto_file_decl_data fields.  */
  const pph_stream *stream = (const pph_stream *) file_data->file_name;
  free (stream->file_data);
}


/* Read into memory the contents of the file in STREAM.  Initialize
   internal tables and data structures needed to re-construct the
   ASTs in the file.  */

static void
pph_stream_init_read (pph_stream *stream)
{
  struct stat st;
  size_t i, bytes_read, strtab_size, body_size;
  int retcode;
  pph_file_header *header;
  const char *strtab, *body;

  /* Read STREAM->NAME into the memory buffer STREAM->FILE_DATA.
     FIXME pph, we are reading the whole file at once.  This seems
     wasteful.  */
  retcode = fstat (fileno (stream->file), &st);
  gcc_assert (retcode == 0);
  stream->file_size = (size_t) st.st_size;
  stream->file_data = XCNEWVEC (char, stream->file_size);
  bytes_read = fread (stream->file_data, 1, stream->file_size, stream->file);
  gcc_assert (bytes_read == stream->file_size);

  /* Set LTO callbacks to read the PPH file.  */
  stream->pph_sections = XCNEWVEC (struct lto_file_decl_data *,
				   PPH_NUM_SECTIONS);
  for (i = 0; i < PPH_NUM_SECTIONS; i++)
    {
      stream->pph_sections[i] = XCNEW (struct lto_file_decl_data);
      /* FIXME pph - Stop abusing fields in lto_file_decl_data.  */
      stream->pph_sections[i]->file_name = (const char *) stream;
    }

  lto_set_in_hooks (stream->pph_sections, pph_get_section_data,
		    pph_free_section_data);

  header = (pph_file_header *) stream->file_data;
  strtab = (const char *) header + sizeof (pph_file_header);
  strtab_size = header->strtab_size;
  body = strtab + strtab_size;
  gcc_assert (stream->file_size >= strtab_size + sizeof (pph_file_header));
  body_size = stream->file_size - strtab_size - sizeof (pph_file_header);

  /* Create an input block structure pointing right after the string
     table.  */
  stream->ib = XCNEW (struct lto_input_block);
  LTO_INIT_INPUT_BLOCK_PTR (stream->ib, body, 0, body_size);
  stream->data_in = lto_data_in_create (stream->pph_sections[0], strtab,
                                        strtab_size, NULL);
}


/* Initialize buffers and tables in STREAM for writing.  */

static void
pph_stream_init_write (pph_stream *stream)
{
  stream->out_state = lto_new_out_decl_state ();
  lto_push_out_decl_state (stream->out_state);
  stream->decl_state_stream = XCNEW (struct lto_output_stream);
  stream->ob = create_output_block (LTO_section_decls);
}


/* Initialize all the streamer hooks used for streaming ASTs.  */

static void
pph_streamer_hooks_init (void)
{
  lto_streamer_hooks *h = streamer_hooks_init ();
  h->reader_init = NULL;
  h->writer_init = NULL;
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
      pph_streamer_hooks_init ();
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


/* Callback for lang_hooks.lto.begin_section.  Open file NAME.  */

static void
pph_stream_begin_section (const char *name ATTRIBUTE_UNUSED)
{
}


/* Callback for lang_hooks.lto.append_data.  Write LEN bytes from DATA
   into current_pph_file.  BLOCK is currently unused, but this hook is
   required to free it.  */

static void
pph_stream_write (const void *data, size_t len, void *block)
{
  if (data)
    fwrite (data, len, 1, current_pph_file);
  free (block);
}


/* Callback for lang_hooks.lto.end_section.  */

static void
pph_stream_end_section (void)
{
}


/* Write the header for the PPH file represented by STREAM.  */

static void
pph_stream_write_header (pph_stream *stream)
{
  pph_file_header header;
  struct lto_output_stream header_stream;
  int major, minor, patchlevel;

  /* Collect version information.  */
  parse_basever (&major, &minor, &patchlevel);
  gcc_assert (major == (char) major);
  gcc_assert (minor == (char) minor);
  gcc_assert (patchlevel == (char) patchlevel);

  /* Write the header for the PPH file.  */
  memset (&header, 0, sizeof (header));
  strcpy (header.id_str, pph_id_str);
  header.major_version = (char) major;
  header.minor_version = (char) minor;
  header.patchlevel = (char) patchlevel;
  header.strtab_size = stream->ob->string_stream->total_size;

  memset (&header_stream, 0, sizeof (header_stream));
  lto_output_data_stream (&header_stream, &header, sizeof (header));
  lto_write_stream (&header_stream);
}


/* Write the body of the PPH file represented by STREAM.  */

static void
pph_stream_write_body (pph_stream *stream)
{
  /* Write the string table.  */
  lto_write_stream (stream->ob->string_stream);

  /* Write out the physical representation for every AST in all the
     streams in STREAM->OUT_STATE.  */
  lto_output_decl_state_streams (stream->ob, stream->out_state);

  /* Now write the vector of all AST references.  */
  lto_output_decl_state_refs (stream->ob, stream->decl_state_stream,
			      stream->out_state);

  /* Finally, physically write all the streams.  */
  lto_write_stream (stream->ob->main_stream);
}


/* Close PPH stream STREAM.  Write all the ASTs to disk and deallocate
   all memory used by it.  */

void
pph_stream_close (pph_stream *stream)
{
  if (stream->write_p)
    {
      gcc_assert (current_pph_file == NULL);
      current_pph_file = stream->file;

      /* Redirect the LTO basic I/O langhooks.  */
      lang_hooks.lto.begin_section = pph_stream_begin_section;
      lang_hooks.lto.append_data = pph_stream_write;
      lang_hooks.lto.end_section = pph_stream_end_section;

      /* Write the state buffers built by pph_output_*() calls.  */
      lto_begin_section (stream->name, false);
      pph_stream_write_header (stream);
      pph_stream_write_body (stream);
      lto_end_section ();
    }

  fclose (stream->file);
  stream->file = current_pph_file = NULL;
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
