/* Passes for transactional memory support.
   Copyright (C) 2008 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "gimple.h"
#include "tree-flow.h"
#include "tree-pass.h"
#include "except.h"
#include "diagnostic.h"
#include "toplev.h"
#include "flags.h"


/* The representation of a transaction changes several times during the
   lowering process.  In the beginning, in the front-end we have the
   GENERIC tree TM_ATOMIC.  For example,

	__tm_atomic {
	  local++;
	  if (++global == 10)
	    __tm_abort;
	}

  During initial gimplification (gimplify.c) the TM_ATOMIC node is
  trivially replaced with a GIMPLE_TM_ATOMIC node.

  During pass_lower_tm, we examine the body of transactions looking
  for aborts.  Transactions that do not contain an abort may be 
  merged into an outer transaction.  We also add a TRY-FINALLY node
  to arrange for the transaction to be committed on any exit.

  [??? Think about how this arrangement affects throw-with-commit
  and throw-with-abort operations.  In this case we want the TRY to
  handle gotos, but not to catch any exceptions because the transaction
  will already be closed.]

	GIMPLE_TM_ATOMIC [label=NULL] {
	  try {
	    local = local + 1;
	    t0 = global;
	    t1 = t0 + 1;
	    global = t1;
	    if (t1 == 10)
	      __builtin___tm_abort ();
	  } finally {
	    __builtin___tm_commit ();
	  }
	}

  During pass_lower_eh, we create EH regions for the transactions,
  intermixed with the regular EH stuff.  This gives us a nice persistent
  mapping (all the way through rtl) from transactional memory operation
  back to the transaction, which allows us to get the abnormal edges
  correct to model transaction aborts and restarts:

	GIMPLE_TM_ATOMIC [label=over]
	local = local + 1;
	t0 = global;
	t1 = t0 + 1;
	global = t1;
	if (t1 == 10)
	  __builtin___tm_abort ();
	__builtin___tm_commit ();
	over:

  This is the end of all_lowering_passes, and so is what is present
  during the IPA passes, and through all of the optimization passes.

  During pass_ipa_tm, we examine all GIMPLE_TM_ATOMIC blocks in all
  functions and mark functions for cloning.

  At the end of gimple optimization, before exiting SSA form, 
  pass_expand_tm replaces statements that perform transactional
  memory operations with the appropriate TM builtins, and swap
  out function calls with their transactional clones.  At this
  point we introduce the abnormal transaction restart edges and
  complete lowering of the GIMPLE_TM_ATOMIC node.

	x = __builtin___tm_start (MAY_ABORT);
	eh_label:
	if (x & abort_transaction)
	  goto over;
	local = local + 1;
        t0 = __builtin___tm_load (global);
	t1 = t0 + 1;
        __builtin___tm_store (&global, t1);
	if (t1 == 10)
	  __builtin___tm_abort ();
	__builtin___tm_commit ();
	over:
*/

static void lower_sequence_tm (unsigned *, gimple_seq);
static void lower_sequence_no_tm (gimple_seq);

/* Determine whether X has to be instrumented using a read
   or write barrier.  */

static bool
requires_barrier (tree x)
{
  while (handled_component_p (x))
    x = TREE_OPERAND (x, 0);

  switch (TREE_CODE (x))
    {
    case INDIRECT_REF:
      return true;

    case ALIGN_INDIRECT_REF:
    case MISALIGNED_INDIRECT_REF:
      gcc_unreachable ();

    case VAR_DECL:
      if (DECL_IS_TM_PURE_VAR (x))
	return false;
      if (is_global_var (x))
	return !TREE_READONLY (x);
      return TREE_ADDRESSABLE (x);

    default:
      return false;
    }
}

/* Mark the GIMPLE_ASSIGN statement as appropriate for being inside
   a transaction region.  */

static void
examine_assign_tm (unsigned *state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);

  if (requires_barrier (gimple_assign_rhs1 (stmt)))
    *state |= GTMA_HAVE_LOAD;
  if (requires_barrier (gimple_assign_lhs (stmt)))
    *state |= GTMA_HAVE_STORE;
}

