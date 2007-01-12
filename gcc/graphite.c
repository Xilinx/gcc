/* Gimple Represented as Polyhedra.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@inria.fr>.

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


/* Print the schedules from SCHED.  */

void
print_graphite_bb (FILE *file, graphite_bb_p gb, int indent, int verbosity)
{
  fprintf (file, "\nGBB (\n");
  print_loop_ir_bb (file, GBB_BB (gb), indent+2, verbosity);
  dump_data_references (file, GBB_DATA_REFS (gb));
  fprintf (file, ")\n");
}

/* Print SCOP to FILE.  */

static void
print_scop (FILE *file, scop_p scop, int verbosity)
{
  unsigned i;
  lambda_vector v;

  fprintf (file, "\nSCoP_%d_%d (\n",
	   scop->entry->index, scop->exit->index);

  fprintf (file, "       (static schedule: ");
  print_lambda_vector (file, SCOP_STATIC_SCHEDULE (scop), scop_nb_loops (scop));
  fprintf (file, "       )\n");

  fprintf (file, "       (domain: \n");
  for (i = 0; VEC_iterate (lambda_vector, SCOP_DOMAIN (scop), i, v); i++)
    print_lambda_vector (file, v, scop_dim_domain (scop));
  fprintf (file, "       )\n");

  if (scop->bbs)
    {
      graphite_bb_p gb;
      unsigned i;

      for (i = 0; VEC_iterate (graphite_bb_p, scop->bbs, i, gb); i++)
	print_graphite_bb (file, gb, 0, verbosity);
    }

  fprintf (file, ")\n");
}

/* Print all the SCOPs to FILE.  */

static void
print_scops (FILE *file, int verbosity)
{
  unsigned i;
  scop_p scop;

  for (i = 0; VEC_iterate (scop_p, current_scops, i, scop); i++)
    print_scop (file, scop, verbosity);
}

/* Debug SCOP.  */

void
debug_scop (scop_p scop, int verbosity)
{
  print_scop (stderr, scop, verbosity);
}

/* Debug all SCOPs from CURRENT_SCOPS.  */

