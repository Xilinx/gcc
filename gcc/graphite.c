/* Gimple Represented as Polyhedra.
   Copyright (C) 2006, 2007 Free Software Foundation, Inc.
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
#include "polylib/polylibgmp.h"
#include "cloog/cloog.h"
#include "graphite.h"


VEC (scop_p, heap) *current_scops;

static tree harmful_stmt_in_bb (struct loop *outermost_loop, basic_block);
static CloogMatrix *schedule_to_scattering (graphite_bb_p, int);
static inline int nb_loops_around_gb (graphite_bb_p gb);

/* Returns a new loop_to_cloog_loop_str structure.  */

static inline struct loop_to_cloog_loop_str *
new_loop_to_cloog_loop_str (unsigned int loop_num,
                            unsigned int loop_position,
                            CloogLoop *cloog_loop)
{
  struct loop_to_cloog_loop_str *result;

  result = XNEWVEC (struct loop_to_cloog_loop_str, 1);
  result->loop_num = loop_num;
  result->cloog_loop = cloog_loop;
  result->loop_position = loop_position;

  return result;
}

/* Hash function for SCOP_LOOP2CLOOG_LOOP hash table.  */

static hashval_t
hash_loop_to_cloog_loop (const void *elt)
{
  return ((const struct loop_to_cloog_loop_str *) elt)->loop_num;
}

/* Equality function for SCOP_LOOP2CLOOG_LOOP hash table.  */

static int
eq_loop_to_cloog_loop (const void *el1, const void *el2)
{
  const struct loop_to_cloog_loop_str *elt1, *elt2;

  elt1 = (const struct loop_to_cloog_loop_str *) el1;
  elt2 = (const struct loop_to_cloog_loop_str *) el2;
  return elt1->loop_num == elt2->loop_num;
}

/* Free function for SCOP_LOOP2CLOOG_LOOP.  */

static void
del_loop_to_cloog_loop (void *e)
{
  free (e);
}

/* Print the schedules from SCHED.  */

void
print_graphite_bb (FILE *file, graphite_bb_p gb, int indent, int verbosity)
{
  fprintf (file, "\nGBB (\n");

  fprintf (file, "       (static schedule: ");
  print_lambda_vector (file, GBB_STATIC_SCHEDULE (gb), scop_nb_loops (GBB_SCOP (gb)) + 1);
  fprintf (file, "       )\n");

  print_loops_bb (file, GBB_BB (gb), indent+2, verbosity);

  if (GBB_DATA_REFS (gb))
    dump_data_references (file, GBB_DATA_REFS (gb));

  fprintf (file, "       (scattering: \n");
  cloog_matrix_print (file,
                      schedule_to_scattering (gb, 2 * nb_loops_around_gb (gb)
                                                 + 1));
                                                    
  fprintf (file, "       )\n");

  fprintf (file, ")\n");
}

/* Print the schedules from SCHED.  */

void
debug_gbb (graphite_bb_p gb, int verbosity)
{
  print_graphite_bb (stderr, gb, 0, verbosity);
}


/* Print SCOP to FILE.  */

static void
print_scop (FILE *file, scop_p scop, int verbosity)
{
  if (scop == NULL)
    return;

  fprintf (file, "\nSCoP_%d_%d (\n",
	   SCOP_ENTRY (scop)->index, SCOP_EXIT (scop)->index);

  fprintf (file, "       (cloog: \n");
  cloog_program_print (file, SCOP_PROG (scop));
  fprintf (file, "       )\n");

  if (SCOP_BBS (scop))
    {
      graphite_bb_p gb;
      unsigned i;

      for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
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

/* Return true when BB is contained in SCOP.  */

static inline bool
bb_in_scop_p (basic_block bb, scop_p scop)
{
  return bitmap_bit_p (SCOP_BBS_B (scop), bb->index);
}

/* Pretty print a scop in DOT format.  */

static void 
dot_scop_1 (FILE *file, scop_p scop)
{
  edge e;
  edge_iterator ei;
  basic_block bb;
  basic_block entry = SCOP_ENTRY (scop);
  basic_block exit = SCOP_EXIT (scop);
    
  fprintf (file, "digraph SCoP_%d_%d {\n", entry->index,
	   exit->index);

  FOR_ALL_BB (bb)
    {
      if (bb == entry)
	fprintf (file, "%d [shape=triangle];\n", bb->index);

      if (bb == exit)
	fprintf (file, "%d [shape=box];\n", bb->index);

      if (bb_in_scop_p (bb, scop)) 
	fprintf (file, "%d [color=red];\n", bb->index);

      FOR_EACH_EDGE (e, ei, bb->succs)
	fprintf (file, "%d -> %d;\n", bb->index, e->dest->index);
    }

  fputs ("}\n\n", file);
}

/* Display SCOP using dotty.  */

void
dot_scop (scop_p scop)
{
  FILE *stream = fopen ("/tmp/scop.dot", "w");
  gcc_assert (stream != NULL);

  dot_scop_1 (stream, scop);
  fclose (stream);

  system ("dotty /tmp/scop.dot");
}

/* Pretty print all SCoPs in DOT format and mark them with different colors.
   If there are not enough colors (8 at the moment), paint later SCoPs gray.
   Special nodes:
   - "*" after the node number: entry of a SCoP,
   - "#" after the node number: exit of a SCoP,
   - "()" entry or exit not part of SCoP.  */

static void
dot_all_scops_1 (FILE *file)
{
  basic_block bb;
  edge e;
  edge_iterator ei;
  scop_p scop;
  const char* color;
  int i;

  /* Disable debugging while printing graph.  */
  int tmp_dump_flags = dump_flags;
  dump_flags = 0;

  fprintf (file, "digraph all {\n");

  FOR_ALL_BB (bb)
    {
      int part_of_scop = false;

      /* Use HTML for every bb label.  So we are able to print bbs
         which are part of two different SCoPs, with two different
         background colors.  */
      fprintf (file, "%d [label=<\n  <TABLE BORDER=\"0\" CELLBORDER=\"1\" ",
                     bb->index);
      fprintf (file, "CELLSPACING=\"0\">\n");

      /* Select color for SCoP.  */
      for (i = 0; VEC_iterate (scop_p, current_scops, i, scop); i++)
	if (bb_in_scop_p (bb, scop) || scop->exit == bb || scop->entry == bb)
	  {
	    switch (i % 17)
	      {
	      case 0: /* red */
		color = "#e41a1c";
		break;
	      case 1: /* blue */
		color = "#377eb8";
		break;
	      case 2: /* green */
		color = "#4daf4a";
		break;
	      case 3: /* purple */
		color = "#984ea3";
		break;
	      case 4: /* orange */
		color = "#ff7f00";
		break;
	      case 5: /* yellow */
		color = "#ffff33";
		break;
	      case 6: /* brown */
		color = "#a65628";
		break;
	      case 7: /* rose */
		color = "#f781bf";
		break;
	      case 8:
		color = "#8dd3c7";
		break;
	      case 9:
		color = "#ffffb3";
		break;
	      case 10:
		color = "#bebada";
		break;
	      case 11:
		color = "#fb8072";
		break;
	      case 12:
		color = "#80b1d3";
		break;
	      case 13:
		color = "#fdb462";
		break;
	      case 14:
		color = "#b3de69";
		break;
	      case 15:
		color = "#fccde5";
		break;
	      case 16:
		color = "#bc80bd";
		break;
	      default: /* gray */
		color = "#999999";
	      }

	    fprintf (file, "    <TR><TD WIDTH=\"50\" BGCOLOR=\"%s\">", color);
        
	    if (!bb_in_scop_p (bb, scop))
	      fprintf (file, " ("); 

	    if (bb == scop->entry && bb == scop->exit)
	      fprintf (file, " %d*# ", bb->index);
	    else if (bb == scop->entry)
	      fprintf (file, " %d* ", bb->index);
	    else if (bb == scop->exit)
	      fprintf (file, " %d# ", bb->index);
	    else
	      fprintf (file, " %d ", bb->index);

	    if (!bb_in_scop_p (bb, scop))
	      fprintf (file, ")");

	    fprintf (file, "</TD></TR>\n");

	    part_of_scop  = true;
	  }

      if (!part_of_scop)
        {
          fprintf (file, "    <TR><TD WIDTH=\"50\" BGCOLOR=\"#ffffff\">");
          fprintf (file, " %d </TD></TR>\n", bb->index);
        }

      fprintf (file, "  </TABLE>>, shape=box, style=\"setlinewidth(0)\"]\n");
    }

  FOR_ALL_BB (bb)
    {
      FOR_EACH_EDGE (e, ei, bb->succs)
	      fprintf (file, "%d -> %d;\n", bb->index, e->dest->index);
    }

  fputs ("}\n\n", file);

  /* Enable debugging again.  */
  dump_flags = tmp_dump_flags;
}

/* Display all SCoPs using dotty.  */

void
dot_all_scops (void)
{
  FILE *stream = fopen ("/tmp/allscops.dot", "w");
  gcc_assert (stream != NULL);

  dot_all_scops_1 (stream);
  fclose (stream);

  system ("dotty /tmp/allscops.dot");
}



/* Returns true when LOOP is in SCOP.  */

static inline bool 
loop_in_scop_p (struct loop *loop, scop_p scop)
{
  return (bb_in_scop_p (loop->header, scop)
	  && bb_in_scop_p (loop->latch, scop));
}

/* Returns the outermost loop in SCOP that contains BB.  */

static struct loop *
outermost_loop_in_scop (scop_p scop, basic_block bb)
{
  struct loop *nest;

  nest = bb->loop_father;
  while (loop_outer (nest) && loop_in_scop_p (loop_outer (nest), scop))
    nest = loop_outer (nest);

  return nest;
}

/* Return true when EXPR is an affine function in LOOP with parameters
   instantiated relative to outermost_loop.  */

static bool
loop_affine_expr (struct loop *outermost_loop, struct loop *loop, tree expr)
{
  tree scev = analyze_scalar_evolution (loop, expr);

  scev = instantiate_scev (outermost_loop, loop, scev);

  return (evolution_function_is_invariant_p (scev, outermost_loop->num)
	  || evolution_function_is_affine_multivariate_p (scev,
							  outermost_loop->num));
}


/* Return true only when STMT is simple enough for being handled by Graphite.
   This depends on outermost_loop, as the parametetrs are initialized relativ
   to this loop.  */

static bool
stmt_simple_for_scop_p (struct loop *outermost_loop, tree stmt)
{
  basic_block bb = bb_for_stmt (stmt);
  struct loop *loop = bb->loop_father;

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
    case RETURN_EXPR:
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
	    return (outermost_loop
		    && loop_affine_expr (outermost_loop, loop, TREE_OPERAND (opnd0, 0))
		    && loop_affine_expr (outermost_loop, loop, TREE_OPERAND (opnd0, 1)));
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
	    data_reference_p dr;

	    dr = create_data_ref (loop, opnd0, stmt, false);
	    if (!dr)
	      return false;

	    free_data_ref (dr);

	    if (TREE_CODE (opnd1) == ARRAY_REF 
		|| TREE_CODE (opnd1) == INDIRECT_REF
		|| TREE_CODE (opnd1) == COMPONENT_REF)
	      {
		dr = create_data_ref (loop, opnd1, stmt, true);
		if (!dr)
		  return false;

		free_data_ref (dr);
		return true;
	      }
	  }

	if (TREE_CODE (opnd1) == ARRAY_REF 
	     || TREE_CODE (opnd1) == INDIRECT_REF
	     || TREE_CODE (opnd1) == COMPONENT_REF)
	  {
	    data_reference_p dr;

	    dr = create_data_ref (loop, opnd1, stmt, true);
	    if (!dr)
	      return false;

	    free_data_ref (dr);
	    return true;
	  }

	/* We cannot return (loop_affine_expr (loop, opnd0) &&
	   loop_affine_expr (loop, opnd1)) because D.1882_16 is
	   not affine in the following:

	   D.1881_15 = a[j_13][pretmp.22_20];
	   D.1882_16 = D.1881_15 + 2;
	   a[j_22][i_12] = D.1882_16;

	   but this is valid code in a scop.

	   FIXME: I'm not yet 100% sure that returning true is safe:
	   there might be exponential scevs.  On the other hand, if
	   these exponential scevs do not reference arrays, then
	   access functions, domains and schedules remain affine.  So
	   it is very well possible that we can handle exponential
	   scalar computations.  */
	return true;
      }

    case CALL_EXPR:
      {
	tree args;

	for (args = TREE_OPERAND (stmt, 1); args; args = TREE_CHAIN (args))
	  if (TREE_CODE (TREE_VALUE (args)) == ARRAY_REF
	      || TREE_CODE (TREE_VALUE (args)) == INDIRECT_REF
	      || TREE_CODE (TREE_VALUE (args)) == COMPONENT_REF)
	    {
	      data_reference_p dr;

	      dr = create_data_ref (loop, TREE_VALUE (args), stmt, true);
	      if (!dr)
		return false;

	      free_data_ref (dr);
	    }

	return true;
      }

    default:
      /* These nodes cut a new scope.  */
      return false;
    }

  return false;
}

