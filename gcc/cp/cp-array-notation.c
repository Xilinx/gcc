/* This file is part of the Intel(R) Cilk(TM) Plus support
   It contains routines to handle Array Notation expression
   handling routines in the C++ Compiler.
   Copyright (C) 2011-2013  Free Software Foundation, Inc.
   Contributed by Balaji V. Iyer <balaji.v.iyer@intel.com>,
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
#include "cp-tree.h"
#include "diagnostic.h"
#include "tree-iterator.h"
#include "vec.h"
#include "gimple.h"

void replace_array_notations (tree *, bool, vec<tree, va_gc> *, 
			      vec<tree, va_gc> *);
void find_rank (tree, bool, size_t *);
static tree fix_conditional_array_notations_1 (tree);
tree fix_unary_array_notation_exprs (tree stmt);
bool is_builtin_array_notation_fn (tree, an_reduce_type *);
bool contains_array_notation_expr (tree);
extern bool is_sec_implicit_index_fn (tree);
extern int extract_sec_implicit_index_arg (location_t, tree fn);
void extract_array_notation_exprs (tree, bool, vec<tree, va_gc> **);
static bool has_call_expr_with_array_notation (tree expr);
static tree fix_builtin_array_notation_fn (tree an_builtin_fn, tree *new_var);
tree find_correct_array_notation_type (tree op);
static tree fix_return_expr (tree expr);

struct inv_list
{
  vec<tree, va_gc> *list_values;
  vec<tree, va_gc> *replacement;
};


int array_notation_label_no;

/* Returns the rank of ARRAY through the *RANK.  The user can specify whether
   (s)he wants to step into array_notation-specific builtin functions
   (specified by the IGNORE_BUILTIN_FN).

   For example, an array notation of A[:][:] or B[0:10][0:5:2] or C[5][:][1:0]
   all have a rank of 2.  */

void
find_rank (tree array, bool ignore_builtin_fn, size_t *rank)
{
  tree ii_tree;
  size_t current_rank = 0, ii = 0;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
  if (!array)
    return;
  else if (TREE_CODE (array) == ARRAY_NOTATION_REF)
    {
      ii_tree = array;
      while (ii_tree)
	{
	  if (TREE_CODE (ii_tree) == ARRAY_NOTATION_REF)
	    {
	      current_rank++;
	      ii_tree = ARRAY_NOTATION_ARRAY (ii_tree);
	    }
	  else if (TREE_CODE (ii_tree) == ARRAY_REF)
	    ii_tree = TREE_OPERAND (ii_tree, 0);
	  else if (TREE_CODE (ii_tree) == PARM_DECL
		   || TREE_CODE (ii_tree) == VAR_DECL)
	    /* When VAR_DECL or PARM_DECL has reached, it signifies the base of
	       the node.  It is not necessary to go any further.  */
	    break;
	}
      if (*rank == 0)
	*rank = current_rank;
    }
  else if (TREE_CODE (array) == STATEMENT_LIST)
    {
      tree_stmt_iterator ii_tsi;
      for (ii_tsi = tsi_start (array); !tsi_end_p (ii_tsi);
	   tsi_next (&ii_tsi))
	find_rank (*tsi_stmt_ptr (ii_tsi), ignore_builtin_fn, rank);
    }
  else if (TREE_CODE (array) == TREE_LIST)
    {
      find_rank (TREE_PURPOSE (array), ignore_builtin_fn, rank);
      find_rank (TREE_VALUE (array), ignore_builtin_fn, rank);
      find_rank (TREE_CHAIN (array), ignore_builtin_fn, rank);
    }
  else if (TREE_CODE (array) == DECL_EXPR)
    {
      tree x = DECL_EXPR_DECL (array);
      find_rank (x, ignore_builtin_fn, rank);
      
      if (TREE_CODE (x) != FUNCTION_DECL)
	if (DECL_INITIAL (x))
	  find_rank (DECL_INITIAL (x), ignore_builtin_fn, rank);
    }	  
  else
    {
      if (TREE_CODE (array) == CALL_EXPR)
	{
	  tree func_name = CALL_EXPR_FN (array);
	  if (TREE_CODE (func_name) == ADDR_EXPR ||
	      TREE_CODE (func_name) == FUNCTION_DECL)
	    if (ignore_builtin_fn)
	      if (is_builtin_array_notation_fn (func_name, &dummy_type))
		/* If it is a builtin function, then we know it returns a
		   scalar.  */
		return;
	  if (TREE_CODE (TREE_OPERAND (array, 0)) == INTEGER_CST)
	    {
	      size_t length = TREE_INT_CST_LOW (TREE_OPERAND (array, 0));
	      for (ii = 0; ii < length; ii++)
		find_rank (TREE_OPERAND (array, ii), ignore_builtin_fn, rank);
	    }
	  else
	    gcc_unreachable ();
	}
      else
	{
	  for (ii = 0; ii < TREE_CODE_LENGTH (TREE_CODE (array)); ii++)
	    find_rank (TREE_OPERAND (array, ii), ignore_builtin_fn, rank);
	}
    }
  return;
}



/* Extracts all the array notations specified in NODE and stores them in a
   dynamic tree array of ARRAY_LIST whose size is stored in *LIST_SIZE.  The
   user can specify if (s)he wants to ignore the array notations inside the
   array-notation specific builtin functions (by setting IGNORE_BUILTIN_FN to
   true).  */

void
extract_array_notation_exprs (tree node, bool ignore_builtin_fn,
			      vec<tree, va_gc> **array_list)
{
  size_t ii = 0;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
  
  if (!node)
    return;
  else if (TREE_CODE (node) == ARRAY_NOTATION_REF)
    {
      vec_safe_push (*array_list, node);
      return;
    }
  else if (TREE_CODE (node) == BIND_EXPR)
    extract_array_notation_exprs (BIND_EXPR_BODY (node), ignore_builtin_fn,
				  array_list);
  else if (TREE_CODE (node) == TREE_LIST)
    {
      extract_array_notation_exprs (TREE_PURPOSE (node), ignore_builtin_fn,
				    array_list);
      extract_array_notation_exprs (TREE_VALUE (node), ignore_builtin_fn,
				    array_list);
      extract_array_notation_exprs (TREE_CHAIN (node), ignore_builtin_fn,
				    array_list);
    }
  else if (TREE_CODE (node) == STATEMENT_LIST)
    {
      tree_stmt_iterator ii_tsi;
      for (ii_tsi = tsi_start (node); !tsi_end_p (ii_tsi); tsi_next (&ii_tsi))
	extract_array_notation_exprs (*tsi_stmt_ptr (ii_tsi),
				      ignore_builtin_fn, array_list);
    }
  else if (TREE_CODE (node) == CALL_EXPR)
    {
      if (is_builtin_array_notation_fn (CALL_EXPR_FN (node), &dummy_type))
	{
	  if (ignore_builtin_fn)
	    return;
	  else
	    {
	      vec_safe_push (*array_list, node);
	      return;
	    }
	}
      if (is_sec_implicit_index_fn (CALL_EXPR_FN (node)))
	{
	  vec_safe_push (*array_list, node);
	  return;
	}
      if (TREE_CODE (TREE_OPERAND (node, 0)) == INTEGER_CST)
	{
	  int length = TREE_INT_CST_LOW (TREE_OPERAND (node, 0));

	  for (ii = 0; ii < (size_t) length; ii++)
	    extract_array_notation_exprs
	      (TREE_OPERAND (node, ii), ignore_builtin_fn, array_list);
	}
      else
	gcc_unreachable (); /* We should not get here.  */
	  
    } 
  else 
    for (ii = 0; ii < TREE_CODE_LENGTH (TREE_CODE (node)); ii++) 
      extract_array_notation_exprs (TREE_OPERAND (node, ii), 
				    ignore_builtin_fn, array_list);
  return;
}


/* Replaces all occurances of array notations in tree ORIG that matches the
   ones in LIST with the one in ARRAY_OPERAND.  The size of list and
   ARRAY_OPERAND is ARRAY_SIZE.  For example, ARRAY_OPERAND[x] for some index
   'x' will have the equivalent ARRAY_REF for the ARRAY_NOTATION_REF specified
   in LIST[x].   The  user can specify if (s)he wants to ignore the array
   notations inside the array-notation specific builtin functions (using the
   bool variable IGNORE_BUILTIN_FN).  */

void
replace_array_notations (tree *orig, bool ignore_builtin_fn,
			 vec<tree, va_gc> *list,
			 vec<tree, va_gc> *array_operand)
{
  size_t ii = 0;
  tree node = NULL_TREE, node_replacement = NULL_TREE;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
  
  if (vec_safe_length (list) == 0 || !*orig)
    return;

  if (TREE_CODE (*orig) == ARRAY_NOTATION_REF)
    {
      for (ii = 0; vec_safe_iterate (list, ii, &node); ii++) 
	if (*orig == node)
	  {
	    node_replacement = (*array_operand)[ii];
	    *orig = node_replacement;
	  }
    }
  else if (TREE_CODE (*orig) == STATEMENT_LIST)
    {
      tree_stmt_iterator ii_tsi;
      for (ii_tsi = tsi_start (*orig); !tsi_end_p (ii_tsi); tsi_next (&ii_tsi))
	replace_array_notations (tsi_stmt_ptr (ii_tsi), ignore_builtin_fn, list,
				 array_operand);
    }
  else if (TREE_CODE (*orig) == CALL_EXPR)
    {
      if (is_builtin_array_notation_fn (CALL_EXPR_FN (*orig), &dummy_type))
	{
	  if (!ignore_builtin_fn)
	    {
	      for (ii = 0; vec_safe_iterate (list, ii, &node); ii++) 
		if (*orig == node)
		  {
		    node_replacement = (*array_operand)[ii];
		    *orig = node_replacement;
		  }
	    }
	  return;
	}
      if (is_sec_implicit_index_fn (CALL_EXPR_FN (*orig)))
	{
	  for (ii = 0; vec_safe_iterate (list, ii, &node); ii++)
	    if (*orig == node)
	      {
		node_replacement = (*array_operand)[ii];
		*orig = build_c_cast (EXPR_LOCATION (*orig), integer_type_node,
				      node_replacement);
	      }
	  return;
	}
      if (TREE_CODE (TREE_OPERAND (*orig, 0)) == INTEGER_CST)
	{
	  int length = TREE_INT_CST_LOW (TREE_OPERAND (*orig, 0));
	  for (ii = 0; ii < (size_t) length; ii++)
	    replace_array_notations (&TREE_OPERAND (*orig, ii),
				     ignore_builtin_fn, list, array_operand);
	}
      else
	gcc_unreachable (); /* We should not get here!  */
    }
  else
    {
      for (ii = 0; ii < (size_t) TREE_CODE_LENGTH (TREE_CODE (*orig)); ii++) 
	replace_array_notations (&TREE_OPERAND (*orig, ii), ignore_builtin_fn, 
				 list, array_operand);
    }
  return;
}

/* This function will find all the scalar expressions in *TP and push it in
   DATA struct, typecasted to (void *).  If *WALK_SUBTREES is set to 0 then
   we have do not go into the *TP's subtrees.  */

static tree
find_inv_trees (tree *tp, int *walk_subtrees, void *data)
{
  struct inv_list *i_list = (struct inv_list *) data;

  if (!tp || !*tp)
    return NULL_TREE;
  if (TREE_CONSTANT (*tp))
    return NULL_TREE; /* No need to save constant to a variable.  */
  if (TREE_CODE (*tp) != COMPOUND_EXPR
      && !contains_array_notation_expr (*tp))
    {
      vec_safe_push (i_list->list_values, *tp);
      *walk_subtrees = 0;
    }
  else if (TREE_CODE (*tp) == ARRAY_NOTATION_REF
	   || TREE_CODE (*tp) == ARRAY_REF
	   || TREE_CODE (*tp) == TARGET_EXPR /* We skip target expressions.  */
	   || TREE_CODE (*tp) == CALL_EXPR)
    /* No need to step through the internals of array notation.  */
    *walk_subtrees = 0;
  else
    *walk_subtrees = 1;
  return NULL_TREE;
}

/* Replace all the scalar expressions in *TP with the appropriate replacement
   stored in the struct *DATA (typecasted to void*).  The subtrees are not
   touched if *WALK_SUBTREES is set to zero.  */

static tree
replace_inv_trees (tree *tp, int *walk_subtrees, void *data)
{
  size_t ii = 0;
  tree t, r;
  struct inv_list *i_list = (struct inv_list *) data;

  if (vec_safe_length (i_list->list_values))
    {
      for (ii = 0; vec_safe_iterate (i_list->list_values, ii, &t); ii++)
	{
	  if (simple_cst_equal (*tp, t) == 1)
	    {
	      vec_safe_iterate (i_list->replacement, ii, &r);
	      gcc_assert (r != NULL_TREE);
	      *tp = r;
	      *walk_subtrees = 0;
	    }
	}
    }
  else
    *walk_subtrees = 0;

  return NULL_TREE;
}

/* Replaces all the scalar expressions in *NODE. */

tree
replace_invariant_exprs (tree *node)
{
  size_t ix = 0;
  tree node_list = NULL_TREE;
  tree t = NULL_TREE, new_var = NULL_TREE, new_node; 
  struct inv_list data;
  if (!node || !*node)
    return NULL_TREE;

  data.list_values = NULL;
  data.replacement = NULL;
  cp_walk_tree (node, find_inv_trees, (void *) &data, NULL);

  if (vec_safe_length (data.list_values))
    {
      node_list = push_stmt_list ();
      for (ix = 0; vec_safe_iterate (data.list_values, ix, &t); ix++)
	{
	  if (processing_template_decl || !TREE_TYPE (t))
	    new_var = build_min_nt_loc (EXPR_LOCATION (t), VAR_DECL, NULL_TREE,
					NULL_TREE);
	  else
	    new_var = build_decl (EXPR_LOCATION (t), VAR_DECL, NULL_TREE,
				  TREE_TYPE (t));
	  gcc_assert (new_var != NULL_TREE && new_var != error_mark_node);
	  new_node = build_x_modify_expr (EXPR_LOCATION (t), new_var, NOP_EXPR,
					  t, tf_warning_or_error);
	  add_stmt (new_node);
	  vec_safe_push (data.replacement, new_var);
	}
      cp_walk_tree (node, replace_inv_trees, (void *)&data, NULL);
      node_list = pop_stmt_list (node_list);
    }
  return node_list;
}


/* Returns a loop with ARRAY_REF inside it with an appropriate modify expr.
   The LHS and/or RHS will be array notation expressions that have a
   MODIFYCODE.  The location of the variable is specified by LOCATION.  */

