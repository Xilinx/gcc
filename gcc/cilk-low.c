/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains the Cilk Sync elimination and Lowering passes
   
   Copyright (C) 2011  Free Software Foundation, Inc.
   Written by Balaji V. Iyer <balaji.v.iyer@intel.com>,
              Intel Corporation

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
#include "tree.h"
#include "gimple.h"
#include "cilk.h"
#include "toplev.h"
#include "basic-block.h"
#include "tree-pass.h"



static bool
expand_cilk_pass1 (void)
{
  bool has_syncs = false;
  basic_block bb;

  int nblocks = n_basic_blocks_for_function (cfun);
#define BB_SYNCS	1	/* block ends synched */
#define BB_SPAWNS	2	/* block spawns */
#define BB_SPAWNS_E	4	/* spawned function may throw */
#define BB_UNSYNCHED	8	/* starts unsynched */
#define BB_UNSYNCHED_E	16	/* starts with possible unsynched exception */
#define BB_SYNC_TEST	32	/* contains call to unsynched() */
  unsigned char *flags = (unsigned char *)alloca (nblocks);

  memset (flags, 0, nblocks);

  FOR_EACH_BB (bb)
    {
      gimple_stmt_iterator i;
      unsigned char this_flags;

      /* gcc_assert (bb->index < nblocks); */

      /* special = bb_has_abnormal_pred (bb); */

      this_flags = flags[bb->index];

      for (i = gsi_start_bb (bb); !gsi_end_p (i); gsi_next (&i))
	{
	  gimple stmt = gsi_stmt (i);
	  switch (gimple_code (stmt))
	    {
#if 0
	    case GIMPLE_CILK_SYNC:
	      gimple_set_plf (stmt, GF_PLF_1, 0);
	      /* Even if this basic block detached, it ends synched.
		 It may have both DETACHES and SYNCS set.  Then it
		 ends with sync but the sync is not redundant. */
	      this_flags |= BB_SYNCS;
	      has_syncs = true;
	      /* Do not mark the function as using Cilk because a sync
		 without a spawn has no effect. */
	      break;
#endif
	    case GIMPLE_CALL:
	      {
		tree fn = gimple_call_fndecl (stmt);
		/*tree lhs = gimple_call_lhs (stmt);*/
		if (fn && DECL_DETACHES_P (fn))
		  {
		    this_flags |= BB_SPAWNS;
		    if (! (gimple_call_flags (stmt) & ECF_NOTHROW))
		      this_flags |= BB_SPAWNS_E;
		  }
		else if (fn == cilk_synched_fndecl)
		  this_flags |= BB_SYNC_TEST;
	      }
	      break;
	    default:
	      break;
	    }
	}

      flags[bb->index] = this_flags;
    }

  /* If this function contains no syncs, the second half of this
     function has nothing to do. */
  if (! has_syncs)
    return false;

  /* If the user doesn't care about redundant syncs this function
     has nothing more to do. */
  if (/* !flag_optimize_sync  && !warn_redundant_sync */ 1)
    return has_syncs;

  /* Now every block starts synched unless it follows a block that
     may be unsynched.
     XXX Should this be a depth-first recursive traversal? */
  while (1)
    {
      bool changed = false;
      FOR_EACH_BB (bb)
      {
	edge e;
	edge_iterator ei;
	int this_flags = flags[bb->index];
	int out_flags;

	if (this_flags == 0)	/* No state to propagate. */
	  continue;
	if (this_flags & BB_SYNCS)
	  continue;		/* does not make successors unsynched */
	if (this_flags == 0)
	  continue;		/* does not make successors unsynched */
	/* Compute the flags to propagate */
	out_flags = this_flags & (BB_UNSYNCHED|BB_UNSYNCHED_E);
	if (this_flags & BB_SPAWNS)
	  out_flags |= BB_UNSYNCHED;
	if (this_flags & BB_SPAWNS_E)
	  out_flags |= BB_UNSYNCHED_E;

	FOR_EACH_EDGE (e, ei, bb->succs)
	  {
	    /* Ignore EH edges.  The runtime syncs the function. */
	    if (e->flags & EDGE_EH)
	      continue;
	    if ((flags[e->dest->index] & out_flags) == out_flags)
	      continue;
	    flags[e->dest->index] |= out_flags;
	    changed = true;
	  }
      }
      if (!changed)
	break;
    }

#if 0
  FOR_EACH_BB (bb)
  {
    gimple sync;
    gimple_stmt_iterator gsi;
    int bb_flags = flags[bb->index];

    /* This loop only affects blocks that end with a sync. */
    if (! (bb_flags & BB_SYNCS))
      continue;

    gsi = gsi_last_bb (bb);
    sync = gsi_stmt (gsi);
    gcc_assert (sync && gimple_code (sync) == GIMPLE_CILK_SYNC);

    /* If there are no asynchronous exceptions, this sync can't throw. */
    gimple_set_plf (sync, GF_PLF_1, (bb_flags & (BB_SPAWNS_E|BB_UNSYNCHED_E)) == 0);

    /* If flags is BB_SYNCS that means the block does not start
       unsynched, does not detach, and ends with a sync. */
    if (bb_flags != BB_SYNCS)
      continue;
    /* Don't complain if the user writes sync at the end of a block
       with an implied sync.  The parser sets TREE_NO_WARNING on
       the implicit sync, which is the one that will appear redundant. */
    if (! gimple_no_warning_p (sync))
      {
	location_t where = gimple_location (sync);
	warning (OPT_Wredundant_sync, "%Hcilk_sync has no effect", &where);
      }
    if (flag_optimize_sync)
      {
	gsi_remove (&gsi, true);
      }
  }
#endif

  return has_syncs;
}

