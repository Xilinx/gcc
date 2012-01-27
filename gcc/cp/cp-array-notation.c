/* This file is part of the Intel(R) Cilk(TM) Plus support
   It contains routines to handle Array Notation expression
   handling routines in the C++ Compiler.
   Copyright (C) 2011, 2012  Free Software Foundation, Inc.
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
#include "tm.h"
#include "tree.h"
#include "cp-tree.h"
#include "c-family/c-common.h"
#include "c-family/c-objc.h"
#include "tree-inline.h"
#include "tree-mudflap.h"
#include "intl.h"
#include "toplev.h"
#include "flags.h"
#include "output.h"
#include "timevar.h"
#include "diagnostic.h"
#include "cgraph.h"
#include "tree-iterator.h"
#include "vec.h"
#include "target.h"
#include "gimple.h"
#include "bitmap.h"


void replace_array_notations (tree *, bool, tree *, tree *, int);
void find_rank (tree, bool, int *);
static tree fix_conditional_array_notations_1 (tree stmt);
tree fix_unary_array_notation_exprs (tree stmt);
static bool is_builtin_array_notation_fn (tree, an_reduce_type *);
static tree build_x_reduce_expr (tree, enum tree_code, tree, tsubst_flags_t,
				 an_reduce_type);
bool contains_array_notation_expr (tree);
extern bool is_sec_implicit_index_fn (tree);
extern int extract_sec_implicit_index_arg (tree fn);
void extract_array_notation_exprs (tree node, bool ignore_builtin_fn,
				   tree **array_list, int *list_size);

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
	    if (ignore_builtin_fn)
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

/* this function is synonymous to the build_x_modify_expr. This function
 * will build the equivalent array notation expression
 */