/* Returns the statement of BB that contains a harmful operation: that
   can be a function call with side effects, data dependences that
   cannot be computed, etc.  The current open scop should end before
   this statement.  */

static tree
harmful_stmt_in_bb (struct loop *outermost_loop, basic_block bb)
{
  block_stmt_iterator bsi;

  for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
    if (!stmt_simple_for_scop_p (outermost_loop, bsi_stmt (bsi)))
      return bsi_stmt (bsi);

  return NULL_TREE;
}

/* Creates a new scop starting with ENTRY.  */

static scop_p
new_scop (basic_block entry)
{
  scop_p scop = XNEW (struct scop);

  gcc_assert (entry);
  SCOP_ENTRY (scop) = entry;
  SCOP_BBS (scop) = VEC_alloc (graphite_bb_p, heap, 3);
  SCOP_BBS_B (scop) = BITMAP_ALLOC (NULL);
  SCOP_LOOPS (scop) = BITMAP_ALLOC (NULL);
  SCOP_LOOP_NEST (scop) = VEC_alloc (loop_p, heap, 3);
  SCOP_PARAMS (scop) = VEC_alloc (name_tree, heap, 3);
  SCOP_PROG (scop) = cloog_program_malloc ();
  SCOP_PROG (scop)->names = cloog_names_malloc ();
  SCOP_LOOP2CLOOG_LOOP (scop) = htab_create (10, hash_loop_to_cloog_loop,
					     eq_loop_to_cloog_loop,
					     del_loop_to_cloog_loop);
  return scop;
}

/* Deletes the scop.  */

static void
free_scop (scop_p scop)
{
  int i;
  name_tree p;

  VEC_free (graphite_bb_p, heap, SCOP_BBS (scop));
  BITMAP_FREE (SCOP_BBS_B (scop));
  BITMAP_FREE (SCOP_LOOPS (scop));
  VEC_free (loop_p, heap, SCOP_LOOP_NEST (scop));

  for (i = 0; VEC_iterate (name_tree, SCOP_PARAMS (scop), i, p); i++)
    free (p);

  VEC_free (name_tree, heap, SCOP_PARAMS (scop));
  cloog_program_free (SCOP_PROG (scop));
  htab_delete (SCOP_LOOP2CLOOG_LOOP (scop)); 
  free (scop);
}

static void
free_scops (VEC (scop_p, heap) *scops)
{
  unsigned i;
  scop_p scop;

  for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
    free_scop (scop);

  VEC_free (scop_p, heap, scops);
}

#define GBB_UNKNOWN 0
#define GBB_LOOP_SING_EXIT_HEADER 1
#define GBB_LOOP_MULT_EXIT_HEADER 2
#define GBB_LOOP_EXIT 3
#define GBB_COND_HEADER 5
#define GBB_SIMPLE 6
#define GBB_LAST 7

/* Detect the tyoe of the bb.  Loop headers are only marked, if they are new.
   This means their loop_father is different to last_loop.  Otherwise they are
   treated like any other bb and their type can be any other type.  */

static int
get_bb_type (basic_block bb, struct loop *last_loop)
{
  VEC (basic_block, heap) *dom = get_dominated_by (CDI_DOMINATORS, bb);
  int nb_dom = VEC_length (basic_block, dom);
  int nb_suc = VEC_length (edge, bb->succs);
  struct loop *loop = bb->loop_father;

  /* Check, if we entry into a new loop. */
  if (loop != last_loop)
    {
      if (single_exit (loop) != NULL)
        return GBB_LOOP_SING_EXIT_HEADER;
      else if (loop->num != 0)
        return GBB_LOOP_MULT_EXIT_HEADER;
      else
	return GBB_COND_HEADER;
    }

  if (nb_dom == 0)
    return GBB_LAST;

  if (nb_dom == 1 && nb_suc == 1)
    return GBB_SIMPLE;

  return GBB_COND_HEADER;
}

/* Move the scops from source to target and clean up target.  */

static void
move_scops (VEC (scop_p, heap) **source, VEC (scop_p, heap) **target)
{
  scop_p s;
  int i;

  for (i = 0; VEC_iterate (scop_p, *source, i, s); i++)
    VEC_safe_push (scop_p, heap, *target, s);
  
  VEC_free (scop_p, heap, *source);
}

static bool build_scops_1 (basic_block, VEC (scop_p, heap) **, struct loop *,
			   struct loop *, basic_block *, bool *);

/* Check if 'exit_bb' is a bb, which follows a loop exit edge.  */ 

static bool
is_loop_exit (struct loop *loop, basic_block exit_bb)
{
  int i;
  VEC (edge, heap) *exits;
  edge e;
  bool is_exit = false;

  if (loop_outer (loop) == NULL)
    return false;
 
  exits = get_loop_exit_edges (loop);
  
  for (i = 0; VEC_iterate (edge, exits, i, e); i++)
    if (e->dest == exit_bb)
      is_exit = true;

  VEC_free (edge, heap, exits);

  return is_exit;
}

/* Check if 'pred' is predecessor of 'succ'.  */

static bool
is_pred (basic_block pred, basic_block succ)
{
  int i;
  edge e;

  for (i = 0; VEC_iterate (edge, succ->preds, i, e); i++)
    if (e->src == pred)
      return true;

  return false;
}

/* Checks, if a bb can be added to a SCoP.  */

static bool
is_bb_addable (basic_block bb, struct loop *outermost_loop,
	       VEC (scop_p, heap) **scops, int type, basic_block *next,
	       bool *bb_simple, basic_block *last, tree *stmt)
{
  struct loop *loop = bb->loop_father;
  bool bb_addable;

  *stmt = harmful_stmt_in_bb (outermost_loop, bb);
  bb_addable = (*stmt == NULL_TREE);

  switch (type)
    {
    case GBB_LAST:
      *next = NULL;
      *last = bb;
      *bb_simple = bb_addable;
      break;

    case GBB_SIMPLE:
      *next = VEC_last (edge, bb->succs)->dest;
      *last = bb;
      *bb_simple = bb_addable;
      break;

    case GBB_LOOP_SING_EXIT_HEADER:
      {
        VEC (scop_p, heap) *tmp_scops = VEC_alloc (scop_p, heap, 3);

        bb_addable = build_scops_1 (bb, &tmp_scops, loop, outermost_loop, last,
                                  bb_simple);

        *next = single_exit (bb->loop_father)->dest;

        if (!dominated_by_p (CDI_DOMINATORS, *next, bb))
          *next = NULL;

        *last = single_exit (bb->loop_father)->src;

        if (!bb_addable)
          move_scops (&tmp_scops, scops);
        else 
          free_scops (tmp_scops);

        return bb_addable;
      }

    case GBB_LOOP_MULT_EXIT_HEADER:
      {
        /* XXX: Handle loop nests with the same header.  */
        /* XXX: Handle iterative optimization of outermost_loop.  */
        /* XXX: For now we just do not join loops with multiple exits. If the 
           exits lead to the seam bb it may be possible to join the loop.  */
        VEC (scop_p, heap) *tmp_scops = VEC_alloc (scop_p, heap, 3);
        VEC (edge, heap) *exits = get_loop_exit_edges (loop);
        edge e;
        int i;

        bb_addable = build_scops_1 (bb, &tmp_scops, loop, outermost_loop, last,
                                  bb_simple);

        for (i = 0; VEC_iterate (edge, exits, i, e); i++)
          if (dominated_by_p (CDI_DOMINATORS, e->dest, e->src)
              && e->dest->loop_father == loop_outer (loop))
            build_scops_1 (e->dest, &tmp_scops, e->dest->loop_father,
                           outermost_loop, last, bb_simple);

        *next = NULL; 
        *last = NULL;
        *bb_simple = false;
        bb_addable = false;
        move_scops (&tmp_scops, scops);
        VEC_free (edge, heap, exits);

        return bb_addable;
      }
    case GBB_COND_HEADER:
    {
      VEC (scop_p, heap) *tmp_scops = VEC_alloc (scop_p, heap, 3);
      VEC (basic_block, heap) *dominated = get_dominated_by (CDI_DOMINATORS,
                                                             bb);
      int i;
      basic_block dom_bb;
      basic_block last_bb = NULL;
      edge e;
      *bb_simple = true;
 
      /* First check the successors of bb, and check if it is possible to join
         the different branches.  */
      for (i = 0; VEC_iterate (edge, bb->succs, i, e); i++)
        {
          bool bb_simple_tmp;
          basic_block next_tmp;
          dom_bb = e->dest;

          /* Ignore loop exits.  They will be handled after the loop body.  */
          if (is_loop_exit (bb->loop_father, dom_bb))
            {
              bb_addable = false;
              continue;
            }

          /* Only handle bb dominated by 'bb'.  The others will be handled by
             the bb, that dominates this bb.  */
          if (!dominated_by_p (CDI_DOMINATORS, dom_bb, bb))
            {
              /* Check, if edge leads direct to the end of this condition.  If
                 this is true, the condition stays joinable.  */
              if (!last_bb)
                last_bb = dom_bb;

              if (dom_bb != last_bb)
                *bb_simple = false;

              continue;
            }

          /* Ignore edges, which jump forward.  */
          if (VEC_length (edge, dom_bb->preds) > 1)
            {

              /* Check, if edge leads direct to the end of this condition.  If
                 this is true, the condition stays joinable.  */
              if (!last_bb)
                last_bb = dom_bb;

              if (dom_bb != last_bb)
                *bb_simple = false;

              continue;
            }

	  bb_addable &= build_scops_1 (dom_bb, &tmp_scops, loop, outermost_loop,
                                       last, &bb_simple_tmp);
          *bb_simple &= bb_simple_tmp; 
          
          /* Checks, if all branches end at the same point. If that is true, the
             condition stays joinable.  */
          if (*last &&  VEC_length (edge, (*last)->succs) == 1)
            {
              next_tmp = VEC_last (edge, (*last)->succs)->dest;
                  
              if (!last_bb)
                last_bb = next_tmp;

              if (next_tmp != last_bb)
                *bb_simple = false;
            }
          else
            *bb_simple = false;
        }

      bb_addable &= *bb_simple;

      /* If the condition is joinable, break and return the endpoint of the
         condition.  */
      if (bb_addable && *bb_simple)
        {
          /* Only return a next pointer, if we dominate this pointer.  Otherwise
             it will be handled by the bb dominating it.  */ 
          if (dominated_by_p (CDI_DOMINATORS, last_bb, bb) && last_bb != bb)
            *next = last_bb;
          else
            *next = NULL; 

          return bb_addable;
        }

      /* Scan the remaining dominated bbs.  */
      for (i = 0; VEC_iterate (basic_block, dominated, i, dom_bb); i++)
        {
          bool bb_simple_tmp;

          /* Ignore loop exits.  They will be handled after the loop body.  */
          if (is_loop_exit (bb->loop_father, dom_bb))
            continue;

          /* Ignore the bbs processed above.  */
          if (is_pred (bb, dom_bb) && VEC_length (edge, dom_bb->preds) == 1)
            continue;

	  build_scops_1 (dom_bb, &tmp_scops, loop, outermost_loop, last,
            &bb_simple_tmp);
          *bb_simple = false;
        }

      bb_addable &= *bb_simple;
      *next = NULL; 
      move_scops (&tmp_scops, scops);

      return bb_addable;
    }
    default:
      gcc_unreachable ();
    }

  return bb_addable;
}

