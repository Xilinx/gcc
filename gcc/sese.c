/* Single entry single exit control flow regions.
   Copyright (C) 2008, 2009  Free Software Foundation, Inc.
   Contributed by Jan Sjodin <jan.sjodin@amd.com> and
   Sebastian Pop <sebastian.pop@amd.com>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

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
#include "toplev.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "domwalk.h"
#include "value-prof.h"
#include "pointer-set.h"
#include "gimple.h"
#include "sese.h"

/* Returns the number of reduction phi nodes in LOOP.  */

static int
nb_reductions_in_loop (loop_p loop)
{
  int res = 0;
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_phis (loop->header); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple phi = gsi_stmt (gsi);
      tree scev;
      affine_iv iv;

      if (!is_gimple_reg (PHI_RESULT (phi)))
	continue;

      scev = analyze_scalar_evolution (loop, PHI_RESULT (phi));
      scev = instantiate_parameters (loop, scev);
      if (!simple_iv (loop, phi, PHI_RESULT (phi), &iv, true))
	res++;
    }

  return res;
}

/* A LOOP is in normal form when it contains only one scalar phi node
   that defines the main induction variable of the loop, only one
   increment of the IV, and only one exit condition. */

static tree
graphite_loop_normal_form (loop_p loop)
{
  struct tree_niter_desc niter;
  tree nit;
  gimple_seq stmts;
  edge exit = single_dom_exit (loop);

  gcc_assert (number_of_iterations_exit (loop, exit, &niter, false));
  nit = force_gimple_operand (unshare_expr (niter.niter), &stmts, true,
			      NULL_TREE);
  if (stmts)
    gsi_insert_seq_on_edge_immediate (loop_preheader_edge (loop), stmts);

  /* One IV per loop.  */
  if (nb_reductions_in_loop (loop) > 0)
    return NULL_TREE;

  return canonicalize_loop_ivs (loop, NULL, nit);
}

/* Record LOOP as occuring in REGION.  Returns true when the operation
   was successful.  */

static bool
sese_record_loop (sese region, loop_p loop)
{
  tree induction_var;
  name_tree oldiv;

  if (sese_contains_loop (region, loop))
    return true;

  bitmap_set_bit (SESE_LOOPS (region), loop->num);
  VEC_safe_push (loop_p, heap, SESE_LOOP_NEST (region), loop);

  induction_var = graphite_loop_normal_form (loop);
  if (!induction_var)
    return false;

  oldiv = XNEW (struct name_tree);
  oldiv->t = induction_var;
  oldiv->name = get_name (SSA_NAME_VAR (oldiv->t));
  oldiv->loop = loop;
  VEC_safe_push (name_tree, heap, SESE_OLDIVS (region), oldiv);
  return true;
}

/* Build the loop nests contained in REGION.  Returns true when the
   operation was successful.  */

bool
build_sese_loop_nests (sese region)
{
  unsigned i;
  basic_block bb;
  struct loop *loop0, *loop1;

  FOR_EACH_BB (bb)
    if (bb_in_sese_p (bb, region))
      {
	struct loop *loop = bb->loop_father;

	/* Only add loops if they are completely contained in the SCoP.  */
	if (loop->header == bb
	    && bb_in_sese_p (loop->latch, region))
	  {
	    if (!sese_record_loop (region, loop))
	      return false;
	  }
      }

  /* Make sure that the loops in the SESE_LOOP_NEST are ordered.  It
     can be the case that an inner loop is inserted before an outer
     loop.  To avoid this, semi-sort once.  */
  for (i = 0; VEC_iterate (loop_p, SESE_LOOP_NEST (region), i, loop0); i++)
    {
      if (VEC_length (loop_p, SESE_LOOP_NEST (region)) == i + 1)
	break;

      loop1 = VEC_index (loop_p, SESE_LOOP_NEST (region), i + 1);
      if (loop0->num > loop1->num)
	{
	  VEC_replace (loop_p, SESE_LOOP_NEST (region), i, loop1);
	  VEC_replace (loop_p, SESE_LOOP_NEST (region), i + 1, loop0);
	}
    }

  return true;
}

/* For a USE in BB, if BB is outside REGION, mark the USE in the
   SESE_LIVEIN and SESE_LIVEOUT sets.  */

static void
sese_build_livein_liveouts_use (sese region, basic_block bb, tree use)
{
  unsigned ver;
  basic_block def_bb;

  if (TREE_CODE (use) != SSA_NAME)
    return;

  ver = SSA_NAME_VERSION (use);
  def_bb = gimple_bb (SSA_NAME_DEF_STMT (use));
  if (!def_bb
      || !bb_in_sese_p (def_bb, region)
      || bb_in_sese_p (bb, region))
    return;

  if (!SESE_LIVEIN_VER (region, ver))
    SESE_LIVEIN_VER (region, ver) = BITMAP_ALLOC (NULL);

  bitmap_set_bit (SESE_LIVEIN_VER (region, ver), bb->index);
  bitmap_set_bit (SESE_LIVEOUT (region), ver);
}