void 
debug_scops (int verbosity)
{
  print_scops (stderr, verbosity);
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

    case GIMPLE_MODIFY_STMT:
      {
	tree opnd0 = GIMPLE_STMT_OPERAND (stmt, 0);
	tree opnd1 = GIMPLE_STMT_OPERAND (stmt, 1);

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

	/* We cannot return (affine_expr (loop, opnd0) &&
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
	   it is very well possible that we can handle this.  */
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

/* Return true when BB is contained in SCOP.  */

static inline bool
bb_in_scop_p (basic_block bb, scop_p scop)
{
  return dominated_by_p (CDI_DOMINATORS, bb, scop->entry);
}

/* Return true when STMT is contained in SCOP.  */

static inline bool
stmt_in_scop_p (tree stmt, scop_p scop)
{
  return bb_in_scop_p (bb_for_stmt (stmt), scop);
}

/* Return true when VAR is invariant in SCOP.  In that case, VAR is a
   parameter of SCOP.  */

static inline bool
invariant_in_scop_p (tree var, scop_p scop)
{
  gcc_assert (TREE_CODE (var) == SSA_NAME);

  return !stmt_in_scop_p (SSA_NAME_DEF_STMT (var), scop);
}

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

static scop_p down_open_scop;

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
  SCOP_ENTRY (new_scop) = loop_start;
  SCOP_EXIT (new_scop) = bb;
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

  if (debug_p ())
    fprintf (dump_file, "down bb_%d\n", bb->index);

  /* Exiting the loop containing the open scop ends the scop.  */
  if (down_open_scop->entry->loop_father->depth > bb->loop_father->depth)
    {
      down_open_scop->exit = bb;
      VEC_safe_push (scop_p, heap, current_scops, down_open_scop);

      /* Then we begin a new scop at this block.  */
      down_open_scop = XNEW (struct scop);
      down_open_scop->entry = bb;

      if (debug_p ())
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

      if (debug_p ())
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



/* Get the index corresponding to VAR in the current LOOP.  If
   it's the first time we ask for this VAR, then we return
   chrec_not_analyzed_yet for this VAR and return its index.  */

static int
param_index (tree var, scop_p scop)
{
  int i;
  tree p;

  for (i = 0; VEC_iterate (tree, SCOP_PARAMS (scop), i, p); i++)
    if (p == var)
      return i;

  VEC_safe_push (tree, heap, SCOP_PARAMS (scop), var);
  return VEC_length (tree, SCOP_PARAMS (scop)) - 1;
}

/* Scan EXPR and translate it to an inequality vector INEQ that will
   be inserted in the domain matrix.  */

static void
scan_tree_for_params (scop_p scop, tree expr, lambda_vector ineq, int k)
{
  switch (TREE_CODE (expr))
    {
    case MULT_EXPR:
      if (chrec_contains_symbols (TREE_OPERAND (expr, 0)))
	{
	  gcc_assert (TREE_CODE (TREE_OPERAND (expr, 1)) == INTEGER_CST);
	  k *= int_cst_value (TREE_OPERAND (expr, 1));
	  scan_tree_for_params (scop, TREE_OPERAND (expr, 0), ineq, k);
	}
      else
	{
	  gcc_assert (TREE_CODE (TREE_OPERAND (expr, 0)) == INTEGER_CST);
	  k *= int_cst_value (TREE_OPERAND (expr, 0));
	  scan_tree_for_params (scop, TREE_OPERAND (expr, 1), ineq, k);
	}
      break;

    case PLUS_EXPR:
      scan_tree_for_params (scop, TREE_OPERAND (expr, 0), ineq, k);
      scan_tree_for_params (scop, TREE_OPERAND (expr, 1), ineq, k);
      break;

    case MINUS_EXPR:
      scan_tree_for_params (scop, TREE_OPERAND (expr, 0), ineq, k);
      scan_tree_for_params (scop, TREE_OPERAND (expr, 1), ineq, -1 * k);
      break;

    case SSA_NAME:
      ineq[scop_nb_loops (scop) + param_index (expr, scop)] = k;
      break;

    case INTEGER_CST:
      ineq[scop_nb_loops (scop) + scop_nb_params (scop)] = int_cst_value (expr);
      break;

    case NOP_EXPR:
    case CONVERT_EXPR:
    case NON_LVALUE_EXPR:
      scan_tree_for_params (scop, TREE_OPERAND (expr, 0), ineq, k);
      break;

    default:
      break;
    }
}

/* Record LOOP as occuring in SCOP.  */

static inline void
scop_record_loop (scop_p scop, struct loop *loop)
{
  unsigned i;
  struct loop *l;

  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, l); i++)
    if (l == loop)
      return;

  VEC_safe_push (loop_p, heap, SCOP_LOOP_NEST (scop), loop);
}

/* Build the loop nest around basic block BB.  */

static void
build_scop_loop_nests (scop_p scop)
{
  unsigned i;
  graphite_bb_p gb;

  SCOP_LOOP_NEST (scop) = VEC_alloc (loop_p, heap, 3);

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    scop_record_loop (scop, GBB_BB (gb)->loop_father);
}

/* Build the current domain matrix: the loops belonging to the current
   SCOP, and that vary for the execution of the current basic block.  */

static void
build_scop_domain (scop_p scop)
{
  unsigned i;
  struct loop *loop;

  SCOP_DOMAIN (scop) = VEC_alloc (lambda_vector, heap, scop_dim_domain (scop));

  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, loop); i++)
    {
      tree nb_iters = number_of_latch_executions (loop);
      lambda_vector ineq_low = lambda_vector_new (scop_dim_domain (scop));
      lambda_vector ineq_up = lambda_vector_new (scop_dim_domain (scop));

      if (chrec_contains_undetermined (nb_iters))
	continue;

      /* 0 <= loop_i */
      ineq_low[i] = 1;
      VEC_safe_push (lambda_vector, heap, SCOP_DOMAIN (scop), ineq_low);

      if (TREE_CODE (nb_iters) == INTEGER_CST)
	{
	  int nbi = int_cst_value (nb_iters);

	  /* loop_i <= nb_iters */
	  ineq_up[i] = -1;
	  ineq_up[scop_dim_domain (scop) - 1] = nbi;
	  VEC_safe_push (lambda_vector, heap, SCOP_DOMAIN (scop), ineq_up);
	}

      /* Otherwise nb_iters contains parameters: scan the nb_iters
	 expression and build its matrix representation.  */
      else
	{
	  nb_iters = instantiate_parameters (scop->entry->loop_father,
					     nb_iters);
	  scan_tree_for_params (scop, nb_iters, ineq_up, 1);
	  VEC_safe_push (lambda_vector, heap, SCOP_DOMAIN (scop), ineq_up);
	}
    }
}