/* Mark a GIMPLE_CALL as appropriate for being inside a transaction.  */

static void
examine_call_tm (unsigned *state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree fn_decl;
  unsigned flags;

  flags = gimple_call_flags (stmt);
  if (flags & ECF_CONST)
    return;

  if (flag_exceptions && !(flags & ECF_NOTHROW))
    *state |= GTMA_HAVE_UNCOMMITTED_THROW;

  fn_decl = gimple_call_fndecl (stmt);
  if (!fn_decl)
    {
      *state |= GTMA_HAVE_CALL_IRREVOKABLE;
      return;
    }

  /* If this function is pure, we can ignore it.  */
  if (DECL_IS_TM_PURE (fn_decl))
    return;

  /* Check if this call is a transaction abort.  */
  if (DECL_BUILT_IN_CLASS (fn_decl) == BUILT_IN_NORMAL
      && DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_TM_ABORT)
    {
      *state |= GTMA_HAVE_ABORT;
      return;
    }

  /* At this point pass_ipa_tm has not run, so no transactional
     clones exist yet, so there's no point in looking for them.  */
  *state |= GTMA_HAVE_CALL_IRREVOKABLE;
}

/* Lower a GIMPLE_TM_ATOMIC statement.  The GSI is advanced.  */

static void
lower_tm_atomic (unsigned int *outer_state, gimple_stmt_iterator *gsi)
{
  gimple g, stmt = gsi_stmt (*gsi);
  unsigned int this_state = 0;

  /* First, lower the body.  The scanning that we do inside gives
     us some idea of what we're dealing with.  */
  lower_sequence_tm (&this_state, gimple_seq_body (stmt));

  /* If there was absolutely nothing transaction related inside the
     transaction, we may elide it.  Likewise if this is a nested
     transaction and does not contain an abort.  */
  if (this_state == 0
      || (!(this_state & GTMA_HAVE_ABORT) && outer_state != NULL))
    {
      if (outer_state)
	*outer_state |= this_state;

      gsi_insert_seq_before (gsi, gimple_seq_body (stmt), GSI_SAME_STMT);
      gimple_seq_set_body (stmt, NULL);
      gsi_remove (gsi, true);
      return;
    }

  /* Wrap the body of the transaction in a try-finally node so that
     the commit call is always properly called.  */
  g = gimple_build_call (built_in_decls[BUILT_IN_TM_COMMIT], 0);
  g = gimple_build_try (gimple_seq_body (stmt),
			gimple_seq_alloc_with_stmt (g), GIMPLE_TRY_FINALLY);
  gimple_seq_set_body (stmt, gimple_seq_alloc_with_stmt (g));

  /* If the transaction calls abort, add an "over" label afterwards.  */
  if (this_state & GTMA_HAVE_ABORT)
    {
      tree label = create_artificial_label ();
      gimple_tm_atomic_set_label (stmt, label);
      gsi_insert_after (gsi, gimple_build_label (label), GSI_CONTINUE_LINKING);
    }

  /* Record the set of operations found for use later.  */
  gimple_tm_atomic_set_subcode (stmt, this_state);

  /* Always update the iterator.  */
  gsi_next (gsi);
}

/* Iterate through the statements in the sequence, lowering them all
   as appropriate for being in a transaction.  */

static void
lower_sequence_tm (unsigned int *state, gimple_seq seq)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start (seq); !gsi_end_p (gsi); )
    {
      gimple stmt = gsi_stmt (gsi);
      switch (gimple_code (stmt))
	{
	case GIMPLE_ASSIGN:
	  /* Only memory reads/writes need to be instrumented.  */
	  if (gimple_assign_single_p (stmt))
	    examine_assign_tm (state, &gsi);
	  break;

	case GIMPLE_CALL:
	  examine_call_tm (state, &gsi);
	  break;

	case GIMPLE_ASM:
	  *state |= GTMA_HAVE_CALL_IRREVOKABLE;
	  break;

	case GIMPLE_TM_ATOMIC:
	  lower_tm_atomic (state, &gsi);
	  goto no_update;

	default:
	  break;
	}
      gsi_next (&gsi);

    no_update:;
    }
}

