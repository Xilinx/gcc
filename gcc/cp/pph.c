/* Factored pre-parsed header (PPH) support for C++
   Copyright (C) 2010, 2011 Free Software Foundation, Inc.
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "pph.h"
#include "cpplib.h"
#include "toplev.h"
#include "tree.h"
#include "cp-tree.h"
#include "timevar.h"
#include "pointer-set.h"
#include "fixed-value.h"
#include "md5.h"
#include "tree-pass.h"
#include "tree-dump.h"
#include "tree-inline.h"
#include "tree-pretty-print.h"
#include "parser.h"
#include "pph-streamer.h"

/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
FILE *pph_logfile = NULL;


/* Dump a complicated name for tree T to FILE using FLAGS.
   See TDF_* in tree-pass.h for flags.  */

void
pph_dump_tree_name (FILE *file, tree t, int flags)
{
  enum tree_code code = TREE_CODE (t);
  fprintf (file, "%s\t", tree_code_name[code]);
  if (code == FUNCTION_TYPE || code == METHOD_TYPE)
    {
      dump_function_to_file (t, file, flags);
    }
  else
    {
      print_generic_expr (file, TREE_TYPE (t), flags);
      /* FIXME pph: fprintf (file, " ", cxx_printable_name (t, 0)); */
      fprintf (file, " " );
      print_generic_expr (file, t, flags);
    }
  fprintf (file, "\n");
}


/* Dump namespace NS for PPH.  */

void
pph_dump_namespace (FILE *file, tree ns)
{
  cp_binding_level *level;
  tree t, chain;
  level = NAMESPACE_LEVEL (ns);

  fprintf (file, "namespace ");
  print_generic_expr (file, ns, 0);
  fprintf (file, " {\n");
  for (t = level->names; t; t = chain)
    {
      chain = DECL_CHAIN (t);
      if (!DECL_IS_BUILTIN (t))
        pph_dump_tree_name (file, t, 0);
    }
  for (t = level->namespaces; t; t = chain)
    {
      chain = DECL_CHAIN (t);
      if (!DECL_IS_BUILTIN (t))
        pph_dump_namespace (file, t);
    }
  fprintf (file, "}\n");
}


/* Return true if PPH image NAME can be used at the point of inclusion
   (given by LOC).  */

static bool
pph_is_valid_here (const char *name, location_t loc)
{
  /* If we are inside a scope, reject the image.  We could be inside a
     namespace or a structure which changes the parsing context for
     the original text file.  */
  if (scope_chain->x_brace_nesting > 0)
    {
      error_at (loc, "PPH file %s not included at global scope", name);
      return false;
    }

  return true;
}


/* Record a #include or #include_next for PPH.  */

static bool
pph_include_handler (cpp_reader *reader,
                     location_t loc,
                     const unsigned char *dname,
                     const char *name,
                     int angle_brackets,
                     const cpp_token **tok_p ATTRIBUTE_UNUSED)
{
  const char *pph_file;
  bool read_text_file_p;

  if (flag_pph_debug >= 1)
    {
      fprintf (pph_logfile, "PPH: #%s", dname);
      fprintf (pph_logfile, " %c", angle_brackets ? '<' : '"');
      fprintf (pph_logfile, "%s", name);
      fprintf (pph_logfile, "%c\n", angle_brackets ? '>' : '"');
    }

  read_text_file_p = true;
  pph_file = query_pph_include_map (name);
  if (pph_file != NULL
      && pph_is_valid_here (name, loc)
      && !cpp_included_before (reader, name, input_location))
    {
      /* Hack. We do this to mimic what the non-pph compiler does in
	_cpp_stack_include as our goal is to have identical line_tables.  */
      line_table->highest_location--;

      pph_read_file (pph_file);
      read_text_file_p = false;
    }

  return read_text_file_p;
}


/* The initial order of the size of the lexical lookaside table,
   which will accomodate as many as half of its slots in use.  */

static const unsigned int cpp_lt_order = /* 2 to the power of */ 9;

/* Initialize PPH support.  */

void
pph_init (void)
{
  cpp_callbacks *cb;
  cpp_lookaside *table;

  if (flag_pph_logfile)
    {
      pph_logfile = fopen (flag_pph_logfile, "w");
      if (!pph_logfile)
	fatal_error ("Cannot create %s for writing: %m", flag_pph_logfile);
    }
  else
    pph_logfile = stdout;

  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Initializing.\n");

  /* Set up the libcpp handler for #include.  */
  cb = cpp_get_callbacks (parse_in);
  cb->include = pph_include_handler;

  table = cpp_lt_exchange (parse_in,
                           cpp_lt_create (cpp_lt_order, flag_pph_debug));
  gcc_assert (table == NULL);

  /* If we are generating a PPH file, initialize the writer.  */
  if (pph_out_file != NULL)
    pph_writer_init ();

  pph_init_preloaded_cache ();

  pph_read_images = NULL;
}


/* Finalize PPH support.  */

void
pph_finish (void)
{
  /* Finalize the writer.  */
  pph_writer_finish ();

  /* Finalize the reader.  */
  pph_reader_finish ();

  /* Close log files.  */
  if (flag_pph_debug >= 1)
    fprintf (pph_logfile, "PPH: Finishing.\n");

  if (flag_pph_logfile)
    fclose (pph_logfile);
}