tree
build_x_array_notation_expr (location_t location, tree lhs,
			     enum tree_code modifycode, tree rhs,
			     tsubst_flags_t complain)
{
  bool **lhs_vector = NULL, **rhs_vector = NULL;
  tree **lhs_array = NULL, **rhs_array = NULL;
  tree array_expr_lhs = NULL_TREE, array_expr_rhs = NULL_TREE;
  tree array_expr = NULL_TREE;
  tree **lhs_value = NULL, **rhs_value = NULL;
  tree **lhs_stride = NULL, **lhs_length = NULL, **lhs_start = NULL;
  tree **rhs_stride = NULL, **rhs_length = NULL, **rhs_start = NULL;
  tree loop = NULL_TREE, *lhs_var = NULL, *rhs_var = NULL;
  tree *body_label = NULL, *body_label_expr = NULL;
  tree *exit_label = NULL, *exit_label_expr = NULL, *cond_expr = NULL;
  tree *if_stmt_label = NULL;
  tree *lhs_expr_incr = NULL, *rhs_expr_incr = NULL;
  tree *lhs_ind_init = NULL, *rhs_ind_init = NULL;
  bool **lhs_count_down = NULL, **rhs_count_down = NULL;
  tree *lhs_compare = NULL, *rhs_compare = NULL;
  vec<tree, va_gc> *lhs_array_operand = NULL, *rhs_array_operand = NULL;
  size_t lhs_rank = 0, rhs_rank = 0, ii = 0, jj = 0;
  tree ii_tree = NULL_TREE, comp_stmt;
  vec<tree, va_gc> *rhs_list = NULL, *lhs_list = NULL;
  size_t rhs_list_size = 0, lhs_list_size = 0;
  tree new_modify_expr, new_var, builtin_loop, scalar_mods;
  tree begin_var, lngth_var, strde_var;
  bool found_builtin_fn = false;
  char label_name[50];
  int s_jj = 0;

  /* In the first part, we try to break up the builtin functions for array
     notations.  */
  find_rank (rhs, false, &rhs_rank);
  extract_array_notation_exprs (rhs, false, &rhs_list);
  rhs_list_size = vec_safe_length (rhs_list);
  loop = push_stmt_list ();
    
  scalar_mods = replace_invariant_exprs (&rhs);
  if (scalar_mods)
    add_stmt (scalar_mods);

    /* Here we assign the array notation components to variable so that we can
     satisfy the exec once rule.  */
  for (ii = 0; ii < lhs_list_size; ii++)
    {
      tree array_node = (*lhs_list)[ii];
      tree array_begin = ARRAY_NOTATION_START (array_node);
      tree array_lngth = ARRAY_NOTATION_LENGTH (array_node);
      tree array_strde = ARRAY_NOTATION_STRIDE (array_node);
      
      begin_var = build_decl (location, VAR_DECL, NULL_TREE,
			      integer_type_node);
      lngth_var = build_decl (location, VAR_DECL, NULL_TREE,
			      integer_type_node);
      strde_var = build_decl (location, VAR_DECL, NULL_TREE,
			      integer_type_node);

      add_stmt (build_modify_expr (location, begin_var, TREE_TYPE (begin_var),
				   NOP_EXPR, location, array_begin,
				   TREE_TYPE (array_begin)));
      add_stmt (build_modify_expr (location, lngth_var, TREE_TYPE (lngth_var),
				   NOP_EXPR, location, array_lngth,
				   TREE_TYPE (array_lngth)));
      add_stmt (build_modify_expr (location, strde_var, TREE_TYPE (strde_var),
				   NOP_EXPR, location, array_strde,
				   TREE_TYPE (array_strde)));
      
      ARRAY_NOTATION_START (array_node) = begin_var;
      ARRAY_NOTATION_LENGTH (array_node) = lngth_var;
      ARRAY_NOTATION_STRIDE (array_node) = strde_var;
    }
  for (ii = 0; ii < rhs_list_size; ii++)
    {
      tree rhs_node = (*rhs_list)[ii];
      if (TREE_CODE (rhs_node) == CALL_EXPR)
	{
	  builtin_loop = fix_builtin_array_notation_fn (rhs_node, &new_var);
	  if (builtin_loop == error_mark_node)
	    return error_mark_node;
	  else if (builtin_loop)
	    {
	      add_stmt (builtin_loop);
	      found_builtin_fn = true;
	      if (new_var)
		{
		  vec <tree, va_gc> *rhs_sub_list = NULL, *new_var_list = NULL;
		  vec_safe_push (rhs_sub_list, rhs_node);
		  vec_safe_push (new_var_list, new_var);
		  replace_array_notations (&rhs, false, rhs_sub_list,
					   new_var_list);
		}
	    }
	}
    }
  
  lhs_rank = 0;
  rhs_rank = 0;
  find_rank (lhs, true, &lhs_rank);
  find_rank (rhs, true, &rhs_rank);

  /* If both are scalar, then the only reason why we will get this far is if
     there is some array notations inside it and was using a builtin array
     notation functions.  If so, we have already broken those guys up and now 
     a simple build_x_modify_expr would do.  */
  if (lhs_rank == 0 && rhs_rank == 0)
    {
      if (found_builtin_fn)
	{
	  new_modify_expr = build_x_modify_expr (location, lhs,
						 modifycode, rhs, complain);
	  add_stmt (finish_expr_stmt (new_modify_expr));
	  pop_stmt_list (loop);
	  return loop;
	}
      else
	{
	  /* Generally, we should not get here at all.  */
	  pop_stmt_list (loop);
	  return NULL_TREE;
	}
    }

  /* We need this when we have a scatter issue.  */
  extract_array_notation_exprs (lhs, true, &lhs_list);
  rhs_list = NULL;
  extract_array_notation_exprs (rhs, true, &rhs_list);
  rhs_list_size = vec_safe_length (rhs_list);
  lhs_list_size = vec_safe_length (lhs_list);
    
  if (lhs_rank == 0 && rhs_rank != 0)
    {
      tree rhs_base = rhs;
      if (TREE_CODE (rhs_base ) == COMPOUND_EXPR)
	rhs_base = TREE_OPERAND (rhs_base, 0);

      if (TREE_CODE (rhs_base) == TARGET_EXPR)
	rhs_base = TARGET_EXPR_INITIAL (rhs_base);

      if (TREE_CODE (rhs_base) != CALL_EXPR
	  && !has_call_expr_with_array_notation (rhs_base))
	{
	  for (ii = 0; ii < rhs_rank; ii++)
	    rhs_base = ARRAY_NOTATION_ARRAY (rhs_base);
      
	  if (location == UNKNOWN_LOCATION && EXPR_HAS_LOCATION (rhs))
	    location = EXPR_LOCATION (rhs);
	  error_at (location, "%qD cannot be scalar when %qD is not", lhs,
		    rhs_base);
	  return error_mark_node;
	}
    }
  if (lhs_rank != 0 && rhs_rank != 0 && lhs_rank != rhs_rank)
    {
      tree lhs_base = lhs;
      tree rhs_base = rhs;

      for (ii = 0; ii < lhs_rank; ii++)
	lhs_base = ARRAY_NOTATION_ARRAY (lhs_base);
      for (ii = 0; ii < rhs_rank; ii++)
	rhs_base = ARRAY_NOTATION_ARRAY (rhs_base);

      if (location == UNKNOWN_LOCATION && EXPR_HAS_LOCATION (lhs))
	location = EXPR_LOCATION (lhs);
      error_at (location, "rank mismatch between %qD and %qD", lhs_base,
		rhs_base);
      return error_mark_node;
    }

  lhs_vector = XNEWVEC (bool *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_vector[ii] = XNEWVEC (bool, lhs_rank);
  
  rhs_vector = XNEWVEC (bool *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_vector[ii] = XNEWVEC (bool, rhs_rank);

  lhs_array = XNEWVEC (tree *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_array[ii] = XNEWVEC (tree, lhs_rank);
  
  rhs_array = XNEWVEC (tree *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_array[ii] = XNEWVEC (tree, rhs_rank);

  lhs_value = XNEWVEC (tree *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_value[ii] = XNEWVEC (tree, lhs_rank);
  
  rhs_value = XNEWVEC (tree *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_value[ii] = XNEWVEC (tree, rhs_rank);

  lhs_stride = XNEWVEC (tree *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_stride[ii] = XNEWVEC (tree, lhs_rank);

  rhs_stride = XNEWVEC (tree *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_stride[ii] = XNEWVEC (tree, rhs_rank);

  lhs_length = XNEWVEC (tree *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_length[ii] = XNEWVEC (tree, lhs_rank);
  
  rhs_length = XNEWVEC (tree *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_length[ii] = XNEWVEC (tree, rhs_rank);

  lhs_start = XNEWVEC (tree *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_start[ii] = XNEWVEC (tree, lhs_rank);
  
  rhs_start = XNEWVEC (tree *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_start[ii] = XNEWVEC (tree, rhs_rank);
   
  lhs_var = XNEWVEC (tree, lhs_rank);
  rhs_var = XNEWVEC (tree, rhs_rank);
  

  /* The reason why we are just using lhs_rank for this is because we have then
     following scenarios:
     1.  LHS_RANK == RHS_RANK
     2.  LHS_RANK != RHS_RANK && RHS_RANK = 0

     In both the scenarios, just checking the LHS_RANK is OK.  */
  body_label = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  body_label_expr = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  exit_label = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  exit_label_expr = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  cond_expr = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  if_stmt_label = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));

  lhs_expr_incr = XNEWVEC (tree, lhs_rank);
  rhs_expr_incr = XNEWVEC (tree, rhs_rank);

  lhs_ind_init = XNEWVEC (tree, lhs_rank);
  rhs_ind_init = XNEWVEC (tree, rhs_rank);

  lhs_count_down = XNEWVEC (bool *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_count_down[ii] = XNEWVEC (bool, lhs_rank);
  
  rhs_count_down = XNEWVEC (bool *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_count_down[ii] = XNEWVEC (bool, rhs_rank);

  lhs_compare = XNEWVEC (tree, lhs_rank);
  rhs_compare = XNEWVEC (tree, rhs_rank);

  if (lhs_rank)
    {
      for (ii = 0; ii < lhs_list_size; ii++)
	for (jj = 0; jj < lhs_rank; jj++)
	  lhs_array[ii][jj] = NULL_TREE;

      for (ii = 0; ii < lhs_list_size; ii++)
	{
	  jj = 0;
	  ii_tree = (*lhs_list)[ii];
	  while (ii_tree)
	    {
	      if (TREE_CODE (ii_tree) == ARRAY_NOTATION_REF)
		{
		  lhs_array[ii][jj] = ii_tree;
		  jj++;
		  ii_tree = ARRAY_NOTATION_ARRAY (ii_tree);
		}
	      else if (TREE_CODE (ii_tree) == ARRAY_REF)
		ii_tree = TREE_OPERAND (ii_tree, 0);
	      else if (TREE_CODE (ii_tree) == VAR_DECL
		       || TREE_CODE (ii_tree) == PARM_DECL)
		break;
	    }
	}
    }
  else
    lhs_array[0][0] = NULL_TREE;

  if (rhs_rank)
    {
      for (ii = 0; ii < rhs_list_size; ii++)
	{ 
	  jj = 0;
	  ii_tree = (*rhs_list)[ii];
	  while (ii_tree)
	    {
	      if (TREE_CODE (ii_tree) == ARRAY_NOTATION_REF)
		{
		  rhs_array[ii][jj] = ii_tree;
		  jj++;
		  ii_tree = ARRAY_NOTATION_ARRAY (ii_tree);
		}
	      else if (TREE_CODE (ii_tree) == ARRAY_REF)
		ii_tree = TREE_OPERAND (ii_tree, 0);
	      else if (TREE_CODE (ii_tree) == VAR_DECL
		       || TREE_CODE (ii_tree) == PARM_DECL)
		break;
	    }
	}
    }

  
  for (ii = 0; ii < lhs_list_size; ii++)
    {
      if (TREE_CODE ((*lhs_list)[ii]) == ARRAY_NOTATION_REF)
	{
	  for (jj = 0; jj < lhs_rank; jj++)
	    {
	      if (TREE_CODE (lhs_array[ii][jj]) == ARRAY_NOTATION_REF)
		{
		  lhs_value[ii][jj] = ARRAY_NOTATION_ARRAY (lhs_array[ii][jj]);
		  lhs_start[ii][jj] = ARRAY_NOTATION_START (lhs_array[ii][jj]);
		  lhs_length[ii][jj] =
		    ARRAY_NOTATION_LENGTH (lhs_array[ii][jj]);
		  lhs_stride[ii][jj] =
		    ARRAY_NOTATION_STRIDE (lhs_array[ii][jj]);
		  lhs_vector[ii][jj] = true;
		  
		  /* If the stride value is variable (i.e. not constant) then
		   * assume that the length is positive.  */
		  if (!really_constant_p (lhs_length[ii][jj])
		      || TREE_CODE (lhs_length[ii][jj]) == VAR_DECL)
		    lhs_count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (lhs_length[ii][jj],
			    build_zero_cst (TREE_TYPE (lhs_length[ii][jj]))))
		    lhs_count_down[ii][jj] = true;
		  else
		    lhs_count_down[ii][jj] = false;
		}
	      else
		lhs_vector[ii][jj] = false;
	    }
	}
    }

  for (ii = 0; ii < rhs_list_size; ii++)
    {
      if (TREE_CODE ((*rhs_list)[ii]) == ARRAY_NOTATION_REF)
	{
	  for (jj = 0; jj < rhs_rank; jj++)
	    {
	      if (TREE_CODE (rhs_array[ii][jj]) == ARRAY_NOTATION_REF)
		{
		  rhs_value[ii][jj]  = ARRAY_NOTATION_ARRAY (rhs_array[ii][jj]);
		  rhs_start[ii][jj]  = ARRAY_NOTATION_START (rhs_array[ii][jj]);
		  rhs_length[ii][jj] =
		    ARRAY_NOTATION_LENGTH (rhs_array[ii][jj]);
		  rhs_stride[ii][jj] =
		    ARRAY_NOTATION_STRIDE (rhs_array[ii][jj]);
		  rhs_vector[ii][jj] = true;
		  /* If the stride value is variable (i.e. not constant) then
		     assume that the length is positive.  */
		  if (!integer_zerop (rhs_length[ii][jj])
		      && !integer_nonzerop (rhs_length[ii][jj]))
		    rhs_count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (rhs_length[ii][jj],
			    build_zero_cst (TREE_TYPE (rhs_length[ii][jj]))))
		    rhs_count_down[ii][jj] = true;
		  else
		    rhs_count_down[ii][jj] = false;	
		}
	      else
		rhs_vector[ii][jj] = false;
	    }
	}
      else
	rhs_vector[ii][0] = false;
    }
   for (ii = 0; ii < lhs_rank; ii++)
    if (lhs_start[0][ii] && TREE_TYPE (lhs_start[0][ii]))
	lhs_var[ii] =  build_decl (location, VAR_DECL, NULL_TREE,
				   TREE_TYPE (lhs_start[0][ii]));
    else
      lhs_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				integer_type_node);

  
  for (ii = 0; ii < rhs_list_size; ii++)
    {
      if (TREE_CODE ((*rhs_list)[ii]) == CALL_EXPR)
	{
	  int idx_value = 0;
	  tree func_name = CALL_EXPR_FN ((*rhs_list)[ii]);
	  if (TREE_CODE (func_name) == ADDR_EXPR)
	    {
	      if (is_sec_implicit_index_fn (func_name))
		{
		  idx_value = 
		    extract_sec_implicit_index_arg (location, (*rhs_list)[ii]);
		  if (idx_value < (int) lhs_rank && idx_value >= 0)
		    vec_safe_push (rhs_array_operand, lhs_var[idx_value]);
		  else if (idx_value == -1)
		    return error_mark_node;
		  else
		    {
		      size_t ee = 0;
		      tree lhs_base = (*lhs_list)[ii];
		      for (ee = 0; ee < lhs_rank; ee++)
			if (lhs_base
			    && TREE_CODE (lhs_base) == ARRAY_NOTATION_REF)
			  lhs_base = ARRAY_NOTATION_ARRAY (lhs_base);

		      if (location == UNKNOWN_LOCATION
			  && EXPR_HAS_LOCATION (lhs))
			location = EXPR_LOCATION (lhs);
		      error_at (location, "__sec_implicit_index argument %d "
				"must be less than the rank of %qD", idx_value,
				lhs_base);
		      return error_mark_node;
		    }
		}
	      else
		vec_safe_push (rhs_array_operand, (*rhs_list)[ii]);
	    }
	  else
	    vec_safe_push (rhs_array_operand, (*rhs_list)[ii]);
	}
      else
	vec_safe_push (rhs_array_operand, (*rhs_list)[ii]);
    }

  replace_array_notations (&rhs, true, rhs_list, rhs_array_operand);
  rhs_list_size = 0;
  rhs_list = NULL;
  extract_array_notation_exprs (rhs, true, &rhs_list);
  rhs_list_size = vec_safe_length (rhs_list);    
  
  for (ii = 0; ii < lhs_rank; ii++)
    {
      if (lhs_vector[0][ii])
	{
	  lhs_ind_init[ii] = build_x_modify_expr
	    (location, lhs_var[ii], NOP_EXPR,
	     build_zero_cst (TREE_TYPE (lhs_var[ii])), tf_warning_or_error);
	}
    }

  for (ii = 0; ii < rhs_rank; ii++)
    {
      /* When we have a polynomial, we assume that the indices are of type
	 integer.  */
      rhs_var[ii] =  build_decl (location, VAR_DECL, NULL_TREE,
				 TREE_TYPE (rhs_start[0][ii]));
      rhs_ind_init[ii] = build_x_modify_expr
	(location, rhs_var[ii], NOP_EXPR, 
	 build_zero_cst (TREE_TYPE (rhs_var[ii])), tf_warning_or_error);
    }

  for (ii = 0; ii < MAX (lhs_rank, rhs_rank); ii++)
    {
      array_notation_label_no++;
      memset (label_name, 0, 50);
      sprintf (label_name, "if_stmt_label_%05d", array_notation_label_no);
      
      /* This will create the if statement label.  */
      if_stmt_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      memset (label_name, 0, 50);
      sprintf (label_name, "body_label_%05d", array_notation_label_no);
      /* This label statment will point to the loop body.  */
      body_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      body_label_expr[ii] = build_stmt (UNKNOWN_LOCATION, LABEL_EXPR,
					body_label[ii]);
      
      /* this will create the exit label..i.e. where the while loop will
	 branch out of.  */
      memset (label_name, 0, 50);
      sprintf (label_name, "exit_label_%05d", array_notation_label_no);
      exit_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_IGNORED_P (exit_label[ii]) = 1;
      DECL_ARTIFICIAL (exit_label[ii]) = 0;
      DECL_CONTEXT (exit_label[ii]) = current_function_decl;
      exit_label_expr[ii] = build_stmt  (location, LABEL_EXPR,
					 exit_label[ii]);
    }

  if (lhs_rank)
    {
      for (ii = 0; ii < lhs_list_size; ii++)
	{
	  if (lhs_vector[ii][0])
	    {
	      /* The last ARRAY_NOTATION element's ARRAY component should be
		 the array's base value.  */
	      tree lhs_array_opr = lhs_value[ii][lhs_rank - 1];
	      if (TREE_CODE (lhs_array_opr) == ARRAY_REF)
		/* Here, if the rhs_array_opr is an ARRAY_REF, then we have a
		   case like this:
		   A[X][0:10:1].....

		   If the rhs_value is an array operand, it is fixed right
		   before grok_array_decl is called.  So, we undo it here so
		   that the fix can take place below.  */
		lhs_array_opr = TREE_OPERAND (lhs_array_opr, 0);
	      
	      for (s_jj = lhs_rank - 1; s_jj >= 0; s_jj--)
		{
		  tree base_var = NULL_TREE;
		  tree stride = NULL_TREE, var = NULL_TREE, start = NULL_TREE;
		  if ((TREE_TYPE (lhs_start[ii][s_jj]) ==
		       TREE_TYPE (lhs_stride[ii][s_jj]))
		      && (TREE_TYPE (lhs_stride[ii][s_jj]) !=
			  TREE_TYPE (lhs_var[s_jj])))
		    {
		      /* If stride and start are of same type and the induction
			 var is not, we convert induction variable to stride's
			 type.  */
		      start = lhs_start[ii][s_jj];
		      stride = lhs_stride[ii][s_jj];
		      var = build_c_cast (location,
					  TREE_TYPE (lhs_stride[ii][s_jj]),
					  lhs_var[s_jj]);
		    }
		  else if (TREE_TYPE (lhs_start[ii][s_jj]) !=
			   TREE_TYPE (lhs_stride[ii][s_jj]))
		    {
		      /* If we reach here, then the stride and start are of
			 different types, and so it doesn't really matter what
			 the induction variable type is, we stay safe and
			 convert everything to integer.  The reason why we
			 pick integer instead of something like size_t is
			 because the stride and length can be + or -.  */
		      start = build_c_cast (location, integer_type_node,
					    lhs_start[ii][s_jj]);
		      stride = build_c_cast (location, integer_type_node,
					     lhs_stride[ii][s_jj]);
		      var = build_c_cast (location, integer_type_node,
					  lhs_var[s_jj]);
		    }
		  else
		    {
		      start = lhs_start[ii][s_jj];
		      stride = lhs_stride[ii][s_jj];
		      var = lhs_var[s_jj];
		    }
		  base_var = ARRAY_NOTATION_ARRAY (lhs_array[ii][s_jj]);
		  if (TREE_CODE (base_var) == ARRAY_REF)
		    lhs_array_opr = grok_array_decl
		      (location, lhs_array_opr,  TREE_OPERAND (base_var, 1));
		  
		  if (lhs_count_down[ii][s_jj])
		    /* Array[start_index - (induction_var * stride)] */
		    lhs_array_opr = grok_array_decl
		      (location, lhs_array_opr,
		       build2 (MINUS_EXPR, TREE_TYPE (var), start,
			       build2 (MULT_EXPR, TREE_TYPE (var), var,
				       stride)));	
		  else
		    /* Array[start_index + (induction_var * stride)] */
		    lhs_array_opr = grok_array_decl
		      (location, lhs_array_opr,
		       build2 (PLUS_EXPR, TREE_TYPE (var), start,
			       build2 (MULT_EXPR, TREE_TYPE (var), var,
				       stride)));
		}
	      vec_safe_push (lhs_array_operand, lhs_array_opr);
	    }
	  else
	    vec_safe_push (lhs_array_operand, integer_one_node);
	}
      replace_array_notations (&lhs, true, lhs_list, lhs_array_operand);
      array_expr_lhs = lhs;
    }

  if (rhs_array_operand)
    vec_safe_truncate (rhs_array_operand, 0);

  if (rhs_rank)
    {
      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  if (rhs_vector[ii][0])
	    {
	      tree rhs_array_opr = rhs_value[ii][rhs_rank - 1];
	      if (TREE_CODE (rhs_array_opr) == ARRAY_REF)
		rhs_array_opr = TREE_OPERAND (rhs_array_opr, 0);
		
	      for (s_jj = rhs_rank - 1; s_jj >= 0; s_jj--)
		{
		  tree base_var = NULL_TREE;
		  tree stride = NULL_TREE, var = NULL_TREE, start = NULL_TREE;
		  if ((TREE_TYPE (rhs_start[ii][s_jj]) ==
		       TREE_TYPE (rhs_stride[ii][s_jj]))
		      && (TREE_TYPE (rhs_stride[ii][s_jj]) !=
			  TREE_TYPE (rhs_var[s_jj])))
		    {
		      /* If stride and start are of same type and the induction
			 var is not, we convert induction variable to stride's
			 type.  */
		      start = rhs_start[ii][s_jj];
		      stride = rhs_stride[ii][s_jj];
		      var = build_c_cast (location,
					  TREE_TYPE (rhs_stride[ii][s_jj]),
					  rhs_var[s_jj]);
		    }
		  else if (TREE_TYPE (rhs_start[ii][s_jj]) !=
			   TREE_TYPE (rhs_stride[ii][s_jj]))
		    {
		      /* If we reach here, then the stride and start are of
			 different types, and so it doesn't really matter what
			 the induction variable type is, we stay safe and
			 convert everything to integer.  The reason why we
			 pick integer instead of something like size_t is
			 because the stride and length can be + or -.  */
		      start = build_c_cast (location, integer_type_node,
					    rhs_start[ii][s_jj]);
		      stride = build_c_cast (location, integer_type_node,
					     rhs_stride[ii][s_jj]);
		      var = build_c_cast (location, integer_type_node,
					  rhs_var[s_jj]);
		    }
		  else
		    {
		      start = rhs_start[ii][s_jj];
		      stride = rhs_stride[ii][s_jj];
		      var = rhs_var[s_jj];
		    }
		  base_var = ARRAY_NOTATION_ARRAY (rhs_array[ii][s_jj]);
		  if (TREE_CODE (base_var) == ARRAY_REF)
		    rhs_array_opr = grok_array_decl
		      (location, rhs_array_opr,  TREE_OPERAND (base_var, 1));

		  if (rhs_count_down[ii][s_jj])
		    /* Array[start_index - (induction_var * stride)] */
		    rhs_array_opr = grok_array_decl
		      (location, rhs_array_opr,
		       build2 (MINUS_EXPR, TREE_TYPE (var), start,
			       build2 (MULT_EXPR, TREE_TYPE (var), var,
				       stride)));	
		  else
		    /* Array[start_index + (induction_var * stride)] */
		    rhs_array_opr = grok_array_decl
		      (location, rhs_array_opr,
		       build2 (PLUS_EXPR, TREE_TYPE (var), start,
			       build2 (MULT_EXPR, TREE_TYPE (var), var,
				       stride)));
		}
	      vec_safe_push (rhs_array_operand, rhs_array_opr);
	    }
	  else
	    /* This is just a dummy node to make sure the list sizes for both
	       array list and array operand list are the same.  */
	    vec_safe_push (rhs_array_operand, integer_one_node);
	}
      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  tree rhs_node = (*rhs_list)[ii];
	  if (TREE_CODE (rhs_node) == CALL_EXPR)
	    {
	      int idx_value = 0;
	      tree func_name = CALL_EXPR_FN (rhs_node);
	      if (TREE_CODE (func_name) == ADDR_EXPR)
		if (is_sec_implicit_index_fn (func_name))
		  {
		    idx_value = 
		      extract_sec_implicit_index_arg (location, rhs_node);
		    if (idx_value < (int) lhs_rank && idx_value >= 0)
		      vec_safe_push (rhs_array_operand, rhs_var[idx_value]);
		    else
		      {
			size_t ee = 0;
			tree rhs_base = (*lhs_list)[ii];
			for (ee = 0; ee < rhs_rank; ee++)
			  if (rhs_base
			      && TREE_CODE (rhs_base) == ARRAY_NOTATION_REF)
			    rhs_base = ARRAY_NOTATION_ARRAY (rhs_base);

			error_at (location, "__sec_implicit_index argument %d "
				  "must be less than rank of %qD", idx_value,
				  rhs_base);
			return error_mark_node;
		      }
		  }
	    }
	}	
      replace_array_notations (&rhs, true, rhs_list, rhs_array_operand);
      array_expr_rhs = rhs;
    }
  else
    {
      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  tree rhs_node = (*rhs_list)[ii];
	  if (TREE_CODE (rhs_node) == CALL_EXPR)
	    {
	      int idx_value = 0;
	      tree func_name = CALL_EXPR_FN (rhs_node);
	      if (is_sec_implicit_index_fn (func_name))
		{
		  idx_value =  extract_sec_implicit_index_arg (location,
							       rhs_node);
		  if (idx_value < (int) lhs_rank && idx_value >= 0)
		    vec_safe_push (rhs_array_operand, lhs_var[idx_value]);
		  else
		    {
		      size_t ee = 0;
		      tree lhs_base = (*lhs_list)[ii];
		      for (ee = 0; ee < lhs_rank; ee++)
			if (lhs_base
			    && TREE_CODE (lhs_base) == ARRAY_NOTATION_REF)
			  lhs_base = ARRAY_NOTATION_ARRAY (lhs_base);
		      error_at (location, "__sec_implicit_index argument %d "
				"must be less than the rank of %qD", idx_value,
				lhs_base);
		      return error_mark_node;
		    }
		}
	    }
	}
      replace_array_notations (&rhs, true, rhs_list, rhs_array_operand);
      array_expr_rhs = rhs;
      rhs_expr_incr[0] = NULL_TREE;
    }
  
  for (ii = 0; ii < rhs_rank; ii++)
    if (rhs_count_down[0][ii])
      rhs_expr_incr[ii] = build_x_unary_op (location, POSTDECREMENT_EXPR,
					    rhs_var[ii], tf_warning_or_error);
    else
      rhs_expr_incr[ii] = build_x_unary_op (location, POSTINCREMENT_EXPR,
					    rhs_var[ii], tf_warning_or_error);
  
  for (ii = 0; ii < lhs_rank; ii++)
    if (lhs_count_down[0][ii])
      lhs_expr_incr[ii] = build_x_unary_op (location, POSTDECREMENT_EXPR,
					    lhs_var[ii], tf_warning_or_error);
  else
    lhs_expr_incr[ii] = build_x_unary_op (location, POSTINCREMENT_EXPR,
					  lhs_var[ii], tf_warning_or_error);
	
  if (!array_expr_lhs)
    array_expr_lhs = lhs;
  
  array_expr = build_x_modify_expr (location, array_expr_lhs, 
				    modifycode, array_expr_rhs, complain);

  for (jj = 0; jj < MAX (lhs_rank, rhs_rank); jj++)
    {
      if (rhs_rank && rhs_expr_incr[jj])
	{
	  size_t iii = 0;
	  if (lhs_rank)
	    {
	      if (lhs_count_down[0][jj])
		lhs_compare[jj] = build_x_binary_op
		  (location, GT_EXPR, lhs_var[jj], TREE_CODE (lhs_var[jj]),
		   lhs_length[0][jj], TREE_CODE (lhs_length[0][jj]), NULL,
		   tf_warning_or_error);
	      else
		lhs_compare[jj] = build_x_binary_op
		  (location, LT_EXPR, lhs_var[jj], TREE_CODE (lhs_var[jj]),
		   lhs_length[0][jj], TREE_CODE (lhs_length[0][jj]), NULL,
		   tf_warning_or_error);
	    }
	  else
	    lhs_compare[jj] = NULL_TREE;

	  for (iii = 0; iii < rhs_list_size; iii++)
	    if (rhs_vector[iii][jj])
	      break;
	  
	  /* What we are doing here is this:
	   * We always count up, so:
	   *    if (length is negative ==> which means we count down)
	   *       we multiply length by -1 and count up => ii < -LENGTH
	   *    else
	   *       we just count up, so we compare for  ii < LENGTH
	   */
	  if (rhs_count_down[iii][jj])
	    {
	      tree new_rhs = build_x_modify_expr
		(location, rhs_length[iii][jj], MULT_EXPR,
		 build_int_cst (TREE_TYPE (rhs_length[iii][jj]), -1),
		 tf_warning_or_error);
	      rhs_compare[jj] = build_x_binary_op
		(location, GT_EXPR, rhs_var[jj], TREE_CODE (rhs_var[jj]),
		 new_rhs, TREE_CODE (new_rhs), NULL, tf_warning_or_error);
	    }
	  else
	    rhs_compare[jj] = build_x_binary_op
	      (location, LT_EXPR, rhs_var[jj], TREE_CODE (rhs_var[jj]),
	       rhs_length[iii][jj], TREE_CODE (rhs_length[0][jj]), NULL,
	       tf_warning_or_error);
	   
	  if (lhs_rank)
	    cond_expr[jj] = build_x_binary_op
	      (location, TRUTH_ANDIF_EXPR, lhs_compare[jj],
	       TREE_CODE (lhs_compare[jj]),
	       rhs_compare[jj], TREE_CODE (rhs_compare[jj]), NULL,
	       tf_warning_or_error);
	  else
	    cond_expr[jj] = rhs_compare[jj];
	}
      else
	{
	  if (lhs_count_down[0][jj])
	    cond_expr[jj] = build_x_binary_op
	      (location, GT_EXPR, lhs_var[jj], TREE_CODE (lhs_var[jj]),
	       lhs_length[0][jj], TREE_CODE (lhs_length[0][jj]), NULL,
	       tf_warning_or_error);
	    else
	      cond_expr[jj] = build_x_binary_op
		(location, LT_EXPR, lhs_var[jj], TREE_CODE (lhs_var[jj]),
		 lhs_length[0][jj], TREE_CODE (lhs_length[0][jj]), NULL,
		 tf_warning_or_error);
	}
    }
  
  /* The following statements will do the following:
   * <if_stmt_label>: (in order from outermost to innermost)
   *                  if (cond_expr) then go to body_label
   *                  else                go to exit_label
   * <body_label>:
   *                  array expression
   *
   *                  (the increment, goto and exit_label goes from innermost to
   *                   outermost).
   *                  ii++ and jj++
   *                  go to if_stmt_label
   * <exit_label>:
   *                  <REST OF CODE>
   */

  
  for (ii = 0; ii < MAX (lhs_rank, rhs_rank); ii++)
    {
      if (lhs_rank)
	add_stmt (build_stmt (location, EXPR_STMT, lhs_ind_init[ii]));
      
      if (rhs_rank)
	add_stmt (build_stmt (location, EXPR_STMT, rhs_ind_init[ii]));

      add_stmt (build_stmt (location, LABEL_EXPR, if_stmt_label[ii]));

      comp_stmt = begin_if_stmt ();
      finish_if_stmt_cond (cond_expr[ii], comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, body_label[ii]));
      finish_then_clause (comp_stmt);
      begin_else_clause (comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, exit_label[ii]));
      finish_else_clause (comp_stmt);
      finish_if_stmt (comp_stmt);
      add_stmt (build_stmt (location, LABEL_EXPR, body_label[ii]));
    }
    
  if (MAX (lhs_rank, rhs_rank))
    add_stmt (build_stmt (location, EXPR_STMT, array_expr));
  
  for (s_jj = MAX (lhs_rank, rhs_rank) - 1; s_jj >= 0; s_jj--)
    {
      if (lhs_rank)
	add_stmt (build_stmt (location, EXPR_STMT, lhs_expr_incr[s_jj]));
      if (rhs_rank && rhs_expr_incr[s_jj])
	add_stmt (build_stmt (location, EXPR_STMT, rhs_expr_incr[s_jj]));
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (build_stmt (location, LABEL_EXPR, exit_label[s_jj]));
     }

  loop = pop_stmt_list (loop);
  
  for (ii = 0; ii < rhs_list_size; ii++)
    {
      XDELETEVEC (rhs_vector[ii]);
      XDELETEVEC (rhs_array[ii]);
      XDELETEVEC (rhs_value[ii]);
      XDELETEVEC (rhs_length[ii]);
      XDELETEVEC (rhs_stride[ii]);
      XDELETEVEC (rhs_start[ii]);
    }
    for (ii = 0; ii < lhs_list_size; ii++)
    {
      XDELETEVEC (lhs_vector[ii]);
      XDELETEVEC (lhs_array[ii]);
      XDELETEVEC (lhs_value[ii]);
      XDELETEVEC (lhs_length[ii]);
      XDELETEVEC (lhs_stride[ii]);
      XDELETEVEC (lhs_start[ii]);
    }
  if (rhs_vector)
    XDELETEVEC (rhs_vector);

  if (rhs_array)
    XDELETEVEC (rhs_array);
  if (rhs_value)
    XDELETEVEC (rhs_value);
  if (rhs_length)
    XDELETEVEC (rhs_length);
  if (rhs_stride)
    XDELETEVEC (rhs_stride);
  if (rhs_start)
    XDELETEVEC (rhs_start);

  
  if (exit_label)    
    XDELETEVEC (exit_label);
  if (exit_label_expr)
    XDELETEVEC (exit_label_expr);

  if (if_stmt_label)
    XDELETEVEC (if_stmt_label);
  if (body_label)
    XDELETEVEC (body_label);
  if (body_label_expr)
    XDELETEVEC (body_label_expr);
  
  if (rhs_expr_incr)    
    XDELETEVEC (rhs_expr_incr);
  if (rhs_ind_init)
    XDELETEVEC (rhs_ind_init);
 
  if (rhs_compare)
    XDELETEVEC (rhs_compare);
  if (lhs_compare)
    XDELETEVEC (lhs_compare);
  
  return loop;
}

/* Encloses the conditional statement passed in STMT with a loop around it
   and replaces the condition in STMT with a ARRAY_REF tree-node to the array.
   The condition must have a ARRAY_NOTATION_REF tree.  */

static tree
fix_conditional_array_notations_1 (tree orig_stmt)
{
  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  size_t list_size = 0;
  size_t rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, jj_tree, loop, stmt = NULL_TREE;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init, comp_stmt;
  tree builtin_loop, new_var = NULL_TREE;
  bool **count_down, **array_vector;
  char label_name[50];
  int s_jj = 0;
  location_t location = UNKNOWN_LOCATION;

  if (TREE_CODE (orig_stmt) == COND_EXPR)
    {
      size_t cond_rank = 0, yes_rank = 0, no_rank = 0;
      tree yes_expr = COND_EXPR_THEN (orig_stmt);
      tree no_expr = COND_EXPR_ELSE (orig_stmt);
      tree cond = COND_EXPR_COND (orig_stmt);
      find_rank (cond, true, &cond_rank);
      find_rank (yes_expr, true, &yes_rank);
      find_rank (no_expr, true, &no_rank);
      if (cond_rank != 0 && cond_rank != yes_rank)
	{
	  error_at (EXPR_LOCATION (yes_expr), "rank mismatch with controlling"
		    " expression of parent if-statement");
	  return error_mark_node;
	}
      else if (cond_rank != 0 && cond_rank != no_rank)
	{
	  error_at (EXPR_LOCATION (no_expr), "rank mismatch with controlling "
		    "expression of parent if-statement");
	  return error_mark_node;
	}
    }
  else if (TREE_CODE (orig_stmt) == IF_STMT)
    {
      size_t cond_rank = 0, yes_rank = 0, no_rank = 0;
      tree yes_expr = THEN_CLAUSE (orig_stmt);
      tree no_expr = ELSE_CLAUSE (orig_stmt);
      tree cond = IF_COND (orig_stmt);
      find_rank (cond, true, &cond_rank);
      find_rank (yes_expr, true, &yes_rank);
      find_rank (no_expr, true, &no_rank);
      if (cond_rank != 0 && cond_rank != yes_rank)
	{
	  error_at (EXPR_LOCATION (yes_expr), "rank mismatch with controlling"
		    " expression of parent if-statement");
	  return error_mark_node;
	}
      else if (cond_rank != 0 && cond_rank != no_rank)
	{
	  error_at (EXPR_LOCATION (no_expr), "rank mismatch with controlling "
		    "expression of parent if-statement");
	  return error_mark_node;
	}
    }

  find_rank (orig_stmt, false, &rank);
  if (rank == 0)
    return orig_stmt;

  extract_array_notation_exprs (orig_stmt, false, &array_list);
  stmt = alloc_stmt_list ();
  for (ii = 0; ii < vec_safe_length (array_list); ii++)
    {
      tree array_node = (*array_list)[ii];
      if (TREE_CODE (array_node) == CALL_EXPR
	  || TREE_CODE (array_node) == AGGR_INIT_EXPR)
	{
	  builtin_loop = fix_builtin_array_notation_fn (array_node, &new_var);
	  if (builtin_loop == error_mark_node)
	    add_stmt (error_mark_node);
	  else if (new_var)
	    {
	      vec<tree, va_gc> *sub_list = NULL, *new_var_list = NULL;
	      vec_safe_push (sub_list, array_node);
	      vec_safe_push (new_var_list, new_var);
	      replace_array_notations (&orig_stmt, false, sub_list,
				       new_var_list);
	      append_to_statement_list_force (builtin_loop, &stmt);
	    }
	}
    }
  append_to_statement_list_force (orig_stmt, &stmt);
  rank = 0;
  array_list = NULL;
  find_rank (stmt, true, &rank);
  if (rank == 0)
    return stmt;
  
  extract_array_notation_exprs (stmt, true, &array_list);
  list_size = vec_safe_length (array_list);
  if (list_size == 0)
    return stmt;

  location = EXPR_LOCATION (orig_stmt);
  
  array_ops = XNEWVEC (tree *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = XNEWVEC (tree, rank);
  
  array_vector = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = XNEWVEC (bool, rank);

  array_value  = XNEWVEC (tree *, list_size);
  array_stride = XNEWVEC (tree *, list_size);
  array_length = XNEWVEC (tree *, list_size);
  array_start  = XNEWVEC (tree *, list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = XNEWVEC (tree, rank);
      array_stride[ii] = XNEWVEC (tree, rank);
      array_length[ii] = XNEWVEC (tree, rank);
      array_start[ii]  = XNEWVEC (tree, rank);
    }

  body_label = XNEWVEC (tree, rank);
  body_label_expr = XNEWVEC (tree, rank);
  exit_label = XNEWVEC (tree, rank);
  exit_label_expr = XNEWVEC (tree, rank);
  compare_expr = XNEWVEC (tree, rank);
  if_stmt_label = XNEWVEC (tree, rank);
  
  expr_incr = XNEWVEC (tree, rank);
  ind_init = XNEWVEC (tree, rank);

  list_size = vec_safe_length (array_list);
  count_down = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = XNEWVEC (bool, rank);
  
  array_var = XNEWVEC (tree, rank);
  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      for (jj_tree = (*array_list)[ii];
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE ((*array_list)[ii]) == ARRAY_NOTATION_REF)
	{
	  for (jj = 0; jj < rank; jj++)
	    {
	      if (TREE_CODE (array_ops[ii][jj]) == ARRAY_NOTATION_REF)
		{
		  array_value[ii][jj] =
		    ARRAY_NOTATION_ARRAY (array_ops[ii][jj]);
		  array_start[ii][jj] =
		    ARRAY_NOTATION_START (array_ops[ii][jj]);
		  array_length[ii][jj] =
		    ARRAY_NOTATION_LENGTH (array_ops[ii][jj]);
		  array_stride[ii][jj] =
		    ARRAY_NOTATION_STRIDE (array_ops[ii][jj]);
		  array_vector[ii][jj] = true;

		  if (!integer_zerop (array_length[ii][jj])
		      && !integer_nonzerop (array_length[ii][jj]))
		    count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (array_length[ii][jj],
			    build_zero_cst (TREE_TYPE (array_length[ii][jj]))))
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }

  loop = push_stmt_list ();
  
  for (ii = 0; ii < rank; ii++)
    {
      if (TREE_TYPE (array_start[0][ii])
	  && TREE_CODE (TREE_TYPE (array_start[0][ii])) != TEMPLATE_TYPE_PARM)
	{
	  array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				      TREE_TYPE (array_start[0][ii]));
	  ind_init[ii] = build_x_modify_expr
	    (location, array_var[ii], NOP_EXPR, 
	     build_zero_cst (TREE_TYPE (array_var[ii])), tf_warning_or_error);
	}
      else
	{
	  array_var[ii] = build_min_nt_loc (location, VAR_DECL,
					    NULL_TREE, NULL_TREE);
	  ind_init[ii] = build_x_modify_expr (location, array_var[ii], 
					      NOP_EXPR,
					      integer_zero_node, 1);
	}
    }

  for (ii = 0; ii < rank; ii++)
    {
      array_notation_label_no++;
      memset (label_name, 0, 50);
      sprintf (label_name, "if_stmt_label_%05d", array_notation_label_no);
      
      /* This will create the if statement label.  */
      if_stmt_label[ii] =
	define_label (location, get_identifier (label_name));
      
      memset (label_name, 0, 50);
      sprintf (label_name, "body_label_%05d", array_notation_label_no);
      /* This label statment will point to the loop body.  */
      body_label[ii] = define_label (location,
				     get_identifier (label_name));
      body_label_expr[ii] = build_stmt (location, LABEL_EXPR,
					body_label[ii]);
      
      /* This will create the exit label..i.e. where the while loop will branch
	 out of.  */
      memset (label_name, 0, 50);
      sprintf (label_name, "exit_label_%05d", array_notation_label_no);
      exit_label[ii] = define_label (location,
				     get_identifier (label_name));
      exit_label_expr[ii] = build_stmt  (location, LABEL_EXPR,
					 exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  tree array_opr = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      tree stride = NULL_TREE, var = NULL_TREE, start = NULL_TREE;

	      /* If stride and start are of same type and the induction var
		 is not, we convert induction variable to stride's type.  */
	      if ((TREE_TYPE (array_start[ii][s_jj]) ==
		   TREE_TYPE (array_stride[ii][s_jj]))
		  && (TREE_TYPE (array_stride[ii][s_jj]) !=
		      TREE_TYPE (array_var[s_jj])))
		{
		  start = array_start[ii][s_jj];
		  stride = array_stride[ii][s_jj];
		  var =
		    build_c_cast (location, TREE_TYPE (array_stride[ii][s_jj]),
				  array_var[s_jj]);
		}
	      else if (TREE_TYPE (array_start[ii][s_jj]) !=
			TREE_TYPE (array_stride[ii][s_jj]))
		{
		  /* If we reach here, then the stride and start are of
		     different types, and so it doesn't really matter what
		     the induction variable type is, we stay safe and convert
		     everything to integer.  The reason why we pick integer
		     instead of something like size_t is because the stride
		     and length can be + or -.  */
		  start = build_c_cast (location, integer_type_node,
					array_start[ii][s_jj]);
		  stride = build_c_cast (location, integer_type_node,
					 array_stride[ii][s_jj]);
		  var = build_c_cast (location, integer_type_node,
				      array_var[s_jj]);
		}
	      else
		{
		  start = array_start[ii][s_jj];
		  stride = array_stride[ii][s_jj];
		  var = array_var[s_jj];
		}
	      if (count_down[ii][s_jj])
		/* Array[start_index - (induction_var * stride)] */
		array_opr = grok_array_decl
		  (location, array_opr,
		   build2 (MINUS_EXPR, TREE_TYPE (var), start,
			   build2 (MULT_EXPR, TREE_TYPE (var), var, stride)));	
	      else
		/* Array[start_index + (induction_var * stride)] */
		array_opr = grok_array_decl
		  (location, array_opr,
		   build2 (PLUS_EXPR, TREE_TYPE (var), start,
			   build2 (MULT_EXPR, TREE_TYPE (var), var, stride)));
		
	    }
	  vec_safe_push (array_operand, array_opr);
	}
      else
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&stmt, true, array_list, array_operand);

  for (ii = 0; ii < rank; ii++)
    if (count_down[0][ii])
      expr_incr[ii] = build_x_unary_op (location, POSTDECREMENT_EXPR,
					array_var[ii], tf_warning_or_error);
    else
      expr_incr[ii] = build_x_unary_op (location, POSTINCREMENT_EXPR,
					array_var[ii], tf_warning_or_error);
  
  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] = build_x_binary_op
	      (location, GT_EXPR, array_var[jj],
	       TREE_CODE (array_var[jj]),
	       array_length[0][jj], TREE_CODE (array_length[0][jj]), NULL,
	       tf_warning_or_error);
	  else
	    compare_expr[jj] = build_x_binary_op
	      (location, LT_EXPR, array_var[jj],
	       TREE_CODE (array_var[jj]),
	       array_length[0][jj], TREE_CODE (array_length[0][jj]), NULL,
	       tf_warning_or_error);
	}
    }
  
  for (ii = 0; ii < rank; ii++)
    {
      add_stmt (ind_init[ii]);
      add_stmt (build_stmt (location, LABEL_EXPR, if_stmt_label[ii]));

      comp_stmt = begin_if_stmt ();
      finish_if_stmt_cond (compare_expr[ii], comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, body_label[ii]));
      finish_then_clause (comp_stmt);
      begin_else_clause (comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, exit_label[ii]));
      finish_else_clause (comp_stmt);
      finish_if_stmt (comp_stmt);
      add_stmt (build_stmt (location, LABEL_EXPR, body_label[ii]));
    }

  add_stmt (build_stmt (location, EXPR_STMT, stmt));

  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      add_stmt (build_stmt (location, EXPR_STMT, expr_incr[s_jj]));
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (build_stmt (location, LABEL_EXPR, exit_label[s_jj]));
    }

  loop = pop_stmt_list (loop);

  XDELETEVEC (body_label);
  XDELETEVEC (body_label_expr);
  XDELETEVEC (exit_label);
  XDELETEVEC (exit_label_expr);
  XDELETEVEC (compare_expr);
  XDELETEVEC (if_stmt_label);
  XDELETEVEC (expr_incr);
  XDELETEVEC (ind_init);
  XDELETEVEC (array_var);

  if (list_size > 1)
    for (ii = 0; ii < list_size; ii++)
      {
	XDELETEVEC (count_down[ii]);
	XDELETEVEC (array_value[ii]);
	XDELETEVEC (array_stride[ii]);
	XDELETEVEC (array_length[ii]);
	XDELETEVEC (array_start[ii]);
	XDELETEVEC (array_ops[ii]);
	XDELETEVEC (array_vector[ii]);
      }

  XDELETEVEC (count_down);
  XDELETEVEC (array_value);
  XDELETEVEC (array_stride);
  XDELETEVEC (array_length);
  XDELETEVEC (array_start);
  XDELETEVEC (array_ops);
  XDELETEVEC (array_vector);

  return loop;
}

