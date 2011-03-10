/* gccexpr.c - A C/C++ compiler plug-in to parse expressions
   Copyright (C) 2011 Red Hat, Inc.
   Written by Keith Seitz  <keiths@redhat.com>

   This file is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   The program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* This source file actually is used to build two GCC plug-ins, one
   for C code (which is a GCC plug-in) and one for C++ code (which is
   a G++ plug-in).  The accompanying Makefile will build both of these
   plug-ins, named "cexpr" and "cpexpr".  Enable  either plug-in by passing
   its name as the value to "-fplugin" argument.  Then pass a location
   (FILENAME:NUMBER) via "-fplugin-arg-c[p]expr-location" and
   an expression to parse via "-fplugin-arg-c[p]expr-expr".

   NOTE: The location is defined as the end (or sometimes middle)
   of the line. To parse an expression before the given line is
   compiled, you must specify the previous line.

   How to call this plug-in (because I always forget):
 
   In objdir for the file:
   $ g{++,cc} -c -o /dev/null -fplugin=/path/to/c[p]expr	\
     -fplugin-arg-c[p]expr-location="<sourcefile>.<line>"	\
     -fplugin-arg-c[p]expr-expr="<expression>"			\
     [all other build flags, defines, etc] filename

   If you install the plug-in into your GCC installdir, you can omit
   the "/path/to" bits and simply specify "-fplugin=c[p]expr".
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "gcc-plugin.h"
#include "plugin-version.h"
#include "cpplib.h"
#include "coretypes.h"
#include "toplev.h"
#include "input.h"
#include "c-pragma.h"

#ifdef CPLUS
#include "cp/cp-tree.h"
#include "c-pretty-print.h"
extern tree cp_parser_evaluate_expression (const char *);
#else
#include "tree.h"
#include "c-pretty-print.h"
#include "diagnostic.h"
extern tree c_parser_evaluate_expression (const char *);
#endif

#define DEBUGGING 0

#if DEBUGGING
#define DEBUG(FMT, ARGS...)			\
  do						\
    {						\
      printf ("EXPRPLUGIN " FMT "\n", ##ARGS);	\
    }						\
  while (0)
#else
#define DEBUG(FMT, ARGS...) /* nadda */
#endif

/* Freedom!  */
int plugin_is_GPL_compatible;

/* Plug-in information  */
static struct plugin_info gccexpr_info =
  { "0.01", "expression parsing plug-in" };

#define PLUGIN_ARGV_LOCATION "location"
#define PLUGIN_ARGV_EXPRESSION "expr"

/* User data for the PLUGIN_PARSE_LINE callback.  */

struct gccexpr_data
{
  /* The location at which to halt parsing.

     Note that this location must be the line previous
     to where we want expression parsing done.  GDB will have
     to figure this out for us.  */
  expanded_location loc;

  /* The expression to parse when we stop.  */
  char *expr;

  /* Set to TRUE if this expression has already failed parsing
     and should be attempted at exit.  */
  bool at_exit;

  /* Timer variable for start of run.  */
  long start_time;
};

/* Outputs the given TREE in a meaningful way to the terminal.  */

static void
output_tree (tree tree)
{
  /* For now, simply call debug_tree.  */
  debug_tree (tree);  
}

/* A wrapper function to call the correct version of the
   parser's expression parser.  */

static tree
parse_expression (const char *expr)
{
#ifdef CPLUS
  return cp_parser_evaluate_expression (expr);
#else
  return c_parser_evaluate_expression (expr);
#endif
}

/* Print out time statistics.  */

static void
print_time_usage (const char *str, struct gccexpr_data *data)
{
  long now = clock ();
  fprintf (stderr, "time to parse %s %.2f seconds\n", str,
	   (float) (now - data->start_time) / CLOCKS_PER_SEC);
}

/* Print a usage message.  */

