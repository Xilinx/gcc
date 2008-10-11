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
#include "tree-gimple.h"
#include "tree-inline.h"
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

struct gtm_region *root_gtm_region;
unsigned int label_index;
unsigned int e_index;
edge edges_to_instrument[NUM_BB_TXN];

/* Function declarations for GTM expansion and checkpointing. */
void check_and_mark_edges (struct gtm_region *, basic_block); 
char *check_call_expr (tree); 
void checkpoint_live_in_variables (struct gtm_region *, block_stmt_iterator *, basic_block); 
void compensate_for_taking_the_address (tree);
static void expand_gtm_abort (block_stmt_iterator *, enum bsi_iterator_update, tree);


/* Debugging dumps for transactional regions.  */
void dump_gtm_region (FILE *, struct gtm_region *, int);
void debug_gtm_region (struct gtm_region *);
void debug_all_gtm_regions (void);

void execute_lower_gtm (void);
void execute_checkpoint_gtm (void);
static bool gate_expand_gtm (void);
static bool gate_checkpoint_gtm (void);

void init_label_table (void); 
tree insert_temporary  (block_stmt_iterator, tree);
void instrument_edges (tree);
void instrument_return_expr (block_stmt_iterator *, tree);

void may_repair_rhs (tree, block_stmt_iterator, tree);
void record_bb_into_table (struct gtm_region *, basic_block);