/* End SCOP with basic block EXIT, and split EXIT before STMT when
   STMT is non NULL.  */

static void
end_scop (scop_p scop, basic_block exit, basic_block *last, tree stmt)
{
  /* XXX: Disable bb splitting, contains a bug (SIGSEGV in polyhedron
     aermod.f90).  */
  if (0 && stmt && VEC_length (edge, exit->preds) == 1)
    {
      edge e;

      if (stmt == bsi_stmt (bsi_after_labels (exit)))
	stmt = NULL_TREE;
      else
	{
	  block_stmt_iterator bsi = bsi_for_stmt (stmt);
	  bsi_prev (&bsi);
	  stmt = bsi_stmt (bsi);
	}

      e = split_block (exit, stmt);
      set_immediate_dominator (CDI_DOMINATORS, e->dest, e->src);
      set_immediate_dominator (CDI_POST_DOMINATORS, e->src, e->dest);
      exit = e->dest;
      *last = e->dest;
    }

  SCOP_EXIT (scop) = exit;
}

/* Creates the SCoPs and writes entry and exit points for every SCoP.  */

static bool 
build_scops_1 (basic_block start, VEC (scop_p, heap) **scops,
	       struct loop *loop, struct loop *outermost_loop,
	       basic_block *last, bool *all_simple)
{
  basic_block current = start;
  basic_block next = NULL;
  scop_p open_scop = NULL;
  
  bool in_scop = false;

  bool all_addable = true;
  *all_simple = true; 

  /* Loop over the dominance tree.  If we meet a difficult bb jump out of the
     SCoP, after that jump back in.  Loop and condition header start a new 
     layer and can only be added, if all bbs in deeper layers are simple.  */

  while (current != NULL)
    {
      int type = get_bb_type (current, loop);
      bool bb_simple;
      bool bb_addable;
      tree stmt;

      bb_addable = is_bb_addable (current, outermost_loop, scops, type,
				  &next, &bb_simple, last, &stmt);  

      if (!in_scop && bb_addable)
        {
          open_scop = new_scop (current);
          VEC_safe_push (scop_p, heap, *scops, open_scop); 
          in_scop = true;
        }
      else if (in_scop && !bb_addable)
        {
          end_scop (open_scop, current, last, stmt);
          in_scop = false;
        }

      if (!in_scop)
        all_addable &= false;

      *all_simple &= bb_simple;

      if (next == NULL && in_scop)
        end_scop (open_scop, VEC_last (edge, (*last)->succs)->dest, last,
		  stmt);

      current = next;
    }

  if ((start->loop_father->header == start) && *all_simple)
    return true;

  return all_addable;
}

/* Find static control parts.  */

static void
build_scops (void)
{
  struct loop *loop = ENTRY_BLOCK_PTR->loop_father;
  basic_block last;
  bool tmp;
  build_scops_1 (ENTRY_BLOCK_PTR, &current_scops, loop, loop, &last, &tmp);
}


/* Store the GRAPHITE representation of BB.  */

static void
build_graphite_bb (scop_p scop, basic_block bb)
{
  struct graphite_bb *gb;

  /* Build the new representation for the basic block.  */
  gb = XNEW (struct graphite_bb);
  GBB_BB (gb) = bb;
  GBB_SCOP (gb) = scop;
  GBB_DATA_REFS (gb) = NULL; 
  GBB_DOMAIN (gb) = NULL;

  /* Store the GRAPHITE representation of the current BB.  */
  VEC_safe_push (graphite_bb_p, heap, SCOP_BBS (scop), gb);
  bitmap_set_bit (SCOP_BBS_B (scop), bb->index);
}

/* Gather the basic blocks belonging to the SCOP.  */

static void
build_scop_bbs (scop_p scop)
{
  basic_block *stack = XNEWVEC (basic_block, n_basic_blocks + 1);
  sbitmap visited = sbitmap_alloc (last_basic_block);
  int sp = 0;

  sbitmap_zero (visited);
  stack[sp++] = SCOP_ENTRY (scop);

  while (sp)
    {
      basic_block bb = stack[--sp];
      unsigned depth = loop_depth (bb->loop_father);
      int num = bb->loop_father->num;
      edge_iterator ei;
      edge e;

      /* Scop's exit is not in the scop.  Exclude also bbs, which are
	 dominated by the SCoP exit.  These are e.g. loop latches.  */
      if (TEST_BIT (visited, bb->index)
	  || dominated_by_p (CDI_DOMINATORS, bb, SCOP_EXIT (scop))
	  /* Every block in the scop is dominated by scop's entry.  */
	  || !dominated_by_p (CDI_DOMINATORS, bb, SCOP_ENTRY (scop))
	  /* Every block in the scop is postdominated by scop's exit.  */
	  || (bb != ENTRY_BLOCK_PTR 
	      && !dominated_by_p (CDI_POST_DOMINATORS, bb, SCOP_EXIT (scop))))
	continue;

      build_graphite_bb (scop, bb);
      SET_BIT (visited, bb->index);

      /* First push the blocks that have to be processed last.  */
      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && loop_depth (e->dest->loop_father) < depth)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && loop_depth (e->dest->loop_father) == depth
	    && e->dest->loop_father->num != num)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && loop_depth (e->dest->loop_father) == depth
	    && e->dest->loop_father->num == num
	    && EDGE_COUNT (e->dest->preds) > 1)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && loop_depth (e->dest->loop_father) == depth
	    && e->dest->loop_father->num == num
	    && EDGE_COUNT (e->dest->preds) == 1)
	  stack[sp++] = e->dest;

      FOR_EACH_EDGE (e, ei, bb->succs)
	if (! TEST_BIT (visited, e->dest->index)
	    && loop_depth (e->dest->loop_father) > depth)
	  stack[sp++] = e->dest;
    }

  free (stack);
  sbitmap_free (visited);
}

/* Record LOOP as occuring in SCOP.  */

static void
scop_record_loop (scop_p scop, struct loop *loop)
{
  if (!bitmap_bit_p (SCOP_LOOPS (scop), loop->num))
    {
      bitmap_set_bit (SCOP_LOOPS (scop), loop->num);
      VEC_safe_push (loop_p, heap, SCOP_LOOP_NEST (scop), loop);
    }
}

/* Build the loop nests contained in SCOP.  */

static void
build_scop_loop_nests (scop_p scop)
{
  unsigned i;
  graphite_bb_p gb;
  struct loop *loop0, *loop1;

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    {
      struct loop *loop = gbb_loop (gb);

      /* Only add loops, if they are completely contained in the SCoP.  */
      if (loop->header == GBB_BB (gb) && bb_in_scop_p (loop->latch, scop))
        scop_record_loop (scop, gbb_loop (gb));
    }

  /* Make sure that the loops in the SCOP_LOOP_NEST are ordered.  It
     can be the case that an inner loop is inserted before an outer
     loop.  For avoiding this, semi-sort once.  */
  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, loop0); i++)
    {
      if (VEC_length (loop_p, SCOP_LOOP_NEST (scop)) == i + 1)
	break;

      loop1 = VEC_index (loop_p, SCOP_LOOP_NEST (scop), i + 1);
      if (loop0->num > loop1->num)
	{
	  VEC_replace (loop_p, SCOP_LOOP_NEST (scop), i, loop1);
	  VEC_replace (loop_p, SCOP_LOOP_NEST (scop), i + 1, loop0);
	}
    }
}

/* Build dynamic schedules for all the BBs. */

static void
build_scop_dynamic_schedules (scop_p scop)
{
  unsigned i, dim, loop_num, row, col;
  graphite_bb_p gb;


  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    {
      loop_num = GBB_BB (gb) -> loop_father -> num; 
      if (loop_num != 0)
        {
          dim = loop_iteration_vector_dim (loop_num, scop);
          GBB_DYNAMIC_SCHEDULE (gb) = cloog_matrix_alloc (dim, dim);
          for (row = 0; row < GBB_DYNAMIC_SCHEDULE (gb)->NbRows; row++)
            for (col = 0; col < GBB_DYNAMIC_SCHEDULE (gb)->NbColumns; col++)
              if (row == col)
                {
                  value_init (GBB_DYNAMIC_SCHEDULE (gb)->p[row][col]);
                  value_set_si (GBB_DYNAMIC_SCHEDULE (gb)->p[row][col], 1);
                }
              else  
                {
                  value_init (GBB_DYNAMIC_SCHEDULE (gb)->p[row][col]);
                  value_set_si (GBB_DYNAMIC_SCHEDULE (gb)->p[row][col], 0);
                }
        }
      else
        GBB_DYNAMIC_SCHEDULE (gb) = NULL;
    }
}

/* Build for BB the static schedule.

   The STATIC_SCHEDULE is defined like this:

   A
   for (i: ...)
     {
       for (j: ...)
         {
           B
           C 
         }

       for (k: ...)
         {
           D
           E 
         }
     }
   F

   Static schedules for A to F:

     ? i j k
   A 0 0 0 0
   B 1 0 0 0
   C 1 0 1 0
   D 1 1 0 0
   E 1 1 0 1
   F 2 0 0 0  */

