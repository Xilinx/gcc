/* Lowering pass for transactional memory directives.
   Converts markers of transactions into explicit calls to
   the STM runtime library.

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
   <http://www.gnu.org/licenses/>.

*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "rtl.h"
#include "gimple.h"
#include "langhooks.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "timevar.h"
#include "flags.h"
#include "function.h"
#include "expr.h"
#include "toplev.h"
#include "tree-pass.h"
#include "ggc.h"
#include "except.h"
#include "splay-tree.h"
#include "optabs.h"
#include "cfgloop.h"
#include "tree-ssa-live.h"
#include "tree-flow.h"


struct gtm_region *root_gtm_region;

/* Debugging dumps for transactional regions.  */
void dump_gtm_region (FILE *, struct gtm_region *, int);
void debug_gtm_region (struct gtm_region *);
void debug_all_gtm_regions (void);

/* Dump the gtm region tree rooted at REGION.  */

void
dump_gtm_region (FILE *file, struct gtm_region *region, int indent)
{
  fprintf (file, "%*sbb %d: GTM_TXN\n", indent, "", region->entry->index);

  if (region->inner)
    dump_gtm_region (file, region->inner, indent + 4);

  if (region->exit)
    fprintf (file, "%*sbb %d: GTM_RETURN\n", indent, "",
	     region->exit->index);

  if (region->next)
    dump_gtm_region (file, region->next, indent);
}

void
debug_gtm_region (struct gtm_region *region)
{
  dump_gtm_region (stderr, region, 0);
}

void
debug_all_gtm_regions (void)
{
  dump_gtm_region (stderr, root_gtm_region, 0);
}

/* Create a new gtm region starting at STMT inside region PARENT.  */

struct gtm_region *
new_gtm_region (basic_block bb, struct gtm_region *parent)
{
  struct gtm_region *region = XCNEW (struct gtm_region);

  region->outer = parent;
  region->entry = bb;

  if (parent)
    {
      /* This is a nested region.  Add it to the list of inner
	 regions in PARENT.  */
      region->next = parent->inner;
      parent->inner = region;
    }
  else
    {
      /* This is a toplevel region.  Add it to the list of toplevel
	 regions in ROOT_GTM_REGION.  */
      region->next = root_gtm_region;
      root_gtm_region = region;
    }

  return region;
}

/* Release the memory associated with the region tree rooted at REGION.  */

static void
free_gtm_region_1 (struct gtm_region *region)
{
  struct gtm_region *i, *n;

  for (i = region->inner; i ; i = n)
    {
      n = i->next;
      free_gtm_region_1 (i);
    }

  free (region);
}

/* Release the memory for the entire gtm region tree.  */
void
free_gtm_regions (void)
{
  struct gtm_region *r, *n;

  for (r = root_gtm_region; r ; r = n)
    {
      n = r->next;
      free_gtm_region_1 (r);
    }

  root_gtm_region = NULL;
}


/* Helper for build_gtm_regions.  Scan the dominator tree starting at
   block BB.  PARENT is the region that contains BB.  If SINGLE_TREE is
   true, the function ends once a single tree is built.  */

static void
build_gtm_regions_1 (basic_block bb, struct gtm_region *parent,
		     bool single_tree)
{
  gimple_stmt_iterator gsi;
  gimple stmt;
  basic_block son;
  struct gtm_region *region;

  gsi = gsi_last_bb (bb);
  if (!gsi_end_p (gsi))
    {
      stmt = gsi_stmt (gsi);
      switch (gimple_code (stmt))
	{
	case GIMPLE_GTM_TXN:
	  region = new_gtm_region (bb, parent);
	  break;

	case GIMPLE_GTM_RETURN:
	  /* STMT is the return point out of region PARENT.  Mark it
	     as the exit point and make PARENT the immediately enclosing
	     region.  */
	  gcc_assert (parent);
	  region = parent;
	  region->exit = bb;
	  parent = parent->outer;
	  break;

	default:
	  break;
	}
    }

  if (single_tree && !parent)
    return;

  for (son = first_dom_son (CDI_DOMINATORS, bb);
       son;
       son = next_dom_son (CDI_DOMINATORS, son))
    build_gtm_regions_1 (son, parent, single_tree);
}

/* Scan the CFG and build a tree of GTM regions.
   Return the root of the GTM region tree. */

static void
build_gtm_regions (void)
{
  gcc_assert (root_gtm_region == NULL);
  build_gtm_regions_1 (ENTRY_BLOCK_PTR, NULL, false);
}

/* Remove entry and exit marker from region. */

