/* Tree/CFG checking pass.
  Copyright (C) 2006 Free Software Foundation, Inc.
  Contributed by Nic Volanschi <nic.volanschi@free.fr>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "rtl.h"
#include "tm_p.h"
#include "hard-reg-set.h"
#include "basic-block.h"
#include "output.h"
#include "errors.h"
#include "flags.h"
#include "function.h"
#include "expr.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "timevar.h"
#include "tree-dump.h"
#include "tree-pass.h"
#include "toplev.h"
#include "tree-match.h"


/* Raise a warning upon detecting a satisfied condate.  The concept of
   condate (control & data property to be checked) is described in
   tree-match.h.  */

static void 
tree_check_warning (const char *condname, tree stmt, int check_option)
{
  location_t saved_location = input_location;

  if (EXPR_HAS_LOCATION (stmt))
    input_location = EXPR_LOCATION (stmt);

  warning (check_option, "user-defined check failed:");
  fprintf (stderr, "%s:%d: check = %s,\n", 
	   input_filename, input_line, condname);
  fprintf (stderr, "%s:%d: instance = ", input_filename, input_line);
  /*   print_local_holes (); */
  print_global_holes ();
  fprintf (stderr, ",\n");
  fprintf (stderr, "%s:%d: reached: ", input_filename, input_line); 
  print_generic_expr (stderr, stmt, 0);
  fprintf (stderr, ".\n");
  input_location = saved_location;
}

/* Initialization function for the tree-check pass.  */

static void 
tree_check_init (void) 
{
  reset_global_holes ();
}

/* Visit a CFG node.  Used in tree_check_instance.  */

static bool 
check_node (cfg_node node, condate cond) 
{
  tree stmt = cfg_node_stmt (node);

  if (!stmt || TREE_VISITED (stmt))
    return 0;

  TREE_VISITED (stmt) = 1;

  PP_TRACE (TRACE_CHECK_STEPS, {
      fprintf (stderr, "checking stmt:");
      print_generic_expr (stderr, stmt, 0);
      fprintf (stderr, "\n");
  });
    
  if (tree_match_disj (stmt, cond->to, node))
    {
      tree_check_warning (cond->name, stmt, OPT_ftree_checks_);
      return 0;  /* follow_none */
    }

  /* Inspect successors? */
  if (cond->avoid && tree_match_disj (stmt, cond->avoid, node))
    {
      PP_TRACE (TRACE_CHECK, fprintf (stderr, "via node, backtracking\n"));
      return 0; /* follow_none */;
    }
  else
    return 1; /* follow_all */
}

/* Check a condate instance over the CFG of the current function.  */

static void 
tree_check_instance (condate cond)
{
  VEC (cfg_node, heap) *stack = VEC_alloc (cfg_node, heap, 100);
  basic_block bb;
  cfg_node node;
  tree stmt;

  PP_TRACE (TRACE_CHECK, {
    fprintf (stderr, "checking condate instance:\n");
    print_global_holes ();
  });

  /* Push from nodes on the stack.  */
  PP_TRACE (TRACE_CHECK, fprintf (stderr, "searching src pat %s\n", 
				  cond->from->format_spec));

  FOR_EACH_BB (bb)
    {
      block_stmt_iterator bsi;
      tree stmt;

      for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
	{
	  stmt = bsi_stmt (bsi);
	  pattern patt = cond->from;

	  PP_TRACE (TRACE_MATCH, {
	    lazy_print_generic_expr (stderr, stmt, 0);
	    fprintf (stderr, "= ");
	    print_generic_expr (stderr, stmt, 0); 
	    fprintf (stderr, "\n");
	  });

	  if (!patt || tree_match_disj (stmt, patt, bsi_cfg_node (bsi)))
	    {
	      node = bsi_cfg_node (bsi);
	      stmt = cfg_node_stmt (node);

	      VEC_safe_push (cfg_node, heap, stack, node);

	      if (stmt) 
		TREE_VISITED (stmt) = 1;

	      PP_TRACE (TRACE_CHECK_STEPS, {
		fprintf (stderr, "found src stmt:");
		print_generic_expr (stderr, stmt, 0);
		fprintf (stderr, "\n");
	      });
	    }
	}
    }

  PP_TRACE (TRACE_CHECK, fprintf (stderr, "%d src stmts found\n", 
				  (unsigned) VEC_length (cfg_node, stack)));

  /* Perform depth-first search.  */
  while (VEC_length (cfg_node, stack) != 0)
    {
      cfg_node succ_node;
      bool push_it;
    
      node = VEC_pop (cfg_node, stack);
      stmt = cfg_node_stmt (node);

      if (node->next == NULL)
	{
	  edge e;
	  edge_iterator ei;

	  bb = bb_for_stmt (stmt);

	  FOR_EACH_EDGE (e, ei, bb->succs)
	    {
	      if (e->dest == EXIT_BLOCK_PTR) 
		continue;

	      if (TREE_CODE (stmt) == COND_EXPR
		  && (e->flags & EDGE_TRUE_VALUE && cond->avoid_then
		      && tree_match_disj (COND_EXPR_COND (stmt), cond->avoid_then, 
					  node)))
		{
		  PP_TRACE (TRACE_CHECK, 
			    fprintf (stderr, "via-then edge, skipping\n"));
		  continue;
		}

	      if (TREE_CODE (stmt) == COND_EXPR
		  && (e->flags & EDGE_FALSE_VALUE && cond->avoid_else
		      && tree_match_disj (COND_EXPR_COND (stmt), cond->avoid_else, 
					  node)))
		{
		  PP_TRACE (TRACE_CHECK, 
			    fprintf (stderr, "via-else edge, skipping\n"));
		  continue;
		}

	      succ_node = bb_1st_cfg_node (e->dest);
	      push_it = check_node (succ_node, cond);

	      if (push_it)
		VEC_safe_push (cfg_node, heap, stack, succ_node);
	    }
	}
      else
	{
	  succ_node = node->next;
	  push_it = check_node (succ_node, cond);

	  if (push_it)
	    VEC_safe_push (cfg_node, heap, stack, succ_node);
	}
    }

  VEC_free (cfg_node, heap, stack);
}

