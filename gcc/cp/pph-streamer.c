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
#include "lto-streamer.h"
#include "pph-streamer.h"

#if !defined PPH_USE_FILE_IO
static FILE *current_pph_file = NULL;
#endif

/* Read into memory the contents of the file in STREAM.  Initialize
   internal tables and data structures needed to re-construct the
   ASTs in the file.  */

static void
pph_file_read (pph_stream *stream)
{
#if defined PPH_USE_FILE_IO
  stream->file = fopen (stream->name, "rb");
#else
  void *data;
  struct stat st;
  size_t bytes_read, data_size;

  /* Read STREAM->NAME into the memory buffer DATA.  */
  stat (stream->name, &st);
  data_size = (size_t) st.st_size;
  data = XCNEWVEC (char, data_size);
  stream->file = fopen (stream->name, "rb");
  gcc_assert (stream->file);
  bytes_read = fread (data, 1, data_size, stream->file);
  gcc_assert (bytes_read == data_size);
  fclose (stream->file);

  stream->ib = XCNEW (struct lto_input_block);
  LTO_INIT_INPUT_BLOCK_PTR (stream->ib, (const char *) data, 0, data_size);
  stream->data_in = lto_data_in_create (NULL, NULL, 0, NULL);
#endif
}


/* Create a new PPH stream to be stored on the file called NAME.  If
   TO_READ_P is true, the file is open for reading.  */

pph_stream *
pph_stream_open (const char *name, bool to_read_p)
{
  pph_stream *stream = XCNEW (pph_stream);
  stream->name = xstrdup (name);
  if (!to_read_p)
    {
      stream->file = fopen (name, "wb");
      stream->out_state = lto_new_out_decl_state ();
      lto_push_out_decl_state (stream->out_state);
      stream->decl_state_stream = XCNEW (struct lto_output_stream);
      stream->ob = create_output_block (LTO_section_decls);
    }
  else
    pph_file_read (stream);

  return stream;
}


#if !defined PPH_USE_FILE_IO
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
#endif


/* Close PPH stream STREAM.  Write all the ASTs to disk and deallocate
   all memory used by it.  */

void
pph_stream_close (pph_stream *stream)
{
#if defined PPH_USE_FILE_IO
  fclose (stream->file);
#else
  gcc_assert (current_pph_file == NULL);
  current_pph_file = stream->file;

  /* Redirect the LTO basic I/O langhooks.  */
  lang_hooks.lto.begin_section = pph_stream_begin_section;
  lang_hooks.lto.append_data = pph_stream_write;
  lang_hooks.lto.end_section = pph_stream_end_section;

  /* Write the state buffers built by pph_stream_output() calls.  */
  lto_begin_section (stream->name, false);

  /* Make string 0 be a NULL string.  */
  lto_output_1_stream (stream->ob->string_stream, 0);

  /* Write out the physical representation for every AST in all the
     streams in STREAM->OUT_STATE.  */
  lto_output_decl_state_streams (stream->ob, stream->out_state);

  /* Now write the vector of all AST references.  */
  lto_output_decl_state_refs (stream->ob, stream->decl_state_stream,
			      stream->out_state);

  /* Finally, physically write all the streams.  */
  lto_write_stream (stream->ob->main_stream);
  lto_write_stream (stream->ob->string_stream);

  lto_end_section ();
  fclose (stream->file);
  current_pph_file = NULL;
#endif
}
