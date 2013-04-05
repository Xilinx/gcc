/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains routines to handle Array Notation expression
   handling routines in the C Compiler.
   Copyright (C) 2011-2013  Free Software Foundation, Inc.
   Contributed by Balaji V. Iyer <balaji.v.iyer@intel.com>,
                  Intel Corporation.

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
#include "c-tree.h"
#include "tree-iterator.h"
#include "opts.h"
#include "gcc.h"
#include "c-family/c-common.h"

void replace_array_notations (tree *, bool, vec<tree, va_gc> *,
			      vec<tree, va_gc> *);
void find_rank (tree, bool, size_t *);
void extract_array_notation_exprs (tree, bool, vec<tree, va_gc> **);
tree fix_conditional_array_notations (tree);
struct c_expr fix_array_notation_expr (location_t, enum tree_code,
				       struct c_expr);
bool is_builtin_array_notation_fn (tree func_name, an_reduce_type *type);
static tree fix_builtin_array_notation_fn (tree an_builtin_fn, tree *new_var);
bool contains_array_notation_expr (tree expr);
tree expand_array_notation_exprs (tree t);
static tree fix_return_expr (tree expr);


struct inv_list
{
  vec<tree, va_gc> *list_values;
  vec<tree, va_gc> *replacement;
};


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
      for (ii_tree = array;
	   ii_tree && TREE_CODE (ii_tree) == ARRAY_NOTATION_REF;
	   ii_tree = ARRAY_NOTATION_ARRAY (ii_tree))
	current_rank++;
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
  else
    {
      if (TREE_CODE (array) == CALL_EXPR)
	{
	  tree func_name = CALL_EXPR_FN (array);
	  if (TREE_CODE (func_name) == ADDR_EXPR)
	    if (!ignore_builtin_fn)
	      if (is_builtin_array_notation_fn (func_name, &dummy_type))
		/* If it is a builtin function, then we know it returns a 
		   scalar.  */
		return;
	  if (TREE_CODE (TREE_OPERAND (array, 0)) == INTEGER_CST)
	    {
	      int length = TREE_INT_CST_LOW (TREE_OPERAND (array, 0));
	      for (ii = 0; ii < (size_t) length; ii++)
		find_rank (TREE_OPERAND (array, ii), ignore_builtin_fn, rank);
	    }
	  else
	    gcc_unreachable ();
	}
      else 
	for (ii = 0; ii < TREE_CODE_LENGTH (TREE_CODE (array)); ii++) 
	  find_rank (TREE_OPERAND (array, ii), ignore_builtin_fn, rank);
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
		*orig = node_replacement;
	      }
	  return;
	}
      if (TREE_CODE (TREE_OPERAND (*orig, 0)) == INTEGER_CST)
	{
	  int length = TREE_INT_CST_LOW (TREE_OPERAND (*orig, 0));
	  for (ii = 0; ii < (size_t) length; ii++)
	    replace_array_notations
	      (&TREE_OPERAND (*orig, ii), ignore_builtin_fn, list,
	       array_operand);
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
  if (TREE_CODE (*tp) != COMPOUND_EXPR && !contains_array_notation_expr (*tp))
    {
      vec_safe_push (i_list->list_values, *tp);
      *walk_subtrees = 0;
    }
  else if (TREE_CODE (*tp) == ARRAY_NOTATION_REF
	   || TREE_CODE (*tp) == ARRAY_REF
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
  walk_tree (node, find_inv_trees, (void *)&data, NULL);

  if (vec_safe_length (data.list_values))
    {
      node_list = push_stmt_list ();
      for (ix = 0; vec_safe_iterate (data.list_values, ix, &t); ix++)
	{
	  new_var = build_decl (EXPR_LOCATION (t), VAR_DECL, NULL_TREE,
				TREE_TYPE (t));
	  gcc_assert (new_var != NULL_TREE && new_var != error_mark_node);
	  new_node = build2 (MODIFY_EXPR, TREE_TYPE (t), new_var, t);
	  add_stmt (new_node);
	  vec_safe_push (data.replacement, new_var);
	}
      walk_tree (node, replace_inv_trees, (void *)&data, NULL);
      node_list = pop_stmt_list (node_list);
    }
  return node_list;
}
  

/* Returns a loop with ARRAY_REF inside it with an appropriate modify expr.
   The LHS and/or RHS will be array notation expressions that have a MODIFYCODE
   Their locations are specified by LHS_LOC, RHS_LOC.  The location of the
   modify expression is location.  The original type of LHS and RHS are passed
   in LHS_ORIGTYPE and RHS_ORIGTYPE.  */

tree
build_array_notation_expr (location_t location, tree lhs, tree lhs_origtype,
			   enum tree_code modifycode, location_t rhs_loc,
			   tree rhs, tree rhs_origtype)
{
  bool **lhs_vector = NULL, **rhs_vector = NULL, found_builtin_fn = false;
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
  tree scalar_mods = NULL_TREE;
  tree *lhs_expr_incr = NULL, *rhs_expr_incr = NULL;
  tree *lhs_ind_init = NULL, *rhs_ind_init = NULL;
  bool **lhs_count_down = NULL, **rhs_count_down = NULL;
  tree *lhs_compare = NULL, *rhs_compare = NULL;
  vec<tree, va_gc> *rhs_array_operand = NULL, *lhs_array_operand = NULL;
  size_t lhs_rank = 0, rhs_rank = 0;
  size_t ii = 0, jj = 0;
  int s_jj = 0;
  tree ii_tree = NULL_TREE, new_modify_expr;
  vec<tree, va_gc> *lhs_list = NULL, *rhs_list = NULL;
  tree new_var = NULL_TREE, builtin_loop = NULL_TREE;
  size_t rhs_list_size = 0, lhs_list_size = 0;
  tree begin_var, lngth_var, strde_var;
 
  /* If either of this is true, an error message must have been send out
     already.  Not necessary to send out multiple error messages.  */
  if (lhs == error_mark_node || rhs == error_mark_node)
    return error_mark_node;
  find_rank (rhs, false, &rhs_rank);
  
  extract_array_notation_exprs (rhs, false, &rhs_list);
  rhs_list_size = vec_safe_length (rhs_list);
  loop = push_stmt_list ();
  scalar_mods = replace_invariant_exprs (&rhs);
  if (scalar_mods)
    add_stmt (scalar_mods);
    
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
		  vec<tree, va_gc> *rhs_sub_list = NULL, *new_var_list = NULL;
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

  if (lhs_rank == 0 && rhs_rank == 0)
    {
      if (found_builtin_fn)
	{
	  new_modify_expr = build_modify_expr (location, lhs, lhs_origtype,
					       modifycode, rhs_loc, rhs,
					       rhs_origtype);
	  add_stmt (new_modify_expr);
	  pop_stmt_list (loop);
	  
	  return loop;
	}
      else
	{
	  pop_stmt_list (loop);
	  return NULL_TREE;
	}
    }
  rhs_list_size = 0;
  rhs_list = NULL;
  extract_array_notation_exprs (rhs, true, &rhs_list);
  extract_array_notation_exprs (lhs, true, &lhs_list);

  rhs_list_size = vec_safe_length (rhs_list);
  lhs_list_size = vec_safe_length (lhs_list);
  
  if (lhs_rank == 0 && rhs_rank != 0 && TREE_CODE (rhs) != CALL_EXPR)
    {
      tree rhs_base = rhs;
      for (ii = 0; ii < (size_t) rhs_rank; ii++)
	rhs_base = ARRAY_NOTATION_ARRAY (rhs);
      
      error_at (location, "%qD cannot be scalar when %qD is not", lhs,
		rhs_base);
      return error_mark_node;
    }
  if (lhs_rank != 0 && rhs_rank != 0 && lhs_rank != rhs_rank)
    {
      tree lhs_base = lhs;
      tree rhs_base = rhs;
      
      for (ii = 0; ii < lhs_rank; ii++)
	lhs_base = ARRAY_NOTATION_ARRAY (lhs_base);

      while (rhs_base && TREE_CODE (rhs_base) != ARRAY_NOTATION_REF)
	rhs_base = TREE_OPERAND (rhs_base, 0);
      for (ii = 0; ii < rhs_rank; ii++)
	rhs_base = ARRAY_NOTATION_ARRAY (rhs_base);
      
      error_at (location, "rank mismatch between %qD and %qD", lhs_base,
		rhs_base);
      return error_mark_node;
    }
  
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
      tree array_node = (*rhs_list)[ii];
      if (array_node && TREE_CODE (array_node) == ARRAY_NOTATION_REF)
	{
	  tree array_begin = ARRAY_NOTATION_START (array_node);
	  tree array_lngth = ARRAY_NOTATION_LENGTH (array_node);
	  tree array_strde = ARRAY_NOTATION_STRIDE (array_node);

	  begin_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
	  lngth_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
	  strde_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);

	  add_stmt (build_modify_expr (location, begin_var,
				       TREE_TYPE (begin_var),
				       NOP_EXPR, location, array_begin,
				       TREE_TYPE (array_begin)));
	  add_stmt (build_modify_expr (location, lngth_var,
				       TREE_TYPE (lngth_var),
				       NOP_EXPR, location, array_lngth,
				       TREE_TYPE (array_lngth)));
	  add_stmt (build_modify_expr (location, strde_var,
				       TREE_TYPE (strde_var),
				       NOP_EXPR, location, array_strde,
				       TREE_TYPE (array_strde)));
      
	  ARRAY_NOTATION_START (array_node) = begin_var;
	  ARRAY_NOTATION_LENGTH (array_node) = lngth_var;
	  ARRAY_NOTATION_STRIDE (array_node) = strde_var;
	}
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
  

  /* The reason why we are just using lhs_rank for this is because we have the
    following scenarios: 
    1. LHS_RANK == RHS_RANK
    2. LHS_RANK != RHS_RANK && RHS_RANK = 0 
    
    In both the scenarios, just checking the LHS_RANK is OK.  */

  body_label = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  body_label_expr = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  exit_label = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  exit_label_expr = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  cond_expr = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));
  if_stmt_label = XNEWVEC (tree, MAX (lhs_rank, rhs_rank));

  lhs_expr_incr = XNEWVEC (tree, lhs_rank);
  rhs_expr_incr =XNEWVEC (tree, rhs_rank);

  lhs_ind_init = XNEWVEC (tree, lhs_rank);
  rhs_ind_init = XNEWVEC (tree, rhs_rank);

  lhs_count_down = XNEWVEC (bool *, lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_count_down[ii] =  XNEWVEC (bool, lhs_rank);
  
  rhs_count_down =  XNEWVEC (bool *, rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_count_down[ii] = XNEWVEC (bool, rhs_rank);

  lhs_compare =  XNEWVEC (tree, lhs_rank);
  rhs_compare =  XNEWVEC (tree, rhs_rank);

  if (lhs_rank)
    {
      for (ii = 0; ii < lhs_list_size; ii++)
	{
	  jj = 0;
	  for (ii_tree = (*lhs_list)[ii];
	       ii_tree && TREE_CODE (ii_tree) == ARRAY_NOTATION_REF;
	       ii_tree = ARRAY_NOTATION_ARRAY (ii_tree))
	    {
	      lhs_array[ii][jj] = ii_tree;
	      jj++;
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
	  for (ii_tree = (*rhs_list)[ii];
	       ii_tree && TREE_CODE (ii_tree) == ARRAY_NOTATION_REF;
	       ii_tree = ARRAY_NOTATION_ARRAY (ii_tree))
	    {
	      rhs_array[ii][jj] = ii_tree;
	      jj++;
	    }
	}
    }

  for (ii = 0; ii < lhs_list_size; ii++)
    {
      tree lhs_node = (*lhs_list)[ii];
      if (TREE_CODE (lhs_node) == ARRAY_NOTATION_REF)
	{
	  for (jj = 0; jj < lhs_rank; jj++)
	    {
	      if (TREE_CODE (lhs_array[ii][jj]) == ARRAY_NOTATION_REF)
		{
		  lhs_value[ii][jj] = ARRAY_NOTATION_ARRAY (lhs_array[ii][jj]);
		  lhs_start[ii][jj] = ARRAY_NOTATION_START (lhs_array[ii][jj]);
		  lhs_length[ii][jj] =
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_LENGTH (lhs_array[ii][jj]));
		  lhs_stride[ii][jj] =
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_STRIDE (lhs_array[ii][jj]));
		  lhs_vector[ii][jj] = true;
		  /* IF the stride value is variable (i.e. not constant) then 
		     assume that the length is positive.  */
		  if (!TREE_CONSTANT (lhs_length[ii][jj]))
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
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_LENGTH (rhs_array[ii][jj]));
		  rhs_stride[ii][jj] =
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_STRIDE (rhs_array[ii][jj]));
		  rhs_vector[ii][jj] = true;
		  /* If the stride value is variable (i.e. not constant) then 
		     assume that the length is positive.  */
		  if (!TREE_CONSTANT (rhs_length[ii][jj]))
		    rhs_count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (rhs_length[ii][jj],
			    build_int_cst (TREE_TYPE (rhs_length[ii][jj]), 0)))
		    rhs_count_down[ii][jj] = true;
		  else
		    rhs_count_down[ii][jj] = false;	
		}
	      else
		rhs_vector[ii][jj] = false;
	    }
	}
      else
	for (jj = 0; jj < rhs_rank; jj++)
	  rhs_vector[ii][jj] = false;
    }



  for (ii = 0; ii < lhs_rank; ii++)
    {
      if (lhs_vector[0][ii])
	{
	  lhs_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				    integer_type_node);
	  lhs_ind_init[ii] = build_modify_expr
	    (location, lhs_var[ii], TREE_TYPE (lhs_var[ii]),
	     NOP_EXPR,
	     location, build_zero_cst (TREE_TYPE (lhs_var[ii])),
	     TREE_TYPE (lhs_var[ii]));
	  
	}
    }

  for (ii = 0; ii < rhs_rank; ii++)
    {
      /* When we have a polynomial, we assume that the indices are of type 
	 integer.  */
      rhs_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				integer_type_node);
      rhs_ind_init[ii] = build_modify_expr
	(location, rhs_var[ii], TREE_TYPE (rhs_var[ii]),
	 NOP_EXPR,
	 location, build_int_cst (TREE_TYPE (rhs_var[ii]), 0),
	 TREE_TYPE (rhs_var[ii]));
    }
  

  for (ii = 0; ii < MAX (lhs_rank, rhs_rank); ii++)
    {
      /* This will create the if statement label.  */
      if_stmt_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* This label statement will point to the loop body.  */
      body_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* This will create the exit label..i.e. where the while loop will branch
	 out of.  */
      exit_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (exit_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (exit_label[ii]) = 0;
      DECL_IGNORED_P (exit_label[ii]) = 1;
      exit_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, exit_label[ii]);
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
	      for (s_jj = lhs_rank - 1; s_jj >= 0; s_jj--)
		{
		  if (lhs_count_down[ii][s_jj])
	  	      /* Array[start_index + (induction_var * stride)].  */
		      lhs_array_opr = build_array_ref
			(location, lhs_array_opr,
			 build2 (MINUS_EXPR, TREE_TYPE (lhs_var[s_jj]),
				 lhs_start[ii][s_jj],
				 build2 (MULT_EXPR, TREE_TYPE (lhs_var[s_jj]),
					 lhs_var[s_jj],
					 lhs_stride[ii][s_jj])));
		  else
		    lhs_array_opr = build_array_ref
		      (location, lhs_array_opr,
		       build2 (PLUS_EXPR, TREE_TYPE (lhs_var[s_jj]),
			       lhs_start[ii][s_jj],
			       build2 (MULT_EXPR, TREE_TYPE (lhs_var[s_jj]),
				       lhs_var[s_jj],
				       lhs_stride[ii][s_jj])));
		}
	      vec_safe_push (lhs_array_operand, lhs_array_opr);
	    }
	  else
	    vec_safe_push (lhs_array_operand, integer_one_node);
	}
      replace_array_notations (&lhs, true, lhs_list, lhs_array_operand);
      array_expr_lhs = lhs;
    }

  if (rhs_rank)
    {
      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  if (rhs_vector[ii][0])
	    {
	      tree rhs_array_opr = rhs_value[ii][rhs_rank - 1];
	      for (s_jj = rhs_rank - 1; s_jj >= 0; s_jj--)
		{
		  if (rhs_count_down[ii][s_jj])
		    {
		      /* Array[start_index - (induction_var * stride)] */
		      rhs_array_opr = build_array_ref
			(location, rhs_array_opr,
			 build2 (MINUS_EXPR, TREE_TYPE (rhs_var[s_jj]),
				 rhs_start[ii][s_jj],
				 build2 (MULT_EXPR, TREE_TYPE (rhs_var[s_jj]),
					 rhs_var[s_jj],
					 rhs_stride[ii][s_jj])));
		    }
		  else
		    {
		      /* Array[start_index  + (induction_var * stride)] */
		      rhs_array_opr = build_array_ref
			(location, rhs_array_opr,
			 build2 (PLUS_EXPR, TREE_TYPE (rhs_var[s_jj]),
				 rhs_start[ii][s_jj],
				 build2 (MULT_EXPR, TREE_TYPE (rhs_var[s_jj]),
					 rhs_var[s_jj],
					 rhs_stride[ii][s_jj])));
		    }
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
		    if (idx_value == -1) /* This means we have an error.  */
		      return error_mark_node;
		    else if (idx_value < (int) lhs_rank && idx_value >= 0)
		      vec_safe_push (rhs_array_operand, lhs_var[idx_value]);
		    else
		      {
			size_t ee = 0;
			tree lhs_base = (*lhs_list)[ii];
			for (ee = 0; ee < lhs_rank; ee++)
			  lhs_base = ARRAY_NOTATION_ARRAY (lhs_base);
			error_at (location, "__sec_implicit_index argument %d "
				  "must be less than rank of %qD", idx_value,
				  lhs_base);
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
	      if (TREE_CODE (func_name) == ADDR_EXPR)
		if (is_sec_implicit_index_fn (func_name))
		  {
		    idx_value = 
		      extract_sec_implicit_index_arg (location, rhs_node);
		    if (idx_value == -1) /* This means we have an error.  */
		      return error_mark_node;
		    else if (idx_value < (int) lhs_rank && idx_value >= 0)
		      vec_safe_push (rhs_array_operand, lhs_var[idx_value]);
		    else
		      {
			size_t ee = 0;
			tree lhs_base = (*lhs_list)[ii];
			for (ee = 0; ee < lhs_rank; ee++)
			  lhs_base = ARRAY_NOTATION_ARRAY (lhs_base);
			error_at (location, "__sec_implicit_index argument %d "
				  "must be less than rank of %qD", idx_value,
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
    rhs_expr_incr[ii] = build2 (MODIFY_EXPR, void_type_node, rhs_var[ii], 
				build2 
				(PLUS_EXPR, TREE_TYPE (rhs_var[ii]), 
				 rhs_var[ii], 
				 build_one_cst (TREE_TYPE (rhs_var[ii]))));

  for (ii = 0; ii < lhs_rank; ii++) 
    lhs_expr_incr[ii] = build2 
      (MODIFY_EXPR, void_type_node, lhs_var[ii], 
       build2 (PLUS_EXPR, TREE_TYPE (lhs_var[ii]), lhs_var[ii], 
	       build_one_cst (TREE_TYPE (lhs_var[ii]))));
  
  if (!array_expr_lhs)
    array_expr_lhs = lhs;

  array_expr = build_modify_expr (location, array_expr_lhs, lhs_origtype, 
				  modifycode, rhs_loc, array_expr_rhs, 
				  rhs_origtype);

  for (jj = 0; jj < MAX (lhs_rank, rhs_rank); jj++)
    {
      if (rhs_rank && rhs_expr_incr[jj])
	{
	  size_t iii = 0;
	  if (lhs_rank == 0)
	    lhs_compare[jj] = integer_one_node;
	  else if (lhs_count_down[0][jj])
	    lhs_compare[jj] = build2
	      (GT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[0][jj]);
	  else
	    lhs_compare[jj] = build2
	      (LT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[0][jj]);


	  /* The reason why we have this here is for the following case:
	         Array[:][:] = function_call(something) + Array2[:][:];

	     So, we will skip the first operand of RHS and then go to the
	     2nd to find whether we should count up or down.  */
	 
	  for (iii = 0; iii < rhs_list_size; iii++)
	    if (rhs_vector[iii][jj])
	      break;
	      
	  /* What we are doing here is this:
	     We always count up, so:
	       if (length is negative ==> which means we count down)
	          we multiply length by -1 and count up => ii < -LENGTH
	       else
	          we just count up, so we compare for  ii < LENGTH
	   */
	  if (rhs_count_down[iii][jj])
	    /* We use iii for rhs_length because that is the correct countdown
	       we have to use.  */
	      rhs_compare[jj] = build2
		(LT_EXPR, boolean_type_node, rhs_var[jj],
		 build2 (MULT_EXPR, TREE_TYPE (rhs_var[jj]),
			 rhs_length[iii][jj],
			 build_int_cst (TREE_TYPE (rhs_var[jj]), -1)));
	  else
	    rhs_compare[jj] = build2 (LT_EXPR, boolean_type_node, rhs_var[jj],
				      rhs_length[iii][jj]);
	  if (lhs_compare[ii] != integer_one_node)
	    cond_expr[jj] = build2 (TRUTH_ANDIF_EXPR, void_type_node,
				    lhs_compare[jj], rhs_compare[jj]);
	  else
	    cond_expr[jj] = rhs_compare[jj];
	}
      else
	{
	  if (lhs_count_down[0][jj])
	    cond_expr[jj] = build2
	      (GT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[0][jj]);
	  else
	    cond_expr[jj] = build2
	      (LT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[0][jj]);
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
	add_stmt (lhs_ind_init [ii]);
      if (rhs_rank)
	add_stmt (rhs_ind_init[ii]);
      add_stmt (build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (build3 (COND_EXPR, void_type_node, cond_expr[ii],
			build1 (GOTO_EXPR, void_type_node, body_label[ii]),
			build1 (GOTO_EXPR, void_type_node, exit_label[ii])));
      add_stmt (body_label_expr[ii]);
    }

  if (MAX (lhs_rank, rhs_rank))
    add_stmt (array_expr);

  for (s_jj = ((int) MAX (lhs_rank, rhs_rank)) - 1; s_jj >= 0; s_jj--)
    {
      if (lhs_rank)
	add_stmt (lhs_expr_incr[s_jj]);
      if (rhs_rank && rhs_expr_incr[s_jj])
	add_stmt (rhs_expr_incr[s_jj]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (exit_label_expr[s_jj]);
    }
  pop_stmt_list (loop);
  return loop;
}


/* Encloses the conditional statement passed in STMT with a loop around it
   and replaces the condition in STMT with a ARRAY_REF tree-node to the array.
   The condition must have a ARRAY_NOTATION_REF tree.  */

static tree
fix_conditional_array_notations_1 (tree stmt)
{
  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  size_t list_size = 0;
  tree cond = NULL_TREE, builtin_loop = NULL_TREE, new_var = NULL_TREE;
  size_t rank = 0, ii = 0, jj = 0;
  int s_jj = 0;
  tree **array_ops, *array_var, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;
  location_t location = UNKNOWN_LOCATION;
  tree lngth_var, begin_var, strde_var;
  if (TREE_CODE (stmt) == COND_EXPR)
    cond = COND_EXPR_COND (stmt);
  else if (TREE_CODE (stmt) == SWITCH_EXPR)
    cond = SWITCH_COND (stmt);
  else
    /* Otherwise dont even touch the statement.  */
    return stmt;

  find_rank (cond, false, &rank);
  extract_array_notation_exprs (cond, false, &array_list);
  loop = push_stmt_list ();
  for (ii = 0; ii < vec_safe_length (array_list); ii++)
    { 
      tree array_node = (*array_list)[ii];
      if (TREE_CODE (array_node) == CALL_EXPR)
	{
	  builtin_loop = fix_builtin_array_notation_fn (array_node, &new_var);
	  if (builtin_loop == error_mark_node)
	    {
	      add_stmt (error_mark_node);
	      pop_stmt_list (loop);
	      return loop;
	    }
	  else if (builtin_loop)
	    {
	      vec <tree, va_gc>* sub_list = NULL, *new_var_list = NULL;
	      vec_safe_push (sub_list, array_node);
	      vec_safe_push (new_var_list, new_var);
	      add_stmt (builtin_loop);
	      replace_array_notations (&cond, false, sub_list, new_var_list); 
	    }
	}
    }
  find_rank (cond, true, &rank);
  if (rank == 0)
    {
      add_stmt (stmt);
      pop_stmt_list (loop); 
      return loop;
    }
  
  extract_array_notation_exprs (cond, true, &array_list);

  if (vec_safe_length (array_list) == 0)
    return stmt;

  list_size = vec_safe_length (array_list);
  location = EXPR_LOCATION (stmt);

  array_ops =  XNEWVEC (tree *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] =  XNEWVEC (tree, rank);

  array_vector =  XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] =  XNEWVEC (bool, rank);

  array_value = XNEWVEC (tree *, list_size);
  array_stride = XNEWVEC (tree *, list_size);
  array_length = XNEWVEC (tree *, list_size);
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
  exit_label = XNEWVEC (tree,  rank);
  exit_label_expr = XNEWVEC (tree, rank);
  compare_expr = XNEWVEC (tree, rank);
  if_stmt_label = XNEWVEC (tree, rank);

  expr_incr = XNEWVEC (tree, rank);
  ind_init = XNEWVEC (tree,  rank);

  count_down = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = XNEWVEC (bool, rank);

  array_var = XNEWVEC (tree, rank);
  for (ii = 0; ii < list_size; ii++)
    {
      tree array_node = (*array_list)[ii];
      if (array_node && TREE_CODE (array_node) == ARRAY_NOTATION_REF)
	{
	  tree array_begin = ARRAY_NOTATION_START (array_node);
	  tree array_lngth = ARRAY_NOTATION_LENGTH (array_node);
	  tree array_strde = ARRAY_NOTATION_STRIDE (array_node);

	  begin_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
	  lngth_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
	  strde_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);

	  add_stmt (build_modify_expr (location, begin_var,
				       TREE_TYPE (begin_var),
				       NOP_EXPR, location, array_begin,
				       TREE_TYPE (array_begin)));
	  add_stmt (build_modify_expr (location, lngth_var,
				       TREE_TYPE (lngth_var),
				       NOP_EXPR, location, array_lngth,
				       TREE_TYPE (array_lngth)));
	  add_stmt (build_modify_expr (location, strde_var,
				       TREE_TYPE (strde_var),
				       NOP_EXPR, location, array_strde,
				       TREE_TYPE (array_strde)));
      
	  ARRAY_NOTATION_START (array_node) = begin_var;
	  ARRAY_NOTATION_LENGTH (array_node) = lngth_var;
	  ARRAY_NOTATION_STRIDE (array_node) = strde_var;
	}
    }  

  for (ii = 0; ii < list_size; ii++)
    {
      tree array_node = (*array_list)[ii];
      jj = 0;
      for (jj_tree = array_node;
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }
  for (ii = 0; ii < list_size; ii++)
    {
      tree array_node = (*array_list)[ii];
      if (TREE_CODE (array_node) == ARRAY_NOTATION_REF)
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
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_STRIDE (array_ops[ii][jj]));
		  array_vector[ii][jj] = true;

		  if (!TREE_CONSTANT (array_length[ii][jj]))
		      count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (array_length[ii][jj],
			    build_int_cst (TREE_TYPE (array_length[ii][jj]),
					   0)))
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }

  for (ii = 0; ii < rank; ii++)
    {
      array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (location, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   location,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));
	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* This will create the if statement label.  */
      if_stmt_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* This label statment will point to the loop body.  */
      body_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* This will create the exit label..i.e. where the while loop will branch
	 out of. */
      exit_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (exit_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (exit_label[ii]) = 0;
      DECL_IGNORED_P (exit_label[ii]) = 1;
      exit_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  tree array_opr = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      if (count_down[ii][s_jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_opr = build_array_ref
		    (location, array_opr,
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_opr = build_array_ref
		    (location, array_opr,
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	    }
	  vec_safe_push (array_operand, array_opr);
	}
      else
	/* This is just a dummy node to make sure the list sizes for both
	   array list and array operand list are the same.  */
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&stmt, true, array_list, array_operand);
  for (ii = 0; ii < rank; ii++) 
    expr_incr[ii] = build2 (MODIFY_EXPR, void_type_node, array_var[ii], 
			    build2 (PLUS_EXPR, TREE_TYPE (array_var[ii]), 
				    array_var[ii], 
				    build_int_cst (TREE_TYPE (array_var[ii]), 
						   1)));
  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] =
	      build2 (LT_EXPR, boolean_type_node, array_var[jj],
		      build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
			      array_length[0][jj],
			      build_int_cst (TREE_TYPE (array_var[jj]), -1)));
	  else
	    compare_expr[jj] = build2 (LT_EXPR, boolean_type_node,
				       array_var[jj], array_length[0][jj]);
	}
    }
  
  for (ii = 0; ii < rank; ii++)
    {
      add_stmt (ind_init [ii]);
      add_stmt (build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (build3 (COND_EXPR, void_type_node, compare_expr[ii],
			build1 (GOTO_EXPR, void_type_node, body_label[ii]),
			build1 (GOTO_EXPR, void_type_node, exit_label[ii])));
      add_stmt (body_label_expr[ii]);
    }

  add_stmt (stmt);

  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      add_stmt (expr_incr[s_jj]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (exit_label_expr[s_jj]);
    }

  pop_stmt_list (loop);

  XDELETEVEC (body_label);
  XDELETEVEC (body_label_expr);
  XDELETEVEC (exit_label);
  XDELETEVEC (exit_label_expr);
  XDELETEVEC (compare_expr);
  XDELETEVEC (if_stmt_label);
  XDELETEVEC (expr_incr);
  XDELETEVEC (ind_init);
  // XDELETEVEC (array_var);
  
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
fix_conditional_array_notations (tree stmt)
{
  if (TREE_CODE (stmt) == STATEMENT_LIST)
    {
      tree_stmt_iterator tsi;
      for (tsi = tsi_start (stmt); !tsi_end_p (tsi); tsi_next (&tsi))
	{
	  tree single_stmt = *tsi_stmt_ptr (tsi);
	  *tsi_stmt_ptr (tsi) =
	    fix_conditional_array_notations_1 (single_stmt);
	}
      return stmt;
    }
  else
    return fix_conditional_array_notations_1 (stmt);
}

/* Create a struct c_expr that contains a loop with ARRAY_REF expr at location
   LOCATION with the tree_code CODE and the array notation expr is
   passed in ARG.  Returns the fixed c_expr in ARG itself.  */

struct c_expr 
fix_array_notation_expr (location_t location, enum tree_code code,
			 struct c_expr arg)
{

  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  size_t list_size = 0, rank = 0, ii = 0, jj = 0;
  int s_jj = 0;
  tree **array_ops, *array_var, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;
  
  find_rank (arg.value, false, &rank);
  if (rank == 0)
    return arg;
  
  extract_array_notation_exprs (arg.value, true, &array_list);

  loop = push_stmt_list ();
  if (vec_safe_length (array_list) == 0)
    return arg;

  list_size = vec_safe_length (array_list);
  
  array_ops = XNEWVEC (tree *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = XNEWVEC (tree,  rank);
  
  array_vector =  XNEWVEC (bool *, list_size);
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
      tree array_node = (*array_list)[ii];
      if (TREE_CODE (array_node) == ARRAY_NOTATION_REF)
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
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_STRIDE (array_ops[ii][jj]));
		  array_vector[ii][jj] = true;

		  if (!TREE_CONSTANT (array_length[ii][jj])) 
		    count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (array_length[ii][jj],
			    build_int_cst (TREE_TYPE (array_length[ii][jj]),
					   0)))
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }

  for (ii = 0; ii < rank; ii++)
    {
      array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (location, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   location,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));
	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* This will create the if statement label.  */
      if_stmt_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* This label statment will point to the loop body.  */
      body_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* This will create the exit label, i.e. where the while loop will branch
	 out of.  */
      exit_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (exit_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (exit_label[ii]) = 0;
      DECL_IGNORED_P (exit_label[ii]) = 1;
      exit_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  tree array_opr = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      if (count_down[ii][s_jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_opr = build_array_ref
		    (location, array_opr,
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_opr = build_array_ref
		    (location, array_opr,
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	    }
	  vec_safe_push (array_operand, array_opr);
	}
      else
      	/* This is just a dummy node to make sure the list sizes for both
	   array list and array operand list are the same.  */
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&arg.value, true, array_list, array_operand);

  for (ii = 0; ii < rank; ii++)
    {
      expr_incr[ii] =
	build2 (MODIFY_EXPR, void_type_node, array_var[ii],
		build2 (PLUS_EXPR, TREE_TYPE (array_var[ii]), array_var[ii],
			build_int_cst (TREE_TYPE (array_var[ii]), 1)));
    }
  
  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] =
	      build2 (LT_EXPR, boolean_type_node, array_var[jj],
		      build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
			      array_length[0][jj],
			      build_int_cst (TREE_TYPE (array_var[jj]), -1)));
	  else
	    compare_expr[jj] = build2 (LT_EXPR, boolean_type_node,
				       array_var[jj], array_length[0][jj]);
	}
    }

  
  for (ii = 0; ii < rank; ii++)
    {
      add_stmt (ind_init [ii]);
      add_stmt (build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (build3 (COND_EXPR, void_type_node, compare_expr[ii],
			build1 (GOTO_EXPR, void_type_node, body_label[ii]),
			build1 (GOTO_EXPR, void_type_node, exit_label[ii])));
      add_stmt (body_label_expr[ii]);
    }

  if (code == POSTINCREMENT_EXPR || code == POSTDECREMENT_EXPR)
    {
      arg = default_function_array_read_conversion (location, arg);
      arg.value = build_unary_op (location, code, arg.value, 0);
    }
  else if (code == PREINCREMENT_EXPR || code == PREDECREMENT_EXPR)
    {
      arg = default_function_array_read_conversion (location, arg);
      arg = parser_build_unary_op (location, code, arg);
    }

  add_stmt (arg.value);
  
  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      add_stmt (expr_incr[s_jj]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (exit_label_expr[s_jj]);
    }

  pop_stmt_list (loop);

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

  arg.value = loop;
  return arg;
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
  tree new_yes_list, new_cond_expr, new_var_init = NULL_TREE;
  tree new_exp_init = NULL_TREE;
  an_reduce_type an_type = REDUCE_UNKNOWN;
  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  size_t list_size = 0, rank = 0, ii = 0, jj = 0;
  int s_jj = 0;
  tree **array_ops, *array_var, jj_tree, loop, array_op0;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  tree identity_value = NULL_TREE, call_fn = NULL_TREE, new_call_expr;
  bool **count_down, **array_vector;
  location_t location = UNKNOWN_LOCATION;
  
  if (!is_builtin_array_notation_fn (CALL_EXPR_FN (an_builtin_fn), &an_type))
    return NULL_TREE;

  if (an_type != REDUCE_CUSTOM && an_type != REDUCE_MUTATING)
    func_parm = CALL_EXPR_ARG (an_builtin_fn, 0);
  else
    {
      call_fn = CALL_EXPR_ARG (an_builtin_fn, 2);
      while (TREE_CODE (call_fn) == CONVERT_EXPR
	     || TREE_CODE (call_fn) == NOP_EXPR)
	call_fn = TREE_OPERAND (call_fn, 0);
      call_fn = TREE_OPERAND (call_fn, 0);
      
      identity_value = CALL_EXPR_ARG (an_builtin_fn, 0);
      while (TREE_CODE (identity_value) == CONVERT_EXPR
	     || TREE_CODE (identity_value) == NOP_EXPR)
	identity_value = TREE_OPERAND (identity_value, 0);
      func_parm = CALL_EXPR_ARG (an_builtin_fn, 1);
    }
  
  while (TREE_CODE (func_parm) == CONVERT_EXPR
	 || TREE_CODE (func_parm) == EXCESS_PRECISION_EXPR
	 || TREE_CODE (func_parm) == NOP_EXPR)
    func_parm = TREE_OPERAND (func_parm, 0);
  
  find_rank (an_builtin_fn, true, &rank);

  location = EXPR_LOCATION (an_builtin_fn);
 
  if (rank == 0)
    return an_builtin_fn;
  else if (rank > 1 
	   && (an_type == REDUCE_MAX_INDEX  || an_type == REDUCE_MIN_INDEX))
    {
      error_at (location, "__sec_reduce_min_ind or __sec_reduce_max_ind cannot"
		" have arrays with dimension greater than 1.");
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
    case REDUCE_ALL_NONZEROS:
    case REDUCE_ANY_ZEROS:
    case REDUCE_ANY_NONZEROS:
      new_var_type = integer_type_node;
      break;
    case REDUCE_MAX_INDEX:
    case REDUCE_MIN_INDEX:
      new_var_type = integer_type_node;
      break;
    case REDUCE_CUSTOM:
      if (call_fn && identity_value) 
	new_var_type = ARRAY_NOTATION_TYPE ((*array_list)[0]);
      break;
    case REDUCE_MUTATING:
      new_var_type = NULL_TREE;
      break;
    default:
      gcc_unreachable ();  /* You should not reach here.  */
    }
  
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
  
  expr_incr = XNEWVEC (tree,  rank);
  ind_init = XNEWVEC (tree, rank);
  
  count_down = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = XNEWVEC (bool,  rank);
  
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
      tree array_node = (*array_list)[ii];
      if (TREE_CODE (array_node) == ARRAY_NOTATION_REF)
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
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_STRIDE (array_ops[ii][jj]));
		  array_vector[ii][jj] = true;

		  if (!TREE_CONSTANT (array_length[ii][jj]))
		    count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (array_length[ii][jj],
			    build_int_cst (TREE_TYPE (array_length[ii][jj]),
					   0)))
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }

  loop = alloc_stmt_list ();

  for (ii = 0; ii < rank; ii++)
    {
      array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (location, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   location,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* This will create the if statement label.  */
      if_stmt_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* This label statment will point to the loop body.  */
      body_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* This will create the exit label..i.e. where the while loop will branch
	 out of.  */
      exit_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (exit_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (exit_label[ii]) = 0;
      DECL_IGNORED_P (exit_label[ii]) = 1;
      exit_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  tree array_opr_node  = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      if (count_down[ii][s_jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_opr_node = build_array_ref
		    (location, array_opr_node,
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_opr_node = build_array_ref
		    (location, array_opr_node,
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	    }
	  vec_safe_push (array_operand, array_opr_node);
	}
      else
	/* This is just a dummy node to make sure the list sizes for both
	   array list and array operand list are the same.  */
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&func_parm, true, array_list, array_operand);
  for (ii = 0; ii < rank; ii++)
    {
      expr_incr[ii] =
	build2 (MODIFY_EXPR, void_type_node, array_var[ii],
		build2 (PLUS_EXPR, TREE_TYPE (array_var[ii]), array_var[ii],
			build_int_cst (TREE_TYPE (array_var[ii]), 1)));
    }
  
  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] =
	      build2 (LT_EXPR, boolean_type_node, array_var[jj],
		      build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
			      array_length[0][jj],
			      build_int_cst (TREE_TYPE (array_var[jj]), -1)));
	  else
	    compare_expr[jj] = build2 (LT_EXPR, boolean_type_node,
				       array_var[jj], array_length[0][jj]);
	}
    }

  if (an_type != REDUCE_MUTATING)
    {
      *new_var = build_decl (location, VAR_DECL, NULL_TREE, new_var_type);
      gcc_assert (*new_var && *new_var != error_mark_node);
    }
  else
    *new_var = NULL_TREE;
  
  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    array_ind_value = build_decl (location, VAR_DECL, NULL_TREE, 
				  TREE_TYPE (func_parm));
  array_op0 = (*array_operand)[0];			      
  switch (an_type)
    {
    case REDUCE_ADD:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (new_var_type), new_var_type);
      new_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), PLUS_EXPR,
	 location, func_parm, TREE_TYPE (func_parm));
      break;
    case REDUCE_MUL:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_one_cst (new_var_type), new_var_type);
      new_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), MULT_EXPR,
	 location, func_parm, TREE_TYPE (func_parm));
      break;
    case REDUCE_ALL_ZEROS:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_one_cst (new_var_type), new_var_type);
      /* Initially you assume everything is zero, now if we find a case where 
	 it is NOT true, then we set the result to false. Otherwise 
	 we just keep the previous value.  */
      new_yes_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (TREE_TYPE (*new_var)),
	 TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (NE_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(location, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));
      break;
    case REDUCE_ALL_NONZEROS:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_one_cst (new_var_type), new_var_type);
      /* Initially you assume everything is non-zero, now if we find a case
	 where it is NOT true, then we set the result to false.  Otherwise
	 we just keep the previous value.  */
      new_yes_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (TREE_TYPE (*new_var)),
	 TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (EQ_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(location, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));
      break;
    case REDUCE_ANY_ZEROS:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (new_var_type), new_var_type);
      /* Initially we assume there are NO zeros in the list. When we find 
	 a non-zero, we keep the previous value.  If we find a zero, we 
	 set the value to true.  */
      new_yes_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_one_cst (new_var_type), new_var_type);
      new_no_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (EQ_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(location, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));   
      break;
    case REDUCE_ANY_NONZEROS:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (new_var_type), new_var_type);
      /* Initially we assume there are NO non-zeros in the list. When we find 
	 a zero, we keep the previous value.  If we find a non-zero, we set 
	 the value to true.  */
      new_yes_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_one_cst (new_var_type), new_var_type);
      new_no_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (NE_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(location, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));   
      break;
    case REDUCE_MAX:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, func_parm, new_var_type);
      new_no_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_yes_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, func_parm, TREE_TYPE (*new_var));
      new_expr = build_conditional_expr
	(location,
	 build2 (LT_EXPR, TREE_TYPE (*new_var), *new_var, func_parm), false,
	 new_yes_expr, TREE_TYPE (*new_var), new_no_expr, TREE_TYPE (*new_var));
      break;
    case REDUCE_MIN:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, func_parm, new_var_type);
      new_no_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_yes_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, func_parm, TREE_TYPE (*new_var));
      new_expr = build_conditional_expr
	(location,
	 build2 (GT_EXPR, TREE_TYPE (*new_var), *new_var, func_parm), false,
	 new_yes_expr, TREE_TYPE (*new_var), new_no_expr, TREE_TYPE (*new_var));
      break;
    case REDUCE_MAX_INDEX:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (new_var_type), new_var_type);
      new_exp_init = build_modify_expr
	(location, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR, location, func_parm, TREE_TYPE (func_parm));
      new_no_ind = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(location, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR,
	 location, array_ind_value, TREE_TYPE (array_ind_value));
      if (list_size > 1)
	{
	  new_yes_ind = build_modify_expr
	    (location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     location, array_var[0], TREE_TYPE (array_var[0]));
	  new_yes_expr = build_modify_expr
	    (location, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     location, func_parm, TREE_TYPE ((*array_operand)[0]));
	}
      else
	{
	  new_yes_ind = build_modify_expr
	    (location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     location, TREE_OPERAND (array_op0, 1),
	     TREE_TYPE (TREE_OPERAND (array_op0, 1)));
	  new_yes_expr = build_modify_expr
	    (location, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     location, func_parm, TREE_OPERAND (array_op0, 1));
	}
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
 
      new_expr = build_conditional_expr
	(location,
	 build2 (LT_EXPR, TREE_TYPE (array_ind_value), array_ind_value,
		 func_parm),
	 false,
	 new_yes_list, TREE_TYPE (*new_var), new_no_list, TREE_TYPE (*new_var));
      break;
    case REDUCE_MIN_INDEX:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, build_zero_cst (new_var_type), new_var_type);
      new_exp_init = build_modify_expr
	(location, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR, location, func_parm, TREE_TYPE (func_parm));
      new_no_ind = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, *new_var, TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(location, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR,
	 location, array_ind_value, TREE_TYPE (array_ind_value));
      if (list_size > 1)
	{
	  new_yes_ind = build_modify_expr
	    (location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     location, array_var[0], TREE_TYPE (array_var[0]));
	  new_yes_expr = build_modify_expr
	    (location, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     location, func_parm, TREE_TYPE (array_op0));
	}
      else
	{
	  new_yes_ind = build_modify_expr
	    (location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     location, TREE_OPERAND (array_op0, 1),
	     TREE_TYPE (TREE_OPERAND (array_op0, 1)));
	  new_yes_expr = build_modify_expr
	    (location, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     location, func_parm, TREE_OPERAND (array_op0, 1));
	}
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
 
      new_expr = build_conditional_expr
	(location,
	 build2 (GT_EXPR, TREE_TYPE (array_ind_value), array_ind_value,
		 func_parm),
	 false,
	 new_yes_list, TREE_TYPE (*new_var), new_no_list, TREE_TYPE (*new_var));
      break;
    case REDUCE_CUSTOM:
      new_var_init = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, identity_value, new_var_type);
      new_call_expr = build_call_expr (call_fn, 2, *new_var, func_parm);
      new_expr = build_modify_expr
	(location, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 location, new_call_expr, TREE_TYPE (*new_var));
      break;
    case REDUCE_MUTATING:
      new_expr = build_call_expr (call_fn, 2, identity_value, func_parm);
      break;
    default:
      gcc_unreachable ();
      break;
    }

  for (ii = 0; ii < rank; ii++)
    append_to_statement_list (ind_init [ii], &loop);

  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    append_to_statement_list (new_exp_init, &loop);
  if (an_type != REDUCE_MUTATING)
    append_to_statement_list (new_var_init, &loop);
  
  for (ii = 0; ii < rank; ii++)
    {
      append_to_statement_list
	(build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]), &loop);
      append_to_statement_list
	(build3 (COND_EXPR, void_type_node, compare_expr[ii],
		 build1 (GOTO_EXPR, void_type_node, body_label[ii]),
		 build1 (GOTO_EXPR, void_type_node, exit_label[ii])), &loop);
      append_to_statement_list (body_label_expr[ii], &loop);
    }
					   
  append_to_statement_list (new_expr, &loop);
  
  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      append_to_statement_list (expr_incr[s_jj], &loop);
      append_to_statement_list
	(build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]), &loop);
      append_to_statement_list (exit_label_expr[s_jj], &loop);
    }

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
    {
      func_name = TREE_OPERAND (func_name, 0);
      if (TREE_CODE (func_name) == FUNCTION_DECL)
	function_name = IDENTIFIER_POINTER (DECL_NAME (func_name));
    }
  
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
  if (TREE_CODE (expr) == FUNCTION_DECL)
    if (is_builtin_array_notation_fn (DECL_NAME (expr), &type))
      return true;
  
  extract_array_notation_exprs (expr, false, &array_list);
  if (vec_safe_length (array_list) == 0)
    return false;
  else
    return true;
}