/* Top-level function to replace ARRAY_NOTATION_REF in a conditional statement
   in STMT.  */

tree
fix_array_notation_exprs (tree t)
{
  enum tree_code code;
  bool is_expr;
  location_t loc = UNKNOWN_LOCATION;
  
  /* Skip empty subtrees.  */
  if (!t)
    return t;

  loc = EXPR_LOCATION (t);

  code = TREE_CODE (t); 
  is_expr = IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (code));
  switch (code)
    {
    case ERROR_MARK:
    case IDENTIFIER_NODE:
    case INTEGER_CST:
    case REAL_CST:
    case FIXED_CST:
    case STRING_CST:
    case BLOCK:
    case PLACEHOLDER_EXPR:
    case FIELD_DECL:
    case VOID_TYPE:
    case REAL_TYPE:
    case SSA_NAME:
    case LABEL_DECL:
    case RESULT_DECL:
    case VAR_DECL:
    case PARM_DECL:
    case NON_LVALUE_EXPR:
    case NOP_EXPR:
    case INIT_EXPR:
    case ADDR_EXPR:
    case ARRAY_REF:
    case BIT_FIELD_REF:
    case VECTOR_CST:
    case COMPLEX_CST:
      return t;
    case MODIFY_EXPR:
      if (contains_array_notation_expr (t))
	t = build_x_array_notation_expr (loc, TREE_OPERAND (t, 0), NOP_EXPR, 
					 TREE_OPERAND (t, 1), 
					 tf_warning_or_error);
      return t;
    case MODOP_EXPR:
      if (contains_array_notation_expr (t) && !processing_template_decl)
	t = build_x_array_notation_expr
	  (loc, TREE_OPERAND (t, 0), TREE_CODE (TREE_OPERAND (t, 1)),
	   TREE_OPERAND (t, 2), tf_warning_or_error);
      return t;
    case CONSTRUCTOR:
      return t;
    case BIND_EXPR:
      {
	BIND_EXPR_BODY (t) =
	  fix_array_notation_exprs  (BIND_EXPR_BODY (t));
	return t;
      }
    case DECL_EXPR:
      {
	tree x = DECL_EXPR_DECL (t);
	if (t && TREE_CODE (x) != FUNCTION_DECL)
	  if (DECL_INITIAL (x))
	    t = fix_unary_array_notation_exprs (t);
      return t;
      }
    case STATEMENT_LIST:
      {
	tree_stmt_iterator i;
	for (i = tsi_start (t); !tsi_end_p (i); tsi_next (&i))
	  *tsi_stmt_ptr (i) =
	    fix_array_notation_exprs (*tsi_stmt_ptr (i));
	return t;
      }

    case OMP_PARALLEL:
    case OMP_TASK:
    case OMP_FOR:
    case OMP_SINGLE:
    case OMP_SECTION:
    case OMP_SECTIONS:
    case OMP_MASTER:
    case OMP_ORDERED:
    case OMP_CRITICAL:
    case OMP_ATOMIC:
    case OMP_CLAUSE:
    case TARGET_EXPR:
    case INTEGER_TYPE:
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
    case POINTER_TYPE:
    case ARRAY_TYPE:
    case RECORD_TYPE:
    case METHOD_TYPE:
      return t;
    case RETURN_EXPR:
      if (contains_array_notation_expr (t))
	t = fix_return_expr (t);
      return t;
    case PREDECREMENT_EXPR:
    case PREINCREMENT_EXPR:
    case POSTDECREMENT_EXPR:
    case POSTINCREMENT_EXPR:
    case AGGR_INIT_EXPR:
    case CALL_EXPR:
      t = fix_unary_array_notation_exprs (t);
      return t;
    case CONVERT_EXPR:
    case CLEANUP_POINT_EXPR:
    case EXPR_STMT:
      TREE_OPERAND (t, 0) = fix_array_notation_exprs (TREE_OPERAND (t, 0));
      /* It is not necessary to wrap error_mark_node in EXPR_STMT.  */
      if (TREE_OPERAND (t, 0) == error_mark_node)
	return TREE_OPERAND (t, 0); 
      return t;
    case COND_EXPR:
      t = fix_conditional_array_notations_1 (t);
      if (TREE_CODE (t) == COND_EXPR)
	{
	  COND_EXPR_THEN (t) =
	    fix_array_notation_exprs (COND_EXPR_THEN (t));
	  COND_EXPR_ELSE (t) =
	    fix_array_notation_exprs (COND_EXPR_ELSE (t));
	}
      else
	t = fix_array_notation_exprs (t);
      return t;

    case SWITCH_EXPR:
      t = fix_conditional_array_notations_1 (t);
      if (TREE_CODE (t) == SWITCH_EXPR)
	SWITCH_BODY (t) = fix_array_notation_exprs (SWITCH_BODY (t));
      else
	t = fix_array_notation_exprs (t);
      return t;
      
    case FOR_STMT:
      
      /* FIXME: Add a check for CILK_FOR_STMT here when we add Cilk tasking 
	 keywords.  */
      if (TREE_CODE (t) == FOR_STMT)
	FOR_BODY (t) = fix_array_notation_exprs (FOR_BODY (t));
      else
	t = fix_array_notation_exprs (t);
      return t;

    case IF_STMT:
      t = fix_conditional_array_notations_1 (t);
      /* If the above function added some extra instructions above the original
	 if statement, then we can't assume it is still IF_STMT so we have to
	 check again.  */
      if (TREE_CODE (t) == IF_STMT)
	{
	  if (THEN_CLAUSE (t))
	    THEN_CLAUSE (t) = fix_array_notation_exprs (THEN_CLAUSE (t));
	  if (ELSE_CLAUSE (t))
	    ELSE_CLAUSE (t) = fix_array_notation_exprs (ELSE_CLAUSE (t));
	}
      else
	t = fix_array_notation_exprs (t);
      return t;

    case SWITCH_STMT:
      t = fix_conditional_array_notations_1 (t);
      /* If the above function added some extra instructions above the original
	 switch statement, then we can't assume it is still SWITCH_STMT so we
	 have to check again.  */
      if (TREE_CODE (t) == SWITCH_STMT)
	{
	  if (SWITCH_STMT_BODY (t))
	    SWITCH_STMT_BODY (t) =
	      fix_array_notation_exprs (SWITCH_STMT_BODY (t));
	}
      else
	t = fix_array_notation_exprs (t);
      return t;

    case WHILE_STMT:
      t = fix_conditional_array_notations_1 (t);
      /* If the above function added some extra instructions above the original
	 while statement, then we can't assume it is still WHILE_STMTso we
	 have to check again.  */
      if (TREE_CODE (t) == WHILE_STMT)
	{
	  if (WHILE_BODY (t))
	    WHILE_BODY (t) = fix_array_notation_exprs (WHILE_BODY (t));
	}
      else
	t = fix_array_notation_exprs (t);
      return t;
      
    case DO_STMT:
      t = fix_conditional_array_notations_1 (t);
      /* If the above function added some extra instructions above the original
	 do-while statement, then we can't assume it is still DO_STMT so we
	 have to check again.  */
      if (TREE_CODE (t) == DO_STMT)
	{      
	  if (DO_BODY (t))
	    DO_BODY (t) = fix_array_notation_exprs (DO_BODY (t));
	}
      else
	t = fix_array_notation_exprs (t);
      return t;
      
    default:
      if (is_expr)
	{
	  int i, len;

	  /* Walk over all the sub-trees of this operand.  */
	  len = TREE_CODE_LENGTH (code);

	  /* Go through the subtrees.  We need to do this in forward order so
	     that the scope of a FOR_EXPR is handled properly.  */
	  for (i = 0; i < len; ++i)
	    TREE_OPERAND (t, i) =
	      fix_array_notation_exprs (TREE_OPERAND (t, i));
	}
      return t;
    }
  return t;
}

