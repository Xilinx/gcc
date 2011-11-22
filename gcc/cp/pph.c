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
#include "cxx-pretty-print.h"
#include "parser.h"
#include "pph-streamer.h"

/* Log file where PPH analysis is written to.  Controlled by
   -fpph_logfile.  If this flag is not given, stdout is used.  */
FILE *pph_logfile = NULL;

/* Convert a checked tree_code CODE to a string.  */

const char *
pph_tree_code_text (enum tree_code code)
{
  gcc_assert (code < MAX_TREE_CODES);
  return tree_code_name[code];
}


/* Dump a location LOC to FILE.  */

void
pph_dump_location (FILE *file, location_t loc)
{
  expanded_location xloc = expand_location (loc);
  fprintf (file, "%s:%d", xloc.file, xloc.line);
}

/* Dump identifying information for a minimal DECL to FILE.  */

void
pph_dump_min_decl (FILE *file, tree decl)
{
  print_generic_expr (file, DECL_NAME (decl), 0);
  print_generic_expr (file, DECL_CONTEXT (decl), 0);
  pph_dump_location (file, DECL_SOURCE_LOCATION (decl));
}

/* Dump a complicated name for tree T to FILE using FLAGS.
   See TDF_* in tree-pass.h for flags.  */

void
pph_dump_tree_name (FILE *file, tree t, int flags)
{
  enum tree_code code = TREE_CODE (t);
  const char *text = pph_tree_code_text (code);
  if (DECL_P (t))
    fprintf (file, "%s %s\n", text, decl_as_string (t, flags));
  else if (TYPE_P (t))
    fprintf (file, "%s %s\n", text, type_as_string (t, flags));
  else if (EXPR_P (t))
    fprintf (file, "%s %s\n", text, expr_as_string (t, flags));
  else
    {
      fprintf (file, "%s ", text );
      print_generic_expr (file, t, flags);
      fprintf (file, "\n");
    }
}


/* Dump namespace NS for PPH.  */

void
pph_dump_namespace (FILE *file, tree ns)
{
  fprintf (file, "namespace ");
  pph_dump_tree_name (file, ns, 0);
  fprintf (file, "{\n");
  pph_dump_binding (file, NAMESPACE_LEVEL (ns));
  fprintf (file, "}\n");
}


/* Dump cp_binding_level LEVEL for PPH.  */

void
pph_dump_binding (FILE *file, cp_binding_level *level)
{
  tree t, next;
  pph_dump_chain (file, level->names);
  for (t = level->namespaces; t; t = next)
    {
      next = DECL_CHAIN (t);
      if (!DECL_IS_BUILTIN (t))
        pph_dump_namespace (file, t);
    }
}


/* Dump a CHAIN for PPH.  */

void
pph_dump_chain (FILE *file, tree chain)
{
  tree t, next;
  for (t = chain; t; t = next)
    {
      next = DECL_CHAIN (t);
      if (!DECL_IS_BUILTIN (t))
        pph_dump_tree_name (file, t, 0);
    }
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


/* Record a #include or #include_next for PPH.
   READER is the main pre-processor object, LOC is the location where
   the #include is being emitted from, DNAME is the name of the
   #include directive used, NAME is the canonical name of the file being
   included, ANGLE_BRACKETS is non-zero if this #include uses <> and
   TOK_P is a pointer to the current token being pre-processed.  */

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

  if (flag_pph_tracer >= 1)
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
      pph_stream *include;

      /* Hack. We do this to mimic what the non-pph compiler does in
	_cpp_stack_include as our goal is to have identical line_tables.  */
      line_table->highest_location--;

      include = pph_read_file (pph_file);

      /* If we are generating a new PPH image, add the stream we just
	 read to the list of includes.   This way, the parser will be
	 able to resolve references to symbols in INCLUDE and its
	 children.  */
      if (pph_writer_enabled_p ())
	pph_writer_add_include (include);

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

  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Initializing.\n");

  /* Set up the libcpp handler for #include.  */
  cb = cpp_get_callbacks (parse_in);
  cb->include = pph_include_handler;

  table = cpp_lt_exchange (parse_in,
                           cpp_lt_create (cpp_lt_order, flag_pph_debug/2));
  gcc_assert (table == NULL);

  pph_streamer_init ();

  /* If we are generating a PPH file, initialize the writer.  */
  if (pph_writer_enabled_p ())
    pph_writer_init ();

  pph_reader_init ();
}


