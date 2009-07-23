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
#include "tree-inline.h"
#include "except.h"
#include "diagnostic.h"
#include "toplev.h"
#include "flags.h"
#include "demangle.h"
#include "output.h"
#include "trans-mem.h"


#define PROB_VERY_UNLIKELY	(REG_BR_PROB_BASE / 2000 - 1)
#define PROB_ALWAYS		(REG_BR_PROB_BASE)

#define A_RUNINSTRUMENTEDCODE	0x0001
#define A_RUNUNINSTRUMENTEDCODE	0x0002
#define A_SAVELIVEVARIABLES	0x0004
#define A_RESTORELIVEVARIABLES	0x0008
#define A_ABORTTRANSACTION	0x0010

#define AR_USERABORT		0x0001
#define AR_USERRETRY		0x0002
#define AR_TMCONFLICT		0x0004
#define AR_EXCEPTIONBLOCKABORT	0x0008

#define MODE_SERIALIRREVOCABLE	0x0001


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
  pass_tm_edges replaces statements that perform transactional
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


/* Return the attributes we want to examine for X, or NULL if it's not
   something we examine.  We look at function types, but allow pointers
   to function types and function decls and peek through.  */

static tree
get_attrs_for (tree x)
{
  switch (TREE_CODE (x))
    {
    case FUNCTION_DECL:
      return TYPE_ATTRIBUTES (TREE_TYPE (x));
      break;

    case POINTER_TYPE:
      x = TREE_TYPE (x);
      if (TREE_CODE (x) != FUNCTION_TYPE)
	return NULL;
      /* FALLTHRU */

    case FUNCTION_TYPE:
      return TYPE_ATTRIBUTES (x);

    default:
      return NULL;
    }
}

/* Return true if X has been marked TM_PURE.  */

bool
is_tm_pure (tree x)
{
  tree attrs = get_attrs_for (x);

  if (attrs)
    return lookup_attribute ("tm_pure", attrs) != NULL;
  return false;
}

/* Return true if X has been marked TM_IRREVOKABLE.  */

bool
is_tm_irrevokable (tree x)
{
  tree attrs = get_attrs_for (x);

  if (attrs)
    return lookup_attribute ("tm_irrevokable", attrs) != NULL;

  /* A call to the irrevocable builtin is by definition,
     irrevocable.  */
  if (TREE_CODE (x) == ADDR_EXPR)
    x = TREE_OPERAND (x, 0);
  if (TREE_CODE (x) == FUNCTION_DECL
      && DECL_BUILT_IN_CLASS (x) == BUILT_IN_NORMAL
      && DECL_FUNCTION_CODE (x) == BUILT_IN_TM_IRREVOKABLE)
    return true;

  return false;
}

/* Return true if X has been marked TM_SAFE.  */

bool
is_tm_safe (tree x)
{
  tree attrs = get_attrs_for (x);

  if (attrs)
    return lookup_attribute ("tm_safe", attrs) != NULL;
  return false;
}

/* Return true if CALL is const, or tm_pure.  */

static bool
is_tm_pure_call (gimple call)
{
  tree fn = gimple_call_fn (call);
  unsigned flags;

  if (is_tm_pure (TREE_TYPE (fn)))
    return true;

  if (TREE_CODE (fn) == ADDR_EXPR)
    {
      fn = TREE_OPERAND (fn, 0);
      gcc_assert (TREE_CODE (fn) == FUNCTION_DECL);
    }
  else
    fn = TREE_TYPE (fn);
  flags = flags_from_decl_or_type (fn);

  return (flags & ECF_CONST) != 0;
}

/* Return true if X has been marked TM_CALLABLE.  */

bool
is_tm_callable (tree x)
{
  tree attrs = get_attrs_for (x);

  if (attrs)
    {
      if (lookup_attribute ("tm_callable", attrs))
	return true;

      /* TM_SAFE is stricter than TM_CALLABLE.  */
      return lookup_attribute ("tm_safe", attrs) != NULL;
    }
  return false;
}

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

/* Return true for built in functions that "end" a transaction.   */

static bool
is_tm_ending_fndecl (tree fndecl)
{
  if (DECL_BUILT_IN_CLASS (fndecl) == BUILT_IN_NORMAL)
    switch (DECL_FUNCTION_CODE (fndecl))
      {
      case BUILT_IN_TM_COMMIT:
      case BUILT_IN_TM_COMMIT_EH:
      case BUILT_IN_TM_ABORT:
      case BUILT_IN_TM_IRREVOKABLE:
	return true;
      default:
	break;
      }

  return false;
}


/* Return true if FNDECL is BUILT_IN_TM_ABORT.  */

static bool
is_tm_abort (tree fndecl)
{
  return (fndecl
	  && DECL_BUILT_IN_CLASS (fndecl) == BUILT_IN_NORMAL
	  && DECL_FUNCTION_CODE (fndecl) == BUILT_IN_TM_ABORT);
}

/* Build a GENERIC tree for a user abort.  This is called by front ends
   while transforming the __tm_abort statement.  */

tree
build_tm_abort_call (location_t loc)
{
  tree x;

  x = build_call_expr (built_in_decls[BUILT_IN_TM_ABORT], 1,
		       build_int_cst (integer_type_node, AR_USERABORT));
  SET_EXPR_LOCATION (x, loc);

  return x;
}

/* Map for aribtrary function replacement under TM, as created
   by the tm_wrap attribute.  */

static GTY((param_is (struct tree_map))) htab_t tm_wrap_map;

void
record_tm_replacement (tree from, tree to)
{
  struct tree_map **slot, *h;

  if (tm_wrap_map == NULL)
    tm_wrap_map = htab_create_ggc (32, tree_map_hash, tree_map_eq, 0);

  h = GGC_NEW (struct tree_map);
  h->hash = htab_hash_pointer (from);
  h->base.from = from;
  h->to = to;

  slot = (struct tree_map **)
    htab_find_slot_with_hash (tm_wrap_map, h, h->hash, INSERT);
  *slot = h;
}

/* Return a TM-aware replacement function for DECL.  */

static tree
find_tm_replacement_function (tree fndecl)
{
  /* ??? We may well want TM versions of most of the common <string.h>
     functions.  For now, we've already these two defined.  */
  if (DECL_BUILT_IN_CLASS (fndecl) == BUILT_IN_NORMAL)
    switch (DECL_FUNCTION_CODE (fndecl))
      {
      case BUILT_IN_MEMCPY:
	return built_in_decls[BUILT_IN_TM_MEMCPY];
      case BUILT_IN_MEMMOVE:
	return built_in_decls[BUILT_IN_TM_MEMMOVE];
      default:
	return NULL;
      }

  if (tm_wrap_map)
    {
      struct tree_map *h, in;

      in.base.from = fndecl;
      in.hash = htab_hash_pointer (fndecl);
      h = (struct tree_map *) htab_find_with_hash (tm_wrap_map, &in, in.hash);
      if (h)
	return h->to;
    }

  return NULL;
}

/* Diagnostics for tm_safe functions/regions.  Called by the front end
   once we've lowered the function to high-gimple.  */

/* Subroutine of diagnose_tm_safe_errors, called through walk_gimple_seq.
   Process exactly one statement.  WI->INFO is set to non-null when in
   the context of a tm_safe function, and null for a __tm_atomic block.  */

