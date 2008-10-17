/* Lowering pass for transactional memory directives.
   Converts markers of transactions into explicit calls to
   the TM runtime library.

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


/* The representation of a transaction changes several times during the
   lowering process.  In the beginning, in the front-end we have the
   GENERIC tree TM_ATOMIC.  For example,

	__tm_atomic {
	  local++;
	  if (++global == 10)
	    __tm_abort;
	}

  is represented as

	TM_ATOMIC {
	  local++;
	  if (++global == 10)
	    __builtin___tm_abort ();
	}

  During initial gimplification (gimplify.c) the TM_ATOMIC node is
  trivially replaced with a GIMPLE_TM_ATOMIC node, and we add bits
  to handle EH cleanup of the transaction:

	GIMPLE_TM_ATOMIC [label=NULL] {
	  try {
	    local = local + 1;
	    t0 [tm_load]= global;
	    t1 = t0 + 1;
	    global [tm_store]= t1;
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
  correct to model transaction aborts and restarts.

  During pass_lower_tm, we mark the gimple statements that perform
  transactional memory operations with TM_LOAD/TM_STORE, and swap out
  function calls with their (non-)transactional clones.  At this time
  we flatten nested transactions (when possible), and flatten the
  GIMPLE representation.

	GIMPLE_TM_ATOMIC [label=over]
	eh_label:
	local = local + 1;
	t0 [tm_load]= global;
	t1 = t0 + 1;
	global [tm_store]= t1;
	if (t1 == 10)
	  __builtin___tm_abort ();
	__builtin___tm_commit ();
	over:

  During pass_checkpoint_tm, we complete the lowering of the
  GIMPLE_TM_ATOMIC node.  Here we examine the SSA web and arange for
  local variables to be saved and restored in the event of an abort.

	save_local = local;
	x = __builtin___tm_start (MAY_ABORT);
	eh_label:
	if (x & restore_locals) {
	  local = save_local;
	}
	if (x & abort_transaction)
	  goto over;
	local = local + 1;
	t0 [tm_load]= global;
	t1 = t0 + 1;
	global [tm_store]= t1;
	if (t1 == 10)
	  __builtin___tm_abort ();
	__builtin___tm_commit ();
	over:

  During expansion to rtl, we expand the TM_LOAD/TM_STORE markings
  with calls to the appropriate builtin functions.  Delaying this long
  allows the tree optimizers the most visibility into the operations.  */

DEF_VEC_O(gimple_stmt_iterator);
DEF_VEC_ALLOC_O(gimple_stmt_iterator,heap);

struct ltm_state
{
  /* Bits to be stored in the GIMPLE_TM_ATOMIC subcode.  */
  unsigned subcode;

  /* The EH region number for this transaction.  Non-negative numbers
     represent an active transaction within this function; -1 represents
     an active transaction from a calling function (i.e. we're compiling
     a transaction clone).  For no active transaction, the state pointer
     passed will be null.  */
  int region_nr;

  /* Record the iterator pointing to a __tm_commit function call that
     binds to this transaction region.  There may be many such calls,
     depending on how the EH expansion of the try-finally node went.
     But there's usually exactly one such call, and essentially always
     only a small number, so to avoid rescanning the entire sequence
     when we need to remove these calls, record the iterator location.  */
  VEC(gimple_stmt_iterator,heap) *commit_stmts;
};


static void lower_sequence_tm (struct ltm_state *, gimple_seq);
static void lower_sequence_no_tm (gimple_seq);


/* Record the transaction for this statement.  If the statement
   already has a region number that's fine -- it means that the
   statement can also throw.  If there's no region number, it 
   means we're expanding a transactional clone and the region
   is in a calling function.  */

static void
add_stmt_to_transaction (struct ltm_state *state, gimple stmt)
{
  if (state->region_nr >= 0 && lookup_stmt_eh_region (stmt) < 0)
    add_stmt_to_eh_region (stmt, state->region_nr);
}

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
      /* ??? Use must-alias information to reduce this.  */
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
lower_assign_tm (struct ltm_state *state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  bool load_p = requires_barrier (gimple_assign_rhs1 (stmt));
  bool store_p = requires_barrier (gimple_assign_lhs (stmt));

  if (load_p && store_p)
    {
      /* ??? This is a copy between two aggregates in memory.  I
	 believe the Intel compiler handles this with a special
	 version of memcpy.  For now, just consider the transaction
	 irrevokable at this point.  */
      state->subcode |= GTMA_HAVE_CALL_IRREVOKABLE;
      return;
    }
  else if (load_p)
    {
      gimple_assign_set_rhs_code (stmt, TM_LOAD);
      state->subcode |= GTMA_HAVE_LOAD;
    }
  else if (store_p)
    {
      gimple_assign_set_rhs_code (stmt, TM_STORE);
      state->subcode |= GTMA_HAVE_STORE;
    }
  else
    return;

  add_stmt_to_transaction (state, stmt);
}

/* Mark a GIMPLE_CALL as appropriate for being inside a transaction.  */