static void
build_scop_canonical_schedules (scop_p scop)
{
  unsigned i, j;
  graphite_bb_p gb;
  unsigned nb = scop_nb_loops (scop) + 1;

  SCOP_STATIC_SCHEDULE (scop) = lambda_vector_new (nb);

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    {
      struct loop *loop = gbb_loop (gb);
      int offset = 0;

      /* Shift one step, to make space for outermost layer
         not contained in any SCoP.  */
      if (scop_contains_loop (scop, loop))
        offset = scop_loop_index (scop, gbb_loop (gb)) + 1;

      /* After leaving a loop, it is possible that the schedule is not
	 set at zero.  This loop reinitializes components located
	 after OFFSET.  */

      for (j = offset + 1; j < nb; j++)
	if (SCOP_STATIC_SCHEDULE (scop)[j])
	  {
	    memset (&(SCOP_STATIC_SCHEDULE (scop)[j]), 0,
		    sizeof (int) * (nb - j));
	    ++SCOP_STATIC_SCHEDULE (scop)[offset];
	    break;
	  }

      GBB_STATIC_SCHEDULE (gb) = lambda_vector_new (nb);
      lambda_vector_copy (SCOP_STATIC_SCHEDULE (scop), 
			  GBB_STATIC_SCHEDULE (gb), nb);

      ++SCOP_STATIC_SCHEDULE (scop)[offset];
    }
}

/* Get the index for parameter VAR in SCOP.  */

static int
param_index (tree var, scop_p scop)
{
  int i;
  name_tree p;
  name_tree nvar = XNEW (struct name_tree);

  gcc_assert (TREE_CODE (var) == SSA_NAME);

  for (i = 0; VEC_iterate (name_tree, SCOP_PARAMS (scop), i, p); i++)
    if (p->t == var)
      return i;

  nvar->t = var;
  nvar->name = NULL;
  VEC_safe_push (name_tree, heap, SCOP_PARAMS (scop), nvar);
  return VEC_length (name_tree, SCOP_PARAMS (scop)) - 1;
}

/* Scan EXPR and translate it to an inequality vector INEQ that will
   be inserted in the constraint domain matrix C at row R.  N is
   the number of columns for loop iterators in C.  */

static void
scan_tree_for_params (scop_p s, tree e, CloogMatrix *c, int r, Value k)
{
  unsigned cst_col, param_col;

  switch (TREE_CODE (e))
    {
    case POLYNOMIAL_CHREC:
      {
	tree left = CHREC_LEFT (e);
	tree right = CHREC_RIGHT (e);
	int var = CHREC_VARIABLE (e);

	if (TREE_CODE (right) != INTEGER_CST)
	  return;

	if (c)
	  {
	    unsigned var_idx = loop_iteration_vector_dim (var, s);
	    value_init (c->p[r][var_idx]);
	    value_set_si (c->p[r][var_idx], int_cst_value (right));
	  }

	switch (TREE_CODE (left))
	  {
	  case POLYNOMIAL_CHREC:
	    scan_tree_for_params (s, left, c, r, k);
            return;

	  case INTEGER_CST:
	    /* Constant part.  */
	    if (c)
	      {
		cst_col = c->NbColumns - 1;
		value_init (c->p[r][cst_col]);
		value_set_si (c->p[r][cst_col], int_cst_value (left));
	      }
	    return;

	  default:
            {
              scan_tree_for_params (s, left, c, r, k);
              return;
            }
	  }
      }
      break;

    case MULT_EXPR:
      if (chrec_contains_symbols (TREE_OPERAND (e, 0)))
	{
	  Value val;

	  gcc_assert (host_integerp (TREE_OPERAND (e, 1), 1));

	  value_init (val);
	  value_set_si (val, int_cst_value (TREE_OPERAND (e, 1)));
	  value_multiply (k, k, val);
	  value_clear (val);
	  scan_tree_for_params (s, TREE_OPERAND (e, 0), c, r, k);
	}
      else
	{
	  Value val;

	  gcc_assert (host_integerp (TREE_OPERAND (e, 0), 1));

	  value_init (val);
	  value_set_si (val, int_cst_value (TREE_OPERAND (e, 0)));
	  value_multiply (k, k, val);
	  value_clear (val);
	  scan_tree_for_params (s, TREE_OPERAND (e, 1), c, r, k);
	}
      break;

    case PLUS_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, r, k);
      scan_tree_for_params (s, TREE_OPERAND (e, 1), c, r, k);
      break;

    case MINUS_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, r, k);
      value_oppose (k, k);
      scan_tree_for_params (s, TREE_OPERAND (e, 1), c, r, k);
      break;

    case SSA_NAME:
      param_col = param_index (e, s);

      if (c)
	{
          param_col += c->NbColumns - nb_params_in_scop (s);
	  value_init (c->p[r][param_col]);
	  value_assign (c->p[r][param_col], k);
	}
      break;


    case INTEGER_CST:
      if (c)
	{
	  cst_col = c->NbColumns - 1;
	  value_init (c->p[r][cst_col]);
	  value_set_si (c->p[r][cst_col], int_cst_value (e));
	}
      break;

    case NOP_EXPR:
    case CONVERT_EXPR:
    case NON_LVALUE_EXPR:
      scan_tree_for_params (s, TREE_OPERAND (e, 0), c, r, k);
      break;

    default:
      break;
    }
}


struct irp_data
{
  struct loop *loop;
  scop_p scop;
};

/* Record parameters occurring in an evolution function of a data
   access, niter expression, etc.  Callback for for_each_index.  */

static bool
idx_record_params (tree base, tree *idx, void *dta)
{
  struct irp_data *data = dta;

  if (TREE_CODE (base) != ARRAY_REF)
    return true;

  if (TREE_CODE (*idx) == SSA_NAME)
    {
      tree scev;
      scop_p scop = data->scop;
      struct loop *loop = data->loop;

      scev = analyze_scalar_evolution (loop, *idx);
      scev = instantiate_scev (outermost_loop_in_scop (scop, loop->header),
			       loop, scev);

      {
	Value one;

	value_init (one);
	value_set_si (one, 1);
	scan_tree_for_params (scop, scev, NULL, 0, one);
	value_clear (one);
      }
    }

  return true;
}

/* Helper function for walking in dominance order basic blocks.  Find
   parameters with respect to SCOP in memory access functions used in
   BB.  DW_DATA contains the context and the results for extract
   parameters information.  */

static void
find_params_in_bb (struct dom_walk_data *dw_data, basic_block bb)
{
  unsigned i;
  data_reference_p dr;
  VEC (data_reference_p, heap) *drs;
  block_stmt_iterator bsi;
  scop_p scop = (scop_p) dw_data->global_data;
  struct loop *nest = outermost_loop_in_scop (scop, bb);

  /* Find the parameters used in the memory access functions.  */
  drs = VEC_alloc (data_reference_p, heap, 5);
  for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
    find_data_references_in_stmt (nest, bsi_stmt (bsi), &drs);

  for (i = 0; VEC_iterate (data_reference_p, drs, i, dr); i++)
    {
      struct irp_data irp;

      irp.loop = bb->loop_father;
      irp.scop = scop;
      for_each_index (&dr->ref, idx_record_params, &irp);
    }

  VEC_free (data_reference_p, heap, drs);
}

/* Initialize Cloog's parameter names from the names used in GIMPLE.
   Initialize Cloog's iterator names, using 'graphite_iterator_%d'
   from 0 to scop_nb_loops (scop).  */

static void
initialize_cloog_names (scop_p scop)
{
  unsigned i, nb_params = VEC_length (name_tree, SCOP_PARAMS (scop));
  char **params = XNEWVEC (char *, nb_params);
  unsigned nb_iterators = scop_nb_loops(scop);
  unsigned nb_scattering= SCOP_PROG (scop)->nb_scattdims;
  char **iterators = XNEWVEC (char *, nb_iterators);
  char **scattering = XNEWVEC (char *, nb_scattering);
  name_tree p;

  for (i = 0; VEC_iterate (name_tree, SCOP_PARAMS (scop), i, p); i++)
    {
      const char *name = get_name (SSA_NAME_VAR (p->t));

      if (name)
	{
	  params[i] = XNEWVEC (char, strlen (name) + 12);
	  sprintf (params[i], "%s_%d", name, SSA_NAME_VERSION (p->t));
	}
      else
	{
	  params[i] = XNEWVEC (char, 12);
	  sprintf (params[i], "T_%d", SSA_NAME_VERSION (p->t));
	}

      p->name = params[i];
    }

  SCOP_PROG (scop)->names->nb_parameters = nb_params;
  SCOP_PROG (scop)->names->parameters = params;

  for (i = 0; i < nb_iterators; i++)
    {
      iterators[i] = XNEWVEC (char, 18 + 12);
      sprintf (iterators[i], "graphite_iterator_%d", i);
    }

  SCOP_PROG (scop)->names->nb_iterators = nb_iterators;
  SCOP_PROG (scop)->names->iterators = iterators;

  for (i = 0; i < nb_scattering; i++)
    {
      scattering[i] = XNEWVEC (char, 2 + 12);
      sprintf (scattering[i], "s_%d", i);
    }

  SCOP_PROG (scop)->names->nb_scattering = nb_scattering;
  SCOP_PROG (scop)->names->scattering = scattering;
}

/* Record the parameters used in the SCOP.  A variable is a parameter
   in a scop if it does not vary during the execution of that scop.  */

static void
find_scop_parameters (scop_p scop)
{
  unsigned i;
  struct dom_walk_data walk_data;
  struct loop *loop;

  /* Find the parameters used in the loop bounds.  */
  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, loop); i++)
    {
      tree nb_iters = number_of_latch_executions (loop);

      if (chrec_contains_symbols (nb_iters))
	{
	  struct irp_data irp;

	  irp.loop = loop;
	  irp.scop = scop;
          nb_iters = analyze_scalar_evolution (loop, nb_iters);
          nb_iters = instantiate_scev (outermost_loop_in_scop (scop,
							       loop->header),
				       loop, nb_iters);
          
	  {
	    Value one;
	    value_init (one);
	    value_set_si (one, 1);
	    scan_tree_for_params (scop, nb_iters, NULL, 0, one);
	    value_clear (one);
	  }
	}
    }

  /* Find the parameters used in data accesses.  */
  memset (&walk_data, 0, sizeof (struct dom_walk_data));
  walk_data.before_dom_children_before_stmts = find_params_in_bb;
  walk_data.dom_direction = CDI_DOMINATORS;
  walk_data.global_data = scop;
  init_walk_dominator_tree (&walk_data);
  walk_dominator_tree (&walk_data, SCOP_ENTRY (scop));
  fini_walk_dominator_tree (&walk_data);
}

/* Build the context constraints for SCOP: constraints and relations
   on parameters.  */

static void
build_scop_context (scop_p scop)
{
  unsigned nb_params = nb_params_in_scop (scop);
  CloogMatrix *matrix = cloog_matrix_alloc (1, nb_params + 2);

  /* Insert '0 >= 0' in the context matrix, as it is not allowed to be
     empty. */
 
  value_init (matrix->p[0][0]);
  value_set_si (matrix->p[0][0], 1);

  value_init (matrix->p[0][nb_params + 1]);
  value_set_si (matrix->p[0][nb_params + 1], 0);

  SCOP_PROG (scop)->context = cloog_domain_matrix2domain (matrix);
}