/* Iterate through the statements in the sequence, lowering them all
   as appropriate for being outside of a transaction.  */

static void
lower_sequence_no_tm (gimple_seq seq)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start (seq); !gsi_end_p (gsi); )
    if (gimple_code (gsi_stmt (gsi)) == GIMPLE_TM_ATOMIC)
      lower_tm_atomic (NULL, &gsi);
    else
      gsi_next (&gsi);
}

/* Main entry point for flattening GIMPLE_TM_ATOMIC constructs.  After
   this, GIMPLE_TM_ATOMIC nodes still exist, but the nested body has
   been moved out, and all the data required for constructing a proper
   CFG has been recorded.  */

static unsigned int
execute_lower_tm (void)
{
  /* When lowering a transactional clone, we begin the function inside
     a transaction.  */
  if (DECL_IS_TM_CLONE (current_function_decl))
    {
      unsigned state = 0;
      lower_sequence_tm (&state, gimple_body (current_function_decl));
    }
  else
    lower_sequence_no_tm (gimple_body (current_function_decl));

  return 0;
}

static bool
gate_tm (void)
{
  return flag_tm;
}

struct gimple_opt_pass pass_lower_tm =
{
 {
  GIMPLE_PASS,
  "tmlower",				/* name */
  gate_tm,				/* gate */
  execute_lower_tm,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_gimple_lcf,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func		        /* todo_flags_finish */
 }
};

/* Return true if STMT may alter control flow via a transactional edge.  */

bool
is_transactional_stmt (const_gimple stmt)
{
  switch (gimple_code (stmt))
    {
    case GIMPLE_CALL:
      return (gimple_call_flags (stmt) & ECF_TM_OPS) != 0;
    case GIMPLE_TM_ATOMIC:
      return true;
    default:
      return false;
    }
}

/* Collect region information for each transaction.  */

struct tm_region
{
  /* Link to the next unnested transaction.  */
  struct tm_region *next;

  /* The GIMPLE_TM_ATOMIC statement beginning this transaction.  */
  gimple tm_atomic_stmt;

  /* The entry block to this region.  */
  basic_block entry_block;

  /* The set of all blocks that end the region; NULL if only EXIT_BLOCK.  */
  bitmap exit_blocks;

  /* The EH region number assigned to this transaction.  */
  int region_nr;
};

static struct tm_region *all_tm_regions;
static bitmap_obstack tm_obstack;


/* A subroutine of gate_tm_init, callback via for_each_tm_atomic.
   Record the existance of the GIMPLE_TM_ATOMIC statement in a linked
   list of tm_region elements.  */

static void
tm_region_init_1 (gimple stmt, void *xdata)
{
  struct tm_region **pptr = (struct tm_region **) xdata;
  struct tm_region *region;
  basic_block bb = gimple_bb (stmt);

  /* ??? Verify that the statement (and the block) havn't been deleted.  */
  gcc_assert (bb != NULL);
  gcc_assert (gimple_code (stmt) == GIMPLE_TM_ATOMIC);

  region = (struct tm_region *)
    obstack_alloc (&tm_obstack.obstack, sizeof (struct tm_region));
  region->next = *pptr;
  region->tm_atomic_stmt = stmt;

  /* There are either one or two edges out of the block containing
     the GIMPLE_TM_ATOMIC, one to the actual region and one to the
     "over" label if the region contains an abort.  The former will
     always be the one marked FALLTHRU.  */
  region->entry_block = FALLTHRU_EDGE (bb)->dest;

  region->exit_blocks = BITMAP_ALLOC (&tm_obstack);
  region->region_nr = lookup_stmt_eh_region (stmt);

  *pptr = region;
}


/* The "gate" function for all transactional memory expansion and optimization
   passes.  We collect region information for each top-level transaction, and
   if we don't find any, we skip all of the TM passes.  Each region will have
   all of the exit blocks recorded, and the originating statement.  */