static tree
diagnose_tm_safe_1 (gimple_stmt_iterator *gsi, bool *handled_ops_p,
		    struct walk_stmt_info *wi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree fn;

  /* We're not interested in (normal) operands.  */
  *handled_ops_p = !gimple_has_substatements (stmt);

  switch (gimple_code (stmt))
    {
    case GIMPLE_CALL:
      if (is_tm_pure_call (stmt))
	break;

      fn = gimple_call_fn (stmt);
      if (fn && is_tm_safe (TREE_TYPE (fn)))
	break;
      if (TREE_CODE (fn) == ADDR_EXPR
	  && find_tm_replacement_function (TREE_OPERAND (fn, 0)))
	break;

      if (wi->info)
	error_at (gimple_location (stmt),
		  "unsafe function call in %<tm_safe%> function");
      else
	error_at (gimple_location (stmt),
		  "unsafe function call in %<__tm_atomic%>");
      break;

    case GIMPLE_ASM:
      /* ??? The Approved Method of indicating that an inline
	 assembly statement is not relevant to the transaction
	 is to wrap it in a __tm_waiver block.  This is not 
	 yet implemented, so we can't check for it.  */
      if (wi->info)
        error_at (gimple_location (stmt),
		  "asm not allowed in %<tm_safe%> function");
      else
	error_at (gimple_location (stmt),
		  "asm not allowed in %<__tm_atomic%>");
      break;

    default:
      break;
    }

  return NULL_TREE;
}

void
diagnose_tm_safe_errors (tree fndecl)
{
  tree save_current = current_function_decl;
  struct function *save_cfun = cfun;
  struct walk_stmt_info wi;

  /* Only need to check tm_safe functions at the moment.  */
  if (!is_tm_safe (TREE_TYPE (fndecl)))
    return;

  current_function_decl = fndecl;
  set_cfun (DECL_STRUCT_FUNCTION (fndecl));

  memset (&wi, 0, sizeof (wi));
  wi.info = fndecl;
  walk_gimple_seq (gimple_body (fndecl), diagnose_tm_safe_1, NULL, &wi);

  set_cfun (save_cfun);
  current_function_decl = save_current;
}

static tree lower_sequence_tm (gimple_stmt_iterator *, bool *,
			       struct walk_stmt_info *);
static tree lower_sequence_no_tm (gimple_stmt_iterator *, bool *,
				  struct walk_stmt_info *);

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
      /* ??? Insert an irrevokable when it comes to vectorized loops,
	 or handle these somehow.  */
      gcc_unreachable ();

    case TARGET_MEM_REF:
      x = TMR_SYMBOL (x);
      if (x == NULL)
	return true;
      if (TREE_CODE (x) == PARM_DECL)
	return false;
      gcc_assert (TREE_CODE (x) == VAR_DECL);
      /* FALLTHRU */

    case VAR_DECL:
      if (is_global_var (x))
	return !TREE_READONLY (x);
      /* ??? For local memory that doesn't escape, we can either save the
	 value at the beginning of the transaction and restore on restart,
	 or call a tm function to dynamically save and restore on restart.
	 We don't actually need a full barrier here.  */
      return needs_to_live_in_memory (x);

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
  tree fn;

  gimple_call_set_in_tm_atomic (stmt, true);

  if (is_tm_pure_call (stmt))
    return;

  /* Check if this call is a transaction abort.  */
  fn = gimple_call_fndecl (stmt);
  if (is_tm_abort (fn))
    *state |= GTMA_HAVE_ABORT;

  /* Note that something may happen.  */
  *state |= GTMA_HAVE_LOAD | GTMA_HAVE_STORE;
}

/* Lower a GIMPLE_TM_ATOMIC statement.  */

static void
lower_tm_atomic (gimple_stmt_iterator *gsi, struct walk_stmt_info *wi)
{
  gimple g, stmt = gsi_stmt (*gsi);
  unsigned int *outer_state = (unsigned int *) wi->info;
  unsigned int this_state = 0;
  struct walk_stmt_info this_wi;

  /* First, lower the body.  The scanning that we do inside gives
     us some idea of what we're dealing with.  */
  memset (&this_wi, 0, sizeof (this_wi));
  this_wi.info = (void *) &this_state;
  walk_gimple_seq (gimple_tm_atomic_body (stmt),
		   lower_sequence_tm, NULL, &this_wi);

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
      wi->removed_stmt = true;
      return;
    }

  /* Wrap the body of the transaction in a try-finally node so that
     the commit call is always properly called.  */
  g = gimple_build_call (built_in_decls[BUILT_IN_TM_COMMIT], 0);
  if (flag_exceptions)
    {
      tree ptr;
      gimple g2;

      ptr = build0 (EXC_PTR_EXPR, ptr_type_node);
      g2 = gimple_build_call (built_in_decls[BUILT_IN_TM_COMMIT_EH], 1, ptr);

      g = gimple_build_eh_else (gimple_seq_alloc_with_stmt (g),
				gimple_seq_alloc_with_stmt (g2));
    }

  g = gimple_build_try (gimple_tm_atomic_body (stmt),
			gimple_seq_alloc_with_stmt (g), GIMPLE_TRY_FINALLY);
  gimple_tm_atomic_set_body (stmt, gimple_seq_alloc_with_stmt (g));

  /* If the transaction calls abort, add an "over" label afterwards.  */
  if (this_state & GTMA_HAVE_ABORT)
    {
      tree label = create_artificial_label (UNKNOWN_LOCATION);
      gimple_tm_atomic_set_label (stmt, label);
      gsi_insert_after (gsi, gimple_build_label (label), GSI_CONTINUE_LINKING);
    }

  /* Record the set of operations found for use later.  */
  gimple_tm_atomic_set_subcode (stmt, this_state);
}

/* Iterate through the statements in the sequence, lowering them all
   as appropriate for being in a transaction.  */

static tree
lower_sequence_tm (gimple_stmt_iterator *gsi, bool *handled_ops_p,
		   struct walk_stmt_info *wi)
{
  unsigned int *state = (unsigned int *) wi->info;
  gimple stmt = gsi_stmt (*gsi);

  *handled_ops_p = true;
  switch (gimple_code (stmt))
    {
    case GIMPLE_ASSIGN:
      /* Only memory reads/writes need to be instrumented.  */
      if (gimple_assign_single_p (stmt))
      examine_assign_tm (state, gsi);
      break;

    case GIMPLE_CALL:
      examine_call_tm (state, gsi);
      break;

    case GIMPLE_ASM:
      *state |= GTMA_MAY_ENTER_IRREVOKABLE;
      break;

    case GIMPLE_TM_ATOMIC:
      lower_tm_atomic (gsi, wi);
      break;

    default:
      *handled_ops_p = !gimple_has_substatements (stmt);
      break;
    }

  return NULL_TREE;
}

/* Iterate through the statements in the sequence, lowering them all
   as appropriate for being outside of a transaction.  */

static tree
lower_sequence_no_tm (gimple_stmt_iterator *gsi, bool *handled_ops_p,
		      struct walk_stmt_info * wi)
{
  gimple stmt = gsi_stmt (*gsi);