/* Calculate the number of loops around GB in the current SCOP.  */

static inline int
nb_loops_around_gb (graphite_bb_p gb)
{
  scop_p scop = GBB_SCOP (gb);
  struct loop *l = gbb_loop (gb);
  int d = 0;

  for (; loop_in_scop_p (l, scop); d++, l = loop_outer (l));

  return d;
}

/* Returns a graphite_bb from BB.  */

static graphite_bb_p
graphite_bb_from_bb (basic_block bb, scop_p scop)
{
  graphite_bb_p gb;
  unsigned i;

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb); i++)
    if (GBB_BB (gb) == bb)
      return gb;

  gcc_unreachable ();
  return NULL;
}

/* Adds to all bb's in LOOP the DOMAIN.  */

static void
add_bb_domains (struct loop *loop, scop_p scop, CloogMatrix *domain)
{
  basic_block *bbs = get_loop_body (loop);
  unsigned i;

  for (i = 0; i < loop->num_nodes; i++)
    if (bbs[i]->loop_father == loop)
      {
        graphite_bb_p gbb = graphite_bb_from_bb (bbs[i], scop);
        GBB_DOMAIN (gbb) = cloog_matrix_copy (domain);
      }

  free (bbs);
}

/* Builds the constraint matrix for LOOP in SCOP.  NB_OUTER_LOOPS is the
   number of loops surrounding LOOP in SCOP.  OUTER_CSTR gives the
   constraints matrix for the surrounding loops.  */

static void
build_loop_iteration_domains (scop_p scop, struct loop *loop,
                              CloogMatrix *outer_cstr, int nb_outer_loops)
{
  unsigned i, j, row;
  CloogMatrix *cstr;
  struct loop_to_cloog_loop_str tmp;
  PTR *slot;
  CloogLoop *res = cloog_loop_malloc ();

  unsigned nb_rows = outer_cstr->NbRows + 1;
  unsigned nb_cols = outer_cstr->NbColumns + 1;

  /* Last column of CSTR is the column of constants.  */
  unsigned cst_col = nb_cols - 1;

  /* The column for the current loop is just after the columns of
     other outer loops.  */
  unsigned loop_col = nb_outer_loops + 1;

  tree nb_iters = number_of_latch_executions (loop);

  /* When the number of iterations is a constant or a parameter, we
     add a constraint for the upper bound of the loop.  So add a row
     to the constraint matrix before allocating it.  */
  if (TREE_CODE (nb_iters) == INTEGER_CST
      || !chrec_contains_undetermined (nb_iters))
    nb_rows++;

  cstr = cloog_matrix_alloc (nb_rows, nb_cols);

  /* Copy the outer constraints.  */
  for (i = 0; i < outer_cstr->NbRows; i++)
    {
      /* Copy the eq/ineq and loops columns.  */
      for (j = 0; j < loop_col; j++)
        {
          value_init (cstr->p[i][j]);
          value_assign (cstr->p[i][j], outer_cstr->p[i][j]);
        }

      /* Leave an empty column in CSTR for the current loop, and then
        copy the parameter columns.  */
      for (j = loop_col; j < outer_cstr->NbColumns; j++)
        {
          value_init (cstr->p[i][j + 1]);
          value_assign (cstr->p[i][j + 1], outer_cstr->p[i][j]);
        }
    }

  /* 0 <= loop_i */
  row = outer_cstr->NbRows;
  value_init (cstr->p[row][0]);
  value_set_si (cstr->p[row][0], 1);
  value_init (cstr->p[row][loop_col]);
  value_set_si (cstr->p[row][loop_col], 1);

  /* loop_i <= nb_iters */
  if (TREE_CODE (nb_iters) == INTEGER_CST)
    {
      row++;
      value_init (cstr->p[row][0]);
      value_set_si (cstr->p[row][0], 1);
      value_init (cstr->p[row][loop_col]);
      value_set_si (cstr->p[row][loop_col], -1);

      value_init (cstr->p[row][cst_col]);
      value_set_si (cstr->p[row][cst_col],
		    int_cst_value (nb_iters));
    }
  else if (!chrec_contains_undetermined (nb_iters))
    {
      /* Otherwise nb_iters contains parameters: scan the nb_iters
	 expression and build its matrix representation.  */
      Value one;

      row++;
      value_init (cstr->p[row][0]);
      value_set_si (cstr->p[row][0], 1);
      value_init (cstr->p[row][loop_col]);
      value_set_si (cstr->p[row][loop_col], -1);
      nb_iters = analyze_scalar_evolution (loop, nb_iters);
      nb_iters = 
        instantiate_scev (outermost_loop_in_scop (scop, loop->header),
			  loop, nb_iters);
      value_init (one);
      value_set_si (one, 1);
      scan_tree_for_params (scop, nb_iters, cstr, row, one);
      value_clear (one);
    }

  /* XXX: Disabled, as CloogLoops are only generated in find_transforms.
     To connect graphite bbs and gimple loops, we have to use a different
     representation.  */
  if (0)  
    { 
      tmp.loop_num = loop->num;
      slot = htab_find_slot (SCOP_LOOP2CLOOG_LOOP (scop), &tmp, INSERT);
        if (!*slot)
          *slot = new_loop_to_cloog_loop_str (loop->num, loop_col - 1, res);
    }

  if (loop->inner && loop_in_scop_p (loop->inner, scop))
    build_loop_iteration_domains (scop, loop->inner, cstr, nb_outer_loops + 1);

  /* Only go to the next loops, if we are not at the outermost layer.  These
     have to be handled seperately, as we can be sure, that the chain at this
     layer will be connected.  */
  if (nb_outer_loops != 0 && loop->next && loop_in_scop_p (loop->next, scop))
    build_loop_iteration_domains (scop, loop->next, outer_cstr, nb_outer_loops);

  add_bb_domains (loop, scop, cstr);

  cloog_matrix_free (cstr);
}

/* Converts the graphite scheduling function into a cloog scattering
   matrix.  This scattering matrix is used to limit the possible cloog
   output to valid programs in respect to the scheduling function. 

   SCATTERING_DIMENSIONS specifies the dimensionality of the scattering
   matrix. CLooG 0.14.0 and previous versions require, that all scattering
   functions of one CloogProgram have the same dimensionality, therefore we
   allow to specify it. (Should be removed in future versions)  */

static CloogMatrix *
schedule_to_scattering (graphite_bb_p gb, int scattering_dimensions) 
{
  int i;
  scop_p scop = GBB_SCOP (gb);
  struct loop *loop;

  int nb_iterators = nb_loops_around_gb (gb);

  /* The cloog scattering matrix consists of these colums:
     1                        col  = Eq/Inq,
     scattering_dimensions    cols = Scattering dimensions,
     nb_iterators             cols = bb's iterators,
     nb_params_in_scop        cols = Parameters,
     1                        col  = Constant 1.

     Example:

     scattering_dimensions = 5
     max_nb_iterators = 2
     nb_iterators = 1 
     nb_params_in_scop = 2

     Schedule:
     ? i
     4 5

     Scattering Matrix:
     s1  s2  s3  s4  s5  i   p1  p2  1 
     1   0   0   0   0   0   0   0  -4  = 0
     0   1   0   0   0  -1   0   0   0  = 0
     0   0   1   0   0   0   0   0  -5  = 0  */
  int nb_params = nb_params_in_scop (scop);
  int nb_cols = 1 + scattering_dimensions + nb_iterators + nb_params + 1;
  int col_const = nb_cols - 1; 
  int col_iter_offset = 1 + scattering_dimensions;

  CloogMatrix *scat = cloog_matrix_alloc (scattering_dimensions, nb_cols);

  gcc_assert (scattering_dimensions >= nb_iterators * 2 + 1);

  /* Initialize the identity matrix.  */
  for (i = 0; i < scattering_dimensions; i++)
    {
      value_init (scat->p[i][i + 1]);
      value_set_si (scat->p[i][i + 1], 1);
    }

  /* Textual order outside the first loop */
  value_init (scat->p[0][col_const]);
  value_set_si (scat->p[0][col_const], -GBB_STATIC_SCHEDULE (gb)[0]);

  loop = gbb_loop (gb);

  /* For all surrounding loops.  */
  for (i = nb_iterators - 1;  i >= 0; i--)
    {
      int schedule = GBB_STATIC_SCHEDULE (gb)[scop_loop_index (scop, loop) + 1];

      /* Iterations of this loop.  */
      value_init (scat->p[2 * i + 1][col_iter_offset + i]);
      value_set_si (scat->p[2 * i + 1][col_iter_offset + i], -1);

      /* Textual order inside this loop.  */
      value_init (scat->p[2 * i + 2][col_const]);
      value_set_si (scat->p[2 * i + 2][col_const], -schedule);

      loop = loop_outer (loop);
    }

  return scat; 
}

/* Build the current domain matrix: the loops belonging to the current
   SCOP, and that vary for the execution of the current basic block.
   Returns false if there is no loop in SCOP.  */

static bool
build_scop_iteration_domain (scop_p scop)
{
  struct loop *loop;
  CloogMatrix *outer_cstr;
  int i;

  /* Build cloog loop for all loops, that are in the uppermost loop layer of
     this SCoP.  */
  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST(scop), i, loop); i++)
    if (!loop_in_scop_p (loop_outer (loop), scop))
      {
        /* The outermost constraints is a matrix that has:
           -first column: eq/ineq boolean
           -last column: a constant
           -nb_params_in_scop columns for the parameters used in the scop.  */
       outer_cstr = cloog_matrix_alloc (0, nb_params_in_scop (scop) + 2);
       build_loop_iteration_domains (scop, loop, outer_cstr, 0);
       cloog_matrix_free (outer_cstr);
     }

  return (i != 0);
}

/* Initializes an equation CY of the access matrix using the
   information for a subscript from ACCESS_FUN, relatively to the loop
   indexes from LOOP_NEST and parameter indexes from PARAMS.  NDIM is
   the dimension of the array access, i.e. the number of
   subscripts.  Returns true when the operation succeeds.  */

static bool
build_access_matrix_with_af (tree access_fun, lambda_vector cy,
			     scop_p scop, unsigned ndim)
{
  switch (TREE_CODE (access_fun))
    {
    case POLYNOMIAL_CHREC:
      {
	tree left = CHREC_LEFT (access_fun);
	tree right = CHREC_RIGHT (access_fun);
	unsigned var;

	if (TREE_CODE (right) != INTEGER_CST)
	  return false;
        
	var = loop_iteration_vector_dim (CHREC_VARIABLE (access_fun), scop);
	cy[var] = int_cst_value (right);

	switch (TREE_CODE (left))
	  {
	  case POLYNOMIAL_CHREC:
	    return build_access_matrix_with_af (left, cy, scop, ndim);

	  case INTEGER_CST:
	    /* Constant part.  */
	    cy[ndim - 1] = int_cst_value (left);
	    return true;

	  default:
	    /* TODO: also consider that access_fn can have parameters.  */
	    return false;
	  }
      }
    case INTEGER_CST:
      /* Constant part.  */
      cy[ndim - 1] = int_cst_value (access_fun);
      return true;

    default:
      return false;
    }
}