/* Finalize PPH support.  */

void
pph_finish (void)
{
  /* Finalize the streamer.  */
  pph_streamer_finish ();

  /* Close log files.  */
  if (flag_pph_tracer >= 1)
    fprintf (pph_logfile, "PPH: Finishing.\n");

  if (flag_pph_logfile)
    fclose (pph_logfile);
}


/* PPH include tree dumper.  Each entry in this file has the format:

	DEPTH|SYSP|DNAME|CANONICAL-NAME|FULL-NAME|PPH-NAME

  Where:
	DEPTH		is the include depth of the file.
	SYSP		1 for a system header
			2 for a C system header that needs 'extern "C"'
			0 otherwise.
	DNAME		name of the #include directive used.
	CANONICAL-NAME	is the name of the file as specified by the
			#include directive.
	FULL-NAME	is the full path name where the included file
			was found by the pre-processor.
	PPH-NAME	is the name of the associated PPH file.  */
typedef struct {
  /* Name of current #include directive.  */
  const unsigned char *dname;

  /* Canonical name of file being included.  */
  const char *name;

  /* Previous libcpp #include handler.  */
  void (*prev_file_change) (cpp_reader *, const struct line_map *);

  /* Previous libcpp file change handler.  */
  bool (*prev_include) (cpp_reader *, source_location, const unsigned char *,
		        const char *, int, const cpp_token **);
} pph_include_tree_dumper;

static pph_include_tree_dumper tree_dumper;


/* Return a copy of NAME with the characters '/' and '.' replaced with
   '_'.  The caller is reponsible for freeing the returned string.  */

static char *
flatten_name (const char *name)
{
  char *str = xstrdup (name);
  size_t i;

  for (i = 0; i < strlen (str); i++)
    if (str[i] == DIR_SEPARATOR || str[i] == '.')
      str[i] = '_';

  return str;
}


/* File change handler for libcpp.  READER is the main pre-processor object,
   MAP is the line map entry for the file that we are entering into.  */

static void
pph_file_change_handler (cpp_reader *reader, const struct line_map *map)
{
  char *flat;

  if (tree_dumper.prev_file_change)
    tree_dumper.prev_file_change (reader, map);

  /* We are only interested in line maps that describe a new file being
     entered.  */
  if (map == NULL || map->reason != LC_ENTER)
    return;

  /* Emit a line to the map file with the format:

	DEPTH|SYSP|DNAME|CANONICAL-NAME|FULL-NAME|PPH-NAME
  */
  flat = flatten_name (map->d.ordinary.to_file);
  fprintf (stderr, "%d|%d|%s|%s|%s|%s.pph\n", line_table->depth,
	   map->d.ordinary.sysp, tree_dumper.dname, tree_dumper.name,
	   map->d.ordinary.to_file, flat);
  free (flat);
  tree_dumper.dname = NULL;
  tree_dumper.name = NULL;
}


/* #include handler for libcpp.  READER is the main pre-processor object,
   LOC is the location where the #include is being emitted from, DNAME
   is the name of the #include directive used, NAME is the canonical
   name of the file being included, ANGLE_BRACKETS is non-zero if this
   #include uses <> and TOK_P is a pointer to the current token being
   pre-processed.  */

static bool
pph_include_handler_for_map (cpp_reader *reader,
			     location_t loc,
                             const unsigned char *dname,
                             const char *name,
                             int angle_brackets,
                             const cpp_token **tok_p)
{
  bool retval = true;

  if (tree_dumper.prev_include)
    retval &= tree_dumper.prev_include (reader, loc, dname, name,
					angle_brackets, tok_p);
  tree_dumper.dname = dname;
  tree_dumper.name = name;

  return retval;
}


/* Initialize the #include tree dumper.  */

void
pph_init_include_tree (void)
{
  cpp_callbacks *cb;

  memset (&tree_dumper, 0, sizeof (tree_dumper));

  if (pph_enabled_p ())
    fatal_error ("do not use -fpph-map-gen with any other PPH flag");

  /* Set up the libcpp handler for file change events.  Each event
     will generate a new entry in the map file.  */
  cb = cpp_get_callbacks (parse_in);

  tree_dumper.prev_file_change = cb->file_change;
  cb->file_change = pph_file_change_handler;

  tree_dumper.prev_include = cb->include;
  cb->include = pph_include_handler_for_map;
}