tree
build_x_array_notation_expr (tree lhs, enum tree_code modifycode, tree rhs,
			     tsubst_flags_t complain)
{
  bool *lhs_vector = NULL, **rhs_vector = NULL;
  tree *lhs_array = NULL, **rhs_array = NULL;
  tree array_expr_lhs = NULL_TREE, array_expr_rhs = NULL_TREE;
  tree array_expr = NULL_TREE;
  tree *lhs_value = NULL, **rhs_value = NULL;
  tree *lhs_stride = NULL, *lhs_length = NULL, *lhs_start = NULL;
  tree **rhs_stride = NULL, **rhs_length = NULL, **rhs_start = NULL;
  tree loop = NULL_TREE, *lhs_var = NULL, *rhs_var = NULL;
  tree *body_label = NULL, *body_label_expr = NULL;
  tree *exit_label = NULL, *exit_label_expr = NULL, *cond_expr = NULL;
  tree *if_stmt_label = NULL;
  tree *lhs_expr_incr = NULL, *rhs_expr_incr = NULL;
  tree *lhs_ind_init = NULL, *rhs_ind_init = NULL;
  bool *lhs_count_down = NULL, **rhs_count_down = NULL;
  tree *lhs_compare = NULL, *rhs_compare = NULL, *rhs_array_operand = NULL;
  int lhs_rank = 0, rhs_rank = 0, ii = 0, jj = 0;
  tree ii_tree = NULL_TREE;
  tree *rhs_list = NULL;
  int rhs_list_size = 0;
  an_reduce_type type = REDUCE_UNKNOWN;
  
  find_rank (lhs, true, &lhs_rank);
  find_rank (rhs, true, &rhs_rank);

  /* If both are scalar, then no reason to do any of the components inside this
   * function... a simple build_x_modify_expr would do.
   */
  if (lhs_rank == 0 && rhs_rank == 0)
    return NULL_TREE;

  if (TREE_CODE (rhs) == CALL_EXPR)
    {
      if (is_builtin_array_notation_fn (CALL_EXPR_FN (rhs), &type))
	{
	  loop = build_x_reduce_expr (lhs, modifycode, rhs, complain, type);
	  return loop;
	}
    }
  
  extract_array_notation_exprs (rhs, true, &rhs_list, &rhs_list_size);

  if (lhs_rank == 0 && rhs_rank != 0)
    {
      if (TREE_CODE (rhs) != CALL_EXPR)
	{
	  error ( "Left Hand-side rank cannot be scalar when "
		  "right-hand side is not");
	  return error_mark_node;
	}
    }
  if (lhs_rank != 0 && rhs_rank != 0 && lhs_rank != rhs_rank)
    {
      error ("Rank-mismatch");
      return error_mark_node;
    }
  
  lhs_vector = (bool *) xmalloc (sizeof (bool) * lhs_rank);
  rhs_vector = (bool **) xmalloc (sizeof (bool *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_vector[ii] = (bool *) xmalloc (sizeof (bool) * rhs_rank);
  
  lhs_array = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_array = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_array[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_value = (tree *) xmalloc (sizeof (tree) * lhs_rank);

  rhs_value = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_value[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_stride = (tree *) xmalloc (sizeof (tree) * lhs_rank);

  rhs_stride = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_stride[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_length = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_length = (tree **) xmalloc (sizeof (tree *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_length[ii] = (tree *) xmalloc (sizeof (tree) * rhs_rank);
  

  lhs_start = (tree *) xmalloc (sizeof (tree) * lhs_rank);
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
  exit_label = (tree *) xmalloc (sizeof (tree) * max(lhs_rank, rhs_rank));
  exit_label_expr = (tree *) xmalloc (sizeof (tree) * max(lhs_rank, rhs_rank));
  cond_expr = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));
  if_stmt_label = (tree *) xmalloc (sizeof (tree) * max (lhs_rank, rhs_rank));

  lhs_expr_incr = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_expr_incr = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_ind_init = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_ind_init = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  lhs_count_down = (bool *) xmalloc (sizeof (bool) * lhs_rank);
  rhs_count_down = (bool **) xmalloc (sizeof (bool *) * rhs_list_size);
  for (ii = 0; ii < rhs_list_size; ii++)
    rhs_count_down[ii] = (bool *) xmalloc (sizeof (bool) * rhs_rank);

  lhs_compare = (tree *) xmalloc (sizeof (tree) * lhs_rank);
  rhs_compare = (tree *) xmalloc (sizeof (tree) * rhs_rank);

  rhs_array_operand = (tree *) xmalloc (sizeof (tree) * rhs_list_size);

  ii = 0;
  if (lhs_rank)
    {
      for (ii_tree = lhs; ii_tree && TREE_CODE (ii_tree) == ARRAY_NOTATION_REF;
	   ii_tree = ARRAY_NOTATION_ARRAY (ii_tree))
	{
	  lhs_array[ii] = ii_tree;
	  ii++;
	}
    }
  else
    lhs_array[0] = NULL_TREE;

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

  if (TREE_CODE (lhs) == ARRAY_NOTATION_REF)
    {
      for (ii = 0; ii < lhs_rank; ii++)
	{
	  if (TREE_CODE (lhs_array[ii]) == ARRAY_NOTATION_REF)
	    {
	      lhs_value[ii] = ARRAY_NOTATION_ARRAY (lhs_array[ii]);
	      lhs_start[ii] = ARRAY_NOTATION_START (lhs_array[ii]);
	      lhs_length[ii] = ARRAY_NOTATION_LENGTH (lhs_array[ii]);
	      lhs_stride[ii] = ARRAY_NOTATION_STRIDE (lhs_array[ii]);
	      lhs_vector[ii] = true;
	      /* IF the stride value is variable (i.e. not constant) then
	       * assume that the length is positive
	       */
	      if (!TREE_CONSTANT (lhs_length[ii]))
		lhs_count_down[ii] = false;
	      else if (tree_int_cst_lt
		       (lhs_length[ii],
			build_int_cst (TREE_TYPE (lhs_length[ii]), 0)))
		lhs_count_down[ii] = true;
	      else
		lhs_count_down[ii] = false;
	    }
	  else
	    lhs_vector[ii] = false;
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

  loop = alloc_stmt_list();

  for (ii = 0; ii < lhs_rank; ii++)
    {
      if (lhs_vector[ii])
	{
	  lhs_var[ii] = build_decl (UNKNOWN_LOCATION, VAR_DECL, NULL_TREE,
				    TREE_TYPE (lhs_start[ii]));
	  lhs_ind_init[ii] = build_modify_expr
	    (UNKNOWN_LOCATION, lhs_var[ii], TREE_TYPE (lhs_var[ii]),
	     NOP_EXPR,
	     UNKNOWN_LOCATION, build_int_cst (TREE_TYPE (lhs_start[ii]), 0),
	     TREE_TYPE (lhs_start[ii]));
	  
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
	 NOP_EXPR,
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
      /* The last ARRAY_NOTATION element's ARRAY component should be the array's
       * base value
       */
      array_expr_lhs = lhs_value[lhs_rank - 1];
      for (ii = lhs_rank - 1; ii >= 0; ii--)
	{
	  /* Array[start_index + (induction_var * stride)] */
	  array_expr_lhs = build_array_ref
	    (UNKNOWN_LOCATION, array_expr_lhs,
	     build2 (PLUS_EXPR, TREE_TYPE (lhs_var[ii]), lhs_start[ii],
		     build2 (MULT_EXPR, TREE_TYPE (lhs_var[ii]), lhs_var[ii],
			     lhs_stride[ii])));
	  if (lhs_count_down[ii])
	    lhs_expr_incr[ii] =
	      build2 (MODIFY_EXPR, void_type_node, lhs_var[ii],
		      build2 (PLUS_EXPR, TREE_TYPE (lhs_var[ii]), lhs_var[ii],
			      build_int_cst (TREE_TYPE (lhs_var[ii]), -1)));
	  else
	    lhs_expr_incr[ii] =
	      build2 (MODIFY_EXPR, void_type_node, lhs_var[ii],
		      build2 (PLUS_EXPR, TREE_TYPE (lhs_var[ii]), lhs_var[ii],
			      build_int_cst (TREE_TYPE (lhs_var[ii]), 1)));
	}
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
			(UNKNOWN_LOCATION, rhs_array_operand[ii],
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
			(UNKNOWN_LOCATION, rhs_array_operand[ii],
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
		 build_int_cst (TREE_TYPE (rhs_var[ii]), 1)));
    }
  
  if (!array_expr_lhs)
    array_expr_lhs = lhs;
  
  array_expr = build_x_modify_expr (array_expr_lhs, modifycode, array_expr_rhs,
				    complain);

  for (jj = 0; jj < max (lhs_rank, rhs_rank); jj++)
    {
      if (rhs_rank && rhs_expr_incr[jj])
	{
	  if (lhs_count_down[jj])
	    lhs_compare[jj] = build2
	      (GT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[jj]);
	  
	  else
	    lhs_compare[jj] = build2
	      (LT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[jj]);


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

	  if (lhs_rank)
	    cond_expr[jj] = build2 (TRUTH_ANDIF_EXPR, void_type_node,
				    lhs_compare[jj], rhs_compare[jj]);
	  else
	    cond_expr[jj] = rhs_compare[jj];
	}
      else
	{
	  if (lhs_count_down[jj])
	    cond_expr[jj] = build2
	      (GT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[jj]);
	  else
	    cond_expr[jj] = build2
	      (LT_EXPR, boolean_type_node, lhs_var[jj], lhs_length[jj]);
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
      if (lhs_rank)
	append_to_statement_list (lhs_ind_init [ii], &loop);
      if (rhs_rank)
	append_to_statement_list (rhs_ind_init[ii], &loop);
      append_to_statement_list (build1 (LABEL_EXPR, void_type_node,
					if_stmt_label[ii]), &loop);
      append_to_statement_list
	(build3 (COND_EXPR, void_type_node, cond_expr[ii],
		 build1 (GOTO_EXPR, void_type_node, body_label[ii]),
		 build1 (GOTO_EXPR, void_type_node, exit_label[ii])), &loop);

      append_to_statement_list (body_label_expr[ii], &loop);
    }

  if (max (lhs_rank, rhs_rank))
    append_to_statement_list (array_expr, &loop);
  
  for (ii = max (lhs_rank, rhs_rank) - 1; ii >= 0; ii--)
    {
      if (lhs_rank)
	append_to_statement_list (lhs_expr_incr[ii], &loop);
      if (rhs_rank && rhs_expr_incr[ii])
	append_to_statement_list (rhs_expr_incr[ii], &loop);
  
      append_to_statement_list
	(build1 (GOTO_EXPR, void_type_node, if_stmt_label[ii]), &loop);
      append_to_statement_list (exit_label_expr[ii], &loop);
    }
   
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
  else if (TREE_CODE (stmt) == IF_STMT)
    cond = IF_COND (stmt);
  else if (TREE_CODE (stmt) == SWITCH_STMT)
    cond = SWITCH_STMT_COND (stmt);
  else if (TREE_CODE (stmt) == SWITCH_EXPR)
    cond = SWITCH_COND (stmt);
  else if (TREE_CODE (stmt) == WHILE_STMT)
    cond = WHILE_COND (stmt);
  else if (TREE_CODE (stmt) == FOR_STMT || TREE_CODE (stmt) == CILK_FOR_STMT)
    cond = FOR_COND (stmt);
  else if (TREE_CODE (stmt) == DO_STMT)
    cond = DO_COND (stmt);
  else
    /* otherwise don't even touch the statement */
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

  loop = alloc_stmt_list();

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
      append_to_statement_list (ind_init [ii], &loop);

      append_to_statement_list
	(build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]), &loop);
      append_to_statement_list
	(build3 (COND_EXPR, void_type_node, compare_expr[ii],
		 build1 (GOTO_EXPR, void_type_node, body_label[ii]),
		 build1 (GOTO_EXPR, void_type_node, exit_label[ii])), &loop);
      append_to_statement_list (body_label_expr[ii], &loop);
    }

  append_to_statement_list (stmt, &loop);

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

/* This function will recursively go through all the subtrees and find all
 * if, switch, for, while and do-while loops and fix up their conditions and
 * also walk through their subtrees.
 */
tree
fix_array_notation_exprs (tree t)
{
  enum tree_code code;
  bool is_expr;

  /* Skip empty subtrees.  */
  if (!t)
    return t;

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
    case CONVERT_EXPR:
    case NOP_EXPR:
    case INIT_EXPR:
    case MODIFY_EXPR:
    case ADDR_EXPR:
    case ARRAY_REF:
    case BIT_FIELD_REF:
    case VECTOR_CST:
    case COMPLEX_CST:
      return t;
      
    case CONSTRUCTOR:
      return t;
    case BIND_EXPR:
      {
	BIND_EXPR_BODY (t) =
	  fix_array_notation_exprs  (BIND_EXPR_BODY (t));
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
    case RETURN_EXPR:
    case DECL_EXPR:
    case INTEGER_TYPE:
    case ENUMERAL_TYPE:
    case BOOLEAN_TYPE:
    case POINTER_TYPE:
    case ARRAY_TYPE:
    case RECORD_TYPE:
    case METHOD_TYPE:
      return t;
 
    case PREDECREMENT_EXPR:
    case PREINCREMENT_EXPR:
    case POSTDECREMENT_EXPR:
    case POSTINCREMENT_EXPR:
    case AGGR_INIT_EXPR:
    case CALL_EXPR:
    case CLEANUP_POINT_EXPR:
    case EXPR_STMT:
      t = fix_unary_array_notation_exprs (t);
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
    case CILK_FOR_STMT:
      t = fix_conditional_array_notations_1 (t);

      /* If the above function added some extra instructions above the original
       * for statement, then we can't assume it is still FOR_STMT/CILK_FOR_STMT
       * so we have to check again */
      if (TREE_CODE (t) == CILK_FOR_STMT || TREE_CODE (t) == FOR_STMT)
	FOR_BODY (t) = fix_array_notation_exprs (FOR_BODY (t));
      else
	t = fix_array_notation_exprs (t);
      return t;

    case IF_STMT:
      t = fix_conditional_array_notations_1 (t);
      /* If the above function added some extra instructions above the original
       * if statement, then we can't assume it is still IF_STMT
       * so we have to check again */
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
       * switch statement, then we can't assume it is still SWITCH_STMT
       * so we have to check again */
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
       * while statement, then we can't assume it is still WHILE_STMT
       * so we have to check again */
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
       * do-while statement, then we can't assume it is still DO_STMT
       * so we have to check again */
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
  bool **count_down, **array_vector;
  
  if (!is_builtin_array_notation_fn (CALL_EXPR_FN (an_builtin_fn), &an_type))
    return NULL_TREE;

  func_parm = CALL_EXPR_ARG (an_builtin_fn, 0);
  while (TREE_CODE (func_parm) == CONVERT_EXPR
	 || TREE_CODE (func_parm) == NOP_EXPR)
    func_parm = TREE_OPERAND (func_parm, 0);
  
  
  find_rank (an_builtin_fn, false, &rank);
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
    case REDUCE_ANY_ZEROS:
      new_var_type = integer_type_node;
      break;
    case REDUCE_MAX_INDEX:
    case REDUCE_MIN_INDEX:
      new_var_type = integer_type_node;
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
      new_var_init = build_x_modify_expr
	(*new_var, NOP_EXPR, build_zero_cst (new_var_type), 1);
      new_expr = build_x_modify_expr (*new_var, PLUS_EXPR, func_parm, 1);
      break;
    case REDUCE_MUL:
      new_var_init = build_x_modify_expr
	(*new_var, NOP_EXPR, build_one_cst (new_var_type), 1);
      new_expr = build_x_modify_expr (*new_var, MULT_EXPR, func_parm, 1);
      break;
    case REDUCE_ALL_ZEROS:
      new_var_init = build_x_modify_expr
	(*new_var, NOP_EXPR, build_one_cst (new_var_type), 1);
      /* Initially you assume everything is zero, now if we find a case where
       * it is NOT true, then we set the result to false. Otherwise
       * we just keep the previous value
       */
      new_yes_expr = build_x_modify_expr
	(*new_var, NOP_EXPR, build_zero_cst (TREE_TYPE (*new_var)), 1);
      new_no_expr = build_x_modify_expr (*new_var, NOP_EXPR, *new_var, 1);
      new_cond_expr = build2 (NE_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_x_conditional_expr (new_cond_expr, new_yes_expr,
					   new_no_expr, 1);
      break;
    case REDUCE_ANY_ZEROS:
      new_var_init = build_x_modify_expr
	(*new_var, NOP_EXPR, build_zero_cst (new_var_type), 1);
      /* Initially we assume there are NO zeros in the list. When we find
       * a non-zero, we keep the previous value. If we find a zero, we
       * set the value to true
       */
      new_no_expr = build_x_modify_expr
	(*new_var, NOP_EXPR, build_one_cst (TREE_TYPE (*new_var)), 1);
      new_yes_expr = build_x_modify_expr (*new_var, NOP_EXPR, *new_var, 1);
      new_cond_expr = build2 (NE_EXPR, TREE_TYPE (func_parm), func_parm,
			      build_zero_cst (TREE_TYPE (func_parm)));
      new_expr = build_x_conditional_expr (new_cond_expr, new_yes_expr,
					   new_no_expr, 1);      
      break;
    case REDUCE_MAX:
      /* set initial value as the first element in the list */
      new_var_init = build_x_modify_expr (*new_var, NOP_EXPR, func_parm, 1);
      new_no_expr  = build_x_modify_expr (*new_var, NOP_EXPR, *new_var, 1);
      new_yes_expr = build_x_modify_expr (*new_var, NOP_EXPR, func_parm, 1);
      new_expr = build_x_conditional_expr
	(build2 (LT_EXPR, TREE_TYPE (*new_var), *new_var, func_parm),
	 new_yes_expr, new_no_expr, 1);      
      break;
    case REDUCE_MIN:
      new_var_init = build_x_modify_expr (*new_var, NOP_EXPR, func_parm, 1);
      new_no_expr  = build_x_modify_expr (*new_var, NOP_EXPR, *new_var, 1);
      new_yes_expr = build_x_modify_expr (*new_var, NOP_EXPR, func_parm, 1);
      new_expr = build_x_conditional_expr
	(build2 (GT_EXPR, TREE_TYPE (*new_var), *new_var, func_parm),
	 new_yes_expr, new_no_expr, 1);
      break;
    case REDUCE_MAX_INDEX:
      new_var_init = build_x_modify_expr (*new_var, NOP_EXPR, array_var[0], 1);
      new_exp_init = build_x_modify_expr (array_ind_value, NOP_EXPR, func_parm,
					  1);
      new_no_ind   = build_x_modify_expr (*new_var, NOP_EXPR, *new_var, 1);
      new_no_expr  = build_x_modify_expr (array_ind_value, NOP_EXPR,
					  array_ind_value, 1);
      if (list_size > 1) /* this means there is more than 1 */
	{
	  new_yes_ind  = build_x_modify_expr (*new_var, NOP_EXPR, array_var[0],
					      1);
	  new_yes_expr = build_x_modify_expr (array_ind_value, NOP_EXPR,
					      func_parm, 1);
	}
      else
	{
	  new_yes_ind  = build_x_modify_expr
	    (*new_var, NOP_EXPR, TREE_OPERAND (array_operand[0], 1), 1);
	  new_yes_expr = build_x_modify_expr (array_ind_value, NOP_EXPR,
					      func_parm, 1);
	}
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
      
      new_expr = build_x_conditional_expr
	(build2 (LT_EXPR, TREE_TYPE (array_ind_value), array_ind_value,
		 func_parm),
	 new_yes_list, new_no_list, 1);
      break;
    case REDUCE_MIN_INDEX:
      new_var_init = build_x_modify_expr (*new_var, NOP_EXPR, array_var[0], 1);
      new_exp_init = build_x_modify_expr (array_ind_value, NOP_EXPR, func_parm,
					  1);
      new_no_ind   = build_x_modify_expr (*new_var, NOP_EXPR, *new_var, 1);
      new_no_expr  = build_x_modify_expr (array_ind_value, NOP_EXPR,
					  array_ind_value, 1);
      if (list_size > 1) /* this means there is more than 1 */
	{
	  new_yes_ind  = build_x_modify_expr (*new_var, NOP_EXPR, array_var[0],
					      1);
	  new_yes_expr = build_x_modify_expr (array_ind_value, NOP_EXPR,
					      func_parm, 1);
	}
      else
	{
	  new_yes_ind  = build_x_modify_expr
	    (*new_var, NOP_EXPR, TREE_OPERAND (array_operand[0], 1), 1);
	  new_yes_expr = build_x_modify_expr (array_ind_value, NOP_EXPR,
					      func_parm, 1);
	}
      new_yes_list = alloc_stmt_list ();
      append_to_statement_list (new_yes_ind, &new_yes_list);
      append_to_statement_list (new_yes_expr, &new_yes_list);

      new_no_list = alloc_stmt_list ();
      append_to_statement_list (new_no_ind, &new_no_list);
      append_to_statement_list (new_no_expr, &new_no_list);
      
      new_expr = build_x_conditional_expr
	(build2 (GT_EXPR, TREE_TYPE (array_ind_value), array_ind_value,
		 func_parm),
	 new_yes_list, new_no_list, 1);
      break;
    default:
      gcc_unreachable ();
      break;
    }

  for (ii = 0; ii < rank; ii++)
    append_to_statement_list (ind_init[ii], &loop);
    
  append_to_statement_list (new_var_init, &loop);
  if (an_type == REDUCE_MAX_INDEX || an_type == REDUCE_MIN_INDEX)
    append_to_statement_list (new_exp_init, &loop);
  
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


tree
fix_unary_array_notation_exprs (tree orig_stmt)
{

  tree *array_list = NULL;
  int list_size = 0;
  int rank = 0, ii = 0, jj = 0;
  tree **array_ops, *array_var, *array_operand, jj_tree, loop;
  tree **array_value, **array_stride, **array_length, **array_start;
  tree *body_label, *body_label_expr, *exit_label, *exit_label_expr;
  tree *compare_expr, *if_stmt_label, *expr_incr, *ind_init;
  bool **count_down, **array_vector;
  tree builtin_loop, stmt = NULL_TREE, new_var = NULL_TREE;
  
  find_rank (orig_stmt, false, &rank);
  if (rank == 0)
    return orig_stmt;  
  
  extract_array_notation_exprs (orig_stmt, false, &array_list, &list_size);

  stmt = alloc_stmt_list ();
  for (ii = 0; ii < list_size; ii++)
    {
      if (TREE_CODE (array_list[ii]) == CALL_EXPR
	  || TREE_CODE (array_list[ii]) == AGGR_INIT_EXPR)
	{
	  builtin_loop =
	    fix_builtin_array_notation_fn (array_list[ii], &new_var);
	  append_to_statement_list (builtin_loop, &stmt);
	  replace_array_notations (&orig_stmt, false, &array_list[ii],
				   &new_var, 1);
	}
    }
  append_to_statement_list (orig_stmt, &stmt);
  rank = 0;
  list_size = 0;
  array_list = NULL;
  extract_array_notation_exprs (stmt, true, &array_list, &list_size);
  find_rank (stmt, true, &rank);
  if (rank == 0)
    return stmt;
  
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
      append_to_statement_list (ind_init [ii], &loop);

      append_to_statement_list
	(build1 (LABEL_EXPR, void_type_node, if_stmt_label[ii]), &loop);
      append_to_statement_list
	(build3 (COND_EXPR, void_type_node, compare_expr[ii],
		 build1 (GOTO_EXPR, void_type_node, body_label[ii]),
		 build1 (GOTO_EXPR, void_type_node, exit_label[ii])), &loop);
      append_to_statement_list (body_label_expr[ii], &loop);
    }

  append_to_statement_list (stmt, &loop);
  
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

  stmt = loop;
  return stmt;
}

static bool
is_builtin_array_notation_fn (tree func_name, an_reduce_type *type)
{
  const char *function_name = NULL;

  if (!func_name)
    return false;

  if (TREE_CODE (func_name) == IDENTIFIER_NODE)
    function_name = IDENTIFIER_POINTER (DECL_NAME (func_name));
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
  else if (!strcmp (function_name, "__sec_reduce_all_zeros"))
    {
      *type = REDUCE_ALL_ZEROS;
      return true;
    }
  else if (!strcmp (function_name, "__sec_reduce_any_zeros"))
    {
      *type = REDUCE_ANY_ZEROS;
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
  else
    {
      *type = REDUCE_UNKNOWN;
      return false;
    }
  return false;
}
    

static tree
build_x_reduce_expr (tree lhs, enum tree_code modifycode, tree rhs,
		     tsubst_flags_t complain, an_reduce_type type)
{
  if (!type)
    {
      return build_x_modify_expr (lhs, modifycode, rhs, complain);
    }
  return NULL_TREE;
}
      
bool
contains_array_notation_expr (tree expr)
{
  tree *array_list = NULL;
  int list_size = 0;
  
  extract_array_notation_exprs (expr, false, &array_list, &list_size);
  if (array_list == NULL || list_size == 0)
    return false;
  else
    return true;
}
    