/* Replace array notation's built-in function passed in AN_BUILTIN_FN with
   the appropriate loop and computation (all stored in variable LOOP of type
   tree node).  The output of the function function is always a scalar and that
   result is returned in *NEW_VAR.  *NEW_VAR is NULL_TREE if the function is
   __sec_reduce_mutating.  */

static tree
fix_builtin_array_notation_fn (tree an_builtin_fn, tree *new_var)
{
  tree new_var_type = NULL_TREE, func_parm, new_expr, new_yes_expr, new_no_expr;
  tree array_ind_value = NULL_TREE, new_no_ind, new_yes_ind, new_no_list;
  tree new_yes_list, new_cond_expr; 
  tree new_var_init = NULL_TREE, new_exp_init = NULL_TREE;
  an_reduce_type an_type = REDUCE_UNKNOWN;
  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  int s_jj = 0;
  size_t list_size = 0, rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, jj_tree, loop, comp_stmt;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr, array_op0;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init, comp_node;
  tree call_fn = NULL_TREE, identity_value = NULL_TREE, new_call_expr;
  bool **count_down, **array_vector;
  char label_name[50];
  location_t location = UNKNOWN_LOCATION;
  
  if (!is_builtin_array_notation_fn (CALL_EXPR_FN (an_builtin_fn), &an_type))
    return NULL_TREE;

  if (an_type != REDUCE_CUSTOM && an_type != REDUCE_MUTATING)
    func_parm = CALL_EXPR_ARG (an_builtin_fn, 0);
  else
    {
      call_fn = CALL_EXPR_ARG (an_builtin_fn, 2);

      /* We need to do this because we are "faking" the builtin function types,
	 so the compiler does a bunch of typecasts and this will get rid of
	 all that!  */
      while (TREE_CODE (call_fn) == CONVERT_EXPR
	     || TREE_CODE (call_fn) == NOP_EXPR)
	call_fn = TREE_OPERAND (call_fn, 0);

      if (TREE_CODE (call_fn) != OVERLOAD
	  && TREE_CODE (call_fn) != FUNCTION_DECL)
	call_fn = TREE_OPERAND (call_fn, 0);
      identity_value = CALL_EXPR_ARG (an_builtin_fn, 0);
      func_parm = CALL_EXPR_ARG (an_builtin_fn, 1);

      /* We need to do this because we are "faking" the builtin function types
	 so the compiler does a bunch of typecasts and this will get rid of
	 all that!  */
      while (TREE_CODE (identity_value) == CONVERT_EXPR
	     || TREE_CODE (identity_value) == NOP_EXPR)
	identity_value = TREE_OPERAND (identity_value, 0);
    }

  while (TREE_CODE (func_parm) == CONVERT_EXPR
	 || TREE_CODE (func_parm) == NOP_EXPR)
    func_parm = TREE_OPERAND (func_parm, 0);
  
  find_rank (an_builtin_fn, false, &rank);

  location = EXPR_LOCATION (an_builtin_fn);
  
  if (rank == 0)
    return an_builtin_fn;
  else if (rank > 1 
	   && (an_type == REDUCE_MAX_INDEX  || an_type == REDUCE_MIN_INDEX))
    { 
      error_at (location, "__sec_reduce_min_ind or __sec_reduce_max_ind cannot "
		"have arrays with dimension greater than 1.");
      return error_mark_node;
    }
  
  extract_array_notation_exprs (func_parm, true, &array_list);
  list_size = vec_safe_length (array_list);
    switch (an_type)
    {
    case REDUCE_ADD:
    case REDUCE_MUL:
    case REDUCE_MAX:
    case REDUCE_MIN:
      new_var_type = ARRAY_NOTATION_TYPE ((*array_list)[0]);
      break;
    case REDUCE_ALL_ZEROS:
    case REDUCE_ANY_ZEROS:
    case REDUCE_ANY_NONZEROS:
    case REDUCE_ALL_NONZEROS:
      new_var_type = integer_type_node;
      break;
    case REDUCE_MAX_INDEX:
    case REDUCE_MIN_INDEX:
      new_var_type = size_type_node;
      break;
    case REDUCE_CUSTOM:
      if (call_fn && identity_value)
	new_var_type = ARRAY_NOTATION_TYPE ((*array_list)[0]);
      break;
    case REDUCE_MUTATING:
      new_var_type = NULL_TREE;
      break;
    default:
      gcc_unreachable (); /* You should never reach here.  */
    }
    
  if (new_var_type && TREE_CODE (new_var_type) == ARRAY_TYPE)
    new_var_type = TREE_TYPE (new_var_type);
  
  array_ops = XNEWVEC (tree *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = XNEWVEC (tree,  rank);
  
  array_vector = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = XNEWVEC (bool, rank);

  array_value =  XNEWVEC (tree *, list_size);
  array_stride =  XNEWVEC (tree *, list_size);
  array_length =  XNEWVEC (tree *, list_size);
  array_start =  XNEWVEC (tree *, list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = XNEWVEC (tree, rank);
      array_stride[ii] = XNEWVEC (tree, rank);
      array_length[ii] = XNEWVEC (tree, rank);
      array_start[ii]  = XNEWVEC (tree, rank);
    }

  body_label = XNEWVEC (tree, rank);
  body_label_expr = XNEWVEC (tree, rank);
  exit_label = XNEWVEC (tree, rank);
  exit_label_expr = XNEWVEC (tree, rank);
  compare_expr = XNEWVEC (tree, rank);
  if_stmt_label = XNEWVEC (tree, rank);
  
  expr_incr = XNEWVEC (tree, rank);
  ind_init = XNEWVEC (tree, rank);
  
  count_down = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = XNEWVEC (bool, rank);
  
  array_var = XNEWVEC (tree, rank);

  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      for (jj_tree = (*array_list)[ii];
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE ((*array_list)[ii]) == ARRAY_NOTATION_REF)
	{
	  for (jj = 0; jj < rank; jj++)
	    {
	      if (TREE_CODE (array_ops[ii][jj]) == ARRAY_NOTATION_REF)
		{
		  array_value[ii][jj] =
		    ARRAY_NOTATION_ARRAY (array_ops[ii][jj]);
		  array_start[ii][jj] =
		    ARRAY_NOTATION_START (array_ops[ii][jj]);
		  array_length[ii][jj] =
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_LENGTH (array_ops[ii][jj]));
		  array_stride[ii][jj] =
		    ARRAY_NOTATION_STRIDE (array_ops[ii][jj]);
		  array_vector[ii][jj] = true;

		  if (!TREE_CONSTANT (array_length[ii][jj])
		      || TREE_CODE (array_length[ii][jj]) != INTEGER_TYPE)
		      count_down[ii][jj] = false;
		  else if (tree_int_cst_sgn (array_length[ii][jj]) == -1)
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }

  loop = push_stmt_list ();


  for (ii = 0; ii < rank; ii++)
    {
      array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				  TREE_TYPE (array_start[0][ii]));
      ind_init[ii] = build_x_modify_expr
	(location, array_var[ii], NOP_EXPR, 
	 build_zero_cst (TREE_TYPE (array_var[ii])), tf_warning_or_error);
    }

  for (ii = 0; ii < rank ; ii++)
    {
      array_notation_label_no++;
      memset (label_name, 0, 50);
      sprintf (label_name, "if_stmt_label_%05d", array_notation_label_no);
      
      /* This will create the if statement label.  */
      if_stmt_label[ii] =
	define_label (location, get_identifier (label_name));
      
      memset (label_name, 0, 50);
      sprintf (label_name, "body_label_%05d", array_notation_label_no);
      /* This label statment will point to the loop body.  */
      body_label[ii] = define_label (location,
				     get_identifier (label_name));
      body_label_expr[ii] = build_stmt (location, LABEL_EXPR,
					body_label[ii]);
      
      /* This will create the exit label..i.e. where the while loop will branch
	 out of.  */
      memset (label_name, 0, 50);
      sprintf (label_name, "exit_label_%05d", array_notation_label_no);
      exit_label[ii] = define_label (location,
				     get_identifier (label_name));
      exit_label_expr[ii] = build_stmt (location, LABEL_EXPR, 
					exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  tree array_opr = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      tree stride = NULL_TREE, var = NULL_TREE, start = NULL_TREE;

	      /* If stride and start are of same type and the induction var
		 is not, we convert induction variable to stride's type.  */
	      if ((TREE_TYPE (array_start[ii][s_jj]) ==
		   TREE_TYPE (array_stride[ii][s_jj]))
		  && (TREE_TYPE (array_stride[ii][s_jj]) !=
		      TREE_TYPE (array_var[s_jj])))
		{
		  start = array_start[ii][s_jj];
		  stride = array_stride[ii][s_jj];
		  var =
		    build_c_cast (location, TREE_TYPE (array_stride[ii][s_jj]),
				  array_var[s_jj]);
		}
	      else if (TREE_TYPE (array_start[ii][s_jj]) !=
			TREE_TYPE (array_stride[ii][s_jj]))
		{
		  /* If we reach here, then the stride and start are of
		     different types, and so it doesn't really matter what
		     the induction variable type is, we stay safe and convert
		     everything to integer.  The reason why we pick integer
		     instead of something like size_t is because the stride
		     and length can be + or -.  */
		  start = build_c_cast (location, integer_type_node,
					array_start[ii][s_jj]);
		  stride = build_c_cast (location, integer_type_node,
					 array_stride[ii][s_jj]);
		  var = build_c_cast (location, integer_type_node,
				      array_var[s_jj]);
		}
	      else
		{
		  start = array_start[ii][s_jj];
		  stride = array_stride[ii][s_jj];
		  var = array_var[s_jj];
		}
	      if (count_down[ii][s_jj])
		/* Array[start_index - (induction_var * stride)] */
		array_opr = grok_array_decl
		  (location, array_opr,
		   build2 (MINUS_EXPR, TREE_TYPE (var), start,
			   build2 (MULT_EXPR, TREE_TYPE (var), var, stride)));	
	      else
		/* Array[start_index + (induction_var * stride)] */
		array_opr = grok_array_decl
		  (location, array_opr,
		   build2 (PLUS_EXPR, TREE_TYPE (var), start,
			   build2 (MULT_EXPR, TREE_TYPE (var), var, stride)));
	    }
	  vec_safe_push (array_operand, array_opr);
	}
      else
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&func_parm, true, array_list, array_operand);
  
  if (!TREE_TYPE (func_parm))      
    TREE_TYPE (func_parm) = ARRAY_NOTATION_TYPE ((*array_list)[0]);
  
  for (ii = 0; ii < rank; ii++)
    if (count_down[0][ii])
      expr_incr[ii] = build_x_unary_op (location, POSTDECREMENT_EXPR,
					array_var[ii], tf_warning_or_error);
    else
      expr_incr[ii] = build_x_unary_op (location, POSTINCREMENT_EXPR,
					array_var[ii], tf_warning_or_error);
  
  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] = build_x_binary_op
	      (location, GT_EXPR, array_var[jj],
	       TREE_CODE (array_var[jj]),
	       array_length[0][jj], TREE_CODE (array_length[0][jj]), NULL,
	       tf_warning_or_error);
	  else
	    compare_expr[jj] = build_x_binary_op
	      (location, LT_EXPR, array_var[jj],
	       TREE_CODE (array_var[jj]),
	       array_length[0][jj], TREE_CODE (array_length[0][jj]), NULL,
	       tf_warning_or_error);
	} 
    }

  if (an_type != REDUCE_MUTATING)
    {
      if (processing_template_decl)
	*new_var = build_decl (location, VAR_DECL, NULL_TREE, new_var_type);
      else
	*new_var = create_tmp_var (new_var_type, NULL);
       	
      gcc_assert (*new_var && (*new_var != error_mark_node));
    }
  else
    /* We do not require a new variable for mutating.  The "identity value"
       itself is a variable.  */
    *new_var = NULL_TREE;
  
  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    {
      array_ind_value = create_tmp_var (TREE_TYPE (func_parm), NULL);
      gcc_assert (array_ind_value && (array_ind_value != error_mark_node));
      DECL_INITIAL (array_ind_value) = NULL_TREE;
      pushdecl (array_ind_value);
    }
  array_op0 = (*array_operand)[0];
  if (an_type == REDUCE_ADD)
    {
      if (ARITHMETIC_TYPE_P (new_var_type))
	new_var_init = build_x_modify_expr (location, *new_var, 
					    NOP_EXPR,
					    build_zero_cst (new_var_type), 1);
      else
	new_var_init = build_x_modify_expr (location, *new_var, 
					    NOP_EXPR,
					    integer_zero_node, 1);
      new_expr = build_x_modify_expr (location, *new_var, PLUS_EXPR, 
				      func_parm, 1);
    }
  else if (an_type == REDUCE_MUL)
    {
      if (ARITHMETIC_TYPE_P (new_var_type))
	new_var_init = build_x_modify_expr (location, *new_var, 
					    NOP_EXPR,
					    build_one_cst (new_var_type), 1);
      else
	new_var_init = build_x_modify_expr (location, *new_var, 
					    NOP_EXPR,
					    integer_one_node, 1);
      new_expr = build_x_modify_expr (location, *new_var, MULT_EXPR, 
				      func_parm, 1);
    }
  else if (an_type == REDUCE_ALL_ZEROS)
    {
      new_var_init = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  build_one_cst (new_var_type), 1);
      /* Initially you assume everything is zero, now if we find a case where
	 it is NOT true, then we set the result to false. Otherwise we just
	 keep the previous value.  */
      new_yes_expr = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  build_zero_cst (new_var_type), 1);
      new_no_expr = build_x_modify_expr (location, *new_var, NOP_EXPR, 
					 *new_var, 1);
      if (ARITHMETIC_TYPE_P (TREE_TYPE (func_parm)))
	comp_node = build_zero_cst (TREE_TYPE (func_parm));
      else
	comp_node = integer_zero_node;
      new_cond_expr = build_x_binary_op
	(location, NE_EXPR, func_parm, TREE_CODE (func_parm), comp_node,
	 TREE_CODE (comp_node), NULL, tf_warning_or_error);
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_expr, new_no_expr, 
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_ALL_NONZEROS)
    {
      new_var_init = build_x_modify_expr
	(location, *new_var, NOP_EXPR, build_one_cst (new_var_type), 1);
      /* Initially you assume everything is non-zero, now if we find a case
	 where it is NOT true, then we set the result to false. Otherwise we
	 just keep the previous value.  */
      new_yes_expr = build_x_modify_expr
	(location, *new_var, NOP_EXPR, 
	 build_zero_cst (TREE_TYPE (*new_var)), 1);
      new_no_expr = build_x_modify_expr (location, *new_var, NOP_EXPR, 
					 *new_var, 1);
      if (ARITHMETIC_TYPE_P (TREE_TYPE (func_parm)))
	comp_node = build_zero_cst (TREE_TYPE (func_parm));
      else
	comp_node = integer_zero_node;
      new_cond_expr = build_x_binary_op
	(location, EQ_EXPR, func_parm, TREE_CODE (func_parm), comp_node,
	 TREE_CODE (comp_node), NULL, tf_warning_or_error);
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_expr, new_no_expr, 
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_ANY_ZEROS)
    {
      new_var_init = build_x_modify_expr
	(location, *new_var, NOP_EXPR, 
	 build_zero_cst (new_var_type), 1);
      /* Initially we assume there are NO zeros in the list. When we find a
	 non-zero, we keep the previous value. If we find a zero, we set the
	 value to true.  */
      new_no_expr = build_x_modify_expr
	(location, *new_var, NOP_EXPR, 
	 build_one_cst (TREE_TYPE (*new_var)), 1);
      new_yes_expr = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  *new_var, 1);
      if (ARITHMETIC_TYPE_P (TREE_TYPE (func_parm)))
	comp_node = build_zero_cst (TREE_TYPE (func_parm));
      else
	comp_node = integer_zero_node;
      new_cond_expr = build_x_binary_op
	(location, EQ_EXPR, func_parm, TREE_CODE (func_parm), comp_node,
	 TREE_CODE (comp_node), NULL, tf_warning_or_error);
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_expr, new_no_expr, 
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_ANY_NONZEROS)
    {
      new_var_init = build_x_modify_expr
	(location, *new_var, NOP_EXPR, 
	 build_zero_cst (new_var_type), 1);
      /* Initially we assume there are NO non-zeros in the list. When we find a
	 zero, we keep the previous value. If we find a zero, we set the value
	 to true.  */
      new_no_expr = build_x_modify_expr
	(location, *new_var, NOP_EXPR, 
	 build_one_cst (TREE_TYPE (*new_var)), 1);
      new_yes_expr = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  *new_var, 1);
      if (ARITHMETIC_TYPE_P (TREE_TYPE (func_parm)))
	comp_node = build_zero_cst (TREE_TYPE (func_parm));
      else
	comp_node = integer_zero_node;
      new_cond_expr = build_x_binary_op
	(location, NE_EXPR, func_parm, TREE_CODE (func_parm), comp_node,
	 TREE_CODE (comp_node), NULL, tf_warning_or_error); 
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_expr, new_no_expr,
					   tf_warning_or_error);      
    }
  else if (an_type == REDUCE_MAX)
    {
      /* Set initial value as the first element in the list.  */
      new_var_init = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  func_parm, 1);
      new_no_expr  = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  *new_var, 1);
      new_yes_expr = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  func_parm, 1);
      new_cond_expr = build_x_binary_op (location, LT_EXPR, *new_var,
					 TREE_CODE (*new_var), func_parm,
					 TREE_CODE (func_parm), NULL,
					 tf_warning_or_error);
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_expr, new_no_expr,
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_MIN)
    {
      new_var_init = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  func_parm, 1);
      new_no_expr  = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  *new_var, 1);
      new_yes_expr = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  func_parm, 1);
      new_cond_expr = build_x_binary_op (location, GT_EXPR, *new_var,
					 TREE_CODE (*new_var), func_parm,
					 TREE_CODE (func_parm), NULL, 1);
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_expr, new_no_expr,
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_MAX_INDEX)
    {
      new_var_init = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  array_var[0], tf_warning_or_error);
      new_exp_init = build_x_modify_expr (location, array_ind_value, 
					  NOP_EXPR, func_parm, 
					  tf_warning_or_error);
      new_no_ind   = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  *new_var, tf_warning_or_error);
      new_no_expr  = build_x_modify_expr (location, array_ind_value, 
					  NOP_EXPR,
					  array_ind_value, tf_warning_or_error);
      if (list_size > 1) /* This means there is more than 1.  */
	new_yes_ind  = build_x_modify_expr (location, *new_var, 
					    NOP_EXPR, array_var[0],
					    tf_warning_or_error);
      else
	new_yes_ind  = build_x_modify_expr
	  (location, *new_var, NOP_EXPR, 
	   TREE_OPERAND (array_op0, 1), tf_warning_or_error);
      new_yes_expr = build_x_modify_expr (location, array_ind_value, 
					  NOP_EXPR, func_parm,
					  tf_warning_or_error);
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);

      new_cond_expr = build_x_binary_op
	(location, LT_EXPR, array_ind_value,
	 TREE_CODE (array_ind_value),
	 func_parm, TREE_CODE (func_parm), NULL, tf_warning_or_error);
      new_expr = build_x_conditional_expr (location, new_cond_expr, 
					   new_yes_list, new_no_list, 
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_MIN_INDEX)
    {
      new_var_init = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  array_var[0], 1);
      new_exp_init = build_x_modify_expr (location, array_ind_value, 
					  NOP_EXPR, func_parm, 1);
      new_no_ind   = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  *new_var, 1);
      new_no_expr  = build_x_modify_expr (location, array_ind_value, 
					  NOP_EXPR, array_ind_value, 1);
      if (list_size > 1)
	new_yes_ind  = build_x_modify_expr (location, *new_var, 
					    NOP_EXPR, array_var[0], 1);
      else
	new_yes_ind  = build_x_modify_expr
	  (location, *new_var, NOP_EXPR, TREE_OPERAND (array_op0, 1), 1);
      new_yes_expr = build_x_modify_expr (location, array_ind_value, 
					  NOP_EXPR, func_parm, 1);
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
      new_cond_expr = build_x_binary_op
	(location, GT_EXPR, array_ind_value,
	 TREE_CODE (array_ind_value),
	 func_parm, TREE_CODE (func_parm), NULL, tf_warning_or_error);
      new_expr = build_x_conditional_expr (location, new_cond_expr,
					   new_yes_list, new_no_list, 
					   tf_warning_or_error);
    }
  else if (an_type == REDUCE_CUSTOM)
    {
      vec<tree, va_gc> *func_args;
      func_args = make_tree_vector ();
      vec_safe_push (func_args, *new_var);
      vec_safe_push (func_args, func_parm);

      new_var_init = build_x_modify_expr (location, *new_var, NOP_EXPR,
					  identity_value, tf_warning_or_error);
      new_call_expr = finish_call_expr (call_fn, &func_args, false, true,
					CILK_CALL_NORMAL, tf_warning_or_error);
      new_expr = build_x_modify_expr (location, *new_var, NOP_EXPR,
				      new_call_expr, tf_warning_or_error);
     }
  else if (an_type == REDUCE_MUTATING)
    {
      vec<tree, va_gc> *func_args;

      func_args = make_tree_vector (); 
      vec_safe_push (func_args, identity_value); 
      vec_safe_push (func_args, func_parm);
      new_expr = finish_call_expr (call_fn, &func_args, false, true,
				   CILK_CALL_NORMAL, tf_warning_or_error);
    }
  else
    gcc_unreachable ();

  /* We do it twice here so that the new var initialization done below will be
     correct.  */
  for (ii = 0; ii < rank; ii++)
    add_stmt (build_stmt (location, EXPR_STMT, ind_init[ii]));

  if (an_type != REDUCE_MUTATING)
    add_stmt (build_stmt (location, EXPR_STMT, new_var_init));

  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    add_stmt (build_stmt (location, EXPR_STMT, new_exp_init));
  
  for (ii = 0; ii < rank; ii++)
    {
      add_stmt (build_stmt (location, EXPR_STMT, ind_init[ii]));
      add_stmt (build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]));
      comp_stmt = begin_if_stmt ();
      finish_if_stmt_cond (compare_expr[ii], comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, body_label[ii]));
      finish_then_clause (comp_stmt);
      begin_else_clause (comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, exit_label[ii]));
      finish_else_clause (comp_stmt);
      finish_if_stmt (comp_stmt);
      add_stmt (build_stmt (location, LABEL_EXPR, body_label[ii]));
    }

  add_stmt (build_stmt (location, EXPR_STMT, new_expr));
  
  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      add_stmt (build_stmt (location, EXPR_STMT, expr_incr[s_jj]));
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (build_stmt (location, LABEL_EXPR, exit_label[s_jj]));
    }

  loop = pop_stmt_list (loop);
  
  XDELETEVEC (body_label);
  XDELETEVEC (body_label_expr);
  XDELETEVEC (exit_label);
  XDELETEVEC (exit_label_expr);
  XDELETEVEC (compare_expr);
  XDELETEVEC (if_stmt_label);
  XDELETEVEC (expr_incr);
  XDELETEVEC (ind_init);
  XDELETEVEC (array_var);
  
  for (ii = 0; ii < list_size; ii++)
    {
      XDELETEVEC (count_down[ii]);
      XDELETEVEC (array_value[ii]);
      XDELETEVEC (array_stride[ii]);
      XDELETEVEC (array_length[ii]);
      XDELETEVEC (array_start[ii]);
      XDELETEVEC (array_ops[ii]);
      XDELETEVEC (array_vector[ii]);
    }

  XDELETEVEC (count_down);
  XDELETEVEC (array_value);
  XDELETEVEC (array_stride);
  XDELETEVEC (array_length);
  XDELETEVEC (array_start);
  XDELETEVEC (array_ops);
  XDELETEVEC (array_vector);

  
  return loop;
}

