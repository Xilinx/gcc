/* Gimple Represented as Polyhedra.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Contributed by Alexandru Plesco <shurikx@gmail.com>
   and Sebastian Pop <pop@cri.ensmp.fr>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

/* This pass converts GIMPLE to GRAPHITE, performs some loop
   transformations and then converts the resulting representation back
   to GIMPLE.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "domwalk.h"
#include "graphite.h"


VEC (scop_p, heap) *current_scops;

/* Print SCOP to FILE.  */

static void
print_scop (FILE *file, scop_p scop)
{
  fprintf (file, "\nSCoP (\n");
  fprintf (file, "  entry = bb_%d\n", scop->entry->index);
  fprintf (file, "  exit = bb_%d\n", scop->exit->index);
  fprintf (file, ")\n");
}

/* Print all the SCOPs to FILE.  */

static void
print_scops (FILE *file)
{
  unsigned i;
  scop_p scop;

  for (i = 0; VEC_iterate (scop_p, current_scops, i, scop); i++)
    print_scop (file, scop);
}

/* Debug SCOP.  */

void
debug_scop (scop_p scop)
{
  print_scop (stderr, scop);
}

/* Debug all SCOPs from CURRENT_SCOPS.  */

void 
debug_scops (void)
{
  print_scops (stderr);
}

/* Return true when EXPR is an affine function in LOOP.  */

static bool
affine_expr (struct loop *loop, tree expr)
{
  tree scev = analyze_scalar_evolution (loop, expr);

  scev = instantiate_parameters (loop, scev);

  return (evolution_function_is_affine_multivariate_p (scev)
	  || evolution_function_is_constant_p (scev));
}


/* Return true only when STMT is simple enough for being handled by
   GRAPHITE.  */

static bool
stmt_simple_for_scop_p (tree stmt)
{
  struct loop *loop = bb_for_stmt (stmt)->loop_father;

  /* ASM_EXPR and CALL_EXPR may embed arbitrary side effects.
     Calls have side-effects, except those to const or pure
     functions.  */
  if ((TREE_CODE (stmt) == CALL_EXPR
       && !(call_expr_flags (stmt) & (ECF_CONST | ECF_PURE)))
      || (TREE_CODE (stmt) == ASM_EXPR
	  && ASM_VOLATILE_P (stmt)))
    return false;

  switch (TREE_CODE (stmt))
    {
    case LABEL_EXPR:
      return true;

    case COND_EXPR:
      {
	tree opnd0 = TREE_OPERAND (stmt, 0);

	switch (TREE_CODE (opnd0))
	  {
	  case NE_EXPR:
	  case EQ_EXPR:
	  case LT_EXPR:
	  case GT_EXPR:
	  case LE_EXPR:
	  case GE_EXPR:
	    return (affine_expr (loop, TREE_OPERAND (opnd0, 0)) 
		    && affine_expr (loop, TREE_OPERAND (opnd0, 1)));
	  default:
	    return false;
	  }
      }

    case MODIFY_EXPR:
      {
	tree opnd0 = TREE_OPERAND (stmt, 0);
	tree opnd1 = TREE_OPERAND (stmt, 1);

	if (TREE_CODE (opnd0) == ARRAY_REF 
	     || TREE_CODE (opnd0) == INDIRECT_REF
	     || TREE_CODE (opnd0) == COMPONENT_REF)
	  {
	    if (!create_data_ref (opnd0, stmt, false))
	      return false;

	    if (TREE_CODE (opnd1) == ARRAY_REF 
		|| TREE_CODE (opnd1) == INDIRECT_REF
		|| TREE_CODE (opnd1) == COMPONENT_REF)
	      {
		if (!create_data_ref (opnd1, stmt, true))
		  return false;

		return true;
	      }
	  }

	if (TREE_CODE (opnd1) == ARRAY_REF 
	     || TREE_CODE (opnd1) == INDIRECT_REF
	     || TREE_CODE (opnd1) == COMPONENT_REF)
	  {
	    if (!create_data_ref (opnd1, stmt, true))
	      return false;

	    return true;
	  }

	/*
	  We cannot return (affine_expr (loop, opnd0) &&
	   affine_expr (loop, opnd1)) because D.1882_16 is not affine
	   in the following:

	   D.1881_15 = a[j_13][pretmp.22_20];
	   D.1882_16 = D.1881_15 + 2;
	   a[j_22][i_12] = D.1882_16;

	   but this is valid code in a scop.

	   FIXME: I'm not yet 100% sure that returning true is safe:
	   there might be exponential scevs.  On the other hand, if
	   these exponential scevs do not reference arrays, then
	   access functions, domains and schedules remain affine.  So
	   it is very well possible that we can handle this.
	*/
	return true;
      }

    case CALL_EXPR:
      {
	tree args;

	for (args = TREE_OPERAND (stmt, 1); args; args = TREE_CHAIN (args))
	  if ((TREE_CODE (TREE_VALUE (args)) == ARRAY_REF
	       || TREE_CODE (TREE_VALUE (args)) == INDIRECT_REF
	       || TREE_CODE (TREE_VALUE (args)) == COMPONENT_REF)
	      && !create_data_ref (TREE_VALUE (args), stmt, true))
	    return false;

	return true;
      }

    case RETURN_EXPR:
    default:
      /* These nodes cut a new scope.  */
      return false;
    }

  return false;
}

