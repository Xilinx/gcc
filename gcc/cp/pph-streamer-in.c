/* Routines for reading PPH data.
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

void
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


/* Callback for reading ASTs from a stream.  This reads all the fields
   that are not processed by default by the common tree pickler.
   IB, DATA_IN are as in lto_read_tree.  EXPR is the partially materialized
   tree.  */

void
pph_stream_read_tree (struct lto_input_block *ib, struct data_in *data_in,
		      tree expr)
{
  if (TREE_CODE (expr) == FUNCTION_DECL)
    DECL_SAVED_TREE (expr) = lto_input_tree (ib, data_in);
  else if (TREE_CODE (expr) == STATEMENT_LIST)
    {
      HOST_WIDE_INT i, num_trees = lto_input_sleb128 (ib);
      for (i = 0; i < num_trees; i++)
	{
	  tree stmt = lto_input_tree (ib, data_in);
	  append_to_statement_list (stmt, &expr);
	}
    }
}


/* Callback for unpacking value fields in ASTs.  BP is the bitpack 
   we are unpacking from.  EXPR is the tree to unpack.  */

void
pph_stream_unpack_value_fields (struct bitpack_d *bp ATTRIBUTE_UNUSED,
				tree expr ATTRIBUTE_UNUSED)
{
  /* Do nothing for now.  */
}