/*  Expands builtin functions in a return.  */

static tree
fix_return_expr (tree expr)
{
  tree new_mod_list, new_var, new_mod, retval_expr;
  location_t loc = EXPR_LOCATION (expr);
  if (TREE_CODE (expr) != RETURN_EXPR)
    return expr;

  new_mod_list = alloc_stmt_list ();
  retval_expr = TREE_OPERAND (expr, 0);
  new_var = build_decl (EXPR_LOCATION (expr), VAR_DECL, NULL_TREE,
			TREE_TYPE (retval_expr));
  new_mod =
    build_array_notation_expr (loc, new_var, TREE_TYPE (new_var), NOP_EXPR, loc,
			       TREE_OPERAND (retval_expr, 1),
			       TREE_TYPE (TREE_OPERAND (retval_expr, 1)));
  TREE_OPERAND (retval_expr, 1) = new_var;
  TREE_OPERAND (expr, 0) = retval_expr;
  append_to_statement_list_force (new_mod, &new_mod_list);
  append_to_statement_list_force (expr, &new_mod_list);
  return new_mod_list;
}

  

/* Replaces array notations in void function call arguments in ARG with loop and
   tree-node ARRAY_REF and returns that value in a tree node variable called
   LOOP.  */

static tree
fix_array_notation_call_expr (tree arg)
{
  vec<tree, va_gc> *array_list = NULL, *array_operand = NULL;
  tree new_var = NULL_TREE;
  size_t list_size = 0, rank = 0, ii = 0, jj = 0;
  int s_jj = 0;
  tree **array_ops, *array_var, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;
  an_reduce_type an_type = REDUCE_UNKNOWN;
  location_t location = UNKNOWN_LOCATION;
  tree lngth_var, strde_var, begin_var;
  if (TREE_CODE (arg) == CALL_EXPR
      && is_builtin_array_notation_fn (CALL_EXPR_FN (arg), &an_type))
    {
      loop = fix_builtin_array_notation_fn (arg, &new_var);
      /* We are ignoring the new var because either the user does not want to
	 capture it OR he is using sec_reduce_mutating function.  */
      return loop;
    }
  
  find_rank (arg, false, &rank);
  if (rank == 0)
    return arg;
  
  extract_array_notation_exprs (arg, true, &array_list);

  if (vec_safe_length (array_list) == 0)
    return arg;
  
  list_size = vec_safe_length (array_list);
  location = EXPR_LOCATION (arg);

  array_ops = XNEWVEC (tree *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = XNEWVEC (tree, rank);
  
  array_vector = XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = (bool *) XNEWVEC (bool, rank);

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
  
  count_down =  XNEWVEC (bool *, list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = XNEWVEC (bool, rank);
  
  array_var = XNEWVEC (tree, rank);
  loop = push_stmt_list ();
  for (ii = 0; ii < list_size; ii++)
    {
      tree array_node = (*array_list)[ii];
      if (array_node && TREE_CODE (array_node) == ARRAY_NOTATION_REF)
	{
	  tree array_begin = ARRAY_NOTATION_START (array_node);
	  tree array_lngth = ARRAY_NOTATION_LENGTH (array_node);
	  tree array_strde = ARRAY_NOTATION_STRIDE (array_node);

	  begin_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
	  lngth_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
	  strde_var = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);

	  add_stmt (build_modify_expr (location, begin_var,
				       TREE_TYPE (begin_var),
				       NOP_EXPR, location, array_begin,
				       TREE_TYPE (array_begin)));
	  add_stmt (build_modify_expr (location, lngth_var,
				       TREE_TYPE (lngth_var),
				       NOP_EXPR, location, array_lngth,
				       TREE_TYPE (array_lngth)));
	  add_stmt (build_modify_expr (location, strde_var,
				       TREE_TYPE (strde_var),
				       NOP_EXPR, location, array_strde,
				       TREE_TYPE (array_strde)));
      
	  ARRAY_NOTATION_START (array_node) = begin_var;
	  ARRAY_NOTATION_LENGTH (array_node) = lngth_var;
	  ARRAY_NOTATION_STRIDE (array_node) = strde_var;
	}
    }
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
      tree array_node = (*array_list)[ii];
      if (TREE_CODE (array_node) == ARRAY_NOTATION_REF)
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
		    fold_build1 (CONVERT_EXPR, integer_type_node,
				 ARRAY_NOTATION_STRIDE (array_ops[ii][jj]));
		  array_vector[ii][jj] = true;

		  if (!TREE_CONSTANT (array_length[ii][jj])) 
		    count_down[ii][jj] = false;
		  else if (tree_int_cst_lt
			   (array_length[ii][jj],
			    build_int_cst (TREE_TYPE (array_length[ii][jj]),
					   0)))
		    count_down[ii][jj] = true;
		  else
		    count_down[ii][jj] = false;
		}
	      else
		array_vector[ii][jj] = false;
	    }
	}
    }
  for (ii = 0; ii < rank; ii++)
    {
      array_var[ii] = build_decl (location, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (location, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   location,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));
	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* This will create the if statement label.  */
      if_stmt_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* This label statment will point to the loop body.  */
      body_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* This will create the exit label..i.e. where the while loop will branch
	 out of.  */
      exit_label[ii] = build_decl (location, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (exit_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (exit_label[ii]) = 0;
      DECL_IGNORED_P (exit_label[ii]) = 1;
      exit_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, exit_label[ii]);
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (array_vector[ii][0])
	{
	  tree array_opr_node = array_value[ii][rank - 1];
	  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
	    {
	      if (count_down[ii][s_jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_opr_node = build_array_ref
		    (location, array_opr_node,
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_opr_node = build_array_ref
		    (location, array_opr_node,
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[s_jj]),
			     array_start[ii][s_jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[s_jj]),
				     array_var[s_jj], array_stride[ii][s_jj])));
		}
	    }
	  vec_safe_push (array_operand, array_opr_node);
	}
      else
	/* This is just a dummy node to make sure the list sizes for both
	   array list and array operand list are the same.  */
	vec_safe_push (array_operand, integer_one_node);
    }
  replace_array_notations (&arg, true, array_list, array_operand);
  for (ii = 0; ii < rank; ii++) 
    expr_incr[ii] = 
      build2 (MODIFY_EXPR, void_type_node, array_var[ii], 
	      build2 (PLUS_EXPR, TREE_TYPE (array_var[ii]), array_var[ii], 
		      build_int_cst (TREE_TYPE (array_var[ii]), 1)));
  
  for (jj = 0; jj < rank; jj++)
    {
      if (rank && expr_incr[jj])
	{
	  if (count_down[0][jj])
	    compare_expr[jj] =
	      build2 (LT_EXPR, boolean_type_node, array_var[jj],
		      build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
			      array_length[0][jj],
			      build_int_cst (TREE_TYPE (array_var[jj]), -1)));
	  else
	    compare_expr[jj] = build2 (LT_EXPR, boolean_type_node,
				       array_var[jj], array_length[0][jj]);
	}
    }
  for (ii = 0; ii < rank; ii++)
    {
      add_stmt (ind_init [ii]);
      add_stmt (build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (build3 (COND_EXPR, void_type_node, compare_expr[ii],
			build1 (GOTO_EXPR, void_type_node, body_label[ii]),
			build1 (GOTO_EXPR, void_type_node, exit_label[ii])));
      add_stmt (body_label_expr[ii]);
    }
  add_stmt (arg);
  for (s_jj = rank - 1; s_jj >= 0; s_jj--)
    {
      add_stmt (expr_incr[s_jj]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[s_jj]));
      add_stmt (exit_label_expr[s_jj]);
    }

  pop_stmt_list (loop);

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

  arg = loop;
  return arg;
}