static bool
gate_tm_init (void)
{
  struct tm_region *region;
  VEC (basic_block, heap) *queue;

  if (!flag_tm)
    return false;

  calculate_dominance_info (CDI_DOMINATORS);
  bitmap_obstack_initialize (&tm_obstack);

  /* If the function is a TM_CLONE, then the entire function is the region.  */
  if (DECL_IS_TM_CLONE (current_function_decl))
    {
      region = (struct tm_region *)
	obstack_alloc (&tm_obstack.obstack, sizeof (struct tm_region));
      region->next = NULL;
      region->tm_atomic_stmt = NULL;
      region->entry_block = ENTRY_BLOCK_PTR;
      region->exit_blocks = NULL;
      region->region_nr = -1;

      return true;
    }

  /* Find each GIMPLE_TM_ATOMIC statement.  This data is stored
     in the exception handling tables, so it's quickest to get
     it out that way than actually search the function.  */
  for_each_tm_atomic (false, tm_region_init_1, &all_tm_regions);

  /* If we didn't find any regions, cleanup and skip the whole tree
     of tm-related optimizations.  */
  if (all_tm_regions == NULL)
    {
      bitmap_obstack_release (&tm_obstack);
      return false;
    }

  queue = VEC_alloc (basic_block, heap, 10);

  /* Find the exit blocks for each region.  */
  for (region = all_tm_regions; region ; region = region->next)
    {
      basic_block bb;
      gimple_stmt_iterator gsi;

      VEC_quick_push (basic_block, queue, region->entry_block);
      do
	{
	  bb = VEC_pop (basic_block, queue);

	  /* Check to see if this is the end of the region by
	     seeing if it ends in a call to __tm_commit.  */
	  for (gsi = gsi_last_bb (bb); !gsi_end_p (gsi); gsi_prev (&gsi))
	    {
	      gimple g = gsi_stmt (gsi);
	      if (gimple_code (g) == GIMPLE_CALL)
		{
		  tree fn = gimple_call_fndecl (g);
		  if (fn && DECL_BUILT_IN_CLASS (fn) == BUILT_IN_NORMAL
		      && DECL_FUNCTION_CODE (fn) == BUILT_IN_TM_COMMIT
		      && lookup_stmt_eh_region (g) == region->region_nr)
		    {
		      bitmap_set_bit (region->exit_blocks, bb->index);
		      goto skip;
		    }
		}
	    }

	  for (bb = first_dom_son (CDI_DOMINATORS, bb);
	       bb;
	       bb = next_dom_son (CDI_DOMINATORS, bb))
	    VEC_safe_push (basic_block, heap, queue, bb);

	skip:;
	}
      while (!VEC_empty (basic_block, queue));
    }

  VEC_free (basic_block, heap, queue);

  return true;
}

/* Free the transactional memory data structures.  */

static unsigned int
execute_tm_done (void)
{
  bitmap_obstack_release (&tm_obstack);
  free_dominance_info (CDI_DOMINATORS);

  return 0;
}

struct gimple_opt_pass pass_tm_init =
{
 {
  GIMPLE_PASS,
  "tminit",				/* name */
  gate_tm_init,				/* gate */
  NULL,					/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  0,					/* todo_flags_finish */
 }
};

struct gimple_opt_pass pass_tm_done =
{
 {
  GIMPLE_PASS,
  "tminit",				/* name */
  NULL,					/* gate */
  execute_tm_done,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  0,					/* todo_flags_finish */
 }
};

/* Add FLAGS to the GIMPLE_TM_ATOMIC subcode for the transaction region
   represented by STATE.  */

static inline void
tm_atomic_subcode_ior (struct tm_region *state, unsigned flags)
{
  if (state->tm_atomic_stmt)
    gimple_tm_atomic_set_subcode (state->tm_atomic_stmt,
      gimple_tm_atomic_subcode (state->tm_atomic_stmt) | flags);
}


/* Construct a call to TM_IRREVOKABLE and insert it before GSI.  */

static void
expand_irrevokable (struct tm_region *state, gimple_stmt_iterator *gsi)
{
  gimple g;

  tm_atomic_subcode_ior (state, GTMA_HAVE_CALL_IRREVOKABLE);

  g = gimple_build_call (built_in_decls[BUILT_IN_TM_IRREVOKABLE], 0);
  add_stmt_to_eh_region (g, state->region_nr);

  gsi_insert_before (gsi, g, GSI_SAME_STMT);
}