/* Transforms array notations inside unary expression ORIG_STMT with an
   appropriate loop and ARRAY_REF (and returns all this as a super-tree called
   LOOP).  */

tree
fix_unary_array_notation_exprs (tree orig_stmt)
{

  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  int s_jj = 0;
  size_t list_size = 0, rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, jj_tree, loop, array_opr;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init, comp_stmt;
  bool **count_down, **array_vector;
  tree builtin_loop, stmt = NULL_TREE, new_var = NULL_TREE;
  char label_name[50];
  location_t location = UNKNOWN_LOCATION;
  
  find_rank (orig_stmt, false, &rank);
  if (rank == 0)
    return orig_stmt;  
  
  extract_array_notation_exprs (orig_stmt, false, &array_list);
  list_size = vec_safe_length (array_list);
  location = EXPR_LOCATION (orig_stmt);
  stmt = NULL_TREE;
  for (ii = 0; ii < list_size; ii++)
    {
      tree list_node = (*array_list)[ii];
      if (TREE_CODE (list_node) == CALL_EXPR
	  || TREE_CODE (list_node) == AGGR_INIT_EXPR)
	{
	  builtin_loop =
	    fix_builtin_array_notation_fn (list_node, &new_var);
	  if (builtin_loop == error_mark_node)
	    return error_mark_node;
	  else if (builtin_loop)
	    {
	      vec<tree, va_gc> *sub_list = NULL, *new_var_list = NULL;
	      stmt = alloc_stmt_list ();
	      append_to_statement_list_force (builtin_loop, &stmt);
	      vec_safe_push (sub_list, list_node);
	      vec_safe_push (new_var_list, new_var);
	      replace_array_notations (&orig_stmt, false, sub_list,
				       new_var_list);
	    }
	}
    }
  if (stmt != NULL_TREE)
    append_to_statement_list_force (finish_expr_stmt (orig_stmt), &stmt);
  else
    stmt = orig_stmt;
  rank = 0;
  list_size = 0;
  array_list = NULL;
  extract_array_notation_exprs (stmt, true, &array_list);
  list_size = vec_safe_length (array_list);
  find_rank (stmt, true, &rank);
  if (rank == 0)
    return stmt;
  
  if (list_size == 0)
    return stmt;

  array_ops = XNEWVEC (tree *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = XNEWVEC (tree, rank);
  
  array_vector = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = XNEWVEC (bool, rank);

  array_value = XNEWVEC (tree *, list_size);
  array_stride = XNEWVEC (tree *, list_size);
  array_length = XNEWVEC (tree *, list_size);
  array_start = XNEWVEC (tree *, list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = XNEWVEC (tree, rank);
      array_stride[ii] = XNEWVEC (tree, rank);
      array_length[ii] = XNEWVEC (tree, rank);
      array_start[ii]  = XNEWVEC (tree, rank);
    }

  body_label = XNEWVEC (tree, rank);
  body_label_expr = XNEWVEC (tree, rank);
  exit_label = XNEWVEC (tree, rank);
  exit_label_expr = XNEWVEC (tree, rank);
  compare_expr = XNEWVEC (tree, rank);
  if_stmt_label = XNEWVEC (tree, rank);
  
  expr_incr = XNEWVEC (tree, rank);
  ind_init = XNEWVEC (tree, rank);
  array_var = XNEWVEC (tree, rank);
  
  count_down = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++) 
    count_down[ii] = XNEWVEC (bool,  rank);
  
  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      jj_tree = (*array_list)[ii];
      while (jj_tree && (TREE_CODE (jj_tree) == ARRAY_NOTATION_REF
			 || TREE_CODE (jj_tree) == NON_DEPENDENT_EXPR))
	{
	  if (TREE_CODE (jj_tree) == ARRAY_NOTATION_REF)
	    {
	      array_ops[ii][jj] = jj_tree;
	      jj++;
	      jj_tree = ARRAY_NOTATION_ARRAY (jj_tree);
	    }
	  else
	    jj_tree = TREE_OPERAND (jj_tree, 0);
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE ((*array_list)[ii]) == ARRAY_NOTATION_REF)
	{
	  for (jj = 0; jj < rank; jj++)
	    {
	      if (TREE_CODE (array_ops[ii][jj]) == ARRAY_NOTATION_REF)
		{
		  array_value[ii][jj] =
		    ARRAY_NOTATION_ARRAY (array_ops[ii][jj]);
		  array_start[ii][jj] =
		    ARRAY_NOTATION_START (array_ops[ii][jj]);
		  array_length[ii][jj] =
		    ARRAY_NOTATION_LENGTH (array_ops[ii][jj]);
		  array_stride[ii][jj] =
		    ARRAY_NOTATION_STRIDE (array_ops[ii][jj]);
		  array_vector[ii][jj] = true;

		  if (!TREE_CONSTANT (array_length[ii][jj])
		      || TREE_CODE (array_length[ii][jj]) == VAR_DECL)
		    count_down[ii][jj] = false;
		  else if (!integer_zerop (array_length[ii][jj])
		      && !integer_nonzerop (array_length[ii][jj]))
		      count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (array_length[ii][jj],
			    build_zero_cst (TREE_TYPE (array_length[ii][jj]))))
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }

  loop = push_stmt_list ();

  for (ii = 0; ii < rank; ii++)
    {
      array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				  TREE_TYPE (array_start[0][ii]));
      ind_init[ii] = build_x_modify_expr
	(location, array_var[ii], NOP_EXPR, 
	 build_zero_cst (TREE_TYPE (array_var[ii])), tf_warning_or_error);
    }

  for (ii = 0; ii < rank ; ii++)
    {
      array_notation_label_no++;
      memset (label_name, 0, 50);
      sprintf (label_name, "if_stmt_label_%05d", array_notation_label_no);
      
      /* This will create the if statement label.  */
      if_stmt_label[ii] =
	define_label (location, get_identifier (label_name));
      
      memset (label_name, 0, 50);
      sprintf (label_name, "body_label_%05d", array_notation_label_no);
      /* This label statment will point to the loop body.  */
      body_label[ii] = define_label (location,
				     get_identifier (label_name));
      body_label_expr[ii] = build_stmt (location, LABEL_EXPR,
					body_label[ii]);
      
      /* This will create the exit label..i.e. where the while loop will branch
	 out of.  */
      memset (label_name, 0, 50);
      sprintf (label_name, "exit_label_%05d", array_notation_label_no);
      exit_label[ii] = define_label (location,
				     get_identifier (label_name));
      exit_label_expr[ii] = build_stmt (location, LABEL_EXPR,
					 exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  array_opr = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      tree stride = NULL_TREE, var = NULL_TREE, start = NULL_TREE;
	      if ((TREE_TYPE (array_start[ii][s_jj]) ==
		   TREE_TYPE (array_stride[ii][s_jj]))
		  && (TREE_TYPE (array_stride[ii][s_jj]) !=
		      TREE_TYPE (array_var[s_jj])))
		{
		  /* If stride and start are of same type and the induction var
		     is not, we convert induction variable to stride's
		     type.  */
		  start = array_start[ii][s_jj];
		  stride = array_stride[ii][s_jj];
		  var =
		    build_c_cast (location, TREE_TYPE (array_stride[ii][s_jj]),
				  array_var[s_jj]);
		}
	      else if (TREE_TYPE (array_start[ii][s_jj]) !=
			TREE_TYPE (array_stride[ii][s_jj]))
		{
		  /* If we reach here, then the stride and start are of
		     different types, and so it doesn't really matter what
		     the induction variable type is, we stay safe and convert
		     everything to integer.  The reason why we pick integer
		     instead of something like size_t is because the stride
		     and length can be + or -.  */
		  start = build_c_cast (location, integer_type_node,
					array_start[ii][s_jj]);
		  stride = build_c_cast (location, integer_type_node,
					 array_stride[ii][s_jj]);
		  var = build_c_cast (location, integer_type_node,
				      array_var[s_jj]);
		}
	      else
		{
		  start = array_start[ii][s_jj];
		  stride = array_stride[ii][s_jj];
		  var = array_var[s_jj];
		}
	      if (count_down[ii][s_jj])
		/* Array[start_index - (induction_var * stride)] */
		array_opr = grok_array_decl
		  (location, array_opr,
		   build2 (MINUS_EXPR, TREE_TYPE (var), start,
			   build2 (MULT_EXPR, TREE_TYPE (var), var, stride)));	
	      else
		/* Array[start_index + (induction_var * stride)] */
		array_opr = grok_array_decl
		  (location, array_opr,
		   build2 (PLUS_EXPR, TREE_TYPE (var), start,
			   build2 (MULT_EXPR, TREE_TYPE (var), var, stride)));
	    }
	  vec_safe_push (array_operand, array_opr);  
	}
      else
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&stmt, true, array_list, array_operand);

  for (ii = 0; ii < rank; ii++)
    if (count_down[0][ii])
      expr_incr[ii] = build_x_unary_op (location, POSTDECREMENT_EXPR,
					array_var[ii], tf_warning_or_error);
    else
      expr_incr[ii] = build_x_unary_op (location, POSTINCREMENT_EXPR,
					array_var[ii], tf_warning_or_error);

  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] = build_x_binary_op
	      (location, GT_EXPR, array_var[jj],
	       TREE_CODE (array_var[jj]),
	       array_length[0][jj], TREE_CODE (array_length[0][jj]), NULL,
	       tf_warning_or_error);
	  else
	    compare_expr[jj] = build_x_binary_op
	      (location, LT_EXPR, array_var[jj],
	       TREE_CODE (array_var[jj]),
	       array_length[0][jj], TREE_CODE (array_length[0][jj]), NULL,
	       tf_warning_or_error);
	}
    }

  for (ii = 0; ii < rank; ii++)
    {
      add_stmt (build_stmt (location, EXPR_STMT, ind_init[ii]));
      add_stmt (build_stmt (location, LABEL_EXPR, if_stmt_label[ii]));
      comp_stmt = begin_if_stmt ();
      finish_if_stmt_cond (compare_expr[ii], comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, body_label[ii]));
      finish_then_clause (comp_stmt);
      begin_else_clause (comp_stmt);
      add_stmt (build1 (GOTO_EXPR, void_type_node, exit_label[ii]));
      finish_else_clause (comp_stmt);
      finish_if_stmt (comp_stmt);
      add_stmt (build_stmt (location, LABEL_EXPR, body_label[ii]));
    }
  
  add_stmt (build_stmt (location, EXPR_STMT, stmt));
  
  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      add_stmt (build_stmt (location, EXPR_STMT, expr_incr[s_jj]));
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (build_stmt (location, LABEL_EXPR, exit_label[s_jj]));
    }

  loop = pop_stmt_list (loop);

  XDELETEVEC (body_label);
  XDELETEVEC (body_label_expr);
  XDELETEVEC (exit_label);
  XDELETEVEC (exit_label_expr);
  XDELETEVEC (compare_expr);
  XDELETEVEC (if_stmt_label);
  XDELETEVEC (expr_incr);
  XDELETEVEC (ind_init);
  XDELETEVEC (array_var);

  if (list_size > 1)
    {
      for (ii = 0; ii < list_size; ii++)
	{
	  XDELETEVEC (count_down[ii]);
	  XDELETEVEC (array_value[ii]);
	  XDELETEVEC (array_stride[ii]);
	  XDELETEVEC (array_length[ii]);
	  XDELETEVEC (array_start[ii]);
	  XDELETEVEC (array_ops[ii]);
	  XDELETEVEC (array_vector[ii]);
	}
    }
  XDELETEVEC (count_down);
  XDELETEVEC (array_value);
  XDELETEVEC (array_stride);
  XDELETEVEC (array_length);
  XDELETEVEC (array_start);
  XDELETEVEC (array_ops);
  XDELETEVEC (array_vector);

  return loop;
}

