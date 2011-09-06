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

    this_flags = flags[bb->index];

    for (i = gsi_start_bb (bb); !gsi_end_p (i); gsi_next (&i))
      {
	gimple stmt = gsi_stmt (i);
	switch (gimple_code (stmt))
	  {
	  case GIMPLE_CALL:
	    {
	      tree fn = gimple_call_fndecl (stmt);
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


  return has_syncs;
}

static unsigned int
execute_expand_cilk (void)
{
  bool has_syncs;

  has_syncs = expand_cilk_pass1 ();

  if (!has_syncs)
    return 0;

  return 0;

}

struct gimple_opt_pass pass_expand_cilk =
  {
    {
      GIMPLE_PASS,
      "cilkexpand",			/* name */
      0,				/* gate */
      execute_expand_cilk,		/* execute */
      NULL,				/* sub */
      NULL,				/* next */
      0,				/* static_pass_number */
      TV_NONE,				/* tv_id */
      PROP_gimple_any|PROP_cfg,		/* properties_required */
      0,				/* properties_provided */
      0,				/* properties_destroyed */
      0,				/* todo_flags_start */
      TODO_dump_func|TODO_verify_flow,	/* todo_flags_finish */
    }
  };
