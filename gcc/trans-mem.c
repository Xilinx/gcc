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


#define PROB_VERY_UNLIKELY	(REG_BR_PROB_BASE / 2000 - 1)
#define PROB_ALWAYS		(REG_BR_PROB_BASE)


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

  gimple_call_set_in_tm_atomic (stmt, true);

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
  lower_sequence_tm (&this_state, gimple_tm_atomic_body (stmt));

  /* If there was absolutely nothing transaction related inside the
     transaction, we may elide it.  Likewise if this is a nested
     transaction and does not contain an abort.  */
  if (this_state == 0
      || (!(this_state & GTMA_HAVE_ABORT) && outer_state != NULL))
    {
      if (outer_state)
	*outer_state |= this_state;

      gsi_insert_seq_before (gsi, gimple_tm_atomic_body (stmt), GSI_SAME_STMT);
      gimple_tm_atomic_set_body (stmt, NULL);
      gsi_remove (gsi, true);
      return;
    }

  /* Wrap the body of the transaction in a try-finally node so that
     the commit call is always properly called.  */
  g = gimple_build_call (built_in_decls[BUILT_IN_TM_COMMIT], 0);
  g = gimple_build_try (gimple_tm_atomic_body (stmt),
			gimple_seq_alloc_with_stmt (g), GIMPLE_TRY_FINALLY);
  gimple_tm_atomic_set_body (stmt, gimple_seq_alloc_with_stmt (g));

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

/* Add FLAGS to the GIMPLE_TM_ATOMIC subcode for the transaction region
   represented by STATE.  */

static inline void
tm_atomic_subcode_ior (struct tm_region *region, unsigned flags)
{
  if (region->tm_atomic_stmt)
    gimple_tm_atomic_set_subcode (region->tm_atomic_stmt,
      gimple_tm_atomic_subcode (region->tm_atomic_stmt) | flags);
}


/* Add STMT to the EH region for the given TM region.  */

static void
add_stmt_to_tm_region (struct tm_region *region, gimple stmt)
{
  if (region->region_nr >= 0)
    add_stmt_to_eh_region (stmt, region->region_nr);
}


/* Construct a call to TM_IRREVOKABLE and insert it before GSI.  */

static void
expand_irrevokable (struct tm_region *region, gimple_stmt_iterator *gsi)
{
  gimple g;

  tm_atomic_subcode_ior (region, GTMA_HAVE_CALL_IRREVOKABLE);

  g = gimple_build_call (built_in_decls[BUILT_IN_TM_IRREVOKABLE], 0);
  add_stmt_to_tm_region (region, g);

  gsi_insert_before (gsi, g, GSI_SAME_STMT);
}


/* Mark virtuals in STMT for renaming.  */

static void
mark_vops_in_stmt (gimple stmt)
{
  ssa_op_iter iter;
  tree op;

  FOR_EACH_SSA_TREE_OPERAND (op, stmt, iter, SSA_OP_ALL_VIRTUALS)
    {
      mark_sym_for_renaming (SSA_NAME_VAR (op));
    }
}


/* Construct a memory load in a transactional context.  */

static gimple
build_tm_load (tree lhs, tree rhs, gimple_stmt_iterator *gsi)
{
  enum built_in_function code = END_BUILTINS;
  tree t, type = TREE_TYPE (rhs);
  gimple gcall;

  if (type == float_type_node)
    code = BUILT_IN_TM_LOAD_FLOAT;
  else if (type == double_type_node)
    code = BUILT_IN_TM_LOAD_DOUBLE;
  else if (type == long_double_type_node)
    code = BUILT_IN_TM_LOAD_LDOUBLE;
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

  t = build_fold_addr_expr (rhs);
  gcc_assert (is_gimple_operand (t));

  gcall = gimple_build_call (built_in_decls[code], 1, t);

  t = TREE_TYPE (TREE_TYPE (built_in_decls[code]));
  if (useless_type_conversion_p (type, t))
    {
      gimple_call_set_lhs (gcall, lhs);
      gsi_insert_before (gsi, gcall, GSI_SAME_STMT);
    }
  else
    {
      gimple g;
      tree temp;

      temp = make_rename_temp (t, NULL);
      gimple_call_set_lhs (gcall, temp);
      gsi_insert_before (gsi, gcall, GSI_SAME_STMT);

      t = fold_build1 (VIEW_CONVERT_EXPR, type, temp);
      g = gimple_build_assign (lhs, t);
      gsi_insert_before (gsi, g, GSI_SAME_STMT);
    }

  return gcall;
}