/* Returns true of FUNC_NAME is a builtin array notation function.  The type of
   function is returned in *TYPE.  */

bool
is_builtin_array_notation_fn (tree func_name, an_reduce_type *type)
{
  const char *function_name = NULL;

  if (!func_name)
    return false;

  if (TREE_CODE (func_name) == IDENTIFIER_NODE)
    function_name = IDENTIFIER_POINTER (func_name);
  else if (TREE_CODE (func_name) == ADDR_EXPR)
    func_name = TREE_OPERAND (func_name, 0);
  
  if (TREE_CODE (func_name) == FUNCTION_DECL)
    function_name = IDENTIFIER_POINTER (DECL_NAME (func_name));
    
  
  if (!function_name)
    return false;

  if (!strcmp (function_name, "__sec_reduce_add"))
    {
      *type = REDUCE_ADD;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_mul"))
    {
      *type = REDUCE_MUL;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_all_zero"))
    {
      *type = REDUCE_ALL_ZEROS;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_all_nonzero"))
    {
      *type = REDUCE_ALL_NONZEROS;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_any_zero"))
    {
      *type = REDUCE_ANY_ZEROS;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_any_nonzero"))
    {
      *type = REDUCE_ANY_NONZEROS;
      return true;
    }  
  else if (!strcmp (function_name, "__sec_reduce_max"))
    {
      *type = REDUCE_MAX;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_min"))
    {
      *type = REDUCE_MIN;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_min_ind"))
    {
      *type = REDUCE_MIN_INDEX;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_max_ind"))
    {
      *type = REDUCE_MAX_INDEX;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce"))
    {
      *type = REDUCE_CUSTOM;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_mutating"))
    {
      *type = REDUCE_MUTATING;
      return true;
    }
  else
    {
      *type = REDUCE_UNKNOWN;
      return false;
    }
  return false;
}

/* Returns true of EXPR (and its subtrees) contain ARRAY_NOTATION_EXPR node.  */

bool
contains_array_notation_expr (tree expr)
{
  vec<tree, va_gc> *array_list = NULL;
  an_reduce_type type = REDUCE_UNKNOWN;

  if (!expr)
    return false;

  extract_array_notation_exprs (expr, false, &array_list);

  if (TREE_CODE (expr) == FUNCTION_DECL
      && is_builtin_array_notation_fn (expr, &type))
    return true;
  
  if (vec_safe_length (array_list) == 0)
    return false;
  else
    return true;
}

/* Returns true of NODE has a call_expression with ARRAY_NOTATION_REF tree.  */

static bool
has_call_expr_with_array_notation (tree node)
{
  int ii = 0;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
      
  if (!contains_array_notation_expr (node))
    return false;
  
  if (!node)
    return false;
  else if (TREE_CODE (node) == ARRAY_NOTATION_REF)
    return false;
  else if (TREE_CODE (node) == DECL_EXPR)
    {
      tree x = DECL_EXPR_DECL (node);
      if (x && TREE_CODE (x) != FUNCTION_DECL)
	if (DECL_INITIAL (x))
	  return has_call_expr_with_array_notation (DECL_INITIAL (x));
	
    }
  else if (TREE_CODE (node) == TREE_LIST)
    return (has_call_expr_with_array_notation (TREE_PURPOSE (node))
	    || has_call_expr_with_array_notation (TREE_VALUE (node)) 
	    || has_call_expr_with_array_notation (TREE_CHAIN (node)));
    
  else if (TREE_CODE (node) == STATEMENT_LIST)
    {
      tree_stmt_iterator ii_tsi;
      for (ii_tsi = tsi_start (node); !tsi_end_p (ii_tsi); tsi_next (&ii_tsi))
	return has_call_expr_with_array_notation (*tsi_stmt_ptr (ii_tsi));
    }
  else if (TREE_CODE (node) == CALL_EXPR)
    {
      if (is_builtin_array_notation_fn (CALL_EXPR_FN (node), &dummy_type))
	return true;
	
      if (is_sec_implicit_index_fn (CALL_EXPR_FN (node)))
	return true;
	 
      if (TREE_CODE (TREE_OPERAND (node, 0)) == INTEGER_CST)
	{
	  int length = TREE_INT_CST_LOW (TREE_OPERAND (node, 0));
	  bool x = false;
	  for (ii = 0; ii < length; ii++)
	    x |= contains_array_notation_expr (TREE_OPERAND (node, ii));
	  return x;
	}
      else
	gcc_unreachable  (); /* should not get here */
	  
    } 
  else
    {
      bool x = false;
      for (ii = 0; ii < TREE_CODE_LENGTH (TREE_CODE (node)); ii++) 
	x |= has_call_expr_with_array_notation (TREE_OPERAND (node, ii));
      return x;
    }
  return false;

}

/* Returns array notation expression for the array base ARRAY of type TYPE with
   start, index and stride given by STRIDE_INDEX, LENGTH and STRIDE,
   respectively.  */

tree
build_array_notation_ref (location_t loc, tree array, tree start_index,
			       tree length, tree stride, tree type)
{
  tree array_ntn_expr = NULL_TREE;
  size_t stride_rank = 0, length_rank = 0, start_rank = 0;
  tree subtype = type;
  /* If we are dealing with templates, then we will resolve the type checking
     at a later time.  */
  if (processing_template_decl || !type)
    {
      if (!type && TREE_TYPE (array))
	type = TREE_TYPE (array);
      array_ntn_expr = build_min_nt_loc (loc, ARRAY_NOTATION_REF, array,
					 start_index, length, stride, type,
					 NULL_TREE);
      TREE_TYPE (array_ntn_expr) = type;
    }
  if (!TREE_TYPE (start_index) || !INTEGRAL_TYPE_P (TREE_TYPE (start_index)))
    {
      error_at (loc,
		"start-index of array notation triplet is not an integer.");
      return error_mark_node;
    }
  if (!TREE_TYPE (length) || !INTEGRAL_TYPE_P (TREE_TYPE (length)))
    {
      error_at (loc, "length of array notation triplet is not an integer.");
      return error_mark_node;
    }
  if (!TREE_TYPE (stride) || !INTEGRAL_TYPE_P (TREE_TYPE (stride)))
    {
      error_at (loc, "stride of array notation triplet is not an integer.");
      return error_mark_node;
    }

  if (TREE_CODE (type) == FUNCTION_TYPE)
    {
      error_at (loc, "array notations cannot be used with function type.");
      return error_mark_node;
    }
    
  while (subtype && (TREE_CODE (subtype) == POINTER_TYPE
		     || TREE_CODE (subtype) == ARRAY_TYPE))
    {
      subtype = TREE_TYPE (subtype);
      if (subtype && TREE_CODE (subtype) == FUNCTION_TYPE)
	{
	  error_at (loc, "array notations cannot be used with function pointer "
		    "arrays.");
	  return error_mark_node;
	}
    }

  if (!stride)
    {
      if (TREE_CONSTANT (start_index) && TREE_CONSTANT (length)
	  && TREE_CODE (start_index) != VAR_DECL
	  && TREE_CODE (length) != VAR_DECL
	  && tree_int_cst_lt (length, start_index))
	stride = build_int_cst (TREE_TYPE (start_index), -1);
      else
	stride = build_int_cst (TREE_TYPE (start_index), 1);
    }

  find_rank (start_index, false, &start_rank);
  find_rank (length, false, &length_rank);
  find_rank (stride, false, &stride_rank);

  if (start_rank != 0)
    {
      error_at (loc, "rank of an array notation triplet's start-index is not "
		"zero.");
      return error_mark_node;
    }
  if (length_rank != 0)
    {
      error_at (loc, "rank of array notation triplet's length is not zero");
      return error_mark_node;
    }
  if (stride_rank != 0)
    {
      error_at (loc, "rank of array notation triplet's stride is not zero.");
      return error_mark_node;
    }
  if (!processing_template_decl)
    {
      array_ntn_expr = build5 (ARRAY_NOTATION_REF, NULL_TREE, NULL_TREE,
			       NULL_TREE, NULL_TREE, NULL_TREE, NULL_TREE);
      ARRAY_NOTATION_ARRAY (array_ntn_expr) = array;
      ARRAY_NOTATION_START (array_ntn_expr) = start_index;
      ARRAY_NOTATION_LENGTH (array_ntn_expr) = length;
      ARRAY_NOTATION_STRIDE (array_ntn_expr) = stride;
      if (type && (TREE_CODE (type) == ARRAY_TYPE
		   || TREE_CODE (type) == POINTER_TYPE))
	{
	  TREE_TYPE (array_ntn_expr) = TREE_TYPE (type);
	  ARRAY_NOTATION_TYPE (array_ntn_expr) = TREE_TYPE (type);
	}
      else
	{
	  TREE_TYPE (array_ntn_expr) = type;
	  ARRAY_NOTATION_TYPE (array_ntn_expr) = type;
	}

    }
  SET_EXPR_LOCATION (array_ntn_expr, loc);

  return array_ntn_expr;
}

/* This function will check if OP is a CALL_EXPR that is a builtin array
   notation function.  If so, ten it will set its type to be the type of
   array notation expression inside.  */

tree
find_correct_array_notation_type (tree op)
{
  tree fn_arg, return_type = NULL_TREE;
  an_reduce_type dummy = REDUCE_UNKNOWN;

  if (op)
    {
      return_type = TREE_TYPE (op); /* This is the default case.  */
      if (TREE_CODE (op) == CALL_EXPR)
	if (is_builtin_array_notation_fn (CALL_EXPR_FN (op), &dummy))
	  {
	    fn_arg = CALL_EXPR_ARG (op, 0);
	    if (fn_arg)
	      return_type = TREE_TYPE (fn_arg);
	  }
    }
  return return_type;
}

/* Expands the builtin functions in a return.  */

static tree
fix_return_expr (tree expr)
{
  tree new_mod_list, new_var, new_mod, retval_expr;
  location_t loc = EXPR_LOCATION (expr);

  if (TREE_CODE (expr) != RETURN_EXPR)
    return expr;

  new_mod_list = alloc_stmt_list ();
  retval_expr = TREE_OPERAND (expr, 0);
  new_var = build_decl (loc, VAR_DECL, NULL_TREE, TREE_TYPE (retval_expr));
  new_mod = build_x_array_notation_expr (loc, new_var, NOP_EXPR,
					 TREE_OPERAND (retval_expr, 1),
					 tf_warning_or_error);
  TREE_OPERAND (retval_expr, 1) = new_var;
  TREE_OPERAND (expr, 0) = retval_expr;
  append_to_statement_list_force (new_mod, &new_mod_list);
  append_to_statement_list_force (expr, &new_mod_list);
  return new_mod_list;
}