/* Construct a memory load in a transactional context.  */

static tree
build_tm_load (tree lhs, tree rhs)
{
  enum built_in_function code = END_BUILTINS;
  tree t, type = TREE_TYPE (rhs);

  if (type == float_type_node)
    code = BUILT_IN_TM_LOAD_FLOAT;
  else if (type == double_type_node)
    code = BUILT_IN_TM_LOAD_DOUBLE;
  else if (TYPE_SIZE_UNIT (type) != NULL
	   && host_integerp (TYPE_SIZE_UNIT (type), 1))
    {
      switch (tree_low_cst (TYPE_SIZE_UNIT (type), 1))
	{
	case 1:
	  code = BUILT_IN_TM_LOAD_1;
	  break;
	case 2:
	  code = BUILT_IN_TM_LOAD_2;
	  break;
	case 4:
	  code = BUILT_IN_TM_LOAD_4;
	  break;
	case 8:
	  code = BUILT_IN_TM_LOAD_8;
	  break;
	}
    }

  if (code == END_BUILTINS)
    {
      sorry ("transactional load for %T not supported", type);
      code = BUILT_IN_TM_LOAD_4;
    }

  t = built_in_decls[code];
  t = build_call_expr (t, 1, build_fold_addr_expr (rhs));
  if (TYPE_MAIN_VARIANT (TREE_TYPE (t)) != TYPE_MAIN_VARIANT (type))
    t = build1 (VIEW_CONVERT_EXPR, type, t);
  t = build2 (MODIFY_EXPR, TREE_TYPE (lhs), lhs, t);

  return t;
}


/* Similarly for storing TYPE in a transactional context.  */

static tree
build_tm_store (tree lhs, tree rhs)
{
  enum built_in_function code = END_BUILTINS;
  tree t, fn, type = TREE_TYPE (rhs), simple_type;

  if (type == float_type_node)
    code = BUILT_IN_TM_STORE_FLOAT;
  else if (type == double_type_node)
    code = BUILT_IN_TM_STORE_DOUBLE;
  else if (TYPE_SIZE_UNIT (type) != NULL
	   && host_integerp (TYPE_SIZE_UNIT (type), 1))
    {
      switch (tree_low_cst (TYPE_SIZE_UNIT (type), 1))
	{
	case 1:
	  code = BUILT_IN_TM_STORE_1;
	  break;
	case 2:
	  code = BUILT_IN_TM_STORE_2;
	  break;
	case 4:
	  code = BUILT_IN_TM_STORE_4;
	  break;
	case 8:
	  code = BUILT_IN_TM_STORE_8;
	  break;
	}
    }

  if (code == END_BUILTINS)
    {
      sorry ("transactional load for %T not supported", type);
      code = BUILT_IN_TM_STORE_4;
    }

  fn = built_in_decls[code];
  simple_type = TREE_VALUE (TREE_CHAIN (TYPE_ARG_TYPES (TREE_TYPE (fn))));
  if (TYPE_MAIN_VARIANT (simple_type) != TYPE_MAIN_VARIANT (type))
    rhs = build1 (VIEW_CONVERT_EXPR, simple_type, rhs);
  t = build_call_expr (fn, 2, build_fold_addr_expr (lhs), rhs);

  return t;
}


/* Expand an assignment statement into transactional builtins.  */