/* This function verify if a basic block contains simple statements,
   returns true if a BB contains only simple statements.  */

static bool
basic_block_simple_for_scop_p (basic_block bb)
{
  block_stmt_iterator bsi;

  for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
    if (!stmt_simple_for_scop_p (bsi_stmt (bsi)))
      return false;

  return true;
}

static scop_p down_open_scop;

/* Find the first basic block that dominates BB and that exits the
   current loop.  */

static basic_block
get_loop_start (basic_block bb)
{
  basic_block res = bb;
  int depth;

  do {
    res = get_immediate_dominator (CDI_DOMINATORS, res);
    depth = res->loop_father->depth;
    } while (depth != 0 && depth >= bb->loop_father->depth);

  return res;
}


/* Build the SCoP ending with BB.  */

static void
end_scop (basic_block bb)
{
  scop_p new_scop;
  basic_block loop_start = get_loop_start (bb);

  if (dominated_by_p (CDI_DOMINATORS, down_open_scop->entry, loop_start))
    {
      down_open_scop->exit = bb;
      VEC_safe_push (scop_p, heap, current_scops, down_open_scop);
      return;
    }

  new_scop = XNEW (struct scop);
  new_scop->entry = loop_start;
  new_scop->exit = bb;
  end_scop (loop_start);
  VEC_safe_push (scop_p, heap, current_scops, new_scop);
}

/* Mark difficult constructs as boundaries of SCoPs.  Callback for
   walk_dominator_tree.  */

static void
test_for_scop_bound (struct dom_walk_data *dw_data ATTRIBUTE_UNUSED,
		     basic_block bb)
{
  if (bb == ENTRY_BLOCK_PTR)
    return;

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "down bb_%d\n", bb->index);

  /* Exiting the loop containing the open scop ends the scop.  */
  if (down_open_scop->entry->loop_father->depth > bb->loop_father->depth)
    {
      down_open_scop->exit = bb;
      VEC_safe_push (scop_p, heap, current_scops, down_open_scop);

      /* Then we begin a new scop at this block.  */
      down_open_scop = XNEW (struct scop);
      down_open_scop->entry = bb;

      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "dom bound bb_%d\n\n", bb->index);

      return;
    }

  if (!basic_block_simple_for_scop_p (bb))
    {
      /* A difficult construct ends the scop.  */
      end_scop (bb);

      /* Then we begin a new scop at this block.  */
      down_open_scop = XNEW (struct scop);
      down_open_scop->entry = bb;

      if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file, "difficult bound bb_%d\n\n", bb->index);

      return;
    }
}

/* Find static control parts in LOOPS, and save these in the
   CURRENT_SCOPS vector.  */

static void
build_scops (void)
{
  struct dom_walk_data walk_data;

  down_open_scop = XNEW (struct scop);
  down_open_scop->entry = ENTRY_BLOCK_PTR;

  memset (&walk_data, 0, sizeof (struct dom_walk_data));
  walk_data.before_dom_children_before_stmts = test_for_scop_bound;

  init_walk_dominator_tree (&walk_data);
  walk_dominator_tree (&walk_data, ENTRY_BLOCK_PTR);
  fini_walk_dominator_tree (&walk_data);

  /* End the last open scop.  */
  down_open_scop->exit = EXIT_BLOCK_PTR;
  VEC_safe_push (scop_p, heap, current_scops, down_open_scop);
}

/* Build the domains for each loop in the SCOP.  */

static void
build_domains (scop_p scop ATTRIBUTE_UNUSED)
{
  
}


/* Build a schedule for each statement in the SCOP.  */

static void
build_scattering_functions (scop_p scop ATTRIBUTE_UNUSED)
{
  
}


/* Find the right transform for the SCOP.  */

static void
graphite_find_transform (scop_p scop ATTRIBUTE_UNUSED)
{
  
}


/* GIMPLE Loop Generator: generates loops in GIMPLE form for SCOP.  */

static void
gloog (scop_p scop ATTRIBUTE_UNUSED)
{
  
}


/* Perform a set of linear transforms on LOOPS.  */

void
graphite_transform_loops (struct loops *loops ATTRIBUTE_UNUSED)
{
  unsigned i;
  scop_p scop;
  current_scops = VEC_alloc (scop_p, heap, 3);

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "Graphite loop transformations \n");

  build_scops ();

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      print_loop_ir (dump_file, 2);
      print_scops (dump_file);
      fprintf (dump_file, "\nnumber of SCoPs: %d\n",
	       VEC_length (scop_p, current_scops));
    }

  for (i = 0; VEC_iterate (scop_p, current_scops, i, scop); i++)
    {
      build_domains (scop);
      build_scattering_functions (scop);
      graphite_find_transform (scop);
      gloog (scop);
    }
}