/* Collect new condate instances.  An instance is new if the
   combination of global hole values has not been seen yet.  */

static void
push_global_holes_if_new (VEC (hole_p, heap) *stack)
{
  unsigned int i;
  hole_p h;

  /* Check if these global holes were already seen.  */
  for (i = 0; VEC_iterate (hole_p, stack, i, h); i++)
    if (eq_global_holes (global_holes, h))
      {
	reset_global_holes ();
	return;
      }

  VEC_safe_push (hole_p, heap, stack, save_global_holes ());
  reset_global_holes ();
}

/* Check a condate on a function.  */

static void 
tree_check (condate cond)
{
  /* Allocate stack for collecting condate instances.  */
  VEC (hole_p, heap) *stack = VEC_alloc (hole_p, heap, 10);
  pattern patt = cond->from;
  basic_block bb;
  
  PP_TRACE (TRACE_CHECK, 
	    fprintf (stderr, "searching src pat %s\n", 
		     patt->format_spec));

  FOR_EACH_BB (bb)
    {
      block_stmt_iterator bsi;
      tree stmt;

      for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
	{
	  stmt = bsi_stmt (bsi);

	  PP_TRACE (TRACE_MATCH, {
	    lazy_print_generic_expr (stderr, stmt, 0);
	    fprintf (stderr, "= ");
	    print_generic_expr (stderr, stmt, 0); 
	    fprintf (stderr, "\n");
	  });

	  if (!patt || tree_match_disj (stmt, patt, bsi_cfg_node (bsi)))
	    push_global_holes_if_new (stack);
	}
    }

  PP_TRACE (TRACE_CHECK, fprintf (stderr, "%d condate instances found\n", 
				  VEC_length (hole_p, stack)));

  while (VEC_length (hole_p, stack))
    {
      hole_p h = VEC_pop (hole_p, stack);

      restore_global_holes (h);
      tree_check_instance (cond);
      PP_TRACE (TRACE_CHECK, fprintf (stderr, "recounting stmts\n"));
      tree_check_init (); /* clear visited flag */
    }

  VEC_free (hole_p, heap, stack);
}

/* Read from a file a string delimted by double quotes.  */

static char *
read_delimited_string (FILE *infile) 
{
  static char buf[256];
  int c, buf_sp;

  /* lookahead(1), to skip comment lines */
  while ((c = getc (infile)) == '#')
    {
      /* skip to \n */
      while ((c = getc (infile)) != '\n' && c != EOF);
    }

  ungetc (c, infile);

  /* skip to opening \" */
  while ((c = getc (infile)) != '"' && c != '\n' && c != EOF); 

  if (c == '\n' || c == EOF) 
    return NULL; /* no string found */

  /* fill in string contents */
  buf_sp = 0;
  while ((c = getc (infile)) != '"' && c != '\n' && c != EOF)
    buf[buf_sp++] = c;

  if (c == '\n' || c == EOF) 
    return NULL; /* unclosed string */

  /* end string */
  buf[buf_sp] = '\0';
  return buf;
}

/* Print a condate.  */

void 
print_cond (condate cond) 
{
  fprintf (stderr, "check(");
  pat_print (cond->from);
  fprintf (stderr, ", ");
  pat_print (cond->to);
  fprintf (stderr, ", ");
  pat_print (cond->avoid);
  fprintf (stderr, ", ");
  pat_print (cond->avoid_then);
  fprintf (stderr, ", ");
  pat_print (cond->avoid_else);
  fprintf (stderr, ")\n");
}

/* Check a list of condates on the current function.  */