static void ATTRIBUTE_UNUSED
remove_gtm_stmts (struct gtm_region *region)
{
  gimple_stmt_iterator gsi;

  gcc_assert (region->entry);
  gcc_assert (region->exit);

  gsi = gsi_last_bb (region->entry);
  gcc_assert (gimple_code (gsi_stmt (gsi)) == GIMPLE_GTM_TXN);
  gsi_remove (&gsi, true);

  gsi = gsi_last_bb (region->exit);
  gcc_assert (gimple_code (gsi_stmt (gsi)) == GIMPLE_GTM_RETURN);
  gsi_remove (&gsi, true);
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
      if (DECL_IS_GTM_PURE_VAR (x))
	return false;
      if (is_global_var (x))
	return !TREE_READONLY (x);
      return TREE_ADDRESSABLE (x);

    default:
      return false;
    }
}

/* Subsituting a MODIFY_STMT with calls to the STM runtime.  */

static void
maybe_transactify_assign (gimple stmt)
{
  bool load_p = requires_barrier (gimple_assign_rhs1 (stmt));
  bool store_p = requires_barrier (gimple_assign_lhs (stmt));

  if (load_p)
    {
      gcc_assert (!store_p);
      gimple_assign_set_rhs_code (stmt, GTM_LOAD);
    }
  else if (store_p)
    gimple_assign_set_rhs_code (stmt, GTM_STORE);
}

/* Helper function that replaces call expressions inside
   transactions and issues a warning if no transactional
   clone is found. */

static void
maybe_transactify_call (gimple stmt)
{
  bool redirected = false;
  tree fn_decl;
  struct cgraph_node *node, *orig_node;
  int flags;

  flags = gimple_call_flags (stmt);
  if (flags & ECF_CONST)
    return;

  fn_decl = gimple_call_fndecl (stmt);
  if (!fn_decl)
    {
      warning (0, "Indirect call potentially breaks isolation of transactions");
      return;
    }
  if (DECL_IS_GTM_PURE (fn_decl))
    return;

  orig_node = node = cgraph_node (fn_decl);

  /* Find transactional clone of function.  */
  while (node && node->next_clone)
    {
      node = node->next_clone;
      if (DECL_IS_GTM_CLONE (node->decl))
	break;
    }

  if (DECL_IS_GTM_CLONE (node->decl))
    {
      struct cgraph_edge *callers = orig_node->callers;

      /* Find appropriate call stmt to redirect */
      while (callers)
	{
	  if (callers->call_stmt == stmt)
	    break;
	  callers = callers->next_caller;
	}

      /* Substitute call stmt. */
      if (callers)
	{
	  gimple_call_set_fndecl (stmt, node->decl);
	  cgraph_redirect_edge_callee (callers, node);
	  if (dump_file)
	    fprintf (dump_file, "redirected edge to %s\n",
		     get_name (node->decl));
	  redirected = true;
	}
    }

  /* In case the function call was not redirected and the function
     not marked as const or tm_pure, issue a warning. */
  /* ??? Handling of calls to irrevocable functions can be expanded here. */
  if (!redirected)
    warning (0, "Call to %qD potentially breaks isolation of transactions.",
	     fn_decl);
}

/* This function expands the stmts within a transaction so that
   the corresponding STM versions of the stmt is called. */

static void ATTRIBUTE_UNUSED
transactify_stmt (gimple_stmt_iterator *gsi)
{
  gimple stmt = gsi_stmt (*gsi);

  switch (gimple_code (stmt))
    {
    case GIMPLE_CALL:
      maybe_transactify_call (stmt);
      break;

    case GIMPLE_ASSIGN:
      /* Only memory reads/writes need to be instrumented.  */
      if (gimple_assign_single_p (stmt))
	maybe_transactify_assign (stmt);
      break;

    default:
      break;
    }
}

/* Main entry point for expanding GTM-GIMPLE into runtime calls to the STM. */

static unsigned int
execute_expand_gtm (void)
{
  /* In case we have to instrument a transactional clone. */
  if (DECL_IS_GTM_CLONE (current_function_decl))
    {
      if (!DECL_IS_GTM_PURE (current_function_decl))
	/* annotate_gtm_function_body () */ ;
      else
	{
#ifdef ENABLE_CHECKING
	  /* Function with tm_pure attribute specified.  The front-end
	     should have generated the appropriate errors and dropped
	     the transaction trees on the floor.  */
	  calculate_dominance_info (CDI_DOMINATORS);
	  build_gtm_regions ();
	  gcc_assert (root_gtm_region == NULL);
	  free_dominance_info (CDI_DOMINATORS);
#endif
	}
      return 0;
    }

  calculate_dominance_info (CDI_DOMINATORS);
  build_gtm_regions ();
  if (root_gtm_region)
    {
      if (dump_file)
	{
	  fprintf (dump_file, "\nGTM region tree\n\n");
	  dump_gtm_region (dump_file, root_gtm_region, 0);
	  fprintf (dump_file, "\n");
	}

      /* expand_gtm (root_gtm_region); */
      cleanup_tree_cfg ();
    }

  free_dominance_info (CDI_DOMINATORS);
  return 0;
}

