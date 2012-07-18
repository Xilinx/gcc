/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains routines to handle Array Notation expression
   handling routines in the C Compiler.
   Copyright (C) 2011, 2012  Free Software Foundation, Inc.
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
#include "tm.h"
#include "tree.h"
#include "langhooks.h"
#include "c-tree.h"
#include "c-lang.h"
#include "flags.h"
#include "output.h"
#include "intl.h"
#include "target.h"
#include "tree-iterator.h"
#include "bitmap.h"
#include "gimple.h"
#include "c-family/c-objc.h"

void replace_array_notations (tree *, bool, tree *, tree *, int);
void find_rank (tree, bool, int *);
void extract_array_notation_exprs (tree, bool, tree **, int *);
tree fix_conditional_array_notations (tree);
struct c_expr fix_array_notation_expr (location_t, enum tree_code,
				       struct c_expr);
static bool is_builtin_array_notation_fn (tree func_name, an_reduce_type *type);
static tree fix_builtin_array_notation_fn (tree an_builtin_fn, tree *new_var);
bool contains_array_notation_expr (tree expr);
extern bool is_sec_implicit_index_fn (tree);
extern int extract_sec_implicit_index_arg (tree fn);
tree expand_array_notation_exprs (tree t);

/* This function is to find the rank of an array notation expression.
 * For example, an array notation of A[:][:] has a rank of 2.
 */
