/* Loop fusion.
   Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Sandeep Maram <smaram_b04@iiita.ac.in>.

This file is part of GCC.
   
GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.
   
GCC is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
   
You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* This pass performs loop fusion: for example, the loops

	|loop_1
	|  A[i] = ...
	|endloop_1


	|loop_2
	|  ... = A[i]
	|endloop_2

   that becomes after fusion:

	|loop_1
	|  A[i] = ...
	|  ... = A[i]
	|endloop_1

*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "target.h"

#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "expr.h"
#include "optabs.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "lambda.h"
#include "langhooks.h"
#include "tree-vectorizer.h"

/* Returns true when LOOP is parallel.  */

static bool
is_parallel_loop (struct loop *loop)
{
  VEC (ddr_p, heap) * dependence_relations;
  VEC (data_reference_p, heap) * datarefs;
  lambda_trans_matrix trans;
  bool ret = false;
  
  /* If the loop can be reversed, the iterations are independent.  */
  datarefs = VEC_alloc (data_reference_p, heap, 10);
  dependence_relations = VEC_alloc (ddr_p, heap, 10 * 10);
  compute_data_dependences_for_loop (loop, true, &datarefs,
				     &dependence_relations);
  trans = lambda_trans_matrix_new (1, 1);
  LTM_MATRIX (trans)[0][0] = -1;

  if (lambda_transform_legal_p (trans, 1, dependence_relations))
    ret = true;

  free_dependence_relations (dependence_relations);
  free_data_refs (datarefs);

  if (ret == true)
    fprintf (dump_file, " loop %d is a parallel loop\n ", loop->num);

  return ret;
}

/* Returns true if there is no fusion preventing constraint between
   LOOP_A and LOOP_B.  */

static bool
no_fusion_preventing_constraint (struct loop *loop_a, struct loop *loop_b)
{
  return is_parallel_loop (loop_a) && is_parallel_loop (loop_b);
}


/* Returns true if LOOP_A and LOOP_B have similar memory accesses.  */

static bool
loops_have_similar_memory_accesses (struct loop *loop_a, struct loop *loop_b)
{
  unsigned int i, j;
  struct data_reference *dr_a, *dr_b;
  bool ret = false;
  VEC (data_reference_p, heap) *datarefs_a = VEC_alloc (data_reference_p,
							heap, 10);
  VEC (data_reference_p, heap) *datarefs_b = VEC_alloc (data_reference_p,
							heap, 10);
  
  /* Find data references for both the loops.  */
  if ((find_data_references_in_loop (loop_a, &datarefs_a) == NULL_TREE)
      && (find_data_references_in_loop (loop_b, &datarefs_b) == NULL_TREE))
    for (i = 0; VEC_iterate (data_reference_p, datarefs_a, i, dr_a); i++)
      for (j = 0; VEC_iterate (data_reference_p, datarefs_b, j, dr_b); j++)
	if (have_similar_memory_accesses (DR_STMT (dr_a), DR_STMT (dr_b)))
	  {
	    ret = true;
	    goto end;
	  }     

 end:
  free_data_refs (datarefs_a);
  free_data_refs (datarefs_b);
  return ret;
}

/* Returns true if two loops can be fused.  */

static bool
can_fuse_loops (struct loop *loop_a, struct loop *loop_b)
{
  bool ret = true;
  
  /* If the loops do not access common data, no worth fusing them.  */
  if (!loops_have_similar_memory_accesses (loop_a, loop_b))
    return false;
  
  /* If there is no fusion preventing constraint between the loops.  */
  if (no_fusion_preventing_constraint (loop_a, loop_b))
    return ret;
  
  return false; 
}

static struct loop *
fuse_loops (struct loop *loop_a ATTRIBUTE_UNUSED,
	    struct loop *loop_b ATTRIBUTE_UNUSED)
{
  struct loop *fused_loop;
  return fused_loop;
}

/* Performs loops fusion in the current function.  */

static unsigned int
tree_loop_fusion (void)
{
  struct loop *loop;
  loop_iterator li;

  if (!current_loops)
    return 0;
   
  /* For all the loops in the program pick consecutive loops loop_a
     and loop_b.  */
  FOR_EACH_LOOP (li, loop, 0)
    {
      if (loop->next)
        {
          bool ret = loops_have_similar_memory_accesses (loop, loop->next);
          fprintf (dump_file, "%d\n", ret);
          fprintf (stderr, "%d\n",ret);  

          if (can_fuse_loops (loop, loop->next))
            {
	      struct loop *new_loop;
	      new_loop = fuse_loops (loop, loop->next);
	      /* 
		 loop = new_loop;
		 cancel_loop_tree (loop->next);
	      */
	    }
        }        
    }

  return 0;
}

static bool
gate_tree_loop_fusion (void)
{
  return flag_tree_loop_fusion != 0;
}

struct gimple_opt_pass pass_loop_fusion =
  {
    {
      GIMPLE_PASS, 
      "lfusion",		        	/* name */
      gate_tree_loop_fusion,                /* gate */
      tree_loop_fusion,                     /* execute */
      NULL,			        	/* sub */
      NULL,			        	/* next */
      0,				        /* static_pass_number */
      TV_TREE_LOOP_FUSION,                  /* tv_id */
      PROP_cfg | PROP_ssa,		        /* properties_required */
      0,				        /* properties_provided */
      0,				        /* properties_destroyed */
      0,				        /* todo_flags_start */
      TODO_dump_func | TODO_verify_loops | TODO_update_ssa,   /* todo_flags_finish */
    }
  };