  if (gimple_code (stmt) == GIMPLE_TM_ATOMIC)
    {
      *handled_ops_p = true;
      lower_tm_atomic (gsi, wi);
    }
  else
    *handled_ops_p = !gimple_has_substatements (stmt);

  return NULL_TREE;
}

/* Main entry point for flattening GIMPLE_TM_ATOMIC constructs.  After
   this, GIMPLE_TM_ATOMIC nodes still exist, but the nested body has
   been moved out, and all the data required for constructing a proper
   CFG has been recorded.  */

static unsigned int
execute_lower_tm (void)
{
  struct walk_stmt_info wi;

  /* Transactional clones aren't created until a later pass.  */
  gcc_assert (!DECL_IS_TM_CLONE (current_function_decl));

  memset (&wi, 0, sizeof (wi));
  walk_gimple_seq (gimple_body (current_function_decl),
		   lower_sequence_no_tm, NULL, &wi);

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
  TV_TRANS_MEM,				/* tv_id */
  PROP_gimple_lcf,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func		        /* todo_flags_finish */
 }
};

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

static void
tm_region_init_2 (struct tm_region *region, VEC (basic_block, heap) **pqueue)
{
  gcc_assert (VEC_empty (basic_block, *pqueue));

  VEC_quick_push (basic_block, *pqueue, region->entry_block);
  do
    {
      basic_block bb = VEC_pop (basic_block, *pqueue);
      gimple_stmt_iterator gsi;

      /* Check to see if this is the end of the region by seeing if it
	 ends in a call to __tm_commit.  */
      for (gsi = gsi_last_bb (bb); !gsi_end_p (gsi); gsi_prev (&gsi))
	{
	  gimple g = gsi_stmt (gsi);
	  if (gimple_code (g) == GIMPLE_CALL)
	    {
	      tree fn = gimple_call_fndecl (g);
	      if (fn && DECL_BUILT_IN_CLASS (fn) == BUILT_IN_NORMAL
		  && (DECL_FUNCTION_CODE (fn) == BUILT_IN_TM_COMMIT
		      || DECL_FUNCTION_CODE (fn) == BUILT_IN_TM_COMMIT_EH)
		  && lookup_stmt_eh_region (g) == region->region_nr)
		{
		  bitmap_set_bit (region->exit_blocks, bb->index);
		  goto skip;
		}
	    }
	}

      for (bb = first_dom_son (CDI_DOMINATORS, bb); bb;
	   bb = next_dom_son (CDI_DOMINATORS, bb))
	VEC_safe_push (basic_block, heap, *pqueue, bb);

    skip:;
    }
  while (!VEC_empty (basic_block, *pqueue));
}

static struct tm_region *
tm_region_init (void)
{
  struct tm_region *r, *regions = NULL;
  VEC (basic_block, heap) *queue;

  /* Find each GIMPLE_TM_ATOMIC statement.  This data is stored
     in the exception handling tables, so it's quickest to get
     it out that way than actually search the function.  */
  for_each_tm_atomic (false, tm_region_init_1, &regions);

  if (regions == NULL)
    return NULL;

  /* Find the exit blocks for each region.  */
  queue = VEC_alloc (basic_block, heap, 10);
  for (r = regions; r; r = r->next)
    tm_region_init_2 (r, &queue);
  VEC_free (basic_block, heap, queue);

  return regions;
}

/* The "gate" function for all transactional memory expansion and optimization
   passes.  We collect region information for each top-level transaction, and
   if we don't find any, we skip all of the TM passes.  Each region will have
   all of the exit blocks recorded, and the originating statement.  */