/* Initialize the access matrix in the data reference REF with respect
   to the loop nesting LOOP_NEST.  Return true when the operation
   succeeded.  */

static bool
build_access_matrix (data_reference_p ref, graphite_bb_p gb)
{
  unsigned i, ndim = DR_NUM_DIMENSIONS (ref);
  struct access_matrix *am = GGC_NEW (struct access_matrix);

  AM_MATRIX (am) = VEC_alloc (lambda_vector, heap, ndim);
  DR_SCOP (ref) = GBB_SCOP (gb);

  for (i = 0; i < ndim; i++)
    {
      lambda_vector v = lambda_vector_new (ref_nb_loops (ref));
      scop_p scop = GBB_SCOP (gb);
      tree af = DR_ACCESS_FN (ref, i);

      if (!build_access_matrix_with_af (af, v, scop, ref_nb_loops (ref)))
	return false;

      VEC_safe_push (lambda_vector, heap, AM_MATRIX (am), v);
    }

  DR_ACCESS_MATRIX (ref) = am;
  return true;
}

/* Build the access matrices for the data references in the SCOP.  */

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
      struct loop *nest = outermost_loop_in_scop (scop, GBB_BB (gb));

      /* On each statement of the basic block, gather all the occurences
	 to read/write memory.  */
      GBB_DATA_REFS (gb) = VEC_alloc (data_reference_p, heap, 5);
      for (bsi = bsi_start (GBB_BB (gb)); !bsi_end_p (bsi); bsi_next (&bsi))
	find_data_references_in_stmt (nest, bsi_stmt (bsi),
				      &GBB_DATA_REFS (gb));

      /* Construct the access matrix for each data ref, with respect to
	 the loop nest of the current BB in the considered SCOP.  */
      for (j = 0; VEC_iterate (data_reference_p, GBB_DATA_REFS (gb), j, dr); j++)
	build_access_matrix (dr, gb);
    }
}

/* Converts a GMP constant value to a tree and returns it.  */

static tree
gmp_cst_to_tree (Value v)
{
  return build_int_cst (integer_type_node, value_get_si (v));
}

/* Returns the tree variable from the name that it was given in Cloog
   representation.  

   FIXME: This is a hack, and Cloog should be fixed to not work with
   variable names represented as "char *string", but with void
   pointers that could be casted back to a tree.  The only problem in
   doing that is that Cloog's pretty printer still assumes that
   variable names are char *strings.  The solution would be to have a
   function pointer for pretty-printing that can be redirected to be
   print_generic_stmt in our case, or fprintf by default.  */

static tree
clast_name_to_gcc (const char *name, VEC (name_tree, heap) *new_ivs,
		   VEC (name_tree, heap) *params)
{
  unsigned i;
  name_tree t;

  for (i = 0; VEC_iterate (name_tree, new_ivs, i, t); i++)
    if (!strcmp (name, t->name))
      return t->t;

  for (i = 0; VEC_iterate (name_tree, params, i, t); i++)
    if (!strcmp (name, t->name))
      return t->t;

  gcc_unreachable ();
  return NULL_TREE;
}

/* Converts a Cloog AST back to a GCC expression tree.  */

static tree
clast_to_gcc_expression (struct clast_expr *e, tree type,
			 VEC (name_tree, heap) *new_ivs,
			 VEC (name_tree, heap) *params)
{
  gcc_assert (e);

  switch (e->type)
    {
    case expr_term:
      {
	struct clast_term *t = (struct clast_term *) e;

	if (t->var)
	  {
	    if (value_one_p (t->val))
 	      return clast_name_to_gcc (t->var, new_ivs, params);

	    else if (value_mone_p (t->val))
	      return fold_build1 (NEGATE_EXPR, integer_type_node,
				  clast_name_to_gcc (t->var, new_ivs, params));
	    else
	      return fold_build2 (MULT_EXPR, integer_type_node,
				  gmp_cst_to_tree (t->val),
				  clast_name_to_gcc (t->var, new_ivs, params));
	  }
	else
	  return gmp_cst_to_tree (t->val);
      }

    case expr_red:
      {
        struct clast_reduction *r = (struct clast_reduction *) e;
        switch (r->type)
          {
            case clast_red_min:
            case clast_red_max:
              if (r->n == 1)
		return clast_to_gcc_expression (r->elts[0], type, new_ivs, params);

            default:
              gcc_unreachable ();
          }
        break;
      }

    case expr_bin: 
      {
	struct clast_binary *b = (struct clast_binary *) e;
	struct clast_expr *lhs = (struct clast_expr *) b->LHS;
	struct clast_expr *rhs = (struct clast_expr *) b->RHS;
	tree tl = clast_to_gcc_expression (lhs, type, new_ivs, params);
	tree tr = clast_to_gcc_expression (rhs, type, new_ivs, params);

	switch (b->type)
	  {
	  case clast_bin_fdiv:
	    return fold_build2 (FLOOR_DIV_EXPR, type, tl, tr);

	  case clast_bin_cdiv:
	    return fold_build2 (CEIL_DIV_EXPR, type, tl, tr);

	  case clast_bin_div:
	    return fold_build2 (EXACT_DIV_EXPR, type, tl, tr);

	  case clast_bin_mod:
	    return fold_build2 (TRUNC_MOD_EXPR, type, tl, tr);

	  default:
	    gcc_unreachable ();
	  }
      }

    default:
      gcc_unreachable ();
    }

  return NULL_TREE;
}

/* Creates and inserts an induction variable for the new LOOP,
   corresponding to Cloog's STMT.  */

static struct loop *
graphite_create_new_loop (scop_p scop, edge header_edge,
			  struct clast_for *stmt)
{
  int prob;
  edge true_edge, false_edge;
  basic_block loop_header, loop_latch, succ_bb, pred_bb, switch_bb;
  struct loop *loop;
  tree ivvar;
  block_stmt_iterator bsi;
  bool insert_after;
  tree stmts, stride, cond_expr, lowb, upb;
  edge exit_e;
  
  switch_bb = create_empty_bb (EXIT_BLOCK_PTR->prev_bb);
  cond_expr = build3 (COND_EXPR, void_type_node, boolean_false_node,
		      NULL_TREE, NULL_TREE);
  bsi = bsi_after_labels (switch_bb);
  bsi_insert_after (&bsi, cond_expr, BSI_NEW_STMT);

  
  pred_bb = header_edge->src;
  loop_header = split_edge (header_edge);
  loop_latch = split_edge (single_succ_edge (loop_header));
  succ_bb = single_succ (loop_latch);

  make_edge (loop_header, succ_bb, 0);
  set_immediate_dominator (CDI_DOMINATORS, loop_header, switch_bb);
  set_immediate_dominator (CDI_DOMINATORS, loop_latch, loop_header);

  false_edge = make_edge (switch_bb, loop_header, 0);
  true_edge = make_edge (switch_bb, succ_bb, EDGE_FALLTHRU);

  prob = REG_BR_PROB_BASE / 2;
  loop = loopify (single_succ_edge (loop_latch), header_edge, switch_bb, 
		  true_edge, false_edge, 1, prob, REG_BR_PROB_BASE - prob);

  /* Create the new induction variable.  */
  stride = gmp_cst_to_tree (stmt->stride);
  lowb = clast_to_gcc_expression (stmt->LB, integer_type_node,
				  SCOP_NEWIVS (scop), SCOP_PARAMS (scop));
  ivvar = create_tmp_var (integer_type_node, "graphiteIV");

  lowb = force_gimple_operand (lowb, &stmts, true, ivvar);
  if (stmts)
    {
      bsi_insert_on_edge (loop_preheader_edge (loop), stmts);
      bsi_commit_edge_inserts ();
    }

  add_referenced_var (ivvar);
  standard_iv_increment_position (loop, &bsi, &insert_after);
  create_iv (lowb, stride, ivvar, loop, &bsi, insert_after, &ivvar, NULL);

  /* Create the loop exit condition.  */
  upb = clast_to_gcc_expression (stmt->UB, integer_type_node,
				 SCOP_NEWIVS (scop), SCOP_PARAMS (scop));
  exit_e = single_exit (loop);
  bsi = bsi_last (exit_e->src);

  upb = force_gimple_operand (upb, &stmts, true, NULL);
  if (stmts)
    bsi_insert_after (&bsi, stmts, BSI_NEW_STMT);

  cond_expr = build2 (LT_EXPR, boolean_type_node, upb, ivvar);
  cond_expr = build3 (COND_EXPR, void_type_node, cond_expr,
		      NULL_TREE, NULL_TREE);
  bsi_insert_after (&bsi, cond_expr, BSI_NEW_STMT);

  return loop;
}

/* Remove all the edges from BB.  */

static void
remove_all_edges (basic_block bb)
{
  edge e;
  edge_iterator ei;

  for (ei = ei_start (bb->succs); (e = ei_safe_edge (ei)); )
    remove_edge (e);

  for (ei = ei_start (bb->preds); (e = ei_safe_edge (ei)); )
    remove_edge (e);
}

/* Translates a CLAST statement STMT to GCC representation.  */

static edge
translate_clast (scop_p scop, struct clast_stmt *stmt, edge next_e)
{
  if (!stmt)
    return next_e;

  switch (stmt->type) 
    {
    case stmt_root:
      return translate_clast (scop, stmt->next, next_e);

    case stmt_user:
      {
	CloogStatement *cs = ((struct clast_user_stmt *) stmt)->statement;
	graphite_bb_p gbb = (graphite_bb_p) cs->usr;
	basic_block bb = gbb->bb;

	remove_all_edges (bb);
	redirect_edge_succ_nodup (next_e, bb);
	next_e = make_edge (bb, EXIT_BLOCK_PTR, 0);

#if 0
	graphite_rename_ivs ();
#endif

	return translate_clast (scop, stmt->next, next_e);
      }

    case stmt_for:
      {
	struct loop *loop = graphite_create_new_loop (scop, next_e,
						      (struct clast_for *) stmt);
	edge last_e = single_exit (loop);

	next_e = translate_clast (scop, ((struct clast_for *) stmt)->body,
				  single_pred_edge (loop->latch));
	redirect_edge_succ_nodup (next_e, loop->latch);
	return translate_clast (scop, stmt->next, last_e);
      }

    case stmt_block:
      next_e = translate_clast (scop, ((struct clast_block *) stmt)->body, next_e);
      return translate_clast (scop, stmt->next, next_e);

    case stmt_guard:
    case stmt_ass:
    default:
      /* NIY.  */
      return next_e;
    }
}

/* Build cloog program for SCoP.  */