/* Record parameters occurring in the index IDX of a data access.
   Callback for for_each_index.  */

struct irp_data
{
  struct loop *loop;
  scop_p scop;
};

static bool
idx_record_param (tree *var, void *dta)
{
  struct irp_data *data = dta;

  switch (TREE_CODE (*var))
    {
    case SSA_NAME:
      if (invariant_in_scop_p (*var, data->scop))
	param_index (*var, data->scop);
      return true;

    default:
      return true;
    }
}

static bool
idx_record_params (tree base, tree *idx, void *dta)
{
  struct irp_data *data = dta;

  if (TREE_CODE (base) != ARRAY_REF)
    return true;

  if (TREE_CODE (*idx) == SSA_NAME)
    {
      tree scev = instantiate_parameters
	(data->scop->entry->loop_father, 
	 analyze_scalar_evolution (data->loop, *idx));

      for_each_scev_op (&scev, idx_record_param, dta);
    }

  return true;
}

/* Initialize the access matrix in the data reference REF with respect
   to the loop nesting LOOP_NEST.  Return true when the operation
   succeeded.  */

static bool
build_access_matrix (data_reference_p ref, graphite_bb_p gb)
{
  unsigned i;

  DR_SCOP (ref) = GBB_SCOP (gb);
  DR_ACCESS_MATRIX (ref) = VEC_alloc (lambda_vector, heap, 
				      DR_NUM_DIMENSIONS (ref));

  for (i = 0; i < DR_NUM_DIMENSIONS (ref); i++)
    {
      lambda_vector v = lambda_vector_new (gbb_dim_domain (gb));
      bool res = build_access_matrix_with_af (DR_ACCESS_FN (ref, i), v,
					      SCOP_LOOP_NEST (GBB_SCOP (gb)),
					      SCOP_PARAMS (GBB_SCOP (gb)));
      if (!res)
	return false;

      VEC_safe_push (lambda_vector, heap, DR_ACCESS_MATRIX (ref), v);
    }

  return true;
}

/* Build a schedule for each basic-block in the SCOP.  */

static void
build_scop_data_accesses (scop_p scop)
{
  unsigned i;
  graphite_bb_p gb;

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    {
      unsigned j;
      block_stmt_iterator bsi;
      data_reference_p dr;

      /* On each statement of the basic block, gather all the occurences
	 to read/write memory.  */
      GBB_DATA_REFS (gb) = VEC_alloc (data_reference_p, heap, 5);
      for (bsi = bsi_start (GBB_BB (gb)); !bsi_end_p (bsi); bsi_next (&bsi))
	find_data_references_in_stmt (bsi_stmt (bsi), &GBB_DATA_REFS (gb));

      /* Construct the access matrix for each data ref, with respect to
	 the loop nest of the current BB in the considered SCOP.  */
      for (j = 0; VEC_iterate (data_reference_p, GBB_DATA_REFS (gb), j, dr); j++)
	build_access_matrix (dr, gb);
    }
}

/* Build for BB the static schedule.  */

static void
build_scop_canonical_schedules (scop_p scop)
{
  unsigned i;
  graphite_bb_p gb;

  SCOP_STATIC_SCHEDULE (scop) = lambda_vector_new (scop_nb_loops (scop) + 1);

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    {
      basic_block bb = GBB_BB (gb);

      SCOP_STATIC_SCHEDULE (scop)[scop_loop_index (scop, bb->loop_father)] += 1;
      GBB_STATIC_SCHEDULE (gb) = lambda_vector_new (scop_nb_loops (scop));
      lambda_vector_copy (SCOP_STATIC_SCHEDULE (scop), GBB_STATIC_SCHEDULE (gb),
			  scop_nb_loops (scop) + 1);
    }
}