static bool
gate_tm_init (void)
{
  if (!flag_tm)
    return false;

  calculate_dominance_info (CDI_DOMINATORS);
  bitmap_obstack_initialize (&tm_obstack);

  /* If the function is a TM_CLONE, then the entire function is the region.  */
  if (DECL_IS_TM_CLONE (current_function_decl))
    {
      struct tm_region *region = (struct tm_region *)
	obstack_alloc (&tm_obstack.obstack, sizeof (struct tm_region));
      region->next = NULL;
      region->tm_atomic_stmt = NULL;
      region->entry_block = ENTRY_BLOCK_PTR;
      region->exit_blocks = NULL;
      region->region_nr = -1;
      all_tm_regions = region;

      return true;
    }

  all_tm_regions = tm_region_init ();

  /* If we didn't find any regions, cleanup and skip the whole tree
     of tm-related optimizations.  */
  if (all_tm_regions == NULL)
    {
      bitmap_obstack_release (&tm_obstack);
      return false;
    }

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
  TV_TRANS_MEM,				/* tv_id */
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


/* Gimplify the address of a TARGET_MEM_REF.  Return the SSA_NAME
   result, insert the new statements before GSI.  */

static tree
gimplify_addr (gimple_stmt_iterator *gsi, tree x)
{
  if (TREE_CODE (x) == TARGET_MEM_REF)
    x = tree_mem_ref_addr (build_pointer_type (TREE_TYPE (x)), x);
  else
    x = build_fold_addr_expr (x);
  return force_gimple_operand_gsi (gsi, x, true, NULL, true, GSI_SAME_STMT);
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

  t = gimplify_addr (gsi, rhs);
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

  t = gimplify_addr (gsi, lhs);
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

  if (!load_p && !store_p)
    {
      gsi_next (gsi);
      return;
    }

  gsi_remove (gsi, true);

  if (load_p && store_p)
    {
      tm_atomic_subcode_ior (region, GTMA_HAVE_LOAD | GTMA_HAVE_STORE);

      /* ??? Figure out if there's any possible overlap between the LHS
	 and the RHS and if not, use MEMCPY.  */
      gcall = gimple_build_call (built_in_decls [BUILT_IN_TM_MEMMOVE], 3,
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
  else
    {
      tm_atomic_subcode_ior (region, GTMA_HAVE_STORE);
      gcall = build_tm_store (lhs, rhs, gsi);
    }

  add_stmt_to_tm_region  (region, gcall);
}


/* Expand a call statement as appropriate for a transaction.  That is,
   either verify that the call does not affect the transaction, or
   redirect the call to a clone that handles transactions, or change
   the transaction state to IRREVOKABLE.  Return true if the call is
   one of the builtins that end a transaction.  */

static bool
expand_call_tm (struct tm_region *region,
		gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);
  tree lhs = gimple_call_lhs (stmt);
  tree fn_decl;

  if (is_tm_pure_call (stmt))
    return false;

  fn_decl = gimple_call_fndecl (stmt);

  /* For indirect calls, we already generated a call into the runtime.  */
  if (!fn_decl)
    return false;

  if (is_tm_abort (fn_decl))
    {
      tm_atomic_subcode_ior (region, GTMA_HAVE_ABORT);
      return true;
    }

  if (lhs && requires_barrier (lhs))
    tm_atomic_subcode_ior (region, GTMA_HAVE_STORE);

  return is_tm_ending_fndecl (fn_decl);
}


/* Expand all statements in BB as appropriate for being inside
   a transaction.  Return true if we reach the end of the transaction,
   or reach an irrevokable state.  */

static bool
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
	    return true;
	  break;

	case GIMPLE_ASM:
	  gcc_unreachable ();

	default:
	  break;
	}
      gsi_next (&gsi);
    }

  return false;
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
	unsigned int subcode
	  = gimple_tm_atomic_subcode (region->tm_atomic_stmt);

	/* Collect a new SUBCODE set, now that optimizations are done...  */
	gimple_tm_atomic_set_subcode (region->tm_atomic_stmt, 0);
	/* ...but keep the bits that require IPA to collect.  Ideally
	   we should do IPA again to make sure things like DCE didn't
	   invalidate irrevocability, but we are certain not to
	   introduce things that go irrevocable, so it's safe to keep
	   the irrevocability bit.  */
	gimple_tm_atomic_set_subcode (region->tm_atomic_stmt,
				      subcode & GTMA_MAY_ENTER_IRREVOKABLE);

	VEC_quick_push (basic_block, queue, region->entry_block);
	do
	  {
	    bb = VEC_pop (basic_block, queue);

	    if (expand_block_tm (region, bb))
	      continue;

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
  TV_TRANS_MEM,				/* tv_id */
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


/* Split block BB as necessary for every TM_OPS function we added, and
   wire up the abnormal back edges implied by the transaction restart.  */

static void
expand_block_edges (struct tm_region *region, basic_block bb)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); )
    {
      gimple stmt = gsi_stmt (gsi);

      /* ??? TM_COMMIT (and any other ECF_TM_OPS function) in a nested
	 transaction has an abnormal edge back to the outer-most transaction
	 (there are no nested retries), while a TM_ABORT has an abnormal
	 backedge to the inner-most transaction.  We havn't actually saved
	 the inner-most transaction here.  We should be able to get to it
	 via the region_nr saved on STMT, and read the tm_atomic_stmt from
	 that, and find the first region block from there.  */
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

/* Expand the GIMPLE_TM_ATOMIC statement into the STM library call.  */

static void
expand_tm_atomic (struct tm_region *region)
{
  tree status, tm_start;
  basic_block atomic_bb;
  gimple_stmt_iterator gsi;
  tree t1, t2;
  gimple g;
  int flags, subcode;

  tm_start = built_in_decls[BUILT_IN_TM_START];
  status = make_rename_temp (TREE_TYPE (TREE_TYPE (tm_start)), "tm_state");

  /* ??? There are plenty of bits here we're not computing.  */
  subcode = gimple_tm_atomic_subcode (region->tm_atomic_stmt);
  flags = PR_INSTRUMENTEDCODE;
  if ((subcode & GTMA_HAVE_ABORT) == 0)
    flags |= PR_HASNOABORT;
  if (subcode & GTMA_MUST_CALL_IRREVOKABLE)
    flags |= PR_DOESGOIRREVOKABLE;
  t2 = build_int_cst (TREE_TYPE (status), flags);
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
      edge e;
      basic_block test_bb;

      region->entry_block = test_bb = create_empty_bb (atomic_bb);
      gsi = gsi_last_bb (test_bb);

      t1 = make_rename_temp (TREE_TYPE (status), NULL);
      t2 = build_int_cst (TREE_TYPE (status), A_ABORTTRANSACTION);
      g = gimple_build_assign_with_ops (BIT_AND_EXPR, t1, status, t2);
      gsi_insert_after (&gsi, g, GSI_CONTINUE_LINKING);

      t2 = build_int_cst (TREE_TYPE (status), 0);
      g = gimple_build_cond (NE_EXPR, t1, t2, NULL, NULL);
      gsi_insert_after (&gsi, g, GSI_CONTINUE_LINKING);

      e = FALLTHRU_EDGE (atomic_bb);
      redirect_edge_pred (e, test_bb);
      e->flags = EDGE_FALSE_VALUE;
      e->probability = PROB_ALWAYS - PROB_VERY_UNLIKELY;

      e = BRANCH_EDGE (atomic_bb);
      redirect_edge_pred (e, test_bb);
      e->flags = EDGE_TRUE_VALUE;
      e->probability = PROB_VERY_UNLIKELY;

      e = make_edge (atomic_bb, test_bb, EDGE_FALLTHRU);
    }

  /* If we've no abort, but we do have PHIs at the beginning of the atomic
     region, that means we've a loop at the beginning of the atomic region
     that shares the first block.  This can cause problems with the abnormal
     edges we're about to add for the transaction restart.  Solve this by
     adding a new empty block to receive the abnormal edges.  */
  else if (phi_nodes (region->entry_block))
    {
      edge e;
      basic_block empty_bb;

      region->entry_block = empty_bb = create_empty_bb (atomic_bb);

      e = FALLTHRU_EDGE (atomic_bb);
      redirect_edge_pred (e, empty_bb);

      e = make_edge (atomic_bb, empty_bb, EDGE_FALLTHRU);
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
  VEC (basic_block, heap) *queue;
  bitmap blocks;

  queue = VEC_alloc (basic_block, heap, 10);
  blocks = BITMAP_ALLOC (&tm_obstack);

  for (region = all_tm_regions; region ; region = region->next)
    if (region->exit_blocks)
      {
	unsigned int i;
	bitmap_iterator iter;

	/* Collect the set of blocks in this region.  Do this before
	   splitting edges, so that we don't have to play with the
	   dominator tree in the middle.  */
	bitmap_clear (blocks);
	VEC_quick_push (basic_block, queue, region->entry_block);
	do
	  {
	    basic_block bb = VEC_pop (basic_block, queue);
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
  all_tm_regions = NULL;

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
  TV_TRANS_MEM,				/* tv_id */
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
  return 0 && optimize > 0;
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
  TV_TRANS_MEM,				/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func,			/* todo_flags_finish */
 }
};


/* Interprocedual analysis for the creation of transactional clones.
   The aim of this pass is to find which functions are referenced in
   a non-irrevokable transaction context, and for those over which
   we have control (or user directive), create a version of the 
   function which uses only the transactional interface to reference
   protected memories.  This analysis proceeds in several steps:

     (1) Collect the set of all possible transactional clones:

	(a) For all local public functions marked tm_callable, push
	    it onto the tm_callee queue.

	(b) For all local functions, scan for calls marked in_tm_atomic.
	    Push the caller and callee onto the tm_caller and tm_callee
	    queues.  Count the number of callers for each callee.

	(c) For each local function on the callee list, assume we will
	    create a transactional clone.  Push *all* calls onto the
	    callee queues; count the number of clone callers separately
	    to the number of original callers.

     (2) Propagate irrevokable status up the dominator tree:

	(a) Any external function on the callee list that is not marked
	    tm_callable is irrevokable.  Push all callers of such onto
	    a worklist.

	(b) For each function on the worklist, mark each block that
	    contains an irrevokable call.  Use the AND operator to
	    propagate that mark up the dominator tree.

	(c) If we reach the entry block for a possible transactional
	    clone, then the transactional clone is irrevokable, and
	    we should not create the clone after all.  Push all 
	    callers onto the worklist.

	(d) Place tm_irrevokable calls at the beginning of the relevant
	    blocks.  Special case here is the entry block for the entire
	    tm_atomic region; there we mark it MUST_CALL_IRREVOKABLE for
	    the library to begin the region in serial mode.  Decrement
	    the call count for all callees in the irrevokable region.

     (3) Create the transactional clones:

	Any tm_callee that still has a non-zero call count is cloned.
*/

/* This structure is stored in the AUX field of each cgraph_node.  */
struct tm_ipa_cg_data
{
  /* The clone of the function that got created.  */
  struct cgraph_node *clone;

  /* The tm regions in the normal function.  */
  struct tm_region *all_tm_regions;

  /* The blocks of the normal/clone functions that contain irrevokable 
     calls, or blocks that are post-dominated by irrevokable calls.  */
  bitmap irrevokable_blocks_normal;
  bitmap irrevokable_blocks_clone;

  /* The number of callers to the transactional clone of this function
     from normal and transactional clones respectively.  */
  unsigned tm_callers_normal;
  unsigned tm_callers_clone;

  /* True if all calls to this function's transactional clone
     are irrevokable.  Also automatically true if the function
     has no transactional clone.  */
  bool is_irrevokable;

  /* Flags indicating the presence of this function in various queues.  */
  bool in_callee_queue;
  bool in_worklist;

  /* Flags indicating the kind of scan desired while in the worklist.  */
  bool want_irr_scan_normal;
};

typedef struct cgraph_node *cgraph_node_p;

DEF_VEC_P (cgraph_node_p);
DEF_VEC_ALLOC_P (cgraph_node_p, heap);

typedef VEC (cgraph_node_p, heap) *cgraph_node_queue;


/* Return the ipa data associated with NODE, allocating zeroed memory
   if necessary.  */

static struct tm_ipa_cg_data *
get_cg_data (struct cgraph_node *node)
{
  struct tm_ipa_cg_data *d = (struct tm_ipa_cg_data *) node->aux;

  if (d == NULL)
    {
      d = (struct tm_ipa_cg_data *)
	obstack_alloc (&tm_obstack.obstack, sizeof (*d));
      node->aux = (void *) d;
      memset (d, 0, sizeof (*d));
    }

  return d;
}

/* Add NODE to the end of QUEUE, unless IN_QUEUE_P indicates that 
   it is already present.  */

static void
maybe_push_queue (struct cgraph_node *node,
		  cgraph_node_queue *queue_p, bool *in_queue_p)
{
  if (!*in_queue_p)
    {
      *in_queue_p = true;
      VEC_safe_push (cgraph_node_p, heap, *queue_p, node);
    }
}

/* Scan all calls in NODE that are within a transaction region,
   and push the resulting nodes into the callee queue.  */

static void
ipa_tm_scan_calls_tm_atomic (struct cgraph_node *node,
			     cgraph_node_queue *callees_p)
{
  struct cgraph_edge *e;

  for (e = node->callees; e ; e = e->next_callee)
    if (gimple_call_in_tm_atomic_p (e->call_stmt))
      {
	struct tm_ipa_cg_data *d;

	if (is_tm_pure_call (e->call_stmt))
	  continue;
	if (find_tm_replacement_function (e->callee->decl))
	  continue;
	
	d = get_cg_data (e->callee);
	d->tm_callers_normal++;
	maybe_push_queue (e->callee, callees_p, &d->in_callee_queue);
      }
}

/* Scan all calls in NODE as if this is the transactional clone,
   and push the destinations into the callee queue.  */

static void
ipa_tm_scan_calls_clone (struct cgraph_node *node, 
			 cgraph_node_queue *callees_p)
{
  struct cgraph_edge *e;

  for (e = node->callees; e ; e = e->next_callee)
    if (!is_tm_pure_call (e->call_stmt))
      {
	struct tm_ipa_cg_data *d = get_cg_data (e->callee);

	d->tm_callers_clone++;
	maybe_push_queue (e->callee, callees_p, &d->in_callee_queue);
      }
}

/* The function NODE has been detected to be irrevokable.  Push all
   of its callers onto WORKLIST for the purpose of re-scanning them.  */

static void
ipa_tm_note_irrevokable (struct cgraph_node *node,
			 cgraph_node_queue *worklist_p)
{
  struct tm_ipa_cg_data *d = get_cg_data (node);
  struct cgraph_edge *e;

  d->is_irrevokable = true;

  for (e = node->callers; e ; e = e->next_caller)
    {
      d = get_cg_data (e->caller);

      /* Don't examine recursive calls.  */
      if (e->caller == node)
	continue;
      if (gimple_call_in_tm_atomic_p (e->call_stmt))
	d->want_irr_scan_normal = true;
      maybe_push_queue (e->caller, worklist_p, &d->in_worklist);
    }
}

/* A subroutine of ipa_tm_scan_irr_blocks; return true iff any statement
   within the block is irrevokable.  */

static bool
ipa_tm_scan_irr_block (basic_block bb)
{
  gimple_stmt_iterator gsi;
  tree fn;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);
      switch (gimple_code (stmt))
	{
	case GIMPLE_CALL:
	  if (is_tm_pure_call (stmt))
	    break;

	  fn = gimple_call_fn (stmt);

	  /* Functions with the attribute are by definition irrevokable.  */
	  if (is_tm_irrevokable (fn))
	    return true;

	  /* For direct function calls, go ahead and check for replacement
	     functions, or transitive irrevokable functions.  For indirect
	     functions, we'll ask the runtime.  */
	  if (TREE_CODE (fn) == ADDR_EXPR)
	    {
	      struct tm_ipa_cg_data *d;

	      fn = TREE_OPERAND (fn, 0);
	      if (is_tm_ending_fndecl (fn))
		break;
	      if (find_tm_replacement_function (fn))
		break;

	      d = get_cg_data (cgraph_node (fn));
	      if (d->is_irrevokable)
		return true;
	    }
	  break;

	case GIMPLE_ASM:
	  /* ??? The Approved Method of indicating that an inline
	     assembly statement is not relevant to the transaction
	     is to wrap it in a __tm_waiver block.  This is not 
	     yet implemented, so we can't check for it.  */
	  return true;

	default:
	  break;
	}
    }

  return false;
}