static void
build_cloog_prog (scop_p scop)
{
  int i;
  int max_nb_loops = 0;
  graphite_bb_p gbb;
  CloogLoop *loop_list = NULL;
  CloogBlockList *block_list = NULL;
  CloogDomainList *scattering = NULL;
  CloogProgram *prog = SCOP_PROG (scop);
  
  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gbb); i++)
    {
      int nb_loops = nb_loops_around_gb (gbb);
      if (max_nb_loops < nb_loops)
        max_nb_loops = nb_loops;
    }

  prog->nb_scattdims = 2 * max_nb_loops + 1; 

  initialize_cloog_names (scop);

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gbb); i++)
    {
      /* Build new block.  */
      CloogMatrix *domain = GBB_DOMAIN (gbb);
      CloogStatement *stmt = cloog_statement_alloc (GBB_BB (gbb)->index);
      CloogBlock *block = cloog_block_alloc (stmt, NULL, 0, NULL,
					     nb_loops_around_gb (gbb));                           
      stmt->usr = gbb;

      /* Add empty domain to all bbs, which do not yet have a domain, as they
         are not part of any loop.  */
      if (domain == NULL)
	domain = cloog_matrix_alloc (0, nb_params_in_scop (scop) + 2);

      /* Build loop list.  */
      {
        CloogLoop *new_loop_list = cloog_loop_malloc ();
        new_loop_list->next = loop_list;
        new_loop_list->domain = cloog_domain_matrix2domain (domain);
        new_loop_list->block = block; 
        loop_list = new_loop_list;
      }

      /* Build block list.  */
      {
        CloogBlockList *new_block_list = cloog_block_list_malloc ();
        new_block_list->next = block_list;
        new_block_list->block = block;
        block_list = new_block_list;
      }

      /* XXX: Unused cloog field.  Not necessary for scattering. Just here
         during developement to document this.  Should be removed in future gcc
         and cloog versions.  */
      block->scattering = schedule_to_scattering (gbb, prog->nb_scattdims);

      /* Build scattering list.  */
      {
        /* XXX: Replace with cloog_domain_list_alloc(), when available.  */
        CloogDomainList *new_scattering = xmalloc (sizeof (CloogDomainList));
        new_scattering->next = scattering;
        new_scattering->domain = cloog_domain_matrix2domain (block->scattering);
        scattering = new_scattering;
      }
    }

  prog->loop = loop_list;
  prog->blocklist = block_list;
  prog->scaldims = (int *) xmalloc (prog->nb_scattdims * (sizeof (int)));

  /* XXX: Work around some libcloog shortcomings.  Cedric will integrate this
     into cloog.  */
  gcc_assert (prog->scaldims);
    
  for (i = 0; i < prog->nb_scattdims; i++)
    prog->scaldims[i] = 0 ;

  /* Extract scalar dimensions to simplify the code generation problem.  */
  cloog_program_extract_scalars (prog, scattering);

  /* Apply scattering.  */
  cloog_program_scatter (prog, scattering);

  /* Iterators corresponding to scalar dimensions have to be extracted.  */
  cloog_names_scalarize (prog->names, prog->nb_scattdims, prog->scaldims);
}

/* Find the right transform for the SCOP, and return a Cloog AST
   representing the new form of the program.  */

static struct clast_stmt *
find_transform (scop_p scop)
{
  CloogOptions *options = cloog_options_malloc ();
  CloogProgram *prog;
  struct clast_stmt *stmt;
  
  /* Connect new cloog prog generation to graphite.  */
  build_cloog_prog (scop);

  /* Change cloog output language to C.  If we do use FORTRAN instead, cloog
     will stop e.g. with "ERROR: unbounded loops not allowed in FORTRAN.", if
     we pass an incomplete program to cloog.  */
  options->language = LANGUAGE_C;

  /* Enable complex equality spreading: removes dummy statements
     (assignments) in the generated code which repeats the
     substitution equations for statements.  This is useless for
     GLooG. */
  options->esp = 1;

  /* Enable C pretty-printing mode: normalizes the substitution
     equations for statements */
  options->cpp = 1;

  /* Print the program we insert into cloog. */
  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "Cloog Input [\n");
      cloog_program_print (dump_file, SCOP_PROG(scop));
      fprintf (dump_file, "]\n");
    }

  /* FIXME: before cloog_program_generate call:
     cloog_program_scatter (p, scattering);  
     Without such a call, there is a risk of having Cloog abort.  */
  prog = cloog_program_generate (SCOP_PROG (scop), options);
  stmt = cloog_clast_create (prog, options);

  /* Print the program we get from cloog. */
  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "Cloog Output[\n");
      pprint (dump_file, stmt, 0, options);
      cloog_program_dump_cloog (dump_file, prog);
      fprintf (dump_file, "]\n");
    }

  return stmt;
}


/* GIMPLE Loop Generator: generates loops from STMT in GIMPLE form for
   the given SCOP.  */

static void
gloog (scop_p scop, struct clast_stmt *stmt)
{
  if (0)
    translate_clast (scop, stmt, single_succ_edge (SCOP_ENTRY (scop)));

  cloog_clast_free (stmt);
}

/* Returns a matrix representing the data dependence between memory
   accesses A and B in the context of SCOP.  */

static CloogMatrix *
initialize_dependence_polyhedron (scop_p scop, 
                                  struct data_reference *a, 
                                  struct data_reference *b)
{
  unsigned nb_cols, nb_rows, nb_params, nb_iter1, nb_iter2;
  struct loop_to_cloog_loop_str tmp, *slot1, *slot2; 
  unsigned row, col;
  CloogMatrix *domain1, *domain2;
  CloogMatrix *dep_constraints;
  lambda_vector access_row_vector;
  struct loop *containing_loop;
  Value value;

  containing_loop = loop_containing_stmt (DR_STMT (a));
  tmp.loop_num = containing_loop->num;
  slot1 = (struct loop_to_cloog_loop_str *) htab_find (SCOP_LOOP2CLOOG_LOOP(scop), &tmp); 
          
  containing_loop = loop_containing_stmt (DR_STMT (b));
  tmp.loop_num = containing_loop->num;
  slot2 = (struct loop_to_cloog_loop_str *) htab_find (SCOP_LOOP2CLOOG_LOOP(scop), &tmp); 
  /* TODO: insert checking for possible null values of slot1 and
     slot2.  */

  domain1 = cloog_domain_domain2matrix (slot1->cloog_loop->domain);
  domain2 = cloog_domain_domain2matrix (slot2->cloog_loop->domain);

  /* Adding 2 columns: one for the eq/neq column, one for constant
     term.  */
  
  nb_params = nb_params_in_scop (scop);
  nb_iter1 = domain1->NbColumns - 2 - nb_params;
  nb_iter2 = domain2->NbColumns - 2 - nb_params;

  nb_cols = nb_iter1 + nb_iter2 + nb_params_in_scop (scop) + 2;
  nb_rows = domain1->NbRows + domain2->NbRows + DR_NUM_DIMENSIONS (a) 
            + 2 * MIN (nb_iter1, nb_iter2);
  dep_constraints = cloog_matrix_alloc (nb_rows, nb_cols);

  /* Initialize dependence polyhedron.  TODO: do we need it?  */
  for (row = 0; row < dep_constraints->NbRows ; row++)
    for (col = 0; col < dep_constraints->NbColumns; col++)
      value_init (dep_constraints->p[row][col]);

  /* Copy the iterator part of Ds (domain of S statement), with eq/neq
     column.  */
  for (row = 0; row < domain1->NbRows; row++)
    for (col = 0; col <= nb_iter1; col++)
      value_assign (dep_constraints->p[row][col], domain1->p[row][col]);

  /* Copy the parametric and constant part of Ds.  */
  for (row = 0; row < domain1->NbRows; row++)
    {
      value_assign (dep_constraints->p[row][nb_cols-1],
		    domain1->p[row][domain1->NbColumns - 1]);
      for (col = 1; col <= nb_params; col++)
	value_assign (dep_constraints->p[row][col + nb_iter1 + nb_iter2],
		      domain1->p[row][col + nb_iter1]);
    }

  /* Copy the iterator part of Dt (domain of T statement), without eq/neq column.  */
  for (row = 0; row < domain2->NbRows; row++)
    for (col = 1; col <= nb_iter2; col++)
      value_assign (dep_constraints->p[row + domain1->NbRows][col + nb_iter2],
		    domain2->p[row][col]);
  
  /* Copy the eq/neq column of Dt to dependence polyhedron.  */
  for (row = 0; row < domain2->NbRows; row++)
    value_assign (dep_constraints->p[row + domain1->NbRows][0], domain2->p[row][0]);

  /* Copy the parametric and constant part of Dt.  */
  for (row = 0; row < domain2->NbRows; row++)
    {
      value_assign (dep_constraints->p[row + domain1->NbRows][nb_cols-1],
		    domain1->p[row][domain2->NbColumns - 1]);
      for (col = 1; col <= nb_params; col++)
        value_assign (dep_constraints->p[row + domain1->NbRows][col + nb_iter1 + nb_iter2],
                      domain2->p[row][col + nb_iter2]);
    }

  /* Copy Ds access matrix.  */
  for (row = 0; VEC_iterate (lambda_vector, AM_MATRIX (DR_ACCESS_MATRIX (a)),
			     row, access_row_vector); row++)
    {
      for (col = 1; col <= nb_iter1; col++)
	value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][col],
		      access_row_vector[col]);              

      value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_cols-1], 
                    access_row_vector[ref_nb_loops (a) - 1]);
      /* TODO: do not forget about parametric part.  */
    }
  value_init (value);
  /* Copy -Dt access matrix.  */
  for (row = 0; VEC_iterate (lambda_vector, AM_MATRIX (DR_ACCESS_MATRIX (b)),
			     row, access_row_vector); row++)
    {
      for (col = 1; col <= nb_iter2; col++)
	value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_iter1 + col], 
		      -access_row_vector[col]);              
      value_set_si (value, access_row_vector[ref_nb_loops (b) - 1]);
      value_subtract (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_cols-1],
                     dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_cols-1],
                     value);
    }
  value_clear (value);
  return dep_constraints;
}

/* Returns a new dependence polyhedron for data references A and B.  */

static struct data_dependence_polyhedron *
initialize_data_dependence_polyhedron (bool loop_carried,
                                       CloogDomain *domain,
                                       unsigned level,
                                       struct data_reference *a,
                                       struct data_reference *b)
{
  struct data_dependence_polyhedron *res;

  res = XNEW (struct data_dependence_polyhedron);
  res -> a = a;
  res -> b = b;
  res -> loop_carried = loop_carried;
  res -> level = level;

  if (loop_carried)
    res -> polyhedron = domain; 
  else
    res -> polyhedron = NULL;

  return res;
}

/* Returns true when the last row of DOMAIN polyhedron is zero.  */

static bool 
is_empty_polyhedron (CloogDomain *domain)
{
  Polyhedron *polyhedron;
  unsigned i, last_column, last_row;
  polyhedron = domain->polyhedron;
  last_column = polyhedron->Dimension + 2;
  last_row = polyhedron->NbConstraints - 1;

  for  (i = 1; i < last_column - 1; i++)
    if (!value_zero_p (polyhedron->Constraint[last_row][i]))
      return false;

  return !value_zero_p (polyhedron->Constraint[last_row][last_column - 1]);
}

/* Returns true if statement A, contained in basic block GB_A,
   precedes statement B, contained in basic block GB_B.  The decision
   is based on static schedule of basic block's and relative position
   of statements.  */