void
find_rank (tree array, bool ignore_builtin_fn, int *rank)
{
  tree ii_tree;
  int current_rank = 0, ii = 0;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
  if (!array)
    return;
  else if (TREE_CODE (array) == ARRAY_NOTATION_REF)
    {
      for (ii_tree = array;
	   ii_tree && TREE_CODE (ii_tree) == ARRAY_NOTATION_REF;
	   ii_tree = ARRAY_NOTATION_ARRAY (ii_tree))
	current_rank++;
      
      if (*rank != 0 && *rank != current_rank)
	error ("Rank Mismatch!");
      else if (*rank == 0)
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
		 * scalar
		 */
		return;
	  if (TREE_CODE (TREE_OPERAND (array, 0)) == INTEGER_CST)
	    {
	      int length = TREE_INT_CST_LOW (TREE_OPERAND (array, 0));
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

/* this function will go through a tree and extract all the array notation
 * expressions inside the subtrees
 */
void
extract_array_notation_exprs (tree node, bool ignore_builtin_fn,
			      tree **array_list, int *list_size)
{
  int ii = 0;
  tree *new_array_list = NULL;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
  
  if (!node)
    return;
  else if (TREE_CODE (node) == ARRAY_NOTATION_REF)
    {
      ii = *list_size;
      new_array_list =
	(tree *) xrealloc (*array_list, (ii + 1) * sizeof (tree));
      gcc_assert (new_array_list);
      new_array_list[ii] = node;
      ii++;
      *list_size = ii;
      *array_list = new_array_list;
      return;
    }
  else if (TREE_CODE (node) == TREE_LIST)
    {
      extract_array_notation_exprs (TREE_PURPOSE (node), ignore_builtin_fn,
				    array_list, list_size);
      extract_array_notation_exprs (TREE_VALUE (node), ignore_builtin_fn,
				    array_list, list_size);
      extract_array_notation_exprs (TREE_CHAIN (node), ignore_builtin_fn,
				    array_list, list_size);
    }
  else if (TREE_CODE (node) == STATEMENT_LIST)
    {
      tree_stmt_iterator ii_tsi;
      for (ii_tsi = tsi_start (node); !tsi_end_p (ii_tsi); tsi_next (&ii_tsi))
	extract_array_notation_exprs (*tsi_stmt_ptr (ii_tsi), ignore_builtin_fn,
				      array_list, list_size);
    }
  else if (TREE_CODE (node) == CALL_EXPR)
    {
      if (is_builtin_array_notation_fn (CALL_EXPR_FN (node), &dummy_type))
	{
	  if (ignore_builtin_fn)
	    return;
	  else
	    {
	      ii = *list_size;
	      new_array_list = (tree *) xrealloc (*array_list, (ii + 1) *
						  sizeof (tree));
	      gcc_assert (new_array_list);
	      new_array_list[ii] = node;
	      ii++;
	      *list_size = ii;
	      *array_list = new_array_list;
	      return;
	    }
	}
      if (is_sec_implicit_index_fn (CALL_EXPR_FN (node)))
	{
	  ii = *list_size;
	  new_array_list = (tree *) xrealloc (*array_list, (ii + 1) *
					      sizeof (tree));
	  gcc_assert (new_array_list);
	  new_array_list[ii] = node;
	  ii++;
	  *list_size = ii;
	  *array_list = new_array_list;
	  return;
	}
      if (TREE_CODE (TREE_OPERAND (node, 0)) == INTEGER_CST)
	{
	  int length = TREE_INT_CST_LOW (TREE_OPERAND (node, 0));

	  for (ii = 0; ii < length; ii++)
	    extract_array_notation_exprs
	      (TREE_OPERAND (node, ii), ignore_builtin_fn, array_list,
	       list_size);
	}
      else
	gcc_unreachable  (); /* should not get here */
	  
    } 
  else
    {
      for (ii = 0; ii < TREE_CODE_LENGTH (TREE_CODE (node)); ii++)
	extract_array_notation_exprs (TREE_OPERAND (node, ii),
				      ignore_builtin_fn, array_list, list_size);
    }
  return;
}

/* this function will replace a subtree that has array notation with the
 * appropriate scalar equivalent
 */
void
replace_array_notations (tree *orig, bool ignore_builtin_fn, tree *list,
			 tree *array_operand, int array_size)
{
  int ii = 0;
  an_reduce_type dummy_type = REDUCE_UNKNOWN;
  
  if (array_size == 0 || *list == NULL || !*orig)
    return;

  if (TREE_CODE (*orig) == ARRAY_NOTATION_REF)
    {
      for (ii = 0; ii < array_size; ii++)
	{
	  if (*orig == list[ii])
	    *orig = array_operand[ii];
	}
    }
  else if (TREE_CODE (*orig) == STATEMENT_LIST)
    {
      tree_stmt_iterator ii_tsi;
      for (ii_tsi = tsi_start (*orig); !tsi_end_p (ii_tsi); tsi_next (&ii_tsi))
	replace_array_notations (tsi_stmt_ptr (ii_tsi), ignore_builtin_fn,
				 list, array_operand, array_size);
    }
  else if (TREE_CODE (*orig) == CALL_EXPR)
    {
      if (is_builtin_array_notation_fn (CALL_EXPR_FN (*orig), &dummy_type))
	{
	  if (!ignore_builtin_fn)
	    {
	      for (ii = 0; ii < array_size; ii++)
		{
		  if (*orig == list[ii])
		    *orig = array_operand[ii];
		}
	    }
	  return;
	}
      if (is_sec_implicit_index_fn (CALL_EXPR_FN (*orig)))
	{
	  for (ii = 0; ii < array_size; ii++)
	    {
	      if (*orig == list[ii])
		*orig = array_operand[ii];
	    }
	  return;
	}
      if (TREE_CODE (TREE_OPERAND (*orig, 0)) == INTEGER_CST)
	{
	  int length = TREE_INT_CST_LOW (TREE_OPERAND (*orig, 0));
	  for (ii = 0; ii < length; ii++)
	    replace_array_notations
	      (&TREE_OPERAND (*orig, ii), ignore_builtin_fn, list,
	       array_operand, array_size);
	}
      else
	gcc_unreachable (); /* should not get here! */
    }
  else
    {
      for (ii = 0; ii < TREE_CODE_LENGTH (TREE_CODE (*orig)); ii++)
	{
	  replace_array_notations
	    (&TREE_OPERAND (*orig, ii), ignore_builtin_fn, list,
	     array_operand, array_size);
	}
    }
  return;
}

/* this is a small function that will give the max of 2 integers */
static int
max (int x, int y)
{
  if (x > y)
    return x;
  else
    return y;
}

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
  tree *lhs_expr_incr = NULL, *rhs_expr_incr = NULL;
  tree *lhs_ind_init = NULL, *rhs_ind_init = NULL;
  bool **lhs_count_down = NULL, **rhs_count_down = NULL;
  tree *lhs_compare = NULL, *rhs_compare = NULL, *rhs_array_operand = NULL;
  tree *lhs_array_operand = NULL;
  int lhs_rank = 0, rhs_rank = 0, ii = 0, jj = 0;
  tree ii_tree = NULL_TREE, new_modify_expr, *lhs_list = NULL;
  tree *rhs_list = NULL, new_var = NULL_TREE, builtin_loop = NULL_TREE;
  int rhs_list_size = 0, lhs_list_size = 0;

  find_rank (rhs, false, &rhs_rank);

  extract_array_notation_exprs (rhs, false, &rhs_list, &rhs_list_size);

  loop = push_stmt_list ();

  for (ii = 0; ii < rhs_list_size; ii++)
    {
      if (TREE_CODE (rhs_list[ii]) == CALL_EXPR)
	{
	  builtin_loop = fix_builtin_array_notation_fn (rhs_list[ii], &new_var);
	  if (builtin_loop)
	    {
	      add_stmt (builtin_loop);
	      found_builtin_fn = true;
	      replace_array_notations (&rhs, false, &rhs_list[ii], &new_var, 1);
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
  extract_array_notation_exprs (rhs, true, &rhs_list, &rhs_list_size);
  extract_array_notation_exprs (lhs, true, &lhs_list, &lhs_list_size);
  
  if (lhs_rank == 0 && rhs_rank != 0)
    {
      error_at (location, "Left Hand-side rank cannot be scalar when "
		"right-hand side is not");
      return error_mark_node;
    }
  if (lhs_rank != 0 && rhs_rank != 0 && lhs_rank != rhs_rank)
    {
      error_at (location, "Rank-mismatch");
      return error_mark_node;
    }
  
  lhs_vector = (bool **) xmalloc (sizeof (bool *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_vector[ii] = (bool *) xmalloc (sizeof (bool) * lhs_rank);
  
  rhs_vector = (bool **) xmalloc (sizeof (bool *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_vector[ii] = (bool *) xmalloc (sizeof (bool) * rhs_rank);
  
  lhs_array = (tree **) xmalloc (sizeof (tree *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_array[ii] = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  
  rhs_array = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_array[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_value = (tree **) xmalloc (sizeof (tree *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_value[ii] = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  
  rhs_value = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_value[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_stride = (tree **) xmalloc (sizeof (tree *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_stride[ii] = (tree *) xmalloc (sizeof (tree *) * lhs_rank);
  
  rhs_stride = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_stride[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_length = (tree **) xmalloc (sizeof (tree *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_length[ii] = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  
  rhs_length = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_length[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);
  

  lhs_start = (tree **) xmalloc (sizeof (tree *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_start[ii] = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  
  rhs_start = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_start[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_var = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_var = (tree *) xmalloc (sizeof (tree) * rhs_rank);
  

  /* The reason why we are just using lhs_rank for this is because we have the
   * following scenarios:
   * LHS_RANK == RHS_RANK
   * LHS_RANK != RHS_RANK && RHS_RANK = 0
   *
   * In both the scenarios, just checking the LHS_RANK is OK
   */
  body_label = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));
  body_label_expr = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));
  exit_label = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));
  exit_label_expr = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));
  cond_expr = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));
  if_stmt_label = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));

  lhs_expr_incr = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_expr_incr = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_ind_init = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_ind_init = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_count_down = (bool **) xmalloc (sizeof (bool *) * lhs_list_size);
  for (ii = 0; ii < lhs_list_size; ii++)
    lhs_count_down[ii] = (bool *) xmalloc (sizeof (bool) * lhs_rank);
  
  rhs_count_down = (bool **) xmalloc (sizeof (bool *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_count_down[ii] = (bool *) xmalloc (sizeof (bool) * rhs_rank);

  lhs_compare = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_compare = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  rhs_array_operand = (tree *) xmalloc (sizeof (tree) * rhs_list_size);
  lhs_array_operand = (tree *) xmalloc (sizeof (tree) * lhs_list_size);

  if (lhs_rank)
    {
      for (ii = 0; ii < lhs_list_size; ii++)
	{
	  jj = 0;
	  for (ii_tree = lhs_list[ii];
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
	  for (ii_tree = rhs_list[ii];
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
      if (TREE_CODE (lhs_list[ii]) == ARRAY_NOTATION_REF)
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
		  /* IF the stride value is variable (i.e. not constant) then
		   * assume that the length is positive
		   */
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
      if (TREE_CODE (rhs_list[ii]) == ARRAY_NOTATION_REF)
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
		   * assume that the length is positive
		   */
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
    }



  for (ii = 0; ii < lhs_rank; ii++)
    {
      if (lhs_vector[0][ii])
	{
	  lhs_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				    integer_type_node);
	  lhs_ind_init[ii] = build_modify_expr
	    (UNKNOWN_LOCATION, lhs_var[ii], TREE_TYPE (lhs_var[ii]),
	     NOP_EXPR,
	     UNKNOWN_LOCATION, build_zero_cst (TREE_TYPE (lhs_var[ii])),
	     TREE_TYPE (lhs_var[ii]));
	  
	}
    }

  for (ii = 0; ii < rhs_rank; ii++)
    {
      /* When we have a polynomial, we assume that the indices are of type
       * integer
       */
      rhs_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				integer_type_node);
      rhs_ind_init[ii] = build_modify_expr
	(UNKNOWN_LOCATION, rhs_var[ii], TREE_TYPE (rhs_var[ii]),
	 modifycode,
	 UNKNOWN_LOCATION, build_int_cst (TREE_TYPE (rhs_var[ii]), 0),
	 TREE_TYPE (rhs_var[ii]));
    }
  

  for (ii = 0; ii < max (lhs_rank, rhs_rank); ii++)
    {
      /* this will create the if statement label */
      if_stmt_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* this label statment will point to the loop body */
      body_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* this will create the exit label..i.e. where the while loop will branch
	 out of
      */
      exit_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
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
      /* The last ARRAY_NOTATION element's ARRAY component should be the array's
       * base value
       */
	      lhs_array_operand[ii] = lhs_value[ii][lhs_rank - 1];
	      gcc_assert (lhs_array_operand[ii]);
	      for (jj = lhs_rank - 1; jj >= 0; jj--)
		{
		  if (lhs_count_down[ii][jj])
		      /* Array[start_index + (induction_var * stride)] */
		      lhs_array_operand[ii] = build_array_ref
			(UNKNOWN_LOCATION, lhs_array_operand[ii],
			 build2 (MINUS_EXPR, TREE_TYPE (lhs_var[jj]),
				 lhs_start[ii][jj],
				 build2 (MULT_EXPR, TREE_TYPE (lhs_var[jj]),
					 lhs_var[jj],
					 lhs_stride[ii][jj])));
		  else
		    lhs_array_operand[ii] = build_array_ref
		      (UNKNOWN_LOCATION, lhs_array_operand[ii],
		       build2 (PLUS_EXPR, TREE_TYPE (lhs_var[jj]),
			       lhs_start[ii][jj],
			       build2 (MULT_EXPR, TREE_TYPE (lhs_var[jj]),
				       lhs_var[jj],
				       lhs_stride[ii][jj])));
		}
	    }
	}
      replace_array_notations (&lhs, true, lhs_list, lhs_array_operand,
			       lhs_list_size);
      array_expr_lhs = lhs;
    }

  if (rhs_rank)
    {
      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  if (rhs_vector[ii][0])
	    {
	      rhs_array_operand[ii] = rhs_value[ii][rhs_rank - 1];
	      gcc_assert (rhs_array_operand[ii]);
	      for (jj = rhs_rank - 1; jj >= 0; jj--)
		{
		  if (rhs_count_down[ii][jj])
		    {
		      /* Array[start_index - (induction_var * stride)] */
		      rhs_array_operand[ii] = build_array_ref
			(location, rhs_array_operand[ii],
			 build2 (MINUS_EXPR, TREE_TYPE (rhs_var[jj]),
				 rhs_start[ii][jj],
				 build2 (MULT_EXPR, TREE_TYPE (rhs_var[jj]),
					 rhs_var[jj],
					 rhs_stride[ii][jj])));
		    }
		  else
		    {
		      /* Array[start_index  + (induction_var * stride)] */
		      rhs_array_operand[ii] = build_array_ref
			(location, rhs_array_operand[ii],
			 build2 (PLUS_EXPR, TREE_TYPE (rhs_var[jj]),
				 rhs_start[ii][jj],
				 build2 (MULT_EXPR, TREE_TYPE (rhs_var[jj]),
					 rhs_var[jj],
					 rhs_stride[ii][jj])));
		    }
		}
	    }
	}

      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  if (TREE_CODE (rhs_list[ii]) == CALL_EXPR)
	    {
	      int idx_value = 0;
	      tree func_name = CALL_EXPR_FN (rhs_list[ii]);
	      if (TREE_CODE (func_name) == ADDR_EXPR)
		if (is_sec_implicit_index_fn (func_name))
		  {
		    idx_value = extract_sec_implicit_index_arg (rhs_list[ii]);
		    if (idx_value < lhs_rank && idx_value >= 0)
		      rhs_array_operand[ii] = lhs_var[idx_value];
		    else
		      {
			error ("__sec_implicit_index parameter must be less "
			       " than the rank of the Left Hand Side expr. ");
			error ("Bailing out due to the previous error.");
			exit (ICE_EXIT_CODE);
		      }
		  }  
	    }
	}
		  
      replace_array_notations (&rhs, true, rhs_list, rhs_array_operand,
			       rhs_list_size);
      array_expr_rhs = rhs;
    }
  else
    {
      for (ii = 0; ii < rhs_list_size; ii++)
	{
	  if (TREE_CODE (rhs_list[ii]) == CALL_EXPR)
	    {
	      int idx_value = 0;
	      tree func_name = CALL_EXPR_FN (rhs_list[ii]);
	      if (TREE_CODE (func_name) == ADDR_EXPR)
		if (is_sec_implicit_index_fn (func_name))
		  {
		    idx_value = extract_sec_implicit_index_arg (rhs_list[ii]);
		    if (idx_value < lhs_rank && idx_value >= 0)
		      rhs_array_operand[ii] = lhs_var[idx_value];
		    else
		      {
			error ("__sec_implicit_index parameter must be less "
			       " than the rank of the Left Hand Side expr. ");
			error ("Bailing out due to the previous error.");
			exit (ICE_EXIT_CODE);
		      }
		  }  
	    }
	}
      replace_array_notations (&rhs, true, rhs_list, rhs_array_operand,
			       rhs_list_size);
      array_expr_rhs = rhs;
      rhs_expr_incr[0] = NULL_TREE;
    }

  for (ii = 0; ii < rhs_rank; ii++)
    {
      rhs_expr_incr[ii] = build2
	(MODIFY_EXPR, void_type_node, rhs_var[ii],
	 build2 (PLUS_EXPR, TREE_TYPE (rhs_var[ii]), rhs_var[ii],
		 build_one_cst (TREE_TYPE (rhs_var[ii]))));
    } 

  for (ii = 0; ii < lhs_rank; ii++)
    {
      lhs_expr_incr[ii] = build2
	(MODIFY_EXPR, void_type_node, lhs_var[ii],
	 build2 (PLUS_EXPR, TREE_TYPE (lhs_var[ii]), lhs_var[ii],
		 build_one_cst (TREE_TYPE (lhs_var[ii]))));
    }
  
  if (!array_expr_lhs)
    array_expr_lhs = lhs;

  array_expr = build_modify_expr (location, array_expr_lhs,
				  lhs_origtype, modifycode, rhs_loc,
				  array_expr_rhs, rhs_origtype);

  for (jj = 0; jj < max (lhs_rank, rhs_rank); jj++)
    {
      if (rhs_rank && rhs_expr_incr[jj])
	{
	  if (lhs_count_down[0][jj])
	    lhs_compare[jj] = build2
	      (GT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[0][jj]);
	  
	  else
	    lhs_compare[jj] = build2
	      (LT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[0][jj]);


	  /* What we are doing here is this:
	   * We always count up, so:
	   *    if (length is negative ==> which means we count down)
	   *       we multiply length by -1 and count up => ii < -LENGTH
	   *    else
	   *       we just count up, so we compare for  ii < LENGTH
	   */
	  if (rhs_count_down[0][jj])
	      rhs_compare[jj] = build2
		(LT_EXPR, boolean_type_node, rhs_var[jj],
		 build2 (MULT_EXPR, TREE_TYPE (rhs_var[jj]), rhs_length[0][jj],
			 build_int_cst (TREE_TYPE (rhs_var[jj]), -1)));
	  else
	    rhs_compare[jj] = build2 (LT_EXPR, boolean_type_node, rhs_var[jj],
				      rhs_length[0][jj]);
      
	  cond_expr[jj] = build2 (TRUTH_ANDIF_EXPR, void_type_node,
				  lhs_compare[jj], rhs_compare[jj]);
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

  
  for (ii = 0; ii < max (lhs_rank, rhs_rank); ii++)
    {
      add_stmt (lhs_ind_init [ii]);
      if (rhs_rank)
	add_stmt (rhs_ind_init[ii]);
      add_stmt (build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (build3 (COND_EXPR, void_type_node, cond_expr[ii],
			build1 (GOTO_EXPR, void_type_node, body_label[ii]),
			build1 (GOTO_EXPR, void_type_node, exit_label[ii])));

      add_stmt (body_label_expr[ii]);
    }

  if (max (lhs_rank, rhs_rank))
    add_stmt (array_expr);

  for (ii = max (lhs_rank, rhs_rank) - 1; ii >= 0; ii--)
    {
      add_stmt (lhs_expr_incr[ii]);
      if (rhs_rank && rhs_expr_incr[ii])
	add_stmt (rhs_expr_incr[ii]);
  
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (exit_label_expr[ii]);
    }
   
  pop_stmt_list (loop);

  return loop;
}

static tree
fix_conditional_array_notations_1 (tree stmt)
{
  tree *array_list = NULL;
  int list_size = 0;
  tree cond = NULL;
  int rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, *array_operand, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;

  if (TREE_CODE (stmt) == COND_EXPR)
    cond = COND_EXPR_COND (stmt);
  else if (TREE_CODE (stmt) == SWITCH_EXPR)
    cond = SWITCH_COND (stmt);
  else if (TREE_CODE (stmt) == FOR_STMT || TREE_CODE (stmt) == CILK_FOR_STMT)
    cond = FOR_COND (stmt);
  else
    /* otherwise dont even touch the statement */
    return stmt;

  find_rank (cond, true, &rank);
  if (rank == 0)
    return stmt;  
  
  extract_array_notation_exprs (cond, true, &array_list, &list_size);

  if (*array_list == NULL_TREE || list_size == 0)
    return stmt;

  array_ops = (tree **) xmalloc (sizeof (tree *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = (tree *) xmalloc (sizeof (tree) * rank);
  
  array_vector = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_value = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_stride = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_length = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_start = (tree **) xmalloc (sizeof (tree *) * list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
      array_stride[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_length[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_start[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
    }

  body_label = (tree *) xmalloc(sizeof (tree) * rank);
  body_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  compare_expr = (tree *) xmalloc (sizeof (tree) * rank);
  if_stmt_label = (tree *) xmalloc (sizeof (tree) * rank);
  
  expr_incr = (tree *) xmalloc (sizeof (tree) * rank);
  ind_init = (tree *) xmalloc (sizeof (tree) * rank);
  
  count_down = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_operand = (tree *) xmalloc (sizeof (tree) * list_size);
  
  array_var = (tree *) xmalloc (sizeof (tree) * rank);
  

  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      for (jj_tree = array_list[ii];
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE (array_list[ii]) == ARRAY_NOTATION_REF)
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

  loop = push_stmt_list();

  for (ii = 0; ii < rank; ii++)
    {
  
      array_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (UNKNOWN_LOCATION, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   UNKNOWN_LOCATION,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));
	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* this will create the if statement label */
      if_stmt_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* this label statment will point to the loop body */
      body_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* this will create the exit label..i.e. where the while loop will branch
	 out of
      */
      exit_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
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
	  array_operand[ii] = array_value[ii][rank - 1];
	  gcc_assert (array_operand[ii]);

	  for (jj = rank - 1; jj >= 0; jj--)
	    {
	      if (count_down[ii][jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	    }
	}
    }
  replace_array_notations (&stmt, true, array_list, array_operand, list_size);

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

  add_stmt (stmt);

  for (ii = rank - 1; ii >= 0; ii--)
    {
      add_stmt (expr_incr[ii]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (exit_label_expr[ii]);
    }

  pop_stmt_list (loop);

  free (body_label);
  free (body_label_expr);
  free (exit_label);
  free (exit_label_expr);
  free (compare_expr);
  free (if_stmt_label);
  free (expr_incr);
  free (ind_init);
  free (array_operand);
  free (array_var);
  
  for (ii = 0; ii < list_size; ii++)
    {
      free (count_down[ii]);
      free (array_value[ii]);
      free (array_stride[ii]);
      free (array_length[ii]);
      free (array_start[ii]);
      free (array_ops[ii]);
      free (array_vector[ii]);
    }

  free (count_down);
  free (array_value);
  free (array_stride);
  free (array_length);
  free (array_start);
  free (array_ops);
  free (array_vector);

  return loop;
}

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

struct c_expr 
fix_array_notation_expr (location_t location, enum tree_code code,
			 struct c_expr arg)
{

  tree *array_list = NULL;
  int list_size = 0;
  int rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, *array_operand, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;
  
  find_rank (arg.value, false, &rank);
  if (rank == 0)
    return arg;

 

  extract_array_notation_exprs (arg.value, true, &array_list, &list_size);

  if (list_size == 0 || *array_list == NULL_TREE)
    return arg;

  array_ops = (tree **) xmalloc (sizeof (tree *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = (tree *) xmalloc (sizeof (tree) * rank);
  
  array_vector = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_value = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_stride = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_length = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_start = (tree **) xmalloc (sizeof (tree *) * list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
      array_stride[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_length[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_start[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
    }

  body_label = (tree *) xmalloc(sizeof (tree) * rank);
  body_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  compare_expr = (tree *) xmalloc (sizeof (tree) * rank);
  if_stmt_label = (tree *) xmalloc (sizeof (tree) * rank);
  
  expr_incr = (tree *) xmalloc (sizeof (tree) * rank);
  ind_init = (tree *) xmalloc (sizeof (tree) * rank);
  
  count_down = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_operand = (tree *) xmalloc (sizeof (tree) * list_size);
  
  array_var = (tree *) xmalloc (sizeof (tree) * rank);
  

  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      for (jj_tree = array_list[ii];
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE (array_list[ii]) == ARRAY_NOTATION_REF)
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

  loop = push_stmt_list ();

  for (ii = 0; ii < rank; ii++)
    {
  
      array_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (UNKNOWN_LOCATION, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   UNKNOWN_LOCATION,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));
	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* this will create the if statement label */
      if_stmt_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* this label statment will point to the loop body */
      body_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* this will create the exit label..i.e. where the while loop will branch
	 out of
      */
      exit_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
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
	  array_operand[ii] = array_value[ii][rank - 1];
	  gcc_assert (array_operand[ii]);

	  for (jj = rank - 1; jj >= 0; jj--)
	    {
	      if (count_down[ii][jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	    }
	}
    }
  replace_array_notations (&arg.value, true, array_list, array_operand,
			   list_size);

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
  
  for (ii = rank - 1; ii >= 0; ii--)
    {
      add_stmt (expr_incr[ii]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (exit_label_expr[ii]);
    }

  pop_stmt_list (loop);

  free (body_label);
  free (body_label_expr);
  free (exit_label);
  free (exit_label_expr);
  free (compare_expr);
  free (if_stmt_label);
  free (expr_incr);
  free (ind_init);
  free (array_operand);
  free (array_var);
  
  for (ii = 0; ii < list_size; ii++)
    {
      free (count_down[ii]);
      free (array_value[ii]);
      free (array_stride[ii]);
      free (array_length[ii]);
      free (array_start[ii]);
      free (array_ops[ii]);
      free (array_vector[ii]);
    }

  free (count_down);
  free (array_value);
  free (array_stride);
  free (array_length);
  free (array_start);
  free (array_ops);
  free (array_vector);

  arg.value = loop;
  return arg;
}


static tree
fix_builtin_array_notation_fn (tree an_builtin_fn, tree *new_var)
{
  tree new_var_type = NULL_TREE, func_parm, new_expr, new_yes_expr, new_no_expr;
  tree array_ind_value = NULL_TREE, new_no_ind, new_yes_ind, new_no_list;
  tree new_yes_list, new_cond_expr, new_var_init, new_exp_init = NULL_TREE;
  an_reduce_type an_type = REDUCE_UNKNOWN;
  tree *array_list = NULL;
  int list_size = 0;
  int rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, *array_operand, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  tree identity_value = NULL_TREE, call_fn = NULL_TREE, new_call_expr;
  bool **count_down, **array_vector;
  
  if (!is_builtin_array_notation_fn (CALL_EXPR_FN (an_builtin_fn), &an_type))
    return NULL_TREE;

  if (an_type != REDUCE_CUSTOM)
    func_parm = CALL_EXPR_ARG (an_builtin_fn, 0);
  else
    {
      call_fn = CALL_EXPR_ARG (an_builtin_fn, 0);
      while (TREE_CODE (call_fn) == CONVERT_EXPR
	     || TREE_CODE (call_fn) == NOP_EXPR)
	call_fn = TREE_OPERAND (call_fn, 0);
      call_fn = TREE_OPERAND (call_fn, 0);
      
      identity_value = CALL_EXPR_ARG (an_builtin_fn, 1);
      while (TREE_CODE (identity_value) == CONVERT_EXPR
	     || TREE_CODE (identity_value) == NOP_EXPR)
	identity_value = TREE_OPERAND (identity_value, 0);
      
      func_parm = CALL_EXPR_ARG (an_builtin_fn, 2);
    }
  
  while (TREE_CODE (func_parm) == CONVERT_EXPR
	 || TREE_CODE (func_parm) == NOP_EXPR)
    func_parm = TREE_OPERAND (func_parm, 0);
  
  find_rank (an_builtin_fn, true, &rank);
  if (rank == 0)
    return an_builtin_fn;
  else if (rank > 1 
	   && (an_type == REDUCE_MAX_INDEX  || an_type == REDUCE_MIN_INDEX))
    { 
      error ("__sec_reduce_min_ind or __sec_reduce_max_ind cannot have arrays"
	     " with dimension greater than 1.");
      fnotice (stderr, "confused by earlier errors, bailing out\n"); 
      exit (ICE_EXIT_CODE);
    }
  
  extract_array_notation_exprs (func_parm, true, &array_list, &list_size);

  switch (an_type)
    {
    case REDUCE_ADD:
    case REDUCE_MUL:
    case REDUCE_MAX:
    case REDUCE_MIN:
      new_var_type = ARRAY_NOTATION_TYPE (array_list[0]);
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
      new_var_type = ARRAY_NOTATION_TYPE (array_list[0]);
      break;
    default:
      gcc_unreachable ();
    }
   
  
  array_ops = (tree **) xmalloc (sizeof (tree *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = (tree *) xmalloc (sizeof (tree) * rank);
  
  array_vector = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_value = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_stride = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_length = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_start = (tree **) xmalloc (sizeof (tree *) * list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
      array_stride[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_length[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_start[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
    }

  body_label = (tree *) xmalloc(sizeof (tree) * rank);
  body_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  compare_expr = (tree *) xmalloc (sizeof (tree) * rank);
  if_stmt_label = (tree *) xmalloc (sizeof (tree) * rank);
  
  expr_incr = (tree *) xmalloc (sizeof (tree) * rank);
  ind_init = (tree *) xmalloc (sizeof (tree) * rank);
  
  count_down = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_operand = (tree *) xmalloc (sizeof (tree) * list_size);
  
  array_var = (tree *) xmalloc (sizeof (tree) * rank);

  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      for (jj_tree = array_list[ii];
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE (array_list[ii]) == ARRAY_NOTATION_REF)
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
      array_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (UNKNOWN_LOCATION, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   UNKNOWN_LOCATION,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* this will create the if statement label */
      if_stmt_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* this label statment will point to the loop body */
      body_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* this will create the exit label..i.e. where the while loop will branch
	 out of
      */
      exit_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
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
	  array_operand[ii] = array_value[ii][rank - 1];
	  gcc_assert (array_operand[ii]);

	  for (jj = rank - 1; jj >= 0; jj--)
	    {
	      if (count_down[ii][jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	    }
	}
    }
  replace_array_notations (&func_parm, true, array_list, array_operand,
			   list_size);

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

  *new_var = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE, new_var_type);
  gcc_assert (*new_var);
  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    array_ind_value = build_decl
      (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE, TREE_TYPE (func_parm));
			      
  switch (an_type)
    {
    case REDUCE_ADD:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (new_var_type), new_var_type);
      new_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), PLUS_EXPR,
	 UNKNOWN_LOCATION, func_parm, TREE_TYPE (func_parm));
      break;
    case REDUCE_MUL:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_one_cst (new_var_type), new_var_type);
      new_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), MULT_EXPR,
	 UNKNOWN_LOCATION, func_parm, TREE_TYPE (func_parm));
      break;
    case REDUCE_ALL_ZEROS:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_one_cst (new_var_type), new_var_type);
      /* Initially you assume everything is zero, now if we find a case where
       * it is NOT true, then we set the result to false. Otherwise
       * we just keep the previous value
       */
      new_yes_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (TREE_TYPE (*new_var)),
	 TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (NE_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));
      break;
    case REDUCE_ALL_NONZEROS:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_one_cst (new_var_type), new_var_type);
      /* Initially you assume everything is non-zero, now if we find a case
	 where it is NOT true, then we set the result to false. Otherwise
	 * we just keep the previous value
	 */
      new_yes_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (TREE_TYPE (*new_var)),
	 TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (EQ_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));
      break;
    case REDUCE_ANY_ZEROS:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (new_var_type), new_var_type);
      /* Initially we assume there are NO zeros in the list. When we find
       * a non-zero, we keep the previous value. If we find a zero, we
       * set the value to true
       */
      new_yes_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_one_cst (new_var_type), new_var_type);
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (EQ_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));   
      break;
    case REDUCE_ANY_NONZEROS:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (new_var_type), new_var_type);
      /* Initially we assume there are NO non-zeros in the list. When we find
       * a zero, we keep the previous value. If we find a non-zero, we
       * set the value to true
       */
      new_yes_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_one_cst (new_var_type), new_var_type);
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_cond_expr = build2 (NE_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION, new_cond_expr, false, new_yes_expr,
	 TREE_TYPE (new_yes_expr), new_no_expr, TREE_TYPE (new_no_expr));   
      break;
    case REDUCE_MAX:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, func_parm, new_var_type);
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_yes_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, func_parm, TREE_TYPE (*new_var));
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION,
	 build2 (LT_EXPR, TREE_TYPE (*new_var), *new_var, func_parm), false,
	 new_yes_expr, TREE_TYPE (*new_var), new_no_expr, TREE_TYPE (*new_var));
      break;
    case REDUCE_MIN:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, func_parm, new_var_type);
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_yes_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, func_parm, TREE_TYPE (*new_var));
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION,
	 build2 (GT_EXPR, TREE_TYPE (*new_var), *new_var, func_parm), false,
	 new_yes_expr, TREE_TYPE (*new_var), new_no_expr, TREE_TYPE (*new_var));
      break;
    case REDUCE_MAX_INDEX:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (new_var_type), new_var_type);
      new_exp_init = build_modify_expr
	(UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR, UNKNOWN_LOCATION, func_parm, TREE_TYPE (func_parm));
      new_no_ind = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR,
	 UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value));
      if (list_size > 1)
	{
	  new_yes_ind = build_modify_expr
	    (UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     UNKNOWN_LOCATION, array_var[0], TREE_TYPE (array_var[0]));
	  new_yes_expr = build_modify_expr
	    (UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     UNKNOWN_LOCATION, func_parm, TREE_TYPE (array_operand[0]));
	}
      else
	{
	  new_yes_ind = build_modify_expr
	    (UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     UNKNOWN_LOCATION, TREE_OPERAND (array_operand[0], 1),
	     TREE_TYPE (TREE_OPERAND (array_operand[0], 1)));
	  new_yes_expr = build_modify_expr
	    (UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     UNKNOWN_LOCATION, func_parm, TREE_OPERAND (array_operand[0], 1));
	}
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
 
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION,
	 build2 (LT_EXPR, TREE_TYPE (array_ind_value), array_ind_value,
		 func_parm),
	 false,
	 new_yes_list, TREE_TYPE (*new_var), new_no_list, TREE_TYPE (*new_var));
      break;
    case REDUCE_MIN_INDEX:
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, build_zero_cst (new_var_type), new_var_type);
      new_exp_init = build_modify_expr
	(UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR, UNKNOWN_LOCATION, func_parm, TREE_TYPE (func_parm));
      new_no_ind = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var));
      new_no_expr = build_modify_expr
	(UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	 NOP_EXPR,
	 UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value));
      if (list_size > 1)
	{
	  new_yes_ind = build_modify_expr
	    (UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     UNKNOWN_LOCATION, array_var[0], TREE_TYPE (array_var[0]));
	  new_yes_expr = build_modify_expr
	    (UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     UNKNOWN_LOCATION, func_parm, TREE_TYPE (array_operand[0]));
	}
      else
	{
	  new_yes_ind = build_modify_expr
	    (UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	     UNKNOWN_LOCATION, TREE_OPERAND (array_operand[0], 1),
	     TREE_TYPE (TREE_OPERAND (array_operand[0], 1)));
	  new_yes_expr = build_modify_expr
	    (UNKNOWN_LOCATION, array_ind_value, TREE_TYPE (array_ind_value),
	     NOP_EXPR,
	     UNKNOWN_LOCATION, func_parm, TREE_OPERAND (array_operand[0], 1));
	}
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
 
      new_expr = build_conditional_expr
	(UNKNOWN_LOCATION,
	 build2 (GT_EXPR, TREE_TYPE (array_ind_value), array_ind_value,
		 func_parm),
	 false,
	 new_yes_list, TREE_TYPE (*new_var), new_no_list, TREE_TYPE (*new_var));
      break;
    case REDUCE_CUSTOM:
      if (!call_fn)
	{
	  error ("Unknown/Invalid function!");
	  exit (ICE_EXIT_CODE);
	}
      if (!identity_value)
	{
	  error ("Invalid Identity Value!");
	  exit (ICE_EXIT_CODE);
	}
      new_var_init = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, identity_value, new_var_type);

      new_call_expr = build_call_expr (call_fn, 2, *new_var, func_parm);
      new_expr = build_modify_expr
	(UNKNOWN_LOCATION, *new_var, TREE_TYPE (*new_var), NOP_EXPR,
	 UNKNOWN_LOCATION, new_call_expr, TREE_TYPE (*new_var));
      break;
      
    default:
      gcc_unreachable ();
      break;
    }

  for (ii = 0; ii < rank; ii++)
    append_to_statement_list (ind_init [ii], &loop);

  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    append_to_statement_list (new_exp_init, &loop);
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
  
  for (ii = rank - 1; ii >= 0; ii--)
    {
      append_to_statement_list (expr_incr[ii], &loop);
      append_to_statement_list
	(build1 (GOTO_EXPR, void_type_node, if_stmt_label[ii]), &loop);
      append_to_statement_list (exit_label_expr[ii], &loop);
    }

  free (body_label);
  free (body_label_expr);
  free (exit_label);
  free (exit_label_expr);
  free (compare_expr);
  free (if_stmt_label);
  free (expr_incr);
  free (ind_init);
  free (array_operand);
  free (array_var);
  
  for (ii = 0; ii < list_size; ii++)
    {
      free (count_down[ii]);
      free (array_value[ii]);
      free (array_stride[ii]);
      free (array_length[ii]);
      free (array_start[ii]);
      free (array_ops[ii]);
      free (array_vector[ii]);
    }

  free (count_down);
  free (array_value);
  free (array_stride);
  free (array_length);
  free (array_start);
  free (array_ops);
  free (array_vector);

  
  return loop;
}