static void
expand_assign_tm (struct tm_region *state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree lhs = gimple_assign_lhs (stmt);
  tree rhs = gimple_assign_rhs1 (stmt);
  bool store_p = requires_barrier (lhs);
  bool load_p = requires_barrier (rhs);
  tree call;
  gimple_seq seq;
  gimple gcall;
  gimple_stmt_iterator gsi2;
  struct gimplify_ctx gctx;

  if (load_p && store_p)
    {
      tm_atomic_subcode_ior (state, GTMA_HAVE_LOAD | GTMA_HAVE_STORE);
      call = build_call_expr (built_in_decls [BUILT_IN_TM_MEMCPY],
			      3, build_fold_addr_expr (lhs),
			      build_fold_addr_expr (rhs),
			      TYPE_SIZE_UNIT (TREE_TYPE (lhs)));
    }
  else if (load_p)
    {
      tm_atomic_subcode_ior (state, GTMA_HAVE_LOAD);
      call = build_tm_load (lhs, rhs);
    }
  else if (store_p)
    {
      tm_atomic_subcode_ior (state, GTMA_HAVE_STORE);
      call = build_tm_store (lhs, rhs);
    }
  else
    return;

  push_gimplify_context (&gctx);
  gctx.into_ssa = false;

  seq = NULL;
  gimplify_and_add (call, &seq);

  pop_gimplify_context (NULL);

  for (gsi2 = gsi_last (seq); ; gsi_prev (&gsi2))
    {
      gcall = gsi_stmt (gsi2);
      if (gimple_code (gcall) == GIMPLE_CALL)
	break;
    }

  add_stmt_to_eh_region  (gcall, state->region_nr);
  gsi_insert_seq_before (gsi, seq, GSI_SAME_STMT);
  gsi_remove (gsi, true);
}


static tree
find_tm_clone (tree orig_decl ATTRIBUTE_UNUSED)
{
  return NULL_TREE;
}

/* Expand a call statement as appropriate for a transaction.  That is,
   either verify that the call does not affect the transaction, or
   redirect the call to a clone that handles transactions, or change
   the transaction state to IRREVOKABLE.  Return true if the call is
   one of the builtins that end a transaction.  */

static bool
expand_call_tm (struct tm_region *state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree fn_decl;
  unsigned flags;

  flags = gimple_call_flags (stmt);
  if (flags & ECF_CONST)
    return false;

  if (flag_exceptions && !(flags & ECF_NOTHROW))
    tm_atomic_subcode_ior (state, GTMA_HAVE_UNCOMMITTED_THROW);

  fn_decl = gimple_call_fndecl (stmt);
  if (!fn_decl)
    {
      /* ??? The ABI under discussion has us calling into the runtime
	 to determine if there's a transactional version of this function.
	 For now, just switch to irrevokable mode.  */
      expand_irrevokable (state, gsi);
      return false;
    }

  if (DECL_IS_TM_PURE (fn_decl))
    return false;

  if (DECL_BUILT_IN_CLASS (fn_decl) == BUILT_IN_NORMAL)
    {
      /* ??? TM_COMMIT in a nested transaction has an abnormal edge back to
	 the outer-most transaction (there are no nested retries), while
	 a TM_ABORT has an abnormal backedge to the inner-most transaction.
	 We havn't actually saved the inner-most transaction here.  We should
	 be able to get to it via the region_nr saved on STMT, and read the
	 tm_atomic_stmt from that, and find the first region block from there.
	 This assumes we don't expand GIMPLE_TM_ATOMIC until after all other
	 statements have been expanded.  */
      switch (DECL_FUNCTION_CODE (fn_decl))
	{
	case BUILT_IN_TM_COMMIT:
	case BUILT_IN_TM_ABORT:
	  /* Both of these calls end a transaction.  */
	  if (lookup_stmt_eh_region (stmt) == state->region_nr)
	    return true;

	default:
	  break;
	}

      return false;
    }

  fn_decl = find_tm_clone (fn_decl);
  if (fn_decl)
    {
      gimple_call_set_fndecl (stmt, fn_decl);
      return false;
    }

  expand_irrevokable (state, gsi);
  return false;
}


/* Expand all statements in BB as appropriate for being inside
   a transaction.  */
static void
expand_block_tm (struct tm_region *state, basic_block bb)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); )
    {
      gimple stmt = gsi_stmt (gsi);
      switch (gimple_code (stmt))
	{
	case GIMPLE_ASSIGN:
	  /* Only memory reads/writes need to be instrumented.  */
	  if (gimple_assign_single_p (stmt))
	    {
	      expand_assign_tm (state, &gsi);
	      continue;
	    }
	  break;

	case GIMPLE_CALL:
	  if (expand_call_tm (state, &gsi))
	    return;
	  break;

	case GIMPLE_ASM:
	  expand_irrevokable (state, &gsi);
	  break;

	default:
	  break;
	}
      gsi_next (&gsi);
    }
}