static void
lower_call_tm (struct ltm_state *state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree fn_decl;
  struct cgraph_node *node, *orig_node;
  int flags;

  flags = gimple_call_flags (stmt);
  if (flags & ECF_CONST)
    return;

  fn_decl = gimple_call_fndecl (stmt);
  if (!fn_decl)
    {
      state->subcode |= GTMA_HAVE_CALL_INDIRECT;
      return;
    }

  /* Check if this call is one of our transactional builtins.  */
  if (DECL_BUILT_IN_CLASS (fn_decl) == BUILT_IN_NORMAL)
    switch (DECL_FUNCTION_CODE (fn_decl))
      {
      case BUILT_IN_TM_COMMIT:
	/* Remember the commit so that we can remove it if
	   we decide to elide the transaction.  */
	VEC_safe_push (gimple_stmt_iterator,heap, state->commit_stmts, gsi);
	return;
      case BUILT_IN_TM_ABORT:
	state->subcode |= GTMA_HAVE_ABORT;
	add_stmt_to_transaction (state, stmt);
	return;

      default:
	break;
      }

  if (DECL_IS_TM_PURE (fn_decl))
    return;

  orig_node = node = cgraph_node (fn_decl);

  /* Find transactional clone of function.  */
  while (node && node->next_clone)
    {
      node = node->next_clone;
      if (DECL_IS_TM_CLONE (node->decl))
	break;
    }
  if (DECL_IS_TM_CLONE (node->decl))
    {
      struct cgraph_edge *callers = orig_node->callers;

      /* Find appropriate call stmt to redirect.  */
      while (callers)
	{
	  if (callers->call_stmt == stmt)
	    break;
	  callers = callers->next_caller;
	}

      /* Substitute call stmt.  */
      if (callers)
	{
	  gimple_call_set_fndecl (stmt, node->decl);
	  cgraph_redirect_edge_callee (callers, node);
	  if (dump_file)
	    {
	      fprintf (dump_file, "redirected edge to");
	      print_generic_expr (dump_file, node->decl, 0);
	      fputc ('\n', dump_file);
	    }

	  state->subcode |= GTMA_HAVE_CALL_TM;
	  add_stmt_to_transaction (state, stmt);
	  return;
	}
    }

  /* The function was not const, tm_pure, or redirected to a 
     transactional clone.  The call is therefore considered to
     be irrevokable.  */
  state->subcode |= GTMA_HAVE_CALL_IRREVOKABLE;
}

/* Remove any calls to __tm_commit inside STATE which belong
   to the transaction.  */

static void
remove_tm_commits (struct ltm_state *state)
{
  gimple_stmt_iterator *gsi;
  unsigned i;

  for (i = 0;
       VEC_iterate(gimple_stmt_iterator, state->commit_stmts, i, gsi);
       ++i)
    gsi_remove (gsi, true);
}

/* Lower a GIMPLE_TM_ATOMIC statement.  The GSI is advanced.  */

static void
lower_tm_atomic (struct ltm_state *outer_state, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  struct ltm_state this_state;
  struct eh_region *eh_region;
  tree label;

  this_state.subcode = 0;
  this_state.region_nr = lookup_stmt_eh_region (stmt);
  this_state.commit_stmts = VEC_alloc(gimple_stmt_iterator, heap, 1);

  gcc_assert (this_state.region_nr >= 0);
  eh_region = get_eh_region_from_number (this_state.region_nr);

  /* First, lower the body.  The scanning that we do inside gives
     us some idea of what we're dealing with.  */
  lower_sequence_tm (&this_state, gimple_seq_body (stmt));

  /* If there was absolutely nothing transaction related inside the
     transaction, we may elide it.  Likewise if this is a nested
     transaction and does not contain an abort.  */
  if (this_state.subcode == 0
      || (!(this_state.subcode & GTMA_HAVE_ABORT)
	  && outer_state != NULL))
    {
      if (outer_state)
	outer_state->subcode |= this_state.subcode;

      remove_tm_commits (&this_state);
      gsi_insert_seq_before (gsi, gimple_seq_body (stmt), GSI_SAME_STMT);
      gimple_seq_set_body (stmt, NULL);
      gsi_remove (gsi, true);
      remove_eh_handler (eh_region);
      goto fini;
    }

  /* Insert an EH_LABEL immediately after the GIMPLE_TM_ATOMIC node.
     This label won't really be used, but it mimicks the effect of 
     the setjmp/longjmp that's going on behind the scenes.  */
  label = create_artificial_label ();
  set_eh_region_tree_label (eh_region, label);
  gsi_insert_after (gsi, gimple_build_label (label), GSI_CONTINUE_LINKING);

  /* Insert the entire transaction sequence.  */
  gsi_insert_seq_after (gsi, gimple_seq_body (stmt), GSI_CONTINUE_LINKING);
  gimple_seq_set_body (stmt, NULL);

  /* Record a label at the end of the transaction that will be used in
     case the transaction aborts.  */
  if (this_state.subcode & GTMA_HAVE_ABORT)
    {
      label = create_artificial_label ();
      gimple_tm_atomic_set_label (stmt, label);
      gsi_insert_after (gsi, gimple_build_label (label), GSI_CONTINUE_LINKING);
    }

  /* Record the set of operations found for use during final lowering
     of the GIMPLE_TM_ATOMIC node.  */
  gimple_tm_atomic_set_subcode (stmt, this_state.subcode);

  /* Always update the iterator.  */
  gsi_next (gsi);

 fini:
  VEC_free (gimple_stmt_iterator, heap, this_state.commit_stmts);
}