/* Similarly for storing TYPE in a transactional context.  */

static gimple
build_tm_store (tree lhs, tree rhs, gimple_stmt_iterator *gsi)
{
  enum built_in_function code = END_BUILTINS;
  tree t, fn, type = TREE_TYPE (rhs), simple_type;
  gimple gcall;

  if (type == float_type_node)
    code = BUILT_IN_TM_STORE_FLOAT;
  else if (type == double_type_node)
    code = BUILT_IN_TM_STORE_DOUBLE;
  else if (type == long_double_type_node)
    code = BUILT_IN_TM_STORE_LDOUBLE;
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

  if (!useless_type_conversion_p (simple_type, type))
    {
      gimple g;
      tree temp;

      temp = make_rename_temp (simple_type, NULL);
      t = fold_build1 (VIEW_CONVERT_EXPR, simple_type, rhs);
      g = gimple_build_assign (temp, t);
      gsi_insert_before (gsi, g, GSI_SAME_STMT);

      rhs = temp;
    }

  t = build_fold_addr_expr (lhs);
  gcall = gimple_build_call (built_in_decls[code], 2, t, rhs);
  gsi_insert_before (gsi, gcall, GSI_SAME_STMT);
  
  return gcall;
}


/* Expand an assignment statement into transactional builtins.  */

static void
expand_assign_tm (struct tm_region *region, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree lhs = gimple_assign_lhs (stmt);
  tree rhs = gimple_assign_rhs1 (stmt);
  bool store_p = requires_barrier (lhs);
  bool load_p = requires_barrier (rhs);
  gimple gcall;

  mark_vops_in_stmt (stmt);
  gsi_remove (gsi, true);

  if (load_p && store_p)
    {
      tm_atomic_subcode_ior (region, GTMA_HAVE_LOAD | GTMA_HAVE_STORE);

      gcall = gimple_build_call (built_in_decls [BUILT_IN_TM_MEMCPY], 3,
				 build_fold_addr_expr (lhs),
				 build_fold_addr_expr (rhs),
				 TYPE_SIZE_UNIT (TREE_TYPE (lhs)));
      gsi_insert_before (gsi, gcall, GSI_SAME_STMT);
    }
  else if (load_p)
    {
      tm_atomic_subcode_ior (region, GTMA_HAVE_LOAD);
      gcall = build_tm_load (lhs, rhs, gsi);
    }
  else if (store_p)
    {
      tm_atomic_subcode_ior (region, GTMA_HAVE_STORE);
      gcall = build_tm_store (lhs, rhs, gsi);
    }
  else
    return;

  add_stmt_to_tm_region  (region, gcall);
}


/* Expand a call statement as appropriate for a transaction.  That is,
   either verify that the call does not affect the transaction, or
   redirect the call to a clone that handles transactions, or change
   the transaction state to IRREVOKABLE.  Return true if the call is
   one of the builtins that end a transaction.  */

static bool
expand_call_tm (struct tm_region *region, gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree fn_decl;
  unsigned flags;

  flags = gimple_call_flags (stmt);
  if (flags & ECF_CONST)
    return false;

  if (flag_exceptions && !(flags & ECF_NOTHROW))
    tm_atomic_subcode_ior (region, GTMA_HAVE_UNCOMMITTED_THROW);

  fn_decl = gimple_call_fndecl (stmt);
  if (!fn_decl)
    {
      /* ??? The ABI under discussion has us calling into the runtime
	 to determine if there's a transactional version of this function.
	 For now, just switch to irrevokable mode.  */
      expand_irrevokable (region, gsi);
      return false;
    }

  if (DECL_IS_TM_PURE (fn_decl) || DECL_IS_TM_CLONE (fn_decl))
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
	  if (lookup_stmt_eh_region (stmt) == region->region_nr)
	    return true;

	default:
	  break;
	}

      return false;
    }

  expand_irrevokable (region, gsi);
  return false;
}