/* GTM expansion -- the default pass, run before creation of SSA form.  */

static bool
gate_expand_gtm (void)
{
  return flag_gtm;
}

struct gimple_opt_pass pass_expand_gtm =
{
 {
  GIMPLE_PASS,
  "gtmexp",				/* name */
  gate_expand_gtm,			/* gate */
  execute_expand_gtm,			/* execute */
  NULL,					/* sub */
  NULL,					/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_gimple_any,			/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_dump_func
  | TODO_cleanup_cfg
  | TODO_ggc_collect,		        /* todo_flags_finish */
 }
};


#if 0
/* Calculate live ranges on SSA. Then checkpoint the
   live-in variables to the transaction. */

static void
checkpoint_live_in_variables (struct gtm_region *region,
			      gimple_stmt_iterator *gsi_recover,
			      basic_block begin_bb)
{
  int index = begin_bb->index;
  block_stmt_iterator bsi_save = bsi_for_stmt (region->setjmp_stmt);
  basic_block save_bb = bb_for_stmt (region->setjmp_stmt);
  basic_block recover_bb = bb_for_stmt (bsi_stmt (*bsi_recover));
  tree ssa_var;
  tree_live_info_p liveinfo;
  var_map map;
  int p;
  tree rep;
  unsigned int i;
  unsigned int j;
  bitmap_iterator bi;

  map = init_var_map (num_ssa_names + 1);

  /* Create liveness information for each SSA_NAME. */
  for (j = 0; j < num_ssa_names; j++)
    {
      ssa_var = ssa_name (j);
      if (!ssa_var)
	continue;

      if (TREE_CODE (ssa_var) == SSA_NAME)
	{
	  register_ssa_partition (map, ssa_var);
	  p = partition_find (map->var_partition, SSA_NAME_VERSION (ssa_var));
	  gcc_assert (p != NO_PARTITION);
	  rep = partition_to_var (map, p);
	}
    }

  liveinfo = calculate_live_ranges (map);

  /* If variable is live-in at beginning of the
     transaction checkpoint its value. */
  if (liveinfo->livein)
    {
      if (dump_file)
	fprintf (dump_file, "\nCheckpoint variables for transaction. BB %d : ", index);

      EXECUTE_IF_SET_IN_BITMAP (liveinfo->livein[index], 0, i, bi)
	{
	  tree var =  partition_to_var (map, i);

	  /* TODO check restricts the use of temporaries by the compiler
	     may impact other optimisations.
	     Maybe reordering this part of the checkpointing before introducing
	     temporary variables would avoid this check. */
	  if ((!DECL_ARTIFICIAL (SSA_NAME_VAR (var)))
	      && (!POINTER_TYPE_P (TREE_TYPE (var))))
	    {
	      if (dump_file)
		{
		  print_generic_expr (dump_file, var, TDF_SLIM);
		  fprintf (dump_file, "  ");
		}
	      /* Create name for temporary variable
		 that checkpoints value of var. */
	      const char* orig = get_name (SSA_NAME_VAR (var));
	      int len = strlen (orig);
	      char *name = xmalloc (sizeof (char) * (len + 10));
	      strncpy (name, "txn_save_", 9);
	      strncpy (name + 9, orig, len);
	      *(name + len + 9) = '\0';

	      /* Create temporary. */
	      tree type = TREE_TYPE (var);
	      tree save = create_tmp_var (type, name);
	      add_referenced_var (save);
	      tree stmt;

	      /* Create gimple statement for saving value of var. */
	      stmt = fold_build2 (GIMPLE_MODIFY_STMT, type, save, var);
	      tree real_save = make_ssa_name (save, stmt);
	      SSA_NAME_OCCURS_IN_ABNORMAL_PHI (real_save) = true;
	      GIMPLE_STMT_OPERAND (stmt, 0) = real_save;

	      bsi_insert_before (&bsi_save, stmt, BSI_SAME_STMT);

	      /* Create gimple statement for restoring value of var. */
 	      stmt = fold_build2 (GIMPLE_MODIFY_STMT, type, var, real_save);
	      tree new_var = make_ssa_name (SSA_NAME_VAR (var), stmt);
	      GIMPLE_STMT_OPERAND (stmt, 0) = new_var;
	      bsi_insert_before (bsi_recover, stmt, BSI_SAME_STMT);

	      /* Merge saved or recovered values before next basic block. */
	      tree phi = create_phi_node (SSA_NAME_VAR (var), begin_bb);
	      add_phi_arg (phi, new_var, FALLTHRU_EDGE (recover_bb));
	      add_phi_arg (phi, var, FALLTHRU_EDGE (save_bb));
	      tree new_var_phi = PHI_RESULT (phi);

	      free_dominance_info (CDI_DOMINATORS);
	      calculate_dominance_info (CDI_DOMINATORS);

	      tree stmt2;
	      imm_use_iterator iter;
	      use_operand_p use_p;
	      FOR_EACH_IMM_USE_STMT (stmt2, iter, var)
		{
		  if (stmt2 == phi)
		    continue;

		  basic_block tmp_bb = bb_for_stmt (stmt2);
		  if (dominated_by_p (CDI_DOMINATORS, tmp_bb, begin_bb))
		    {
		      FOR_EACH_IMM_USE_ON_STMT (use_p, iter)
			propagate_value (use_p, new_var_phi);
		    }
		}
	    }
	}
      if (dump_file)
	fprintf (dump_file, "\n");

    }
  update_ssa(TODO_update_ssa);

  return ;
}

