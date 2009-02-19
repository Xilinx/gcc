/* Gimple Represented as Polyhedra.
   Copyright (C) 2006, 2007, 2008  Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@inria.fr>.

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
#include "tree-pass.h"
#include "domwalk.h"
#include "value-prof.h"
#include "pointer-set.h"
#include "gimple.h"
#include "sese.h"

/* For a USE in BB, if BB is outside REGION, mark the USE in the
   SESE_LIVEIN and SESE_LIVEOUT sets.  */

void
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

void
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

void
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
  
  htab_delete (SESE_LIVEOUT_RENAMES (region));
  XDELETE (region);
}

/* Get the index for parameter VAR in SESE.  */

int
param_index (tree var, sese sese)
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