/* Iterate through the statements in the sequence, lowering them all
   as appropriate for being in a transaction.  */

static void
lower_sequence_tm (struct ltm_state *state, gimple_seq seq)
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
	    lower_assign_tm (state, &gsi);
	  break;

	case GIMPLE_CALL:
	  lower_call_tm (state, &gsi);
	  break;

	case GIMPLE_TM_ATOMIC:
	  lower_tm_atomic (state, &gsi);
	  goto no_update;

	default:
	  break;
	}
      gsi_next (&gsi);
    no_update:
      ;
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
  /* Functions that are marked TM_PURE don't need annotation by definition.  */
  /* ??? The Intel OOPSLA paper talks about performing the same scan of the
     function as we would if the function was marked DECL_IS_TM_CLONE, and
     warning if we find anything for which we would have made a change.  */
  if (DECL_IS_TM_PURE (current_function_decl))
    return 0;

  /* When instrumenting a transactional clone, we begin the function inside
     a transaction.  */
  if (DECL_IS_TM_CLONE (current_function_decl))
    {
      struct ltm_state state;
      state.subcode = 0;
      state.region_nr = -1;
      lower_sequence_tm (&state, gimple_body (current_function_decl));
    }
  else
    lower_sequence_no_tm (gimple_body (current_function_decl));

  return 0;
}

/* TM expansion -- the default pass, run before creation of SSA form.  */

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
  PROP_gimple_leh,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func		        /* todo_flags_finish */
 }
};


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

static void
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

static void
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

/* Entry point to the checkpointing. */

static unsigned int
execute_checkpoint_tm (void)
{
  basic_block bb;

  FOR_EACH_BB (bb)
    {
      gimple_stmt_iterator gsi = gsi_last_bb (bb);
      if (!gsi_end_p (gsi)
	  && gimple_code (gsi_stmt (gsi)) == GIMPLE_TM_ATOMIC)
	expand_tm_atomic (bb, &gsi);
    }

  return 0;
}

struct gimple_opt_pass pass_checkpoint_tm =
{
 {
  GIMPLE_PASS,
  "tmcheckpoint",			/* name */
  gate_tm,				/* gate */
  execute_checkpoint_tm,		/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_update_ssa |
  TODO_verify_ssa |
  TODO_dump_func,			/* todo_flags_finish */
 }
};

/* Construct transaction restart edges for STMT.  */

static void
make_tm_edge_1 (struct eh_region *region, void *data)
{
  gimple stmt = (gimple) data;
  basic_block src, dst;
  unsigned flags;

  src = gimple_bb (stmt);
  dst = label_to_block (get_eh_region_tree_label (region));

  /* Don't set EDGE_EH here, because that's supposed to be used when
     we could in principal redirect the edge by modifying the exception
     tables.  Transactions don't use tables though, only setjmp.  */
  flags = EDGE_ABNORMAL;
  if (gimple_code (stmt) == GIMPLE_CALL)
    flags |= EDGE_ABNORMAL_CALL;
  make_edge (src, dst, flags);
}

void
make_tm_edge (gimple stmt)
{
  int region_nr;

  /* Do nothing if the region is outside this function.  */
  region_nr = lookup_stmt_eh_region (stmt);
  if (region_nr < 0)
    return;

  /* The control structure inside tree-cfg.c isn't the best;
     re-check whether this is actually a transactional stmt.  */
  if (!is_transactional_stmt (stmt))
    return;

  foreach_reachable_transaction (region_nr, make_tm_edge_1, (void *) stmt);
}

/* Return true if STMT may alter control flow via a transactional edge.  */

bool
is_transactional_stmt (const_gimple stmt)
{
  switch (gimple_code (stmt))
    {
    case GIMPLE_ASSIGN:
      {
	/* We only want to process assignments that have been
	   marked for transactional memory.  */
	enum tree_code subcode = gimple_expr_code (stmt);
	return (subcode == TM_LOAD || subcode == TM_STORE);
      }

    case GIMPLE_CALL:
      {
	tree fn_decl = gimple_call_fndecl (stmt);

	/* We only want to process __tm_abort and cloned direct calls,
	   since those are the only ones that can restart the transaction.  */
	if (!fn_decl)
	  return false;
	if (DECL_BUILT_IN_CLASS (fn_decl) == BUILT_IN_NORMAL
	    && DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_TM_ABORT)
	  return true;
	if (DECL_IS_TM_CLONE (fn_decl))
	  return true;
	else
	  return false;
      }

    case GIMPLE_TM_ATOMIC:
      return true;

    default:
      return false;
    }
}