static bool 
statement_precedes_p (scop_p scop,
                      graphite_bb_p gb_a,
                      tree a,
                      graphite_bb_p gb_b,
                      tree b,
                      unsigned p)
{
  block_stmt_iterator bsi;
  bool statm_a_found, statm_b_found;
  struct loop_to_cloog_loop_str tmp, *slot; 

  if (GBB_STATIC_SCHEDULE (gb_a)[p - 1] < GBB_STATIC_SCHEDULE (gb_b)[p - 1])
    return true;

  else if (GBB_STATIC_SCHEDULE (gb_a)[p - 1] == GBB_STATIC_SCHEDULE (gb_b)[p - 1])
    {
      statm_a_found = false;
      statm_b_found = false;
      /* TODO: You can use stmt_ann->uid for a slight speedup.  */
      /* If static schedules are the same -> gb1 = gb2.  */
      /* GBB_BB (gb_a)->loop_father; */
      tmp.loop_num = GBB_BB (gb_a)->loop_father->num;
      slot = (struct loop_to_cloog_loop_str *) htab_find (SCOP_LOOP2CLOOG_LOOP(scop), &tmp);

      if (slot->loop_position == p - 1)
	for (bsi = bsi_start (GBB_BB (gb_a)); !bsi_end_p (bsi); bsi_next (&bsi))
	  {
	    if (bsi_stmt (bsi) == a)
	      statm_a_found = true;
        
	    if (statm_a_found && bsi_stmt (bsi) == b)
	      return true;
	  }
    }

  return false;
}

static struct data_dependence_polyhedron *
test_dependence (scop_p scop, graphite_bb_p gb1, graphite_bb_p gb2,
                 struct data_reference *a, struct data_reference *b)
{
  unsigned i, j, row, iter_vector_dim;
  unsigned loop_a, loop_b;
  signed p;
  CloogMatrix *dep_constraints = NULL, *temp_matrix = NULL;
  CloogDomain *simplified;
  
  loop_a = loop_containing_stmt (DR_STMT (a)) -> num;
  loop_b = loop_containing_stmt (DR_STMT (b)) -> num;
  
  iter_vector_dim = MIN (loop_iteration_vector_dim (loop_a, scop),
                         loop_iteration_vector_dim (loop_b, scop));
  
  for (i = 1; i <= 2 * iter_vector_dim + 1; i++)
  {
    /* S - gb1 */
    /* T - gb2 */
    /* S -> T, T - S >=1 */
    /* p is alternating sequence 0,1,-1,2,-2,... */
    p = (i / 2) * (1 - (i % 2)*2);
    if (p == 0)
      dep_constraints = initialize_dependence_polyhedron (scop, a, b);
    else if (p > 0)
      {
        /* assert B0, B1, ..., Bp-1 satisfy the equality */
        
        for (j = 0; j < iter_vector_dim; j++)
        {
          temp_matrix = AddANullRow (dep_constraints);
        
          row = j + dep_constraints->NbRows - iter_vector_dim;           
          value_set_si (temp_matrix->p[row][0], 1); /* >= */
          value_oppose (temp_matrix->p[row][p], 
                        GBB_DYNAMIC_SCHEDULE (gb1)->p[j][p - 1]);
          value_assign (temp_matrix->p[row][loop_iteration_vector_dim (loop_a, scop) + p], 
                        GBB_DYNAMIC_SCHEDULE (gb1)->p[j][p - 1]);
          value_set_si (temp_matrix->p[row][temp_matrix->NbColumns - 1], -1);

          simplified = cloog_domain_matrix2domain (temp_matrix);
          if (is_empty_polyhedron (simplified))
          {
            value_assign (dep_constraints->p[j + dep_constraints->NbRows - 2*iter_vector_dim][p], 
                          GBB_DYNAMIC_SCHEDULE (gb1)->p[j][p - 1]);
          
            value_oppose (dep_constraints->p[j + dep_constraints->NbRows - 2*iter_vector_dim]
                                            [loop_iteration_vector_dim (loop_a, scop) + p], 
                          GBB_DYNAMIC_SCHEDULE (gb2)->p[j][p - 1]);
          }
          else
            return initialize_data_dependence_polyhedron (true, simplified, p, a, b);           
          cloog_matrix_free (temp_matrix);
        }
      }
    else if (p < 0)
      {
  
        /* TODO: do not forget about memory leaks,
           temp_matrix is a new matrix!  */

        /*
        for (row = 0; row < iter_vector_dim; row++)
        {
          value_assign (dep_constraints->p[row + dep_constraints->NbRows - 2*iter_vector_dim ][-p], 
                        GBB_DYNAMIC_SCHEDULE (gb1)->p[row][-p -1]);
          
          value_oppose (dep_constraints->p[row + dep_constraints->NbRows - 2*iter_vector_dim ]
                                          [loop_iteration_vector_dim (loop_a, scop) - p], 
                        GBB_DYNAMIC_SCHEDULE (gb2)->p[row][-p -1]);
        }
        */
        /* simplified = cloog_domain_matrix2domain (temp_matrix); */
  
        if (statement_precedes_p (scop, gb1, DR_STMT (a), gb2, DR_STMT (b), -p))
          {
            return initialize_data_dependence_polyhedron (false, simplified, -p, a, b);
            /* VEC_safe_push (ddp_p, heap, ddps, ddp); */
            break;
          }
      }
  }    
  cloog_matrix_free (dep_constraints);
  return NULL;
}

/* Returns the polyhedral data dependence graph for SCOP.  */

static struct graph *
build_rdg_all_levels (scop_p scop)
{
  unsigned i, j, i1, j1;
  int va, vb;
  graphite_bb_p gb1, gb2;
  struct graph * rdg = NULL;
  struct data_reference *a, *b;
  block_stmt_iterator bsi;
  struct graph_edge *e;
  
 /* VEC (data_reference_p, heap) *datarefs;*/
 /* All the statements that are involved in dependences are stored in
    this vector.  */
  VEC (tree, heap) *stmts = VEC_alloc (tree, heap, 10);
  VEC (ddp_p, heap) *dependences = VEC_alloc (ddp_p, heap, 10); 
  ddp_p dependence_polyhedron;    
  /* datarefs = VEC_alloc (data_reference_p, heap, 2);*/
  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb1); i++)
    {
      for (bsi = bsi_start (GBB_BB (gb1)); !bsi_end_p (bsi); bsi_next (&bsi))
	VEC_safe_push (tree, heap, stmts, bsi_stmt (bsi));

      for (i1 = 0; VEC_iterate (data_reference_p, GBB_DATA_REFS (gb1), i1, a); i1++)
	for (j = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), j, gb2); j++)
	  for (j1 = 0; VEC_iterate (data_reference_p, GBB_DATA_REFS (gb2), j1, b); j1++)
	    if ((!DR_IS_READ (a) || !DR_IS_READ (b)) && dr_may_alias_p (a,b)
		&& operand_equal_p (DR_BASE_OBJECT (a), DR_BASE_OBJECT (b), 0))
              {
                dependence_polyhedron = test_dependence (scop, gb1, gb2, a, b);
                if (dependence_polyhedron != NULL)
                  VEC_safe_push (ddp_p, heap, dependences, dependence_polyhedron);
              }
                /* TODO: the previous check might be too restrictive.  */ 
    }
    

  rdg = build_empty_rdg (VEC_length (tree, stmts));
  create_rdg_vertices (rdg, stmts);

  for (i = 0; VEC_iterate (ddp_p, dependences, i, dependence_polyhedron); i++)
    {
      va = rdg_vertex_for_stmt (rdg, DR_STMT (dependence_polyhedron->a)); 
      vb = rdg_vertex_for_stmt (rdg, DR_STMT (dependence_polyhedron->b));
      e = add_edge (rdg, va, vb);
      e->data = dependence_polyhedron;
    }

  VEC_free (tree, heap, stmts);
  return rdg;  
}

/* Dumps the dependence graph G to file F.  */

static void
dump_dependence_graph (FILE *f, struct graph *g)
{
  int i;
  struct graph_edge *e;

  for (i = 0; i < g->n_vertices; i++)
    {
      if (!g->vertices[i].pred
	  && !g->vertices[i].succ)
	continue;

      fprintf (f, "vertex: %d (%d)\nStatement: ", i, g->vertices[i].component);
      print_generic_expr (f, RDGV_STMT (&(g->vertices[i])), 0);
      fprintf (f, "\n-----------------\n");
      
      for (e = g->vertices[i].pred; e; e = e->pred_next)
        {
          struct data_dependence_polyhedron *ddp = RDGE_DDP (e);
          if (ddp->polyhedron != NULL)
            {
              fprintf (f, "edge %d -> %d\n", e->src, i);
              cloog_domain_print (f, ddp->polyhedron); 
              fprintf (f, "-----------------\n");
            }
        }

      for (e = g->vertices[i].succ; e; e = e->succ_next)
        {
          struct data_dependence_polyhedron *ddp = RDGE_DDP (e);
          if (ddp->polyhedron != NULL)
            {
              fprintf (f, "edge %d -> %d\n", i, e->dest);
              cloog_domain_print (f, ddp->polyhedron); 
              fprintf (f, "-----------------\n");
            }
        }
      fprintf (f, "\n");
    }
}

/* Returns the number of data references in SCOP.  */

static int
nb_data_refs_in_scop (scop_p scop)
{
  int i;
  graphite_bb_p gbb;
  int res = 0;

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gbb); i++)
    res += VEC_length (data_reference_p, GBB_DATA_REFS (gbb));

  return res;
}

/* Perform a set of linear transforms on LOOPS.  */

void
graphite_transform_loops (void)
{
  unsigned i;
  scop_p scop;
  struct graph * rdg = NULL;

  if (number_of_loops () <= 1)
    return;

  current_scops = VEC_alloc (scop_p, heap, 3);

  calculate_dominance_info (CDI_DOMINATORS);
  calculate_dominance_info (CDI_POST_DOMINATORS);

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "Graphite loop transformations \n");

  build_scops ();

  for (i = 0; VEC_iterate (scop_p, current_scops, i, scop); i++)
    {
      build_scop_bbs (scop);
      build_scop_loop_nests (scop);
      build_scop_canonical_schedules (scop);
      find_scop_parameters (scop);
      build_scop_context (scop);

      if (dump_file && (dump_flags & TDF_DETAILS))
	{
	  fprintf (dump_file, "\n(In SCoP %d:\n", i);
	  fprintf (dump_file, "\nnumber of bbs: %d\n",
		   VEC_length (graphite_bb_p, SCOP_BBS (scop)));
	  fprintf (dump_file, "\nnumber of loops: %d)\n",
		   VEC_length (loop_p, SCOP_LOOP_NEST (scop)));
	}

      if (!build_scop_iteration_domain (scop))
	continue;

      build_scop_data_accesses (scop);

      /* XXX: Disabled, it does not work at the moment. */
      if (0)
        build_scop_dynamic_schedules (scop);

      if (dump_file && (dump_flags & TDF_DETAILS))
	{
	  int nbrefs = nb_data_refs_in_scop (scop);
	  fprintf (dump_file, "\nnumber of data refs: %d\n", nbrefs);
	}

      if (0)
	build_rdg_all_levels (scop);

      gloog (scop, find_transform (scop));
    }

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      dot_all_scops_1 (dump_file);
      print_scops (dump_file, 2);
      fprintf (dump_file, "\nnumber of SCoPs: %d\n",
	       VEC_length (scop_p, current_scops));
      if (0)
	dump_dependence_graph (dump_file, rdg);
    }

  free_scops (current_scops);
}