/* Walks through tree node T and find all the call-statments that do not return
   anything and fix up any array notations they may carry.  */

tree
expand_array_notation_exprs (tree t)
{
  if (!t || !contains_array_notation_expr (t))
    return t;

  switch (TREE_CODE (t))
    {
    case BIND_EXPR:
      t = expand_array_notation_exprs (BIND_EXPR_BODY (t));
      return t;
    case COND_EXPR:
      t = fix_conditional_array_notations (t);

      /* After the expansion if it is still a COND_EXPR, we go into its
	 subtrees.  */
      if (TREE_CODE (t) == COND_EXPR)
	{
	  COND_EXPR_THEN (t) = expand_array_notation_exprs (COND_EXPR_THEN (t));
	  COND_EXPR_ELSE (t) = expand_array_notation_exprs (COND_EXPR_ELSE (t));
	}
      else
	t = expand_array_notation_exprs (t);
      return t;
    case STATEMENT_LIST:
      {
	tree_stmt_iterator ii_tsi;
	for (ii_tsi = tsi_start (t); !tsi_end_p (ii_tsi); tsi_next (&ii_tsi))
	  *tsi_stmt_ptr (ii_tsi) = 
	    expand_array_notation_exprs (*tsi_stmt_ptr (ii_tsi));
      }
      return t;
    case CALL_EXPR:
      t = fix_array_notation_call_expr (t);
      return t;
    case RETURN_EXPR:
      t = fix_return_expr (t);
      return t;
    default:
      return t;
    }
  return t;
}