/* Implements the checkpointing of transactions. */
static void
checkpoint_gtm_txn (struct gtm_region *region)
{
  basic_block entry_bb = bb_for_stmt (region->setjmp_stmt);

  edge branch = BRANCH_EDGE (entry_bb);
  edge fall = FALLTHRU_EDGE (entry_bb);

  basic_block begin_bb = fall->dest;
  basic_block recover_bb = branch->dest;
  basic_block next_bb = single_succ (recover_bb);

  gcc_assert(begin_bb == next_bb);
  block_stmt_iterator bsi_recover = bsi_start (recover_bb);
  gcc_assert (TREE_CODE (bsi_stmt (bsi_recover)) == LABEL_EXPR);

  bsi_next (&bsi_recover);
  gcc_assert (TREE_CODE (bsi_stmt (bsi_recover)) == GTM_RETURN);

  checkpoint_live_in_variables (region, &bsi_recover, begin_bb);
  /* Remove the previously set GTM_RETURN markers
     from the recover basic block. */
  bsi_remove (&bsi_recover, true);
}

/* Walk the region tree and start checkpointing. */
static void
checkpoint_gtm (struct gtm_region *region)
{
  while (region)
    {
      /* First, introduce checkpoints for the inner regions.
	 TODO: testing. Overlapping of inner and outer
	 regions not handled correctly.
	 Nesting of transactions not implemented correctly.*/
      if (region->inner)
	{
	  checkpoint_gtm_txn (region->inner);
	}
      gcc_assert ((region->type) == GTM_TXN);

      checkpoint_gtm_txn (region);

      region = region->next;
    }
}

/* Entry point to the checkpointing. */
void
execute_checkpoint_gtm (void)
{
  /* Regions are built during GTM expansion pass. */
  if (!root_gtm_region)
    return;

  /* Checkpointing is done here. */
  checkpoint_gtm (root_gtm_region);

  if (dump_file)
    {
      fprintf (dump_file, "\nGTM region tree after checkpointing\n\n");
      dump_gtm_region (dump_file, root_gtm_region, 0);
      fprintf (dump_file, "\n");
    }

  free_dominance_info (CDI_DOMINATORS);
  cleanup_tree_cfg ();
  free_gtm_regions ();

  return;
}

/* Guarding the checkpointing for GTM. */
static bool
gate_checkpoint_gtm (void)
{
  return flag_gtm;
}

struct tree_opt_pass pass_checkpoint_gtm =
{
  "gtmcheckpoint",			/* name */
  gate_checkpoint_gtm,		/* gate */
  execute_checkpoint_gtm,		/* execute */
  NULL,				/* sub */
  NULL,				/* next */
  0,					/* static_pass_number */
  0,					/* tv_id */
  PROP_ssa | PROP_cfg,		/* properties_required */
  0,			                /* properties_provided */
  0,					/* properties_destroyed */
  0,					/* todo_flags_start */
  TODO_update_ssa |
  TODO_verify_ssa |
  TODO_dump_func,			/* todo_flags_finish */
  0					/* letter */
};
#endif