/* For each of the blocks seeded witin PQUEUE, walk its dominator tree
   looking for new irrevokable blocks, marking them in NEW_IRR.  Don't
   bother scanning past OLD_IRR or EXIT_BLOCKS.  */

static bool
ipa_tm_scan_irr_blocks (VEC (basic_block, heap) **pqueue, bitmap new_irr,
		        bitmap old_irr, bitmap exit_blocks)
{
  bool any_new_irr = false;

  do
    {
      basic_block bb = VEC_pop (basic_block, *pqueue);

      /* Don't re-scan blocks we know already are irrevokable.  */
      if (old_irr && bitmap_bit_p (old_irr, bb->index))
	continue;

      if (ipa_tm_scan_irr_block (bb))
	{
	  bitmap_set_bit (new_irr, bb->index);
	  any_new_irr = true;
	}
      else if (exit_blocks == NULL || !bitmap_bit_p (exit_blocks, bb->index))
	for (bb = first_dom_son (CDI_DOMINATORS, bb); bb;
	     bb = next_dom_son (CDI_DOMINATORS, bb))
	  VEC_safe_push (basic_block, heap, *pqueue, bb);
    }
  while (!VEC_empty (basic_block, *pqueue));

  return any_new_irr;
}

/* Propagate the irrevokable property both up and down the dominator tree.
   BB is the current block being scanned; EXIT_BLOCKS are the edges of the
   TM regions; OLD_IRR is the results of a previous scan of the dominator
   tree which has been fully propagated; NEW_IRR is the set of new blocks
   which are gaining the irrevokable property during the current scan.  */