/* Expand all statements in BB as appropriate for being inside
   a transaction.  */
static void
expand_block_tm (struct tm_region *region, basic_block bb)
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
	      expand_assign_tm (region, &gsi);
	      continue;
	    }
	  break;

	case GIMPLE_CALL:
	  if (expand_call_tm (region, &gsi))
	    return;
	  break;

	case GIMPLE_ASM:
	  expand_irrevokable (region, &gsi);
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
make_tm_edge (basic_block bb, struct tm_region *region)
{
  make_edge (bb, region->entry_block, EDGE_ABNORMAL | EDGE_ABNORMAL_CALL);
}


static void
expand_block_edges (struct tm_region *region, basic_block bb)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); )
    {
      gimple stmt = gsi_stmt (gsi);

      if (gimple_code (stmt) == GIMPLE_CALL
	  && (gimple_call_flags (stmt) & ECF_TM_OPS) != 0)
	{
	  if (!gsi_one_before_end_p (gsi))
	    {
	      edge e = split_block (bb, stmt);
	      make_tm_edge (bb, region);
	      bb = e->dest;
	      gsi = gsi_start_bb (bb);
	      continue;
	    }

	  make_tm_edge (bb, region);
	}

      gsi_next (&gsi);
    }
}

/* ??? Find real values for these bits.  */
#define TM_START_RESTORE_LIVE_IN	1
#define TM_START_ABORT			2

static void
expand_tm_atomic (struct tm_region *region)
{
  tree status, tm_start;
  basic_block atomic_bb;
  gimple_stmt_iterator gsi;
  tree t1, t2;
  gimple g;

  mark_vops_in_stmt (region->tm_atomic_stmt);

  tm_start = built_in_decls[BUILT_IN_TM_START];
  status = make_rename_temp (TREE_TYPE (TREE_TYPE (tm_start)), "tm_state");

  /* ??? Need to put the real input to __tm_start here.  */
  t2 = build_int_cst (TREE_TYPE (status), 0);
  g = gimple_build_call (tm_start, 1, t2);
  gimple_call_set_lhs (g, status);

  atomic_bb = gimple_bb (region->tm_atomic_stmt);
  gsi = gsi_last_bb (atomic_bb);
  gsi_insert_before (&gsi, g, GSI_SAME_STMT);
  gsi_remove (&gsi, true);

  /* If we have an ABORT statement, create a test following the start
     call to perform the abort.  */
  if (gimple_tm_atomic_label (region->tm_atomic_stmt))
    {
      edge e1, e2;
      basic_block test_bb;

      region->entry_block = test_bb = create_empty_bb (atomic_bb);
      gsi = gsi_last_bb (test_bb);

      t1 = make_rename_temp (TREE_TYPE (status), NULL);
      t2 = build_int_cst (TREE_TYPE (status), TM_START_ABORT);
      g = gimple_build_assign_with_ops (BIT_AND_EXPR, t1, status, t2);
      gsi_insert_after (&gsi, g, GSI_CONTINUE_LINKING);

      t2 = build_int_cst (TREE_TYPE (status), 0);
      g = gimple_build_cond (NE_EXPR, t1, t2, NULL, NULL);
      gsi_insert_after (&gsi, g, GSI_CONTINUE_LINKING);

      e1 = FALLTHRU_EDGE (atomic_bb);
      e2 = make_edge (test_bb, e1->dest, EDGE_FALSE_VALUE);
      redirect_edge_succ (e1, test_bb);
      e1->probability = PROB_ALWAYS;

      e1 = BRANCH_EDGE (atomic_bb);
      redirect_edge_pred (e1, test_bb);
      e1->flags = EDGE_TRUE_VALUE;
      e1->probability = PROB_VERY_UNLIKELY;
      e2->probability = PROB_ALWAYS - PROB_VERY_UNLIKELY;
    }

  /* Record an EH label for the region.  This will be where the 
     transaction restart backedge goes.  This is sort of fake,
     since the runtime actually uses longjmp to get back here,
     but its existance makes it easier to interface with the
     rest of the EH code in creating the CFG.  */
  set_eh_region_tree_label (get_eh_region_from_number (region->region_nr),
			    gimple_block_label (region->entry_block));

  /* The GIMPLE_TM_ATOMIC statement no longer exists.  */
  region->tm_atomic_stmt = NULL;
}