static void
print_usage (struct plugin_name_args *info)
{
  fprintf (stderr,
	   "%s: usage: -fplugin=%s -fplugin-arg-%s-%s=<file>:<line> "
	   "-fplugin-arg-%s-%s=<expression>\n", info->base_name,
	   info->base_name,
	   info->base_name, PLUGIN_ARGV_LOCATION,
	   info->base_name, PLUGIN_ARGV_EXPRESSION);
}

/* PLUGIN_PARSE_LINE callback function.  */

static void
gccexpr_parse_line_callback (void *gcc_data, void *user_data)
{
  struct gccexpr_data *data = (struct gccexpr_data *) user_data;
  expanded_location loc = expand_location (input_location);

  DEBUG ("parse_line_cb: %s:%d", loc.file, loc.line);

  if (loc.file && !data->at_exit)
    {
      /* FIXME: lbasename? Could this be asking for trouble? */
      if (strcmp (lbasename (loc.file), data->loc.file) == 0
	  && loc.line >= data->loc.line)
	{
	  tree result;

	  DEBUG ("parsinging expression %s", data->expr);

	  /* Parse the expression.  */
	  result = parse_expression (data->expr);

	  /* If we see an error, try again at exit.  */
	  if (result != error_mark_node)
	    {
	      /* Print out the resulting tree.  */
	      output_tree (result);
	      print_time_usage ("in-line expression", data);
	      exit (0);
	    }
	  else
	    {
	      DEBUG ("parsing failed -- deferring");
	      data->at_exit = true;
	    }
	}
    }
}

/* This function is called just before GCC exits.  It is responsible
   for freeing any resources used by this plugin.  */

static void
gccexpr_finish_callback (void *gcc_data, void *user_data)
{
  struct gccexpr_data *data = (struct gccexpr_data *) user_data;

  DEBUG ("finish_callback");
  if (data->at_exit)
    {
      tree result;

      /* Try again.  */
      result = parse_expression (data->expr);
      if (result != error_mark_node)
	{
	  output_tree (result);
	  print_time_usage ("expression at exit", data);
	  exit (0);
	}
    }

  print_time_usage ("parsing failed expression", data);
  free (data->expr);
  free (data);
}

/* Nop pretty-printer to supress diagnostics.  */

static void
gccexpr_pretty_print (c_pretty_printer *pp, tree t)
{
  /* do nothing */
}

static void
gccexpr_init_diagnostics (diagnostic_context *context)
{
  static c_pretty_printer nop_printer;
  static bool initialized = false;
  c_pretty_printer *pp = &nop_printer;

  pp_construct (pp_base (pp), NULL, 0);
  pp->offset_list               = 0;

  pp->declaration               = gccexpr_pretty_print;
  pp->declaration_specifiers    = gccexpr_pretty_print;
  pp->declarator                = gccexpr_pretty_print;
  pp->direct_declarator         = gccexpr_pretty_print;
  pp->type_specifier_seq        = gccexpr_pretty_print;
  pp->abstract_declarator       = gccexpr_pretty_print;
  pp->direct_abstract_declarator = gccexpr_pretty_print;
  pp->ptr_operator              = gccexpr_pretty_print;
  pp->parameter_list            = gccexpr_pretty_print;
  pp->type_id                   = gccexpr_pretty_print;
  pp->simple_type_specifier     = gccexpr_pretty_print;
  pp->function_specifier        = gccexpr_pretty_print;
  pp->storage_class_specifier   = gccexpr_pretty_print;

  pp->statement                 = gccexpr_pretty_print;

  pp->constant                  = gccexpr_pretty_print;
  pp->id_expression             = gccexpr_pretty_print;
  pp->primary_expression        = gccexpr_pretty_print;
  pp->postfix_expression        = gccexpr_pretty_print;
  pp->unary_expression          = gccexpr_pretty_print;
  pp->initializer               = gccexpr_pretty_print;
  pp->multiplicative_expression = gccexpr_pretty_print;
  pp->conditional_expression    = gccexpr_pretty_print;
  pp->assignment_expression     = gccexpr_pretty_print;
  pp->expression                = gccexpr_pretty_print;

  context->printer = (pretty_printer *) pp;
}

/* Plug-in initialization function  */