static bool
ipa_tm_propagate_irr (basic_block bb, bitmap new_irr, bitmap old_irr,
		      bitmap exit_blocks, bool parent_irr)
{
  bool this_irr;
  unsigned index = bb->index;

  /* If this block is in the old set, no need to rescan.  */
  if (old_irr && bitmap_bit_p (old_irr, index))
    return true;

  /* For downward propagation, the block is irrevokable if either 
     the parent block is irrevokable or a scan of the the block
     revealed an irrevokable statement.  */
  this_irr = (parent_irr || bitmap_bit_p (new_irr, index));

  if (!bitmap_bit_p (exit_blocks, index))
    {
      basic_block son = first_dom_son (CDI_DOMINATORS, bb);
      bool all_son_irr = true;

      if (son)
	{
	  do
	    {
	      if (!ipa_tm_propagate_irr (son, new_irr, old_irr,
					 exit_blocks, this_irr))
		all_son_irr = false;
	      son = next_dom_son (CDI_DOMINATORS, son);
	    }
	  while (son);

	  /* For upward propagation, the block is irrevokable if
	     all dominated blocks are irrevokable.  */
	  this_irr |= all_son_irr;
	}
    }

  if (this_irr)
    bitmap_set_bit (new_irr, index);

  return this_irr;
}

static void
ipa_tm_decrement_clone_counts (basic_block bb, bool for_clone)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);
      if (is_gimple_call (stmt) && !is_tm_pure_call (stmt))
	{
	  tree fndecl = gimple_call_fndecl (stmt);
	  if (fndecl)
	    {
	      struct tm_ipa_cg_data *d;
	      unsigned *pcallers;

	      if (is_tm_ending_fndecl (fndecl))
		continue;
	      if (find_tm_replacement_function (fndecl))
		continue;

	      d = get_cg_data (cgraph_node (fndecl));
	      pcallers = (for_clone ? &d->tm_callers_clone
			  : &d->tm_callers_normal);

	      gcc_assert (*pcallers > 0);
	      *pcallers -= 1;
	    }
	}
    }
}

/* (Re-)Scan the tm_atomic blocks in NODE for calls to irrevokable functions,
   as well as other irrevokable actions such as inline assembly.  Mark all
   such blocks as irrevokable and decrement the number of calls to
   transactional clones.  Return true if, for the transactional clone, the
   entire function is irrevokable.  */

static bool
ipa_tm_scan_irr_function (struct cgraph_node *node, bool for_clone)
{
  struct tm_ipa_cg_data *d;
  bitmap new_irr, old_irr;
  VEC (basic_block, heap) *queue;
  bool ret = false;

  current_function_decl = node->decl;
  push_cfun (DECL_STRUCT_FUNCTION (node->decl));
  calculate_dominance_info (CDI_DOMINATORS);

  d = get_cg_data (node);
  queue = VEC_alloc (basic_block, heap, 10);
  new_irr = BITMAP_ALLOC (&tm_obstack);

  /* Scan each tm region, propagating irrevokable status through the tree.  */
  if (for_clone)
    {
      old_irr = d->irrevokable_blocks_clone;
      VEC_quick_push (basic_block, queue, single_succ (ENTRY_BLOCK_PTR));
      if (ipa_tm_scan_irr_blocks (&queue, new_irr, old_irr, NULL))
	ret = ipa_tm_propagate_irr (single_succ (ENTRY_BLOCK_PTR), new_irr,
				    old_irr, NULL, false);
    }
  else
    {
      struct tm_region *region;

      old_irr = d->irrevokable_blocks_normal;
      for (region = d->all_tm_regions; region; region = region->next)
	{
	  VEC_quick_push (basic_block, queue, region->entry_block);
	  if (ipa_tm_scan_irr_blocks (&queue, new_irr, old_irr,
				      region->exit_blocks))
	    ipa_tm_propagate_irr (region->entry_block, new_irr, old_irr,
				  region->exit_blocks, false);
	}
    }

  /* If we found any new irrevokable blocks, reduce the call count for
     transactional clones within the irrevokable blocks.  Save the new
     set of irrevokable blocks for next time.  */
  if (!bitmap_empty_p (new_irr))
    {
      bitmap_iterator bmi;
      unsigned i;

      EXECUTE_IF_SET_IN_BITMAP (new_irr, 0, i, bmi)
	ipa_tm_decrement_clone_counts (BASIC_BLOCK (i), for_clone);

      if (old_irr)
	{
	  bitmap_ior_into (old_irr, new_irr);
	  BITMAP_FREE (new_irr);
	}
      else if (for_clone)
	d->irrevokable_blocks_clone = new_irr;
      else
	d->irrevokable_blocks_normal = new_irr;
    }
  else
    BITMAP_FREE (new_irr);

  VEC_free (basic_block, heap, queue);
  pop_cfun ();
  current_function_decl = NULL;

  return ret;
}

/* Invoke tm_region_init within the context of NODE.  */

static struct tm_region *
ipa_tm_region_init (struct cgraph_node *node)
{
  struct tm_region *regions;

  current_function_decl = node->decl;
  push_cfun (DECL_STRUCT_FUNCTION (node->decl));
  calculate_dominance_info (CDI_DOMINATORS);

  regions = tm_region_init ();

  pop_cfun ();
  current_function_decl = NULL;

  return regions;
}

/* Create a copy of the function (possibly declaration only) of OLD_NODE,
   appropriate for the transactional clone.  */

static void
ipa_tm_create_version (struct cgraph_node *old_node)
{
  tree new_decl, old_decl;
  struct cgraph_node *new_node;
  const char *old_asm_name;
  struct demangle_component *dc;
  char *tm_name;
  void *alloc = NULL;

  old_decl = old_node->decl;
  new_decl = copy_node (old_decl);
  new_node = cgraph_copy_node_for_versioning (old_node, new_decl, NULL);
  get_cg_data (old_node)->clone = new_node;

  if (!DECL_EXTERNAL (old_decl))
    tree_function_versioning (old_decl, new_decl, NULL, false, NULL);

  DECL_IS_TM_CLONE (new_decl) = 1;

  /* Determine if the symbol is already a valid C++ mangled name.  Do this
     even for C, which might be interfacing with C++ code via appropriately
     ugly identifiers.  */
  /* ??? We could probably do just as well checking for "_Z" and be done.  */
  old_asm_name = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (old_decl));
  dc = cplus_demangle_v3_components (old_asm_name, DMGL_NO_OPTS, &alloc);

  if (dc == NULL)
    {
      char length[8];

    do_unencoded:
      sprintf (length, "%u",
	       IDENTIFIER_LENGTH (DECL_ASSEMBLER_NAME (old_decl)));
      tm_name = concat ("_ZGTt", length, old_asm_name, NULL);
    }
  else
    {
      old_asm_name += 2;	/* Skip _Z */

      switch (dc->type)
	{
	case DEMANGLE_COMPONENT_TRANSACTION_CLONE:
	case DEMANGLE_COMPONENT_NONTRANSACTION_CLONE:
	  /* Don't play silly games, you!  */
	  goto do_unencoded;

	case DEMANGLE_COMPONENT_HIDDEN_ALIAS:
	  /* I'd really like to know if we can ever be passed one of
	     these from the C++ front end.  The Logical Thing would
	     seem that hidden-alias should be outer-most, so that we
	     get hidden-alias of a transaction-clone and not vice-versa.  */
	  old_asm_name += 2;
	  break;

	default:
	  break;
	}

      tm_name = concat ("_ZGTt", old_asm_name, NULL);
    }

  SET_DECL_ASSEMBLER_NAME (new_decl, get_identifier (tm_name));
  SET_DECL_RTL (new_decl, NULL);
  free (tm_name);
  free (alloc);

  record_tm_clone_pair (old_decl, new_decl);

  cgraph_call_function_insertion_hooks (new_node);
  if (new_node->local.externally_visible)
    cgraph_mark_needed_node (new_node);
}