/* Entry point to the MARK phase of TM expansion.  Here we replace
   transactional memory statements with calls to builtins, and function
   calls with their transactional clones (if available).  But we don't
   yet lower GIMPLE_TM_ATOMIC or add the transaction restart back-edges.  */

static unsigned int
execute_tm_mark (void)
{
  struct tm_region *region;
  basic_block bb;
  VEC (basic_block, heap) *queue;

  queue = VEC_alloc (basic_block, heap, 10);

  for (region = all_tm_regions; region ; region = region->next)
    if (region->exit_blocks)
      {
	/* Collect a new SUBCODE set, now that optimizations are done.  */
	gimple_tm_atomic_set_subcode (region->tm_atomic_stmt, 0);

	VEC_quick_push (basic_block, queue, region->entry_block);
	do
	  {
	    bb = VEC_pop (basic_block, queue);
	    expand_block_tm (region, bb);

	    if (!bitmap_bit_p (region->exit_blocks, bb->index))
	      for (bb = first_dom_son (CDI_DOMINATORS, bb);
		   bb;
		   bb = next_dom_son (CDI_DOMINATORS, bb))
		VEC_safe_push (basic_block, heap, queue, bb);
	  }
	while (!VEC_empty (basic_block, queue));
      }
    else
      {
	FOR_EACH_BB (bb)
	  expand_block_tm (region, bb);
      }

  VEC_free (basic_block, heap, queue);

  return 0;
}

struct gimple_opt_pass pass_tm_mark =
{
 {
  GIMPLE_PASS,
  "tmmark",				/* name */
  NULL,					/* gate */
  execute_tm_mark,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_update_ssa
  | TODO_verify_ssa
  | TODO_dump_func,			/* todo_flags_finish */
 }
};

/* Create an abnormal call edge from BB to the first block of the region
   represented by STATE.  */

static inline void
make_tm_edge (basic_block bb, struct tm_region *state)
{
  make_edge (bb, state->entry_block, EDGE_ABNORMAL | EDGE_ABNORMAL_CALL);
}


/* Split the block at GSI and create an abnormal back edge.  */

static void ATTRIBUTE_UNUSED
split_and_add_tm_edge (struct tm_region *state, gimple_stmt_iterator *gsi)
{
  basic_block bb = gsi->bb;
  if (!gsi_one_before_end_p (*gsi))
    {
      edge e = split_block (bb, gsi_stmt (*gsi));
      *gsi = gsi_start_bb (e->dest);
    }

  make_tm_edge (bb, state);
}


/* ??? Find real values for these bits.  */
#define TM_START_RESTORE_LIVE_IN	1
#define TM_START_ABORT			2

/* All local variables that have been modified since the beginning of the
   transaction up until the last possible transaction restart need to be
   reset when we restart the transaction.

   Here we implement this by replacing the new SSA_NAME created by the
   PHI at the join of the abnormal backedges by the old SSA_NAME that
   was originally live at entry to the transaction.  This does result in
   the old SSA_NAME continuing to stay live when new values are defined,
   and this isn't necessarily the most efficient implementation, but it
   is just about the easiest.  */