/* Marks for rewrite all the SSA_NAMES defined in REGION and that are
   used in BB that is outside of the REGION.  */

static void
sese_build_livein_liveouts_bb (sese region, basic_block bb)
{
  gimple_stmt_iterator bsi;
  edge e;
  edge_iterator ei;
  ssa_op_iter iter;
  tree var;

  FOR_EACH_EDGE (e, ei, bb->succs)
    for (bsi = gsi_start_phis (e->dest); !gsi_end_p (bsi); gsi_next (&bsi))
      sese_build_livein_liveouts_use (region, bb,
				      PHI_ARG_DEF_FROM_EDGE (gsi_stmt (bsi), e));

  for (bsi = gsi_start_bb (bb); !gsi_end_p (bsi); gsi_next (&bsi))
    FOR_EACH_SSA_TREE_OPERAND (var, gsi_stmt (bsi), iter, SSA_OP_ALL_USES)
      sese_build_livein_liveouts_use (region, bb, var);
}

/* Build the SESE_LIVEIN and SESE_LIVEOUT for REGION.  */

void
sese_build_livein_liveouts (sese region)
{
  basic_block bb;

  SESE_LIVEOUT (region) = BITMAP_ALLOC (NULL);
  SESE_NUM_VER (region) = num_ssa_names;
  SESE_LIVEIN (region) = XCNEWVEC (bitmap, SESE_NUM_VER (region));

  FOR_EACH_BB (bb)
    sese_build_livein_liveouts_bb (region, bb);
}

/* Register basic blocks belonging to a region in a pointer set.  */

static void
register_bb_in_sese (basic_block entry_bb, basic_block exit_bb, sese region)
{
  edge_iterator ei;
  edge e;
  basic_block bb = entry_bb;

  FOR_EACH_EDGE (e, ei, bb->succs)
    {
      if (!pointer_set_contains (SESE_REGION_BBS (region), e->dest) &&
	  e->dest->index != exit_bb->index)
	{	
	  pointer_set_insert (SESE_REGION_BBS (region), e->dest);
	  register_bb_in_sese (e->dest, exit_bb, region);
	}
    }
}

/* Builds a new SESE region from edges ENTRY and EXIT.  */

sese
new_sese (edge entry, edge exit)
{
  sese res = XNEW (struct sese);

  SESE_ENTRY (res) = entry;
  SESE_EXIT (res) = exit;
  SESE_REGION_BBS (res) = pointer_set_create ();
  register_bb_in_sese (entry->dest, exit->dest, res);
  SESE_LOOPS (res) = BITMAP_ALLOC (NULL);
  SESE_LOOP_NEST (res) = VEC_alloc (loop_p, heap, 3);
  SESE_LIVEOUT (res) = NULL;
  SESE_NUM_VER (res) = 0;
  SESE_LIVEIN (res) = NULL;
  SESE_ADD_PARAMS (res) = true;
  SESE_PARAMS (res) = VEC_alloc (name_tree, heap, 3);
  SESE_OLDIVS (res) = VEC_alloc (name_tree, heap, 3);
  SESE_LIVEOUT_RENAMES (res) = NULL;

  return res;
}

/* Deletes REGION.  */

void
free_sese (sese region)
{
  int i;
  name_tree p, iv;

  for (i = 0; i < SESE_NUM_VER (region); i++)
    BITMAP_FREE (SESE_LIVEIN_VER (region, i));

  if (SESE_LIVEIN (region))
    free (SESE_LIVEIN (region));

  if (SESE_LIVEOUT (region))
    BITMAP_FREE (SESE_LIVEOUT (region));

  if (SESE_LOOPS (region))
    SESE_LOOPS (region) = BITMAP_ALLOC (NULL);

  for (i = 0; VEC_iterate (name_tree, SESE_PARAMS (region), i, p); i++)
    free (p);

  VEC_free (name_tree, heap, SESE_PARAMS (region));
  VEC_free (loop_p, heap, SESE_LOOP_NEST(region));
  pointer_set_destroy (SESE_REGION_BBS (region));

  for (i = 0; VEC_iterate (name_tree, SESE_OLDIVS (region), i, iv); i++)
    free (iv);

  VEC_free (name_tree, heap, SESE_OLDIVS (region));

  if (SESE_LIVEOUT_RENAMES (region))
    htab_delete (SESE_LIVEOUT_RENAMES (region));
  XDELETE (region);
}

/* Get the index for parameter VAR in SESE.  */

int
parameter_index_in_region (tree var, sese sese)
{
  int i;
  name_tree p;
  name_tree nvar;

  gcc_assert (TREE_CODE (var) == SSA_NAME);

  for (i = 0; VEC_iterate (name_tree, SESE_PARAMS (sese), i, p); i++)
    if (p->t == var)
      return i;

  gcc_assert (SESE_ADD_PARAMS (sese));

  nvar = XNEW (struct name_tree);
  nvar->t = var;
  nvar->name = NULL;
  VEC_safe_push (name_tree, heap, SESE_PARAMS (sese), nvar);
  return VEC_length (name_tree, SESE_PARAMS (sese)) - 1;
}