/* Construct a call to TM_IRREVOKABLE and insert it at the beginning of BB.  */

static void
ipa_tm_insert_irr_call (struct cgraph_node *node, struct tm_region *region,
			basic_block bb)
{
  gimple_stmt_iterator gsi;
  gimple g;
  edge e;

  tm_atomic_subcode_ior (region, GTMA_MAY_ENTER_IRREVOKABLE);

  g = gimple_build_call (built_in_decls[BUILT_IN_TM_IRREVOKABLE], 0);
  add_stmt_to_tm_region (region, g);

  e = split_block_after_labels (bb);
  gsi = gsi_after_labels (bb);
  gsi_insert_before (&gsi, g, GSI_SAME_STMT);

  cgraph_create_edge (node,
		      cgraph_node (built_in_decls[BUILT_IN_TM_IRREVOKABLE]),
		      g, 0, 0, bb->loop_depth);
}

/* Construct a call to TM_GETTMCLONE and insert it before GSI.  */

static bool
ipa_tm_insert_gettmclone_call (struct cgraph_node *node,
			       struct tm_region *region,
			       gimple_stmt_iterator *gsi, gimple stmt)
{
  tree gettm_fn, ret, old_fn;
  gimple g;
  bool safe;

  old_fn = gimple_call_fn (stmt);

  safe = is_tm_safe (TREE_TYPE (old_fn));
  gettm_fn = built_in_decls[safe ? BUILT_IN_TM_GETTMCLONE_SAFE
			    : BUILT_IN_TM_GETTMCLONE_IRR];
  ret = create_tmp_var (TREE_TYPE (old_fn), NULL);
  add_referenced_var (ret);

  if (!safe)
    tm_atomic_subcode_ior (region, GTMA_MAY_ENTER_IRREVOKABLE);

  /* Discard OBJ_TYPE_REF, since we weren't able to fold it.  */
  if (TREE_CODE (old_fn) == OBJ_TYPE_REF)
    old_fn = OBJ_TYPE_REF_EXPR (old_fn);

  g = gimple_build_call (gettm_fn, 1, old_fn);
  ret = make_ssa_name (ret, g);
  gimple_call_set_lhs (g, ret);

  /* ??? If we need to go irrevokable, we can fail the intermediate
     commit and restart the transaction.  But representing that means
     splitting this basic block, which means busting all of the bitmaps
     we've put together, as well as the dominator tree.  Perhaps we
     can get away with ignoring it, since the indirect function that
     we're about to call should also have the back edge.  */
  if (0 && !safe)
    {
      gimple_call_set_in_tm_atomic (g, true);
      add_stmt_to_tm_region (region, g);
    }

  gsi_insert_before (gsi, g, GSI_SAME_STMT);

  cgraph_create_edge (node, cgraph_node (gettm_fn), g, 0, 0, 0);

  /* ??? This is a hack to preserve the NOTHROW bit on the call,
     which we would have derived from the decl.  Failure to save
     this bit means we might have to split the basic block.  */
  if (gimple_call_nothrow_p (stmt))
    gimple_call_set_nothrow_p (stmt);

  /* ??? This is a hack to prevent tree-eh.c inlineable_call_p from
     deciding that the indirect call we have after this transformation
     might be inlinable, and thus changing the value of can_throw_internal,
     and thus requiring extra EH edges.  */
  gimple_call_set_noinline_p (stmt);

  gimple_call_set_fn (stmt, ret);
  update_stmt (stmt);

  return safe;
}

/* Walk the dominator tree for REGION, beginning at BB.  Install calls to
   tm_irrevokable when IRR_BLOCKS are reached, redirect other calls to the
   generated transactional clone.  */

static bool
ipa_tm_transform_calls (struct cgraph_node *node, struct tm_region *region,
			basic_block bb, bitmap irr_blocks)
{
  gimple_stmt_iterator gsi;
  bool need_ssa_rename = false;

  if (irr_blocks && bitmap_bit_p (irr_blocks, bb->index))
    {
      ipa_tm_insert_irr_call (node, region, bb);
      return true;
    }

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);
      struct cgraph_edge *e;
      struct cgraph_node *new_node;
      tree fndecl;

      if (!is_gimple_call (stmt))
	continue;
      if (is_tm_pure_call (stmt))
	continue;

      fndecl = gimple_call_fndecl (stmt);

      /* For indirect calls, pass the address through the runtime.  */
      if (fndecl == NULL)
	{
	  need_ssa_rename |=
	    ipa_tm_insert_gettmclone_call (node, region, &gsi, stmt);
	  continue;
	}

      /* Don't scan past the end of the transaction.  */
      if (is_tm_ending_fndecl (fndecl))
	continue;
      e = cgraph_edge (node, stmt);

      /* If there is a replacement, use it, otherwise use the clone.  */
      fndecl = find_tm_replacement_function (fndecl);
      if (fndecl)
	new_node = cgraph_node (fndecl);
      else
	{
	  struct tm_ipa_cg_data *d = get_cg_data (e->callee);
	  new_node = d->clone;

	  /* As we've already skipped pure calls and appropriate builtins,
	     and we've already marked irrevokable blocks, if we can't come
	     up with a static replacement, then ask the runtime.  */
	  if (new_node == NULL)
	    {
	      need_ssa_rename |=
	        ipa_tm_insert_gettmclone_call (node, region, &gsi, stmt);
	      cgraph_remove_edge (e);
	      continue;
	    }

	  fndecl = new_node->decl;
	}

      cgraph_redirect_edge_callee (e, new_node);
      gimple_call_set_fndecl (stmt, fndecl);
    }

  if (!region || !bitmap_bit_p (region->exit_blocks, bb->index))
    for (bb = first_dom_son (CDI_DOMINATORS, bb); bb;
	 bb = next_dom_son (CDI_DOMINATORS, bb))
      {
	need_ssa_rename |=
	  ipa_tm_transform_calls (node, region, bb, irr_blocks);
      }

  return need_ssa_rename;
}

/* Transform the calls within the TM regions within NODE.  */

static void
ipa_tm_transform_tm_atomic (struct cgraph_node *node)
{
  struct tm_ipa_cg_data *d = get_cg_data (node);
  struct tm_region *region;
  bool need_ssa_rename = false;

  current_function_decl = node->decl;
  push_cfun (DECL_STRUCT_FUNCTION (node->decl));
  calculate_dominance_info (CDI_DOMINATORS);

  /* ??? Update the marks for the GIMPLE_TM_ATOMIC node, in particular
     note the MUST_CALL_IRREVOKABLE and don't transform anything.  */
  for (region = d->all_tm_regions; region; region = region->next)
    {
      need_ssa_rename |=
	ipa_tm_transform_calls (node, region, region->entry_block,
				d->irrevokable_blocks_normal);
    }

  if (need_ssa_rename)
    update_ssa (TODO_update_ssa_only_virtuals);

  pop_cfun ();
  current_function_decl = NULL;
}