static void ATTRIBUTE_UNUSED
checkpoint_live_in_variables (edge e)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_phis (e->dest); !gsi_end_p (gsi); )
    {
      gimple phi = gsi_stmt (gsi);
      tree old_ssa, new_ssa;
      unsigned i, n;

      new_ssa = gimple_phi_result (phi);
      old_ssa = gimple_phi_arg_def (phi, e->dest_idx);

      /* We need to recompute SSA_NAME_OCCURS_IN_ABNORMAL_PHI on each
	 of the other arguments to the PHI, discounting the one abnormal
	 phi node that we're about to delete.  */
      for (i = 0, n = gimple_phi_num_args (phi); i < n; ++i)
	{
	  tree arg = gimple_phi_arg_def (phi, i);
	  imm_use_iterator imm_iter;
	  use_operand_p use_p;
	  bool in_abnormal_phi;

	  if (TREE_CODE (arg) != SSA_NAME
	      || !SSA_NAME_OCCURS_IN_ABNORMAL_PHI (arg))
	    continue;

	  in_abnormal_phi = false;
	  FOR_EACH_IMM_USE_FAST (use_p, imm_iter, arg)
	    {
	      gimple phi2 = USE_STMT (use_p);
	      if (gimple_code (phi2) == GIMPLE_PHI && phi2 != phi)
		{
		  unsigned ix = PHI_ARG_INDEX_FROM_USE (use_p);
		  if (gimple_phi_arg_edge (phi2, ix)->flags & EDGE_ABNORMAL)
		    {
		      in_abnormal_phi = true;
		      break;
		    }
		}
	    }
	  SSA_NAME_OCCURS_IN_ABNORMAL_PHI (arg) = in_abnormal_phi;
	}

      replace_uses_by (new_ssa, old_ssa);
      remove_phi_node (&gsi, true);
    }
}

static void ATTRIBUTE_UNUSED
expand_tm_atomic (basic_block bb, gimple_stmt_iterator *gsi)
{
  tree status, tm_start;
  basic_block body_bb, test_bb;
  gimple_stmt_iterator gsi2;
  tree t1, t2;
  gimple g;
  edge e;

  tm_start = built_in_decls[BUILT_IN_TM_START];
  status = make_rename_temp (TREE_TYPE (TREE_TYPE (tm_start)), "tm_state");

  e = FALLTHRU_EDGE (bb);
  body_bb = e->dest;
  checkpoint_live_in_variables (e);

  if (gimple_tm_atomic_label (gsi_stmt (*gsi)))
    {
      e = split_block_after_labels (body_bb);
      test_bb = e->src;
      body_bb = e->dest;

      gsi2 = gsi_last_bb (test_bb);

      t1 = make_rename_temp (TREE_TYPE (status), NULL);
      t2 = build_int_cst (TREE_TYPE (status), TM_START_ABORT);
      g = gimple_build_assign_with_ops (BIT_AND_EXPR, t1, status, t2);
      gsi_insert_after (&gsi2, g, GSI_CONTINUE_LINKING);

      t2 = build_int_cst (TREE_TYPE (status), 0);
      g = gimple_build_cond (NE_EXPR, t1, t2, NULL, NULL);
      gsi_insert_after (&gsi2, g, GSI_CONTINUE_LINKING);

      single_succ_edge (test_bb)->flags = EDGE_FALSE_VALUE;

      e = BRANCH_EDGE (bb);
      redirect_edge_pred (e, test_bb);
      e->flags = EDGE_TRUE_VALUE;
    }

  /* ??? Need to put the real input to __tm_start here.  */
  t2 = build_int_cst (TREE_TYPE (status), 0);
  g = gimple_build_call (tm_start, 1, t2);
  gimple_call_set_lhs (g, status);
  gsi_insert_before (gsi, g, GSI_SAME_STMT);
  gsi_remove (gsi, true);
}

/* Entry point to the final expansion of transactional nodes. */

static unsigned int
execute_tm_edges (void)
{
  return 0;
}

struct gimple_opt_pass pass_tm_edges =
{
 {
  GIMPLE_PASS,
  "tmedge",				/* name */
  gate_tm_init,				/* gate */
  execute_tm_edges,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_verify_ssa | TODO_dump_func,	/* todo_flags_finish */
 }
};


static unsigned int
execute_tm_memopt (void)
{
  return 0;
}

static bool
gate_tm_memopt (void)
{
  return optimize > 0;
}

struct gimple_opt_pass pass_tm_memopt =
{
 {
  GIMPLE_PASS,
  "tminit",				/* name */
  gate_tm_memopt,			/* gate */
  execute_tm_memopt,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func,			/* todo_flags_finish */
 }
};



static unsigned int
execute_ipa_tm (void)
{
  return 0;
}

struct simple_ipa_opt_pass pass_ipa_tm =
{
 {
  SIMPLE_IPA_PASS,
  "tmipa",				/* name */
  gate_tm,				/* gate */
  execute_ipa_tm,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  0,					/* todo_flags_finish */
 }
};