static bool
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
  else
    {
      *type = REDUCE_UNKNOWN;
      return false;
    }
  return false;
}

bool
contains_array_notation_expr (tree expr)
{
  tree *array_list = NULL;
  int list_size = 0;
  an_reduce_type type = REDUCE_UNKNOWN;

  if (!expr)
    return false;
  if (TREE_CODE (expr) == FUNCTION_DECL)
    if (is_builtin_array_notation_fn (DECL_NAME (expr), &type))
      return true;
  
  extract_array_notation_exprs (expr, false, &array_list, &list_size);
  if (array_list == NULL || list_size == 0)
    return false;
  else
    return true;
}

/* this function will fix up array notation exprs inside void function calls.*/
static tree
fix_array_notation_call_expr (tree arg)
{
  tree *array_list = NULL;
  int list_size = 0;
  int rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, *array_operand, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;
  
  find_rank (arg, false, &rank);
  if (rank == 0)
    return arg;
  
  extract_array_notation_exprs (arg, true, &array_list, &list_size);

  if (list_size == 0 || *array_list == NULL_TREE)
    return arg;

  array_ops = (tree **) xmalloc (sizeof (tree *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_ops[ii] = (tree *) xmalloc (sizeof (tree) * rank);
  
  array_vector = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    array_vector[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_value = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_stride = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_length = (tree **) xmalloc (sizeof (tree *) * list_size);
  array_start = (tree **) xmalloc (sizeof (tree *) * list_size);

  for (ii = 0; ii < list_size; ii++)
    {
      array_value[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
      array_stride[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_length[ii] = (tree *) xmalloc (sizeof (tree) * rank);
      array_start[ii]  = (tree *) xmalloc (sizeof (tree) * rank);
    }

  body_label = (tree *) xmalloc(sizeof (tree) * rank);
  body_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label = (tree *) xmalloc (sizeof (tree) * rank);
  exit_label_expr = (tree *) xmalloc (sizeof (tree) * rank);
  compare_expr = (tree *) xmalloc (sizeof (tree) * rank);
  if_stmt_label = (tree *) xmalloc (sizeof (tree) * rank);
  
  expr_incr = (tree *) xmalloc (sizeof (tree) * rank);
  ind_init = (tree *) xmalloc (sizeof (tree) * rank);
  
  count_down = (bool **) xmalloc (sizeof (bool *) * list_size);
  for (ii = 0; ii < list_size; ii++)
    count_down[ii] = (bool *) xmalloc (sizeof (bool) * rank);

  array_operand = (tree *) xmalloc (sizeof (tree) * list_size);
  
  array_var = (tree *) xmalloc (sizeof (tree) * rank);
  

  for (ii = 0; ii < list_size; ii++)
    {
      jj = 0;
      for (jj_tree = array_list[ii];
	   jj_tree && TREE_CODE (jj_tree) == ARRAY_NOTATION_REF;
	   jj_tree = ARRAY_NOTATION_ARRAY (jj_tree))
	{
	  array_ops[ii][jj] = jj_tree;
	  jj++;
	}
    }

  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE (array_list[ii]) == ARRAY_NOTATION_REF)
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

  loop = push_stmt_list ();

  for (ii = 0; ii < rank; ii++)
    {
  
      array_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				  integer_type_node);
      ind_init[ii] =
	build_modify_expr (UNKNOWN_LOCATION, array_var[ii],
			   TREE_TYPE (array_var[ii]), NOP_EXPR,
			   UNKNOWN_LOCATION,
			   build_int_cst (TREE_TYPE (array_var[ii]), 0),
			   TREE_TYPE (array_var[ii]));
	
    }

  for (ii = 0; ii < rank ; ii++)
    {
      /* this will create the if statement label */
      if_stmt_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				      void_type_node);
      DECL_CONTEXT (if_stmt_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (if_stmt_label[ii]) = 0;
      DECL_IGNORED_P (if_stmt_label[ii]) = 1;
  
      /* this label statment will point to the loop body */
      body_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
				   void_type_node);
      DECL_CONTEXT (body_label[ii]) = current_function_decl;
      DECL_ARTIFICIAL (body_label[ii]) = 0;
      DECL_IGNORED_P (body_label[ii]) = 1;
      body_label_expr[ii] = build1 (LABEL_EXPR, void_type_node, body_label[ii]);

      /* this will create the exit label..i.e. where the while loop will branch
	 out of
      */
      exit_label[ii] = build_decl (UNKNOWN_LOCATION, LABEL_DECL, NULL_TREE,
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
	  array_operand[ii] = array_value[ii][rank - 1];
	  gcc_assert (array_operand[ii]);

	  for (jj = rank - 1; jj >= 0; jj--)
	    {
	      if (count_down[ii][jj])
		{
		  /* Array[start_index - (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (MINUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	      else
		{
		  /* Array[start_index + (induction_var * stride)] */
		  array_operand[ii] = build_array_ref
		    (UNKNOWN_LOCATION, array_operand[ii],
		     build2 (PLUS_EXPR, TREE_TYPE (array_var[jj]),
			     array_start[ii][jj],
			     build2 (MULT_EXPR, TREE_TYPE (array_var[jj]),
				     array_var[jj], array_stride[ii][jj])));
		}
	    }
	}
    }
  replace_array_notations (&arg, true, array_list, array_operand,
			   list_size);

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

  add_stmt (arg);
  
  for (ii = rank - 1; ii >= 0; ii--)
    {
      add_stmt (expr_incr[ii]);
      add_stmt (build1 (GOTO_EXPR, void_type_node, if_stmt_label[ii]));
      add_stmt (exit_label_expr[ii]);
    }

  pop_stmt_list (loop);

  free (body_label);
  free (body_label_expr);
  free (exit_label);
  free (exit_label_expr);
  free (compare_expr);
  free (if_stmt_label);
  free (expr_incr);
  free (ind_init);
  free (array_operand);
  free (array_var);
  
  for (ii = 0; ii < list_size; ii++)
    {
      free (count_down[ii]);
      free (array_value[ii]);
      free (array_stride[ii]);
      free (array_length[ii]);
      free (array_start[ii]);
      free (array_ops[ii]);
      free (array_vector[ii]);
    }

  free (count_down);
  free (array_value);
  free (array_stride);
  free (array_length);
  free (array_start);
  free (array_ops);
  free (array_vector);

  arg = loop;
  return arg;
}

/* this function will walk through a tree and find all call statements that
 * do not return anything and fix up any array notations they might carry */
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
    default:
      return t;
    }
  return t;
}
      
      