/* Returns array notation expression for the array base ARRAY of type TYPE,
   with start index, length and stride given by START_INDEX, LENGTH and STRIDE,
   respectively.  */

tree
build_array_notation_ref (location_t loc, tree array, tree start_index, 
			  tree length, tree stride, tree type)
{
  tree array_ntn_tree = NULL_TREE;
  size_t stride_rank = 0, length_rank = 0, start_rank = 0;
  
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
  if (stride && (!TREE_TYPE (stride) || !INTEGRAL_TYPE_P (TREE_TYPE (stride))))
    {
      error_at (loc, "stride of array notation triplet is not an integer.");
      return error_mark_node;
    }  
  if (!stride)
    {
      if (TREE_CONSTANT (start_index) && TREE_CONSTANT (length) 
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
      error_at (loc, "rank of an array notation triplet's length is not zero.");
      return error_mark_node;
    }
  if (stride_rank != 0)
    {
      error_at (loc, "rank of array notation triplet's stride is not zero.");
      return error_mark_node;
    }
  
  
  array_ntn_tree = build5 (ARRAY_NOTATION_REF, NULL_TREE, NULL_TREE, NULL_TREE,
			   NULL_TREE, NULL_TREE, NULL_TREE);
  ARRAY_NOTATION_ARRAY (array_ntn_tree) = array;
  ARRAY_NOTATION_START (array_ntn_tree) = start_index;
  ARRAY_NOTATION_LENGTH (array_ntn_tree) = length;
  ARRAY_NOTATION_STRIDE (array_ntn_tree) = stride;
  ARRAY_NOTATION_TYPE (array_ntn_tree) = type;
  TREE_TYPE (array_ntn_tree) = type;
  
  return array_ntn_tree;
}

/* This function will check if OP is a CALL_EXPR that is a builtin array 
   notation function.  If so, then we will return its type to be the type of
   the array notation inside.  */

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