/* Dump the gtm region tree rooted at REGION.  */
void
dump_gtm_region (FILE *file, struct gtm_region *region, int indent)
{
  fprintf (file, "%*sbb %d: %s\n", indent, "", region->entry->index,
	   tree_code_name[region->type]);

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
new_gtm_region (basic_block bb, enum tree_code type, struct gtm_region *parent)
{
  struct gtm_region *region = xcalloc (1, sizeof (*region));

  region->outer = parent;
  region->entry = bb;
  region->type = type;

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
   true, the function ends once a single tree is built (like constructing omp region trees). */
static void
build_gtm_regions_1 (basic_block bb, struct gtm_region *parent,
		     bool single_tree)
{
  block_stmt_iterator si;
  tree stmt;
  basic_block son;

  si = bsi_last (bb);
  if (!bsi_end_p (si)) /* bsi_stmt should not be NULL */
    {
      if (GTM_DIRECTIVE_P (bsi_stmt (si)))
	{
	  struct gtm_region *region;
	  enum tree_code code;
	  stmt = bsi_stmt (si);
	  code = TREE_CODE (stmt);
	  if (code == GTM_RETURN)
	    {
	      /* STMT is the return point out of region PARENT.  Mark it
		 as the exit point and make PARENT the immediately
		 enclosing region.  */
	      gcc_assert (parent);
	      region = parent;
	      region->exit = bb;
	      parent = parent->outer;
	    }
	  else if (code == GTM_TXN)
	    {
	      /* Otherwise, this directive becomes the parent for a new
		 region.  GTM_TXN is the only one left for now. */
	      region = new_gtm_region (bb, code, parent);
	      parent = region;
	    }
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
  calculate_dominance_info (CDI_DOMINATORS);
  build_gtm_regions_1 (ENTRY_BLOCK_PTR, NULL, false);
}

/* Remove entry and exit marker from region. */
static void
remove_gtm_stmts (struct gtm_region *region)
{
  basic_block entry_bb, exit_bb;
  block_stmt_iterator si;
  tree stmt;
  
  entry_bb = region->entry;
  exit_bb = region->exit;

  gcc_assert (entry_bb);
  gcc_assert (exit_bb);

  si = bsi_last (entry_bb);
  stmt = bsi_stmt (si);
  
  gcc_assert (TREE_CODE (stmt) == GTM_TXN);

  bsi_remove (&si, true);
  single_succ_edge (entry_bb)->flags = EDGE_FALLTHRU;

  if (exit_bb)
    {
      si = bsi_last (exit_bb);
      gcc_assert (!bsi_end_p (si)
		  && TREE_CODE (bsi_stmt (si)) == GTM_RETURN);

      bsi_remove (&si, true);
      single_succ_edge (exit_bb)->flags = EDGE_FALLTHRU;
    }

  return ;
}

/* If the STM only supports flat nesting, all 
   nested transactions are collapsed into 
   the outermost one. */
static void
collapse_gtm_regions (struct gtm_region *region)
{
  while (region)
    {
      /* Collapse only the inner regions.  */
      if (region->inner) 
	{
	  remove_gtm_stmts (region->inner);
	  collapse_gtm_regions (region->inner);
	  free (region->inner);
	  region->inner = NULL;
	}

      gcc_assert ((region->type) == GTM_TXN);
      region = region->next;
    }  

  return ;
}

/* TODO: add mechanism to query the
   STM for supported features. */
static bool
query_STM_for_flat_nesting (void) 
{
  return true;
}

/* This is an enhancement to the TM concept.
   It allows the programmer to specify where 
   to drop some instrumentation. Variables in 
   the source code can be marked as tm_pure. 
   The helper function sets the corresponding 
   attribute. */
static void
set_gtm_pure_var (tree t)
{
  if ((TREE_CODE (t) == INDIRECT_REF)
      || (TREE_CODE (t) == COMPONENT_REF)
      || (TREE_CODE (t) == ARRAY_REF))
    {
      return set_gtm_pure_var (TREE_OPERAND (t, 0));
    }
  
  if (TREE_CODE (t) == VAR_DECL)
    {
      DECL_IS_GTM_PURE_VAR (t) = 1;
    }
  return ;
}

/* This is an enhancement to the TM concept.
   It allows the programmer to specify where 
   to drop some instrumentation. Variables in 
   the source code can be marked as tm_pure. 
   The helper function checks whether the 
   attribute was specified. */
static bool
is_gtm_pure_var (tree t) 
{
  if ((TREE_CODE (t) == INDIRECT_REF)
      || (TREE_CODE (t) == COMPONENT_REF)
      || (TREE_CODE (t) == ARRAY_REF))
    {
      return is_gtm_pure_var (TREE_OPERAND (t, 0));
    }

  if (TREE_CODE (t) == VAR_DECL)
    {
      if (DECL_IS_GTM_PURE_VAR (t) == 1)
	{
	  return true;
	}
    }
  return false;
} 

/* Determine whether operand has to be 
   instrumented using a read barrier. */
/* TODO: check if _all_ cases are covered. */
static bool
requires_read_barrier (tree operand) 
{
  if (TREE_CODE (operand) == SSA_NAME)
    operand = SSA_NAME_VAR (operand);
  
  /* Check whether we may drop some instrumentation. */
  if (is_gtm_pure_var (operand))
    return false;

  if ((TREE_CODE (operand) == COMPONENT_REF) 
      || (TREE_CODE (operand) == INDIRECT_REF)
      || (TREE_CODE (operand) == ARRAY_REF))
    return true;

  /* Casts are ignored - descent recursively. */
  if ((TREE_CODE (operand) == NOP_EXPR)
      || (TREE_CODE (operand) == FLOAT_EXPR)
      || (TREE_CODE (operand) == FIX_TRUNC_EXPR))
    return requires_read_barrier (TREE_OPERAND (operand, 0)); 

  /* In case the variable is defined inside the scope of 
     the transaction, there is no need for instrumentation. */
  if (TREE_CODE (operand) == VAR_DECL) 
    if (DECL_IS_GTM_PURE_VAR (operand)) 
      return false;

  if ((CONSTANT_CLASS_P (operand)) 
      || (DECL_ARTIFICIAL (operand))) 
    return false;
  else 
    return ((TREE_CODE (operand) == VAR_DECL && is_global_var (operand)) 
	    || (POINTER_TYPE_P (TREE_TYPE (operand)))
	    || (TREE_ADDRESSABLE (operand)));
}

/* Determine whether operand has to be instrumented 
   using a write barrier. */
/* TODO: check if _all_ cases are covered. */
static bool
requires_write_barrier (tree operand) 
{
  if (TREE_CODE (operand) == SSA_NAME)
    operand = SSA_NAME_VAR (operand);

  /* Check whether we may drop some instrumentation. */
  if (is_gtm_pure_var (operand))
    return false;

  if ( (TREE_CODE (operand) == COMPONENT_REF) 
       || (TREE_CODE (operand) == INDIRECT_REF)
       || (TREE_CODE (operand) == ARRAY_REF))
    return true;

  if ((CONSTANT_CLASS_P (operand))
      || (DECL_ARTIFICIAL (operand))) 
    return false;
  else 
    return ((TREE_CODE (operand) == VAR_DECL && is_global_var (operand)) 
	    || (POINTER_TYPE_P (TREE_TYPE (operand))) 
	    || (TREE_ADDRESSABLE (operand)));
}

/* Helper function returning the declaration 
   of a builtin stm function if the operand 
   has a type real. */
static tree
get_real_stm_decl (bool store, tree op) 
{
  tree decl;
  tree type = TYPE_MAIN_VARIANT (TREE_TYPE (op));
  if (!store)
    {
      if (type == double_type_node)
	decl = built_in_decls [BUILT_IN_GTM_LOAD_DOUBLE];
      else 	
	{
	  gcc_assert (type == float_type_node);
	  decl = built_in_decls [BUILT_IN_GTM_LOAD_FLOAT];
	}
    }
  else 
    {
      if (type == double_type_node)
	decl = built_in_decls [BUILT_IN_GTM_STORE_DOUBLE];
      else
	{
	  gcc_assert (type == float_type_node);
	  decl = built_in_decls [BUILT_IN_GTM_STORE_FLOAT];
	}
    }
  gcc_assert(decl);
  
  return decl;
}

/* Returns function decl determined by 
   type size of operand. */
static tree
get_uint_stm_decl (int builtin, tree op) 
{
  HOST_WIDE_INT index;
  tree decl;
  tree type = TYPE_MAIN_VARIANT (TREE_TYPE (op));

  index = tree_low_cst (TYPE_SIZE_UNIT (type), 1);
  index = exact_log2 (index);

  if (index >= 0 && index <= 4)
    decl = built_in_decls[builtin + index + 1];
  else 
    gcc_unreachable();

  gcc_assert(decl);

  return decl;
}

/* Insert new temporary variable that 
   lives in a GIMPLE register and issue 
   a load of the "old" variable. */
tree 
insert_temporary (block_stmt_iterator bsi, tree op)
{
  tree new_var;
  tree stmt;
  
  new_var = create_tmp_var (TREE_TYPE (op), get_name(op));
  stmt = build_gimple_modify_stmt (new_var, op);

  stmt = unshare_expr (stmt);

  bsi_insert_before (&bsi, stmt, BSI_SAME_STMT);
  return new_var;
}

/* Check whether variables used in stmts are the 
   ones to replace by a use of a GIMPLE register. 
   If they match the replacement is done. */
void
may_repair_rhs (tree rhs, block_stmt_iterator bsi, tree op)
{
  tree new_var;

  if (UNARY_CLASS_P (rhs))
    {
      if (op == rhs)
	{
	  new_var = insert_temporary  (bsi, op);
	  tree stmt = bsi_stmt (bsi);
	  GIMPLE_STMT_OPERAND (stmt, 1) = new_var;
	}
    }
  else 
    {
      if (BINARY_CLASS_P (rhs))
	{
	  if (TREE_OPERAND (rhs, 0) == op)
	    {
	      new_var = insert_temporary (bsi, op);
	      TREE_OPERAND (rhs, 0) = new_var;
	    }
	  if (TREE_OPERAND (rhs, 1) == op)
	    {
	      new_var = insert_temporary (bsi, op);
	      TREE_OPERAND (rhs, 1) = new_var;
	    }
	}
    }

  return;
}

/* Compensate for taking the address of a local variable. 
   By using the address of the variable, the variable escapes 
   the local scope and becomes global. 
   Since global variables on GIMPLE have to be transferred to 
   registers before they can be used, this behaviour has to 
   be added. */
void 
compensate_for_taking_the_address (tree op)
{
  basic_block bb;
  block_stmt_iterator bsi;
  tree stmt;
  tree rhs;

  FOR_EACH_BB (bb)
  {
    for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
      {
	stmt = bsi_stmt (bsi);

	/* TODO: add more possible uses of a variable here! */
	if (TREE_CODE (stmt) == GIMPLE_MODIFY_STMT)
	  {
	    rhs = GIMPLE_STMT_OPERAND (stmt, 1);
	    may_repair_rhs(rhs, bsi, op);
	  }

	if (TREE_CODE (stmt) == CALL_EXPR)
	  {
	    tree arg;
	    call_expr_arg_iterator iter;
	    FOR_EACH_CALL_EXPR_ARG (arg, iter, stmt)
	      may_repair_rhs (arg, bsi, op);
	  }
      }
  }

  return ;
}     

/* Helper function that
   composes the STM store function call. */
static tree
compose_stm_store_call (tree op, tree txn_handle, tree value) 
{
  tree call;
  tree decl;
  bool compensate = false;

  if (TREE_CODE (op) == SSA_NAME)
    op = SSA_NAME_VAR (op);
  
  if (!TREE_ADDRESSABLE (op))
    compensate = true;

  if (TREE_CODE (TREE_TYPE (op)) == INTEGER_TYPE)
    decl = get_uint_stm_decl (BUILT_IN_GTM_STORE_N, op); 
  
  if (TREE_CODE (TREE_TYPE (op)) == POINTER_TYPE)
    {
#ifdef TINYSTM_VERSION_0_9_0b1
      decl = built_in_decls [BUILT_IN_GTM_STORE_PTR];
#endif
#ifdef TANGER
      decl = built_in_decls [BUILT_IN_GTM_STORE_PTR];
#endif
#ifdef TINYSTM_VERSION_0_9_5
      decl = built_in_decls [BUILT_IN_GTM_STORE_PTR];
#endif
    }

  if (TREE_CODE (TREE_TYPE (op)) == REAL_TYPE)
    decl = get_real_stm_decl (true, op); 

  gcc_assert (decl);

  if (txn_handle != NULL_TREE)
    call = build_call_expr (decl, 3, txn_handle, 
			    build_fold_addr_expr (op), value); 
  else 
    call = build_call_expr (decl, 2, build_fold_addr_expr (op), value); 

  if (compensate)
    compensate_for_taking_the_address (op);

  return call;
}

/* Helper function that composes 
   the STM load function call. */
static tree
compose_stm_load_call (tree op, tree txn_handle) 
{
  tree decl = NULL_TREE;
  tree call;
  bool compensate = false;
  
  if (TREE_CODE (op) == SSA_NAME)
    op = SSA_NAME_VAR (op);

  if (!TREE_ADDRESSABLE (op))
    compensate = true;

  if (TREE_CODE (TREE_TYPE (op)) == INTEGER_TYPE)
    decl = get_uint_stm_decl(BUILT_IN_GTM_LOAD_N, op); 

  if (TREE_CODE (TREE_TYPE (op)) == REAL_TYPE)
    decl = get_real_stm_decl(false, op); 

  if (TREE_CODE (TREE_TYPE (op)) == POINTER_TYPE)
    {
#ifdef TINYSTM_VERSION_0_9_0b1
      decl = built_in_decls [BUILT_IN_GTM_LOAD_PTR];
#endif
#ifdef TANGER
      decl = built_in_decls [BUILT_IN_GTM_LOAD_PTR];
#endif
#ifdef TINYSTM_VERSION_0_9_5
      decl = built_in_decls [BUILT_IN_GTM_LOAD_PTR];
#endif
    }

  gcc_assert (decl);
  
  if (txn_handle != NULL_TREE)
    {
      call = build_call_expr (decl, 2, txn_handle, 
			      build_fold_addr_expr(op));
    }
  else 
    {
      call = build_call_expr (decl, 1, build_fold_addr_expr(op));
    }
  
  if (compensate)
    compensate_for_taking_the_address (op);
  
  return call;
}

/* Emits call to stm_load including the 
   txn_handle and address of the variable. */
static tree
insert_rhs_stm_call (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree op, tree txn_handle)
{
  tree stmt;
  tree t_load = create_tmp_var (TREE_TYPE (op), "txn_tmp");
  
  tree call = compose_stm_load_call (op, txn_handle);
  
  stmt = build_gimple_modify_stmt (t_load, call);
  stmt = unshare_expr (stmt);
  bsi_insert_before (bsi, stmt, m);

  return t_load;
} 

/* Introduce temporary variable if necessary 
   and emit call to stm_store. */
static void
replace_lhs (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree op, tree txn_handle)
{
  tree t_store;
  tree stmt, call;
  tree op_type = TREE_TYPE (op);
  tree mod_stmt = bsi_stmt (*bsi);
  tree rhs = GIMPLE_STMT_OPERAND (mod_stmt, 1);
  
  if ((!CONSTANT_CLASS_P (rhs))
      && (!is_gimple_formal_tmp_var (rhs))) 
    {
      t_store = create_tmp_var (op_type, "txn_tmp");
      stmt = build_gimple_modify_stmt (t_store, rhs);
      stmt = unshare_expr (stmt);
      bsi_insert_before (bsi, stmt, m);
    }
  else  
    {
      t_store = rhs;
    }
  
  call = compose_stm_store_call(op, txn_handle, t_store);
  call = unshare_expr (call);
  bsi_insert_after (bsi, call, m);
  bsi_remove (bsi, true);

  return ;
}

/* Check whether the operands need a read 
   barrier and insert it. 
   TODO: relax handling of ARRAY_REFs. */
static void
maybe_replace_rhs_stmt (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree stmt, tree txn_handle) 
{
  tree t_load;
  tree op1, op2;
  tree rhs = GIMPLE_STMT_OPERAND (stmt, 1);  

  if ((UNARY_CLASS_P (rhs)) 
      || (CONSTANT_CLASS_P (rhs))
      || (TREE_CODE (rhs) == VAR_DECL))
    {
      op1 = rhs;
      if (requires_read_barrier (op1))
	{
	  if (TREE_CODE (op1) == NOP_EXPR)
	    op1 = TREE_OPERAND (op1, 0);
	  t_load = insert_rhs_stm_call (bsi, m, op1, txn_handle);
	  GIMPLE_STMT_OPERAND (stmt, 1) = t_load;
	}
    }
  else 
    {
      if (BINARY_CLASS_P (rhs))
	{
	  op1 = TREE_OPERAND (rhs, 0);
	  if (requires_read_barrier (op1))
	    {
	      t_load = insert_rhs_stm_call (bsi, m, op1, txn_handle);
	      TREE_OPERAND (rhs, 0) = t_load; 
	    }
	  op2 = TREE_OPERAND (rhs, 1);
	  if (requires_read_barrier (op2))
	    {
	      t_load = insert_rhs_stm_call (bsi, m, op2, txn_handle);
	      TREE_OPERAND (rhs, 1) = t_load; 
	    }
	}
      else 
	{
	  if ((POINTER_TYPE_P (TREE_TYPE (rhs))
	       || (TREE_CODE (rhs) == COMPONENT_REF)
	       || ((TREE_CODE (rhs) == INDIRECT_REF) 
		   && (DECL_IS_GTM_PURE_VAR (TREE_OPERAND (rhs, 0)) != 1))
	       || (TREE_CODE (rhs) == ARRAY_REF)) 
	      && (TREE_CODE (rhs) != CALL_EXPR)
	      )
	    {
	      op1 = rhs;
	      t_load = insert_rhs_stm_call (bsi, m, op1, txn_handle);
	      GIMPLE_STMT_OPERAND (stmt, 1) = t_load;
	    }
	}
    }

  return ;
}  

/* Subsituting a MODIFY_STMT   
   with calls to the STM runtime, 
   the worst case looks like this:

   t1 = stm_load(b);
   t2 = stm_load(c);
   t3 = t1 * t2;
   stm_store(a, t3); 

   substitutes for:

   a = b * c; */
static void
replace_txn_mod_stmt (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree txn_handle)
{

  tree stmt = bsi_stmt (*bsi);
  tree lhs = GIMPLE_STMT_OPERAND (stmt, 0);
  tree rhs  = GIMPLE_STMT_OPERAND (stmt, 1);

  /* Propagate GTM_PURE_VAR in case of an assignment 
     to a temporary variable. */
  if (is_gtm_pure_var (rhs))
    set_gtm_pure_var (lhs);

  maybe_replace_rhs_stmt (bsi, m, stmt, txn_handle); 

  if (requires_write_barrier (lhs))
    {
      replace_lhs (bsi, m, lhs, txn_handle); 
    }
  
  return ;
} 

/* Build and return a call to a transactional function. 
   Add one parameter to pass a transaction handle. */
tree
build_txn_call_expr (tree orig_ce, struct cgraph_node *tm_node, tree txn_handle) 
{
  tree arglist = CALL_EXPR_ARGS (orig_ce);

  if (txn_handle)
    {
      arglist = chainon (arglist, build_tree_list (NULL_TREE, txn_handle));
    }
 
  return build_function_call_expr (tm_node->decl, arglist);
}

/* Return function name or NULL if function 
   is a STM compiler builtin. */
char *
check_call_expr (tree ce) 
{
  char *name = get_name (get_callee_fndecl (ce));
#ifdef TANGER
  if (strncmp(name, "__builtin_tanger_stm", 20) != 0)
    return name;
#else
  if (strncmp(name, "__builtin_stm", 13) != 0)
    return name;
#endif  
  return NULL;
}

/* Helper function that replaces call expressions inside 
   transactions and issues a warning if no transactional 
   clone is found. */
void
replace_call_expr_in_txn (tree ce, tree stmt, tree txn_handle) 
{
  bool redirected = false;
  char *name;
  name = check_call_expr (ce);
  if (name)
    {
      tree fn_decl = get_callee_fndecl (ce);
      struct cgraph_node *node = cgraph_node (fn_decl);
      struct cgraph_node *orig_node = node;
	  
      /* find transactional clone of function */
      while (node && node->next_clone)
	{
	  node = node->next_clone;
	  if (DECL_IS_GTM_CLONE (node->decl))
	    break;
	}

      if (DECL_IS_GTM_CLONE (node->decl))
	{
	  struct cgraph_edge *callers = orig_node->callers;
	      
	  /* find appropriate call stmt to redirect */
	  while (callers) 
	    {
	      if (callers->call_stmt != stmt) 
		callers = callers->next_caller;
	      else break;
	    }
	      
	  /* substitute call stmt. */
	  if (callers)
	    {
	      tree txn_ce;
	      txn_ce = build_txn_call_expr (ce, node, txn_handle);

	      if (TREE_CODE (stmt) == CALL_EXPR)
		{
		  block_stmt_iterator bsi = bsi_for_stmt (stmt);
		  bsi_insert_before (&bsi, txn_ce, BSI_SAME_STMT);
		  bsi_remove (&bsi, true);
		}
	      else
		GIMPLE_STMT_OPERAND (stmt, 1) = txn_ce;

	      cgraph_redirect_edge_callee (callers, node); 	      
	      if (dump_file)
		fprintf(dump_file, "redirected edge to %s\n", get_name (node->decl));
	      redirected = true;
	    }
	}

      /* Redirect calls to malloc and related functions to transactional versions if available. */
#ifdef TANGER
      if ((DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_MALLOC)
	  ||(DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_CALLOC)
	  || (DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_REALLOC)
	  || (DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_FREE))
	{
	  tree txn_ce;
	  tree new_decl; 
	  if (DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_MALLOC)
	    new_decl = built_in_decls[BUILT_IN_GTM_MALLOC];
	  if (DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_CALLOC)
	    new_decl = built_in_decls[BUILT_IN_GTM_CALLOC];
	  if (DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_REALLOC)
	    new_decl = built_in_decls[BUILT_IN_GTM_REALLOC];
	  if (DECL_FUNCTION_CODE (fn_decl) == BUILT_IN_FREE)
	    new_decl = built_in_decls[BUILT_IN_GTM_FREE];

	  tree arglist = CALL_EXPR_ARGS (ce);

	  if (txn_handle)
	    {
	      arglist = chainon (arglist, build_tree_list (NULL_TREE, txn_handle));
	    }
	  txn_ce = build_function_call_expr (new_decl, arglist);
	  if (DECL_FUNCTION_CODE (fn_decl) != BUILT_IN_FREE)
	    txn_ce = build_gimple_modify_stmt (GIMPLE_STMT_OPERAND (stmt, 0), txn_ce);
	   
	  block_stmt_iterator bsi = bsi_for_stmt (stmt);
	  bsi_insert_before (&bsi, txn_ce, BSI_SAME_STMT);
	  bsi_remove (&bsi, true);
	   	   
	}
#endif
      /* In case the function call was not redirected and the function not marked as const or tm_pure, 
	 issue a warning. */
      /* TODO: handling of calls to irrevocable functions can be expanded here. */
      if ((!redirected) 
	  && (TREE_READONLY(node->decl) != 1)
	  && (DECL_IS_GTM_PURE (node->decl) != 1))
	{
	  warning (0, "GTM: irrevocable functions not supported. Call to %qs potentially breaks isolation of transactions.", name);
	  warning (0, "GTM: No transactional clone found for  %qs.", name);
	}
    }      
  return ;
}

/* This function expands the stmts within a 
   transaction so that the corresponding STM 
   versions of the stmt is called. */
static void
replace_txn_stmt (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree txn_handle)
{
  tree stmt = bsi_stmt(*bsi);

  /* TODO redirect call to tm clone use get_call_expr_in */
  tree ce = get_call_expr_in (stmt);
  if (ce)
    replace_call_expr_in_txn (ce, stmt, txn_handle);

  switch TREE_CODE (stmt) {
      
    case MODIFY_EXPR:
    case GIMPLE_MODIFY_STMT:
      replace_txn_mod_stmt (bsi, m, txn_handle);
      break;
      
    case CALL_EXPR:
      /* already handled above. */
      break;

    case GTM_ABORT:
      expand_gtm_abort (bsi, m, txn_handle);
      break;

    default:
      break;
    }
 
  return;
}

/* Mark recover_bb with a GTM-Return marker
   which is replaced by variables restoring the 
   previous state during the checkpoint_gtm-pass. */
static void
setup_recover_bb (basic_block bb, tree label) 
{
  block_stmt_iterator bsi = bsi_start (bb); 
  tree stmt =  build1 (LABEL_EXPR, void_type_node, label);

  stmt = unshare_expr (stmt);
  bsi_insert_before (&bsi, stmt, BSI_SAME_STMT);
  stmt = make_node (GTM_RETURN);
  stmt = unshare_expr (stmt);
  bsi_insert_before (&bsi, stmt, BSI_SAME_STMT);

  return ;
}

/* Helper function that emits a call to STM 
   run-time indicating the start of a transaction. */
static void
setup_begin_bb (basic_block bb, tree txn_handle, tree jmp_buf)
{
  tree txn_begin;
  tree decl;
  tree read_only = build_int_cst (integer_type_node, 0);
  block_stmt_iterator bsi = bsi_start (bb); 
  
  decl = built_in_decls[BUILT_IN_GTM_TXN_BEGIN];

  if (txn_handle != NULL_TREE)
    txn_begin = build_call_expr (decl, 3, txn_handle, 
				 jmp_buf, build_fold_addr_expr (read_only));
  else 
    txn_begin = build_call_expr (decl, 2, jmp_buf, 
				 build_fold_addr_expr (read_only));
  txn_begin = unshare_expr (txn_begin);
  bsi_insert_before (&bsi, txn_begin, BSI_SAME_STMT);

  return ;
}

/* Helper function that emits call to 
   abort the transaction in an STM-specific way. */
static void 
expand_gtm_abort (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree txn_handle)
{
  tree decl;
  tree call;

  decl = built_in_decls [BUILT_IN_GTM_ABORT];
  if (txn_handle != NULL_TREE)
    call = build_call_expr (decl, 1, txn_handle);
  else 
    call = build_call_expr (decl, 0);
  call = unshare_expr (call);
  bsi_insert_before (bsi, call, m);
  bsi_remove(bsi, true);

  return ;
}

#ifdef GTM_EXPL_HANDLE
/* Insert call to stm_init at beginning of function. */
static tree
maybe_insert_stm_new (enum bsi_iterator_update m)
{
  basic_block bb;

  FOR_EACH_BB (bb) 
  {
    int idx = bb->index;
    /* TODO find a better place to put this call  (works so far) */
    if (idx == 2) 
      {
	/* Avoid redundant calls to stm_new */ 
	block_stmt_iterator bsi = bsi_start (bb);
	tree mod = bsi_stmt (bsi);
	if (TREE_CODE (mod) == GIMPLE_MODIFY_STMT) 
	  {
	    tree ce = GIMPLE_STMT_OPERAND (mod, 1);
	    if (TREE_CODE (ce) == CALL_EXPR) 
	      {
		char *name = get_name (get_callee_fndecl (ce));
#ifdef TINYSTM_VERSION_0_9_0b1
		if (strncmp (name, "__builtin_stm_new", 17) == 0)
#endif
#ifdef TANGER
		  if (strncmp (name, "__builtin_tanger_stm_get_tx", 27) == 0)
#endif
		    {
		      return GIMPLE_STMT_OPERAND (mod, 0);
		    }
	      }
	  }
	
	/* stm_txn_t *tx;
	   tx = stm_new (); */
	tree ptr_void = build_pointer_type (void_type_node);
	tree txn_handle = create_tmp_var (ptr_void, "txn_handle");
	tree decl = built_in_decls[BUILT_IN_GTM_NEW];
	tree call = build_call_expr (decl, 0);
	tree stmt = build_gimple_modify_stmt (txn_handle, call);
	stmt = unshare_expr (stmt);
	bsi_insert_before (&bsi, stmt, m);
	
	return txn_handle;
      }
  }
  return NULL_TREE;
}
#endif

#ifndef TINYSTM_VERSION_0_9_0b1
/* Helper function to insert call to stm_init_thread. */
static void
insert_stm_init_thread (block_stmt_iterator bsi)
{
  tree decl = built_in_decls[BUILT_IN_GTM_INIT_THREAD];
  tree call = build_call_expr (decl, 0);
  call = unshare_expr (call);
  bsi_insert_before (&bsi, call, BSI_SAME_STMT);
}
	
/* Insert call to stm_init_thread at beginning of function. */
static void
maybe_insert_stm_init_thread (void)
{
  basic_block bb;

  FOR_EACH_BB (bb) 
  {
    int idx = bb->index;
    /* TODO find a better place to put this call  (works so far) */
    if (idx == 2) 
      {
	block_stmt_iterator bsi = bsi_start (bb);

	/* Avoid redundant calls to stm_init_thread */ 
	tree tmp = bsi_stmt (bsi);
	if (TREE_CODE (tmp) == CALL_EXPR) 
	  {
	    char *name = get_name (get_callee_fndecl (tmp));
#ifdef TINYSTM_VERSION_0_9_5
	    if (strncmp (name, "__builtin_stm_init_thread", 25) == 0)
	      return ;
#endif
#ifdef TANGER
	    if (strncmp (name, "__builtin_tanger_stm_thread_init", 32) == 0)
	      return ;
#endif
	  }
	insert_stm_init_thread (bsi);
	return ;
      }
  }
  return ;
}

/* Helper function inserting calls to stm_exit_thread. */
static void
insert_stm_exit_thread (block_stmt_iterator bsi, bool after)
{ 
  tree decl = built_in_decls[BUILT_IN_GTM_EXIT_THREAD];
  tree call = build_call_expr (decl, 0);
  call = unshare_expr (call);
 
  if (after)
    bsi_insert_after (&bsi, call, BSI_SAME_STMT);
  else
    bsi_insert_before (&bsi, call, BSI_SAME_STMT);
  
  return ;
}

/* Helper function inserting calls to stm_exit_thread at RET_EXPR. */
static void
insert_stm_exit_thread_ret_expr (block_stmt_iterator bsi_last_bb) 
{
  bool after = false;

  gcc_assert (!bsi_end_p (bsi_last_bb));
  tree ret_expr = bsi_stmt (bsi_last_bb);

  gcc_assert (ret_expr);

  if (TREE_CODE (ret_expr) == RETURN_EXPR) 
    {
      if  (!bsi_end_p (bsi_last_bb))
	{
	  block_stmt_iterator save = bsi_last_bb;
	  bsi_prev (&bsi_last_bb);
	  if (! bsi_end_p (bsi_last_bb))
	    {
	      after = true;
	      tree bef_ret = bsi_stmt (bsi_last_bb);
	      if (TREE_CODE (bef_ret) == CALL_EXPR) 
		{
		  char *name = get_name (get_callee_fndecl (bef_ret));
#ifdef TINYSTM_VERSION_0_9_5
		  if (strncmp (name, "__builtin_stm_exit_thread", 25) == 0)
		    return ;
#endif
#ifdef TANGER
		  if (strncmp (name, "__builtin_tanger_stm_thread_exit", 32) == 0)
		    return ;
#endif
		}
	    }
	  else 
	    {
	      bsi_last_bb = save;
	      after = false;
	    }
	  insert_stm_exit_thread (bsi_last_bb, after);
	}
    }
  else 
    {
      gcc_unreachable();
    }
  return ;
}

/* Insert call to stm_exit_thread at the end of function. */
static void
maybe_insert_stm_exit_thread(void) 
{
  basic_block last_bb;
  block_stmt_iterator bsi_last_bb;
  edge e;
  edge_iterator ei;

  FOR_EACH_EDGE (e, ei, EXIT_BLOCK_PTR->preds)
    {
      last_bb = e->src;
      bsi_last_bb = bsi_last (last_bb);
      insert_stm_exit_thread_ret_expr (bsi_last_bb);
    }
  return ;
}
#endif

/* Expand the begin of an transaction to 
   set up the transaction in an STM-specific way. */
static tree
expand_gtm_txn_marker (struct gtm_region *region, block_stmt_iterator *bsi, enum bsi_iterator_update m, basic_block recover_bb, basic_block begin_bb)
{
  tree decl;
  tree txn_handle;
  tree sigsetjmp, ssjval;
  tree mask;
  tree jmp_buf;
  tree get;
  tree stmt;
  tree list;
  tree recover_label;
  tree ptr_void = build_pointer_type (void_type_node);

  /* stm_txn_t *tx;
     tx = stm_new (); */
#ifdef GTM_EXPL_HANDLE 
  txn_handle = maybe_insert_stm_new (m);
#endif
  /* TODO check if this condition still holds for other STMs. */
#ifndef TINYSTM_VERSION_0_9_0b1 
  insert_stm_init_thread(*bsi);
  txn_handle = NULL_TREE;
#endif

  /* jmp_buf *e;
     e = stm_get_env (tx); */
  jmp_buf = create_tmp_var (ptr_void, "jmp_buf");
  decl = built_in_decls[BUILT_IN_GTM_GET_ENV];
  if (txn_handle != NULL_TREE)
    get = build_call_expr (decl, 1, txn_handle);
  else 
    get = build_call_expr (decl, 0);
  stmt = build_gimple_modify_stmt (jmp_buf, get);
  stmt = unshare_expr (stmt);
  bsi_insert_before (bsi, stmt, m);
  
  /* CHECK: if sigsetjmp is called with a non-zero
     argument it saves the signal mask
     as part of the environment. 
     Currently the setjmp call is done manually.
     TODO: Future work would be to use GCC's EH machinery. */
  ssjval = create_tmp_var (integer_type_node, "ssj_value");
  mask = build_int_cst (integer_type_node, 0);
  list = build_function_type_list (integer_type_node, ptr_type_node, NULL_TREE);
  decl = build_decl (FUNCTION_DECL, get_identifier ("_setjmp"), list);
  sigsetjmp = build_call_expr (decl, 1, jmp_buf);
  stmt = build_gimple_modify_stmt (ssjval, sigsetjmp);
  stmt = unshare_expr (stmt);
  bsi_insert_before (bsi, stmt, m);
  region->setjmp_stmt = stmt;

  recover_label = create_artificial_label ();

  stmt = build3 (COND_EXPR, void_type_node, 
		 fold_build2 (EQ_EXPR, boolean_type_node, ssjval, integer_zero_node), 
		 NULL_TREE,
		 NULL_TREE);
  stmt = unshare_expr (stmt);
  bsi_insert_after (bsi, stmt, m);
  
  setup_recover_bb (recover_bb, recover_label);
  setup_begin_bb (begin_bb, txn_handle, jmp_buf);  

  return txn_handle;
}

/* Helper function that emits call to 
   commit the transaction in an STM-specific way. */
static void 
expand_gtm_return (block_stmt_iterator *bsi, enum bsi_iterator_update m, tree txn_handle, bool end_txn)
{
  tree decl;
  tree call;
  tree stm_commit_retval;
  tree stmt;

  decl = built_in_decls[BUILT_IN_GTM_TXN_COMMIT];
  if (txn_handle != NULL_TREE)
    call = build_call_expr (decl, 1, txn_handle);
  else 
    call = build_call_expr (decl, 0);

  stm_commit_retval  = create_tmp_var (integer_type_node, "stm_commit");
  add_referenced_var (stm_commit_retval);

  stmt = fold_build2 (GIMPLE_MODIFY_STMT, integer_type_node,
		      stm_commit_retval, call);
  stmt = unshare_expr (stmt);
  bsi_insert_before (bsi, stmt, m);

#ifdef  TINYSTM_VERSION_0_9_5 
  if (end_txn)
    maybe_insert_stm_exit_thread(); 
#endif

#ifdef TANGER 
  if (end_txn)
    insert_stm_exit_thread(*bsi, true); 
#endif

  return ;
}

/* Insert call to stm_init at beginning of main function. */
static void
insert_stm_init(void) 
{
  /* TODO allow other functions here, since only C has a mandatory "main" entry point. */
  char *name = get_name (current_function_decl);
  if ((strncmp(name, "main", 4) != 0)
      || (strlen(name) > 4))
    return ;

  basic_block bb;

  FOR_EACH_BB (bb) 
  {
    int idx = bb->index;
    /* TODO find a better place to put this call  (works so far) */
    if (idx == 2) 
      {
	block_stmt_iterator bsi = bsi_start (bb);
	tree decl;
	tree call;
	
	decl = built_in_decls[BUILT_IN_GTM_INIT];
	call = build_call_expr (decl, 0);
	call = unshare_expr (call);

	bsi_insert_before (&bsi, call, BSI_SAME_STMT);
	return ;
      }
  }
  
  gcc_unreachable();
  return ;
}

/* Insert call to stm_exit at the end of main function. */
static void
insert_stm_exit(void) 
{
  /* TODO allow other functions here, since only C has a mandatory "main" entry point. */
  char *name = get_name (current_function_decl);
  if ((strlen(name) > 4)
      || (strncmp(name, "main", 4) != 0))
    return ;

  tree decl;
  tree call;
  
  basic_block last_bb;
  block_stmt_iterator bsi_last_bb;
  
  edge e;
  edge_iterator ei;
  
  FOR_EACH_EDGE (e, ei, EXIT_BLOCK_PTR->preds)
    {
      last_bb = e->src;
      bsi_last_bb = bsi_last (last_bb);
      gcc_assert (!bsi_end_p (bsi_last_bb));
      
      tree tmp = bsi_stmt (bsi_last_bb);  
      if (TREE_CODE (tmp) == RETURN_EXPR) 
	{
	  decl = built_in_decls[BUILT_IN_GTM_EXIT];
	  call = build_call_expr (decl, 0);
	  call = unshare_expr (call);
	  bsi_insert_before (&bsi_last_bb, call, BSI_SAME_STMT);
	}
      else 
	{
	  gcc_unreachable();
	}
    }
  return ;
}

/* Initialize global data structures.*/
void
init_label_table (void) 
{
  unsigned int i;

  label_index = 0;
  e_index = 0;

  for (i = 0; i < NUM_BB_TXN; i++)
    edges_to_instrument[i] = NULL;

  return ;
}

/* Helper function marking all basic blocks 
   of a transaction. */
void
record_bb_into_table (struct gtm_region *region, basic_block bb)
{
  if (dump_file)
    fprintf (dump_file, "Record TXN Basic block %d\n", bb->index);
  region->txn_bbs[label_index] = bb->index;
  label_index++;
  
  return ;
}

/* Helper function that checks whether an edge from a 
   basic block leaves the scope of a transaction. If 
   it does the edge is marked for instrumentation. */
void
check_and_mark_edges (struct gtm_region *region, basic_block bb) 
{
  edge_iterator ei;
  edge e;
  unsigned int j;
  basic_block succ;
  bool found = false;

  FOR_EACH_EDGE (e, ei, bb->succs)
    {
      succ = e->dest;

      for (j = 0; j < label_index; j++)
	{
	  if (region->txn_bbs[j] == succ->index)
	    {
	      if (dump_file)
		fprintf (dump_file, "found BB index %d in TXN\n", succ->index);
	      found = true;
	      break;
	    }
	}
  
      if ((!found)
	  && (succ != ENTRY_BLOCK_PTR)
	  && (succ != EXIT_BLOCK_PTR))
	{
	  if (dump_file)
	    fprintf (dump_file, "Did _NOT_ find BB index %d in TXN\n", succ->index);
	  
	  edges_to_instrument[e_index] = e;
	  e_index++;
	}
    }

  return ;
}

/* Instrument all previously recorded edges with 
   a call to stm_commit. */
void 
instrument_edges (tree txn_handle)
{
  unsigned int j;
  edge e;

  for (j = 0; j < e_index ; j++)
    {
      e = edges_to_instrument[j];
      gcc_assert (e);
      
      basic_block stm_commit_bb = split_edge (e);
      block_stmt_iterator bsi = bsi_start (stm_commit_bb);
      expand_gtm_return (&bsi, BSI_SAME_STMT, txn_handle, false);
      gcc_assert (single_succ_edge (stm_commit_bb)->flags = EDGE_FALLTHRU);
    }

  return ;
}

/* Instruments a return expression with a call to stm_commit. */
void
instrument_return_expr (block_stmt_iterator *bsi, tree txn_handle)
{
  tree stmt = bsi_stmt (*bsi);

  if (TREE_CODE (stmt) == RETURN_EXPR)
    {
      expand_gtm_return (bsi, BSI_SAME_STMT, txn_handle, false);
    }
  
  return ;
}

/* Instruments transactions with calls to the STM runtime. 
   Inserts new basic blocks at the beginning of the transaction
   to allow for variable saving and restoring (checkpointing) 
   on SSA level. */
static void
expand_gtm_txn (struct gtm_region *region)
{
  basic_block entry_bb, exit_bb, bb;
  block_stmt_iterator bsi;
  
  tree txn;
  tree txn_handle;
  
  entry_bb = region->entry;
  exit_bb = region->exit;

  gcc_assert (entry_bb);
  gcc_assert (exit_bb);

  bsi = bsi_last (entry_bb);
  txn = bsi_stmt (bsi);
  
  gcc_assert (TREE_CODE (txn) == GTM_TXN);
  init_label_table ();

  /* Split basic blocks and make edges: */
  /* BB with setjmp (entry_bb) -> BBrestore 
     BB with setjmp (entry_bb) -> BBtxnbegin
     BBrestore -> BBtxnbegin
  */
  basic_block begin_bb = split_edge (single_succ_edge (entry_bb)); 
  basic_block recover_bb = split_edge (single_succ_edge (entry_bb));

  /* TODO redirect edge instead of destroy/create */
  /* Content to the basic blocks is added here. */
  txn_handle = expand_gtm_txn_marker (region, &bsi, BSI_SAME_STMT, recover_bb, begin_bb);
  bsi_remove (&bsi, true);
  remove_edge (single_succ_edge (entry_bb));
  make_edge (entry_bb, recover_bb, EDGE_FALSE_VALUE);
  make_edge (entry_bb, begin_bb, EDGE_TRUE_VALUE);

  single_succ_edge (begin_bb)->flags = EDGE_FALLTHRU;
  remove_edge (single_succ_edge (recover_bb));
  make_edge (recover_bb, begin_bb, EDGE_FALLTHRU);

  gcc_assert (exit_bb->next_bb);

  FOR_BB_BETWEEN (bb, begin_bb, exit_bb->next_bb, next_bb)
    {
      if (!bb) 
	continue;

      record_bb_into_table (region, bb);

      for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
	{
	  instrument_return_expr (&bsi, txn_handle);
	  replace_txn_stmt (&bsi, BSI_SAME_STMT, txn_handle);
	}
    }
  
  FOR_BB_BETWEEN (bb, begin_bb, exit_bb, next_bb)
    {
      if (!bb) 
	continue;

      check_and_mark_edges (region, bb);
    }

  instrument_edges (txn_handle);

  bsi = bsi_last (exit_bb);
  gcc_assert (!bsi_end_p (bsi)
	      && TREE_CODE (bsi_stmt (bsi)) == GTM_RETURN);
  
  expand_gtm_return (&bsi, BSI_SAME_STMT, txn_handle, true);
  bsi_remove (&bsi, true);

  return;
}

/* Instrument the GTM region tree rooted at REGION.  
   We start with the inner regions first. */
/* TODO adjust for nested transactions! */
static void
expand_gtm (struct gtm_region *region)
{
  while (region)
    {
      /* First, expand the inner regions.  */
      if (region->inner)
	expand_gtm (region->inner);

      gcc_assert ((region->type) == GTM_TXN);
      expand_gtm_txn (region);
      region = region->next;
    }
}

/* TM function cloning: helper function that helps to 
   annotate function body of GTM clone. 
   Transactions from the original version are removed. */
static void
annotate_gtm_function_body (void)
{
  tree txn_handle;
  /* By construction txn_handle is last parameter of the
     argument list of the function. */
#ifdef GTM_EXPL_HANDLE
  txn_handle = tree_last (DECL_ARGUMENTS (current_function_decl));
#endif
 
#ifdef TINYSTM_VERSION_0_9_5
  txn_handle = NULL_TREE;
#endif

  basic_block bb;
  block_stmt_iterator bsi;
  tree stmt;

  FOR_EACH_BB (bb)
  {
    for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
      {
	stmt = bsi_stmt (bsi);
	if (GTM_DIRECTIVE_P (stmt))
	  {
	    bsi_remove (&bsi, true);
	    if (bsi_end_p (bsi)) break;
	  }
	else
	  replace_txn_stmt (&bsi, BSI_SAME_STMT, txn_handle);
      }
  }
}

/* Main entry point for expanding GTM-GIMPLE 
   into runtime calls to the STM. */
static unsigned int
execute_expand_gtm (void)
{
  bool flat_nesting;
  
  /* In case we have to instrument 
     a transactional clone. */
  if (DECL_IS_GTM_CLONE (current_function_decl))
    {
      if (!DECL_IS_GTM_PURE (current_function_decl))
	annotate_gtm_function_body();
      else 
	{
	  /* Function with  tm_pure attribute specified. */
	  build_gtm_regions ();
	  if (root_gtm_region)
	    {
	      /* In case a tm_pure function attribute is combined with 
		 transactions, we end up here. Issue a warning and 
		 remove GTM markers. Since there is no standard how to 
		 treat this case, this may have to be changed. */
	      warning (0, "GTM: ignored %qs used in function %qs declared as %qs.", 
		       "#pragma tm atomic", 
		       get_name (current_function_decl), 
		       "tm_pure");
	      collapse_gtm_regions (root_gtm_region);
	      remove_gtm_stmts (root_gtm_region);
	      free (root_gtm_region);
	      root_gtm_region = NULL;
	    }
	}
      return 0;
    }
  
  build_gtm_regions ();
  if (!root_gtm_region)
    {
      /* The reason for the following calls is that a 
	 program maybe transactified without using atomic regions 
	 inside the main-function. Here calls are inserted to ensure 
	 the proper initialisation of the STM library. */
      insert_stm_init();
      insert_stm_exit(); 
      return 0;
    }
 
  if (dump_file)
    {
      fprintf (dump_file, "\nGTM region tree\n\n");
      dump_gtm_region (dump_file, root_gtm_region, 0);
      fprintf (dump_file, "\n");
    }

  flat_nesting = query_STM_for_flat_nesting();

  if (flat_nesting) 
    {
      collapse_gtm_regions (root_gtm_region);

      if (dump_file)
	{
	  fprintf (dump_file, "\nGTM region tree after collapsing regions\n\n");
	  dump_gtm_region (dump_file, root_gtm_region, 0);
	  fprintf (dump_file, "\n");
	}
    }
  
  expand_gtm (root_gtm_region);  /* instrumentation is done here. */

  insert_stm_init();
  insert_stm_exit(); 

  free_dominance_info (CDI_DOMINATORS);
  
  cleanup_tree_cfg ();

  return 0;
}

/* GTM expansion -- the default pass, 
   run before creation of SSA form.  */
static bool
gate_expand_gtm (void)
{
  return flag_gtm;
}

struct tree_opt_pass pass_expand_gtm = 
  {
    "gtmexp",				/* name */
    gate_expand_gtm,			/* gate */
    execute_expand_gtm,			/* execute */
    NULL,				/* sub */
    NULL,				/* next */
    0,					/* static_pass_number */
    0,					/* tv_id */
    PROP_gimple_any,			/* properties_required */
    0,			                /* properties_provided */
    0,					/* properties_destroyed */
    TODO_dump_func,			/* todo_flags_start */
    TODO_cleanup_cfg 
    | TODO_ggc_collect,		        /* todo_flags_finish */
    0					/* letter */
  };

/* Calculate live ranges on SSA. Then checkpoint the 
   live-in variables to the transaction. */
void
checkpoint_live_in_variables (struct gtm_region *region, block_stmt_iterator *bsi_recover, basic_block begin_bb) 
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

  return ;
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
    return ;
  
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