/* Build for BB the static schedule.  */

static void
build_graphite_bb (struct dom_walk_data *dw_data, basic_block bb)
{
  struct graphite_bb *gb;
  scop_p scop = (scop_p) dw_data->global_data;

  /* Scop's exit is not in the scop.  */
  if (bb == scop->exit
      /* Every block in the scop dominates scop's exit.  */
      || !dominated_by_p (CDI_DOMINATORS, scop->exit, bb))
    return;

  /* Build the new representation for the basic block.  */
  gb = XNEW (struct graphite_bb);
  GBB_BB (gb) = bb;
  GBB_SCOP (gb) = scop;

  /* Store the GRAPHITE representation of the current BB.  */
  VEC_safe_push (graphite_bb_p, heap, scop->bbs, gb);
}

static void
build_scop_bbs (scop_p scop)
{
  struct dom_walk_data walk_data;

  memset (&walk_data, 0, sizeof (struct dom_walk_data));

  /* Iterate over all the basic blocks of the scop in their pseudo
     execution order, and associate to each bb a static schedule.
     (pseudo exec order = the branches of a condition are scheduled
     sequentially: the then clause comes before the else clause.)  */
  walk_data.before_dom_children_before_stmts = build_graphite_bb;

  SCOP_BBS (scop) = VEC_alloc (graphite_bb_p, heap, 5);
  walk_data.global_data = scop;
  init_walk_dominator_tree (&walk_data);
  walk_dominator_tree (&walk_data, scop->entry);
  fini_walk_dominator_tree (&walk_data);
}

static void
find_params_in_bb (struct dom_walk_data *dw_data, basic_block bb)
{
  unsigned i;
  data_reference_p dr;
  VEC (data_reference_p, heap) *drs;
  block_stmt_iterator bsi;
  scop_p scop = (scop_p) dw_data->global_data;

  /* Find the parameters used in the memory access functions.  */
  drs = VEC_alloc (data_reference_p, heap, 5);
  for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
    find_data_references_in_stmt (bsi_stmt (bsi), &drs);

  for (i = 0; VEC_iterate (data_reference_p, drs, i, dr); i++)
    {
      struct irp_data irp;

      irp.loop = bb->loop_father;
      irp.scop = scop;
      for_each_index (&dr->ref, idx_record_params, &irp);
    }

  VEC_free (data_reference_p, heap, drs);
}

/* Record the parameters used in the SCOP.  A variable is a parameter
   in a scop if it does not vary during the execution of that scop.  */

static void
build_scop_params (scop_p scop)
{
  unsigned i;
  struct dom_walk_data walk_data;
  struct loop *loop;

  SCOP_PARAMS (scop) = VEC_alloc (tree, heap, 3);

  /* Find the parameters used in the loop bounds.  */
  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, loop); i++)
    {
      tree nb_iters = number_of_latch_executions (loop);

      if (chrec_contains_symbols (nb_iters))
	{
	  struct irp_data irp;

	  irp.loop = loop;
	  irp.scop = scop;
	  for_each_scev_op (&nb_iters, idx_record_param, &irp);
	}
    }

  /* Find the parameters used in data accesses.  */
  memset (&walk_data, 0, sizeof (struct dom_walk_data));
  walk_data.before_dom_children_before_stmts = find_params_in_bb;
  walk_data.global_data = scop;
  init_walk_dominator_tree (&walk_data);
  walk_dominator_tree (&walk_data, scop->entry);
  fini_walk_dominator_tree (&walk_data);
}

/* Find the right transform for the SCOP.  */

static void
find_transform (scop_p scop ATTRIBUTE_UNUSED)
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

  for (i = 0; VEC_iterate (scop_p, current_scops, i, scop); i++)
    {
      build_scop_bbs (scop);
      build_scop_loop_nests (scop);
      build_scop_canonical_schedules (scop);
      build_scop_params (scop);
      build_scop_domain (scop);
      build_scop_data_accesses (scop);

      find_transform (scop);
      gloog (scop);
    }

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      print_scops (dump_file, 2);
      fprintf (dump_file, "\nnumber of SCoPs: %d\n",
	       VEC_length (scop_p, current_scops));
    }
}