#if 0

/* Cilk SYNC statement.  SYNC expands to

     __cilkrts_metadata(begin sync);
     if __cilkrts_frame.unsynched
       __cilkrts_sync();
     __cilkrts_metadata(end sync);

   You'd think this would be simple.  Apparently not.
*/
static void
replace_one_sync (basic_block bb, gimple_stmt_iterator *gsi, bool nothrow)
{
  gimple_seq seq;
#if 0
  /* A temporary variable is needed because memory references are
     not allowed here (even though not allowing them makes code worse). */
  tree flags_var_1, flags_var_2, flags_type;
  /* The jump condition.  This may vary depending on -O as this function
     tries to outwit the code pessimizer. */
  tree cond;
  tree field;
  tree frame;
  tree stmt, call;
  tree synched_label, unsynched_label, goto_synched, goto_unsynched;
  int eh_region = lookup_stmt_eh_region (bsi_stmt (*bsi));
#endif

  seq = 0;
  gimplify_and_add (build_cilk_sync (/*nothrow*/), &seq);

  /* Replace the original SYNC_STMT with a series of new statements. */
  gsi_remove (gsi, true);

  /*cilkscreen_insert_metadata_before (metadata_sync, 0, bsi);*/

  gsi_insert_seq_before (gsi, seq, GSI_SAME_STMT);

  return;
#if 0
  TREE_READONLY (flags_var_2) = 1;
  cond = build2 (NE_EXPR, boolean_type_node, flags_var_2, integer_zero_node);
  synched_label = build1 (LABEL_EXPR, void_type_node, NULL_TREE);
  unsynched_label = build1 (LABEL_EXPR, void_type_node, NULL_TREE);
  goto_synched = build_and_jump (&LABEL_EXPR_LABEL (synched_label));
  goto_unsynched = build_and_jump (&LABEL_EXPR_LABEL (unsynched_label));
  cond = build3 (COND_EXPR, void_type_node, cond, goto_unsynched, goto_synched);

  do_sync_stuff (bb, cond, call, synched_label, unsynched_label, nothrow ? -1 : eh_region);
#endif
  return;
}

static void
expand_cilk_pass2 (void)
{
  basic_block bb;
  FOR_EACH_BB (bb)
    {
      gimple_stmt_iterator i = gsi_last_bb (bb);
      gimple stmt;
      
      if (gsi_end_p (i))
	continue;

      stmt = gsi_stmt (i);

      if (gimple_code (stmt) != GIMPLE_CILK_SYNC)
	continue;

      replace_one_sync (bb, &i, gimple_plf (stmt, GF_PLF_1));
    }
  return;
}
#endif

static unsigned int
execute_expand_cilk (void)
{
#if 0
  struct gimplify_ctx gctx;
#endif
  bool has_syncs;

  has_syncs = expand_cilk_pass1 ();

  if (!has_syncs)
    return 0;
#if 1
  return 0;
#else
  push_gimplify_context (&gctx);
  expand_cilk_pass2 ();
  pop_gimplify_context (0);

  return TODO_cleanup_cfg|TODO_dump_cgraph;
#endif
}

struct gimple_opt_pass pass_expand_cilk =
{
  {
    GIMPLE_PASS,
    "cilkexpand",			/* name */
    0,					/* gate */
    execute_expand_cilk,		/* execute */
    NULL,				/* sub */
    NULL,				/* next */
    0,					/* static_pass_number */
    TV_NONE,				/* tv_id */
    PROP_gimple_any|PROP_cfg,		/* properties_required */
    0,					/* properties_provided */
    0,					/* properties_destroyed */
    0,					/* todo_flags_start */
    TODO_dump_func|TODO_verify_flow,	/* todo_flags_finish */
  }
};