/* Entry point to the final expansion of transactional nodes. */

static unsigned int
execute_tm_edges (void)
{
  struct tm_region *region;
  basic_block bb;
  VEC (basic_block, heap) *queue;
  bitmap blocks;
  bitmap_iterator iter;
  unsigned int i;

  queue = VEC_alloc (basic_block, heap, 10);
  blocks = BITMAP_ALLOC (&tm_obstack);

  for (region = all_tm_regions; region ; region = region->next)
    if (region->exit_blocks)
      {
	/* Collect the set of blocks in this region.  Do this before
	   splitting edges, so that we don't have to play with the
	   dominator tree in the middle.  */
	bitmap_clear (blocks);
	VEC_quick_push (basic_block, queue, region->entry_block);
	do
	  {
	    bb = VEC_pop (basic_block, queue);
	    bitmap_set_bit (blocks, bb->index);
	    if (!bitmap_bit_p (region->exit_blocks, bb->index))
	      for (bb = first_dom_son (CDI_DOMINATORS, bb);
		   bb;
		   bb = next_dom_son (CDI_DOMINATORS, bb))
		VEC_safe_push (basic_block, heap, queue, bb);
	  }
	while (!VEC_empty (basic_block, queue));

	expand_tm_atomic (region);

	EXECUTE_IF_SET_IN_BITMAP (blocks, 0, i, iter)
	  expand_block_edges (region, BASIC_BLOCK (i));
      }

  VEC_free (basic_block, heap, queue);
  BITMAP_FREE (blocks);

  /* We've got to release the dominance info now, to indicate that it
     must be rebuilt completely.  Otherwise we'll crash trying to update
     the SSA web in the TODO section following this pass.  */
  free_dominance_info (CDI_DOMINATORS);
  bitmap_obstack_release (&tm_obstack);

  return 0;
}