/* Transform the calls within the transactional clone of NODE.  */

static void
ipa_tm_transform_clone (struct cgraph_node *node)
{
  struct tm_ipa_cg_data *d = get_cg_data (node);
  bool need_ssa_rename;

  /* If this function makes no calls and has no irrevokable blocks,
     then there's nothing to do.  */
  /* ??? Remove non-aborting top-level transactions.  */
  if (!node->callees && !d->irrevokable_blocks_clone)
    return;

  current_function_decl = d->clone->decl;
  push_cfun (DECL_STRUCT_FUNCTION (current_function_decl));
  calculate_dominance_info (CDI_DOMINATORS);

  need_ssa_rename =
    ipa_tm_transform_calls (d->clone, NULL, single_succ (ENTRY_BLOCK_PTR),
			    d->irrevokable_blocks_clone);

  if (need_ssa_rename)
    update_ssa (TODO_update_ssa_only_virtuals);

  pop_cfun ();
  current_function_decl = NULL;
}

/* Main entry point for the transactional memory IPA pass.  */

static unsigned int
ipa_tm_execute (void)
{
  cgraph_node_queue tm_callees = NULL;
  cgraph_node_queue worklist = NULL;

  struct cgraph_node *node;
  struct tm_ipa_cg_data *d;
  enum availability a;
  unsigned int i;

#ifdef ENABLE_CHECKING
  verify_cgraph ();
#endif

  bitmap_obstack_initialize (&tm_obstack);

  /* For all local public functions marked tm_callable, queue them.  */
  for (node = cgraph_nodes; node; node = node->next)
    {
      a = cgraph_function_body_availability (node);
      if ((a == AVAIL_AVAILABLE || a == AVAIL_OVERWRITABLE)
	  && is_tm_callable (node->decl))
	{
	  d = get_cg_data (node);
	  maybe_push_queue (node, &tm_callees, &d->in_callee_queue);
	}
    }

  /* For all local reachable functions...  */
  for (node = cgraph_nodes; node; node = node->next)
    if (node->reachable && node->lowered
	&& cgraph_function_body_availability (node) >= AVAIL_OVERWRITABLE)
      {
	struct tm_region *regions;

	/* ... marked tm_pure, record that fact for the runtime by
	   indicating that the pure function is its own tm_callable.
	   No need to do this if the function's address can't be taken.  */
	if (is_tm_pure (node->decl))
	  {
	    if (!node->local.local)
	      record_tm_clone_pair (node->decl, node->decl);
	    continue;
	  }

	/* ... otherwise scan for calls marked in_tm_atomic.  */
	regions = ipa_tm_region_init (node);
	if (regions)
	  {
	    d = get_cg_data (node);
	    d->all_tm_regions = regions;
	    ipa_tm_scan_calls_tm_atomic (node, &tm_callees);
	  }
      }

  /* For every local function on the callee list, scan as if we will be
     creating a transactional clone, queueing all new functions we find
     along the way.  */
  for (i = 0; i < VEC_length (cgraph_node_p, tm_callees); ++i)
    {
      node = VEC_index (cgraph_node_p, tm_callees, i);
      a = cgraph_function_body_availability (node);
      d = get_cg_data (node);

      /* Some callees cannot be arbitrarily cloned.  These will always be
	 irrevokable.  Mark these now, so that we need not scan them.  */
      if (is_tm_irrevokable (node->decl)
	  || (a >= AVAIL_OVERWRITABLE
	      && !tree_versionable_function_p (node->decl)))
	{
	  ipa_tm_note_irrevokable (node, &worklist);
	  continue;
	}

      if (a >= AVAIL_OVERWRITABLE && !d->is_irrevokable)
	ipa_tm_scan_calls_clone (node, &tm_callees);
    }

  /* Iterate scans until no more work to be done.  Prefer not to use
     VEC_pop because the worklist tends to follow a breadth-first
     search of the callgraph, which should allow convergance with a
     minimum number of scans.  But we also don't want the worklist
     array to grow without bound, so we shift the array up periodically.  */
  for (i = 0; i < VEC_length (cgraph_node_p, worklist); ++i)
    {
      if (i > 256 && i == VEC_length (cgraph_node_p, worklist) / 8)
	{
	  VEC_block_remove (cgraph_node_p, worklist, 0, i);
	  i = 0;
	}

      node = VEC_index (cgraph_node_p, worklist, i);
      d = get_cg_data (node);
      d->in_worklist = false;

      if (d->want_irr_scan_normal)
	{
	  d->want_irr_scan_normal = false;
	  ipa_tm_scan_irr_function (node, false);
	}
      if (d->in_callee_queue && ipa_tm_scan_irr_function (node, true))
	ipa_tm_note_irrevokable (node, &worklist);
    }

  /* Create clones.  Do those that are not irrevokable and have a
     positive call count.  Do those publicly visible functions that
     the user directed us to clone.  */
  for (i = 0; i < VEC_length (cgraph_node_p, tm_callees); ++i)
    {
      bool doit = false;

      node = VEC_index (cgraph_node_p, tm_callees, i);
      a = cgraph_function_body_availability (node);
      d = get_cg_data (node);

      if (a <= AVAIL_NOT_AVAILABLE)
	doit = is_tm_callable (node->decl);
      else if (a <= AVAIL_AVAILABLE && is_tm_callable (node->decl))
	doit = true;
      else if (!d->is_irrevokable
	       && d->tm_callers_normal + d->tm_callers_clone > 0)
	doit = true;

      if (doit)
	ipa_tm_create_version (node);
    }

  /* Redirect calls to the new clones, and insert irrevokable marks.  */
  for (node = cgraph_nodes; node; node = node->next)
    if (node->reachable && node->lowered
	&& cgraph_function_body_availability (node) >= AVAIL_OVERWRITABLE)
      {
	d = get_cg_data (node);
	if (d->all_tm_regions)
	  ipa_tm_transform_tm_atomic (node);
      }
  for (i = 0; i < VEC_length (cgraph_node_p, tm_callees); ++i)
    {
      node = VEC_index (cgraph_node_p, tm_callees, i);
      if (node->analyzed)
	{
	  d = get_cg_data (node);
	  if (d->clone)
	    ipa_tm_transform_clone (node);
	}
    }

  /* Free and clear all data structures.  */
  VEC_free (cgraph_node_p, heap, tm_callees);
  VEC_free (cgraph_node_p, heap, worklist);
  bitmap_obstack_release (&tm_obstack);

  for (node = cgraph_nodes; node; node = node->next)
    node->aux = NULL;

#ifdef ENABLE_CHECKING
  verify_cgraph ();
#endif

  return 0;
}

struct simple_ipa_opt_pass pass_ipa_tm =
{
 {
  SIMPLE_IPA_PASS,
  "tmipa",				/* name */
  gate_tm,				/* gate */
  ipa_tm_execute,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  TV_TRANS_MEM,				/* tv_id */
  PROP_ssa | PROP_cfg,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func,			/* todo_flags_finish */
 },
};