int
plugin_init (struct plugin_name_args *plugin_info,
	     struct plugin_gcc_version *version)
{
  int i, expr_event;
  const char *p;
  const char *location = NULL;
  char *expression = NULL;
  expanded_location eloc;
  struct gccexpr_data *data;

  if (!plugin_default_version_check (version, &gcc_version))
    return 1;

  DEBUG ("plugin_init");

  /* We must have two parameters, the location and the expression.  */
  if (plugin_info->argc != 2)
    {
      print_usage (plugin_info);
      return 1;
    }

  /* Extract the parse location and expression from the arguments.  */
  for (i = 0; i < plugin_info->argc; ++i)
    {
      if (strcmp (PLUGIN_ARGV_LOCATION, plugin_info->argv[i].key) == 0)
	location = plugin_info->argv[i].value;
      else if (strcmp (PLUGIN_ARGV_EXPRESSION, plugin_info->argv[i].key) == 0)
	{
	  /* The lexer requires that we end in a newline; so append it.  */
	  expression = xmalloc (strlen (plugin_info->argv[i].value) + 3);
	  strcpy (expression, plugin_info->argv[i].value);
	  strcat (expression, "\n");
	}
      else
	{
	  fprintf (stderr, "%s: unknown argument key: \"%s\"\n",
		   plugin_info->base_name, plugin_info->argv[i].key);
	  print_usage (plugin_info);
	  return 1;
	}
    }

  /* If either LOCATION or EXPRESSION was not specified, issue an
     error message and fail initialization.  */
  if (location == NULL || expression == NULL)
    {
      fprintf (stderr, "%s: must specify both -fplugin-args-%s-%s and "
	       "-fplugin-args-%s-%s\n", plugin_info->base_name,
	       plugin_info->base_name, PLUGIN_ARGV_LOCATION,
	       plugin_info->base_name, PLUGIN_ARGV_EXPRESSION);
      return 1;
    }

  /* Parse the location information, which should be of the form
     "FILENAME:LINE".  */
  
  p = location;
  while (*p != ':' && *p != '\0')
    ++p;

  if (*p == '\0')
    {
      /* We ran to the end of the string without finding ':'.  */
      fprintf (stderr, "%s: error in location, expected \"<file>:<line>\"\n",
	       plugin_info->base_name);
      return 1;
    }

  data = XNEW (struct gccexpr_data);
  data->expr = expression;
  data->at_exit = false;
  eloc.file = strndup (location, p - location);
  errno = 0;
  eloc.line = strtoul (++p, (char **) NULL, 10);
  if (errno != 0)
    {
      char buf[256];
      fprintf (stderr, ":error converting line number: %s\n",
	       plugin_info->base_name, strerror_r (errno, buf, sizeof (buf)));
      return 1;

    }
  else if (eloc.line < 0)
    {
      fprintf (stderr, "%s: line number must be >= 0\n",
	       plugin_info->base_name);
      return 1;
    }

  DEBUG ("location: %s:%d\n", eloc.file, eloc.line);

  data->loc = eloc;

  /* Register our plugin info  */
  register_callback (plugin_info->base_name, PLUGIN_INFO,
		     (plugin_callback_func) NULL, &gccexpr_info);

  /* Register a callback for the PLUGIN_PARSE_LINE event.  */
  register_callback (plugin_info->base_name, PLUGIN_PARSE_LINE,
		     gccexpr_parse_line_callback, data);

  /* Register a finish event callback to handle any cleanup.  */
  register_callback (plugin_info->base_name, PLUGIN_FINISH,
		     gccexpr_finish_callback, data);

  /* From here on out, we don't want diagnostic information (warnings
     and errors) output to the terminal, so install a blank pretty
     printer which effectively disables this.  */
  /* NOTE: This does not seem sufficient... */
  gccexpr_init_diagnostics (global_dc);

#ifdef CPLUS
  /* Defer all access checks, too.  */
  push_deferring_access_checks (dk_no_check);
#endif

  /* Set the timer.  */
  data->start_time = clock ();
  return 0;
}