struct gimple_opt_pass pass_tm_edges =
{
 {
  GIMPLE_PASS,
  "tmedge",				/* name */
  NULL,					/* gate */
  execute_tm_edges,			/* execute */
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
  "tmmemopt",				/* name */
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


static struct cgraph_node_hook_list *function_insertion_hook_holder;

static void
ipa_tm_analyze_function (struct cgraph_node *node)
{
  struct cgraph_edge *e;

  if (cgraph_function_body_availability (node) < AVAIL_OVERWRITABLE)
    return;

  /* If this is a transaction clone, then by definition we're already
     inside a transaction, and thus by definition all of our callees
     are within a transaction.  */
  if (DECL_IS_TM_CLONE (node->decl))
    {
      for (e = node->callees; e ; e = e->next_callee)
	e->tm_atomic_call = 1;
    }

  /* Otherwise, scan all blocks and transfer the IN-ATOMIC bit we set
     on the call statement to the cgraph edge.  */
  else
    {
      basic_block bb;

      FOR_EACH_BB_FN (bb, DECL_STRUCT_FUNCTION (node->decl))
	{
	  gimple_stmt_iterator gsi;
	  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
	    {
	      gimple stmt = gsi_stmt (gsi);
	      if (is_gimple_call (stmt)
		  && gimple_call_in_tm_atomic_p (stmt))
		{
		  e = cgraph_edge (node, stmt);
		  e->tm_atomic_call = 1;
		}
	    }
	}
    }
}

static void
ipa_tm_add_new_function (struct cgraph_node *node, void * ARG_UNUSED (data))
{
  ipa_tm_analyze_function (node);
}

static void
ipa_tm_generate_summary (void)
{
  struct cgraph_node *node;

  function_insertion_hook_holder =
    cgraph_add_function_insertion_hook (&ipa_tm_add_new_function, NULL);

  for (node = cgraph_nodes; node; node = node->next)
    if (node->lowered)
      ipa_tm_analyze_function (node);
}

static void
ipa_tm_create_version (struct cgraph_node *old_node,
		       VEC (cgraph_edge_p, heap) *redirections)
{
  struct cgraph_node *new_node;
  char *tm_name;

  new_node = cgraph_function_versioning (old_node, redirections,
					 NULL, NULL);

  /* ??? Versioning can fail at the discression of the inliner.  */
  if (new_node == NULL)
    return;

  /* The generic versioning code forces the function to be visible
     only within this translation unit.  This isn't what we want for
     functions the programmer marked TM_CALLABLE.  */
  if (cgraph_is_master_clone (old_node)
      && DECL_IS_TM_CALLABLE (old_node->decl))
    {
      DECL_EXTERNAL (new_node->decl) = DECL_EXTERNAL (old_node->decl);
      TREE_PUBLIC (new_node->decl) = TREE_PUBLIC (old_node->decl);
      DECL_WEAK (new_node->decl) = DECL_WEAK (old_node->decl);

      new_node->local.externally_visible = old_node->local.externally_visible;
      new_node->local.local = old_node->local.local;
    }

  DECL_IS_TM_CLONE (new_node->decl) = 1;

  /* ??? In tree_function_versioning, we futzed with the DECL_NAME.  I'm
     not sure why we did this, as it's surely going to destroy any hope
     of debugging.  */
  DECL_NAME (new_node->decl) = DECL_NAME (old_node->decl);

  /* ??? The current Intel ABI for these symbols uses this first variant.
     I believe we ought to be considering _ZGT{t,n,m} extensions to the
     C++ name mangling ABI.  */
#if !defined(NO_DOT_IN_LABEL) && !defined(NO_DOLLAR_IN_LABEL)
# define TM_SUFFIX	".$TXN"
#elif !defined(NO_DOT_IN_LABEL)
# define TM_SUFFIX	".TXN"
#elif !defined(NO_DOLLAR_IN_LABEL)
# define TM_SUFFIX	"$TXN"
#else
# define TM_SUFFIX	"__TXN"
#endif

  tm_name = concat (IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (old_node->decl)),
		    TM_SUFFIX, NULL);
  SET_DECL_ASSEMBLER_NAME (new_node->decl, get_identifier (tm_name));
  free (tm_name);
}

static void
ipa_tm_decide_version (struct cgraph_node *node)
{
  VEC (cgraph_edge_p, heap) *redirections = NULL;
  cgraph_edge_p e;

  /* Don't re-process transaction clones.  */
  if (DECL_IS_TM_CLONE (node->decl))
    return;

  /* Collect a vector of all the call sites that are within transactions.  */
  for (e = node->callers; e ; e = e->next_caller)
    if (e->tm_atomic_call)
      VEC_safe_push (cgraph_edge_p, heap, redirections, e);

  /* Create a transaction version if the programmer has explicitly
     requested one.  Create a transaction version if the version of
     the function defined here is known to be used, and it has
     transaction callers.  */
  if ((cgraph_is_master_clone (node) && DECL_IS_TM_CALLABLE (node->decl))
      || (cgraph_function_body_availability (node) >= AVAIL_AVAILABLE
	  && !VEC_empty (cgraph_edge_p, redirections)))
    {
      ipa_tm_create_version (node, redirections);
    }

  VEC_free (cgraph_edge_p, heap, redirections);
}

static unsigned int
ipa_tm_execute (void)
{
  struct cgraph_node *node;

  cgraph_remove_function_insertion_hook (function_insertion_hook_holder);

  for (node = cgraph_nodes; node; node = node->next)
    if (node->lowered
        && (node->needed || node->reachable))
      ipa_tm_decide_version (node);

  return 0;
}

struct ipa_opt_pass pass_ipa_tm =
{
 {
  IPA_PASS,
  "tmipa",				/* name */
  gate_tm,				/* gate */
  ipa_tm_execute,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  0,					/* todo_flags_finish */
 },
 ipa_tm_generate_summary,		/* generate_summary */
 NULL,					/* write_summary */
 NULL,					/* read_summary */
 NULL,					/* function_read_summary */
 0,					/* TODOs */
 NULL,					/* function_transform */
 NULL,					/* variable_transform */
};