static void 
execute_conds (condate conds[], int n) 
{
  int i;
  condate cond;

  for (i = 0; i < n; i++)
    {
      cond = conds[i];
      PP_TRACE (TRACE_CHECK, {
	print_cond (cond);
      });

    tree_check (cond);
  }
}

#define CONDMAX 100
static condate conds[CONDMAX];  /* list of condated to check */
static int n_conds = 0;         /* number of condates to check */

/* Flush the list of condates.  */

static void 
delete_conds (condate conds[], int n) 
{
  int i;
  condate cond;

  for (i = 0; i < n; i++)
    {
      cond = conds[i];
      rmcond (cond);
    }
  n_conds = 0;
}

/* Parse the file containing condates definitions, and cache the result.  */

static int 
parse_tree_check_file_once (void) 
{
  static const char *current_check_file = NULL;
  static char *str;
  static pattern from, to, avoid, avoid_then, avoid_else;
  static char *name;
  FILE *checkfile;
  
  if (current_check_file)
    {
      /* Not called for the first time.  */
      if (!strcmp (current_check_file, tree_check_file)) 
	/* file hasn't changed */
	return 0;
      else
	delete_conds (conds, n_conds);
    }

  current_check_file = tree_check_file;
  checkfile = fopen (tree_check_file, "r");

  if (!checkfile)
    return -1;

  while (1) 
    {
      from = to = avoid = avoid_then = avoid_else = NULL;

      while ((str = read_delimited_string (checkfile)) != NULL)
	from = pat_or (mkpat (str), from);

      if (!from)
	break;

      while ((str = read_delimited_string (checkfile)) != NULL)
	to = pat_or (mkpat (str), to);

      while ((str = read_delimited_string (checkfile)) != NULL)
	avoid = pat_or (mkpat (str), avoid);

      while ((str = read_delimited_string (checkfile)) != NULL)
	avoid_then = pat_or (mkpat (str), avoid_then);

      while ((str = read_delimited_string (checkfile)) != NULL)
	avoid_else = pat_or (mkpat (str), avoid_else);

      name = xmalloc (strlen (tree_check_file) + 6);
      strcpy (name, tree_check_file);
      sprintf (name + strlen (tree_check_file), "[%03d]", n_conds);
      conds[n_conds++] = mkcond (name, from, to, avoid, 
			       avoid_then, avoid_else);
      free (name);
      if (n_conds == CONDMAX)
	{
	  fprintf (stderr, "Warning: ignoring checks beyond %d", CONDMAX);
	  break;
	}
    }

  return 0;
}

/* Main function of the tree-check pass.  Triggered either by
   -ftree-check or -ftree-checks.  */

static unsigned int
execute_tree_check (void) 
{
  const char *fn_name = IDENTIFIER_POINTER (DECL_NAME (current_function_decl));

  PP_TRACE (TRACE_CHECK, fprintf (stderr, "function %s() {\n", fn_name));
  PP_TRACE (TRACE_CHECK, 
	   fprintf (stderr, 
		    "Executing tree reachability checks: file=%s, string=%s\n",
		    tree_check_file, tree_check_string));
  PP_TRACE (TRACE_CHECK, fprintf (stderr, "counting stmts\n"));
  tree_check_init ();

  if (tree_check_file)
    {
      if (parse_tree_check_file_once () < 0)
	{
	  fprintf (stderr, "tree-check-file %s not found\n", tree_check_file);
	  return 0;
	}
      execute_conds (conds, n_conds);
    }
  else
    {
      /* tree_check_string != NULL */
      basic_block bb;
      pattern patt = mkpat (tree_check_string);

      reset_global_holes ();

      FOR_EACH_BB (bb)
	{
	  block_stmt_iterator bsi;
	  tree stmt;

	  for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
	    {
	      stmt = bsi_stmt (bsi);

	      PP_TRACE (TRACE_MATCH, {
		lazy_print_generic_expr (stderr, stmt, 0);
		fprintf (stderr, "= ");
		print_generic_expr (stderr, stmt, 0); 
		fprintf (stderr, "\n");
	      });

	      if (!patt || tree_match_disj (stmt, patt, bsi_cfg_node (bsi)))
		{
		  tree_check_warning (tree_check_string,
				      cfg_node_stmt (bsi_cfg_node (bsi)),
				      OPT_ftree_check_);
		  reset_global_holes ();
		}
	    }
	}

      rmpat (patt);
    }

  PP_TRACE (TRACE_CHECK, fprintf (stderr, "}\n"));
  return 0;
}

static bool
gate_tree_check (void)
{
  return ((tree_check_file != 0 || tree_check_string != 0)
	  && basic_block_info != 0);
}

struct tree_opt_pass pass_check =
{
  "check",				/* name */
  gate_tree_check,			/* gate */
  execute_tree_check,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  TV_TREE_CHECK,			/* tv_id */
  PROP_cfg, /* | PROP_ssa, */			/* properties_required */
  0,					/* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  0,					/* todo_flags_finish */
  0					/* letter */
};
