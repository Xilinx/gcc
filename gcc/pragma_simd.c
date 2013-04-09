/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains routines to handle PRAGMA SIMD 
   assignments by the vectorizer.
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
#include "tm.h"
#include "tree.h"
#include "langhooks.h"
#include "c/c-tree.h"
#include "c/c-lang.h"
#include "flags.h"
#include "output.h"
#include "intl.h"
#include "target.h"
#include "tree-iterator.h"
#include "bitmap.h"
#include "gimple.h"
#include "cilk.h"
#include "c-family/c-objc.h"
		

struct pragma_simd_values *psv_head;

/* Verify that the <#pragma simd> clauses have been properly resolved.
   INDEX is the pragma_simd_index into the global table.  */

void
pragma_simd_verify_clauses (int index)
{
  struct pragma_simd_values *vals = psv_find_node (index);
  location_t loc = vals ? vals->loc : UNKNOWN_LOCATION;

  if ((!clause_resolved_p (P_SIMD_VECTORLENGTH, index)))
    {
      if (pragma_simd_assert_requested_p (index))
	{
	  error_at (loc, "vectorlength in pragma simd not picked from list");
	  exit (ICE_EXIT_CODE);
	}
      else 
	warning_at (0, loc,
		    "vectorlength in pragma simd not picked from list");
    }
  if (!clause_resolved_p (P_SIMD_PRIVATE, index))
    { 
      if (pragma_simd_assert_requested_p (index))
	{ 
	  error_at (loc, "unable to make all variables private");
	  exit (ICE_EXIT_CODE);
	} 
      else
	warning_at (0, loc,
		    "unable to make all variables private in pragma simd");
    }     
  if (!clause_resolved_p (P_SIMD_LINEAR, index))
    {
      if (pragma_simd_assert_requested_p (index))
	{
	  error_at (loc, "unable to pick requested step-size in pragma simd");
	  exit (ICE_EXIT_CODE);
	}
      else
	warning (loc, "unable to pick requested step-size in pragma simd");
    }
  if (!clause_resolved_p (P_SIMD_REDUCTION, index))
    {
      if (pragma_simd_assert_requested_p (index))
	{
	  error_at (loc, "unable to satisfy all reductions in pragma simd");
	  exit (ICE_EXIT_CODE);
	}
      else
	warning_at (0, loc, "unable to satisfy all reductions in pragma simd");
    }
}

/* Clear the pragma simd data structure.  */

void
clear_pragma_simd_list (void)
{
  psv_head = NULL;
}

/* this function will check and see if a certain clause is resolved
   (i.e. checked OK).  */

bool
clause_resolved_p (enum pragma_simd_kind clause_type, int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  
  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return true;

  if (psv_head == NULL)
    return true;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
    {
      if (ps_iter->pragma_encountered && (ps_iter->index == pragma_simd_index))
	{
	  switch (clause_type)
	    {
	    case P_SIMD_VECTORLENGTH:
	      return ps_iter->vlength_OK;
	    case P_SIMD_PRIVATE:
	      return ps_iter->pvars_OK;
	    case P_SIMD_LINEAR:
	      return ps_iter->lvars_OK;
	    case P_SIMD_REDUCTION:
	      return ps_iter->rvars_OK;
	    default:
	      return false;
	    }
	}
    }

  /* if we can't find the certain pragma, then just return true to make sure
     we do not assert/halt and continue */
  return true;
}

/* this function will go through and check off all the clauses we have
   satisfied.  */

void
set_OK_for_certain_clause (enum pragma_simd_kind clause_type, bool set_value,
			   int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return;

  if (!psv_head)
    return;

  // FIXME: Why not use psv_find_node?
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
    {
      if (ps_iter->pragma_encountered && (ps_iter->index == pragma_simd_index))
	break;
    }
  if (!ps_iter)
    return;

  switch (clause_type)
    {
    case P_SIMD_VECTORLENGTH:
      ps_iter->vlength_OK = set_value;
    case P_SIMD_PRIVATE:
      ps_iter->pvars_OK = set_value;
    case P_SIMD_LINEAR:
      ps_iter->lvars_OK = set_value;
    case P_SIMD_REDUCTION:
      ps_iter->rvars_OK = set_value;
    default:
      break;
    }

  return;
}

/* this function will make srue all the reductions given by pragma simd
   reduction clauses are satisfied.  */

bool
all_reductions_satisfied_p (int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  struct reduction_values *rv_iter = NULL;

  if (psv_head == NULL)
    return true;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return true;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
    {
      for (rv_iter = ps_iter->reduction_vals; rv_iter != NULL;
	   rv_iter = rv_iter->ptr_next)
	{
	  if (rv_iter->not_reduced != 0)
	    return false;
	}
    }
  return true;
}

// FIXME: We should really rewrite all this psv* business to use vectors.
/* Given an index into the pragma simd list (PSV_INDEX), find its
   entry and return it.  */

struct pragma_simd_values *
psv_find_node (int psv_index)
{
  struct pragma_simd_values *ps_iter = NULL;

  if (psv_index <= INVALID_PRAGMA_SIMD_SLOT)
    return NULL;

  if (psv_head == NULL)
    return NULL;
  
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
    if ((ps_iter->index == psv_index) && ps_iter->pragma_encountered)
      return ps_iter;

  gcc_unreachable ();
  return NULL;
}

/* Insert LOCAL_SIMD_VALUES into the global pragma simd table.  Return
   the index into the table for the new entry.  */

int
psv_head_insert (struct pragma_simd_values local_simd_values)
{
  struct pragma_simd_values *ps_iter = NULL;
 
  struct reduction_values *rv_iter = NULL;

  gcc_assert (local_simd_values.pragma_encountered == true);
  if (psv_head == NULL)
    {
      psv_head = (struct pragma_simd_values *)
	xcalloc (1, sizeof (struct pragma_simd_values));
      psv_head->loc = local_simd_values.loc;
      psv_head->pragma_encountered  = local_simd_values.pragma_encountered;
      psv_head->index = INVALID_PRAGMA_SIMD_SLOT + 1;
      psv_head->types = local_simd_values.types;
      
      psv_head->vectorlength = local_simd_values.vectorlength;
      if (local_simd_values.vectorlength != NULL_TREE)
	psv_head->vlength_OK = false;
      else
	psv_head->vlength_OK = true;
    
      psv_head->private_vars = local_simd_values.private_vars;
      if (local_simd_values.private_vars != NULL_TREE)
	psv_head->pvars_OK = false;
      else
	psv_head->pvars_OK = true;
    
      psv_head->linear_vars = local_simd_values.linear_vars;
      if (local_simd_values.linear_vars != NULL_TREE)
	psv_head->lvars_OK = false;
      else
	psv_head->lvars_OK = true;
    
      psv_head->linear_steps = local_simd_values.linear_steps;
      if (local_simd_values.reduction_vals == NULL)
	psv_head->reduction_vals = NULL;
    
      psv_head->reduction_vals = NULL;
      for (rv_iter = local_simd_values.reduction_vals; rv_iter != NULL;
	   rv_iter = rv_iter->ptr_next) 
	insert_reduction_values (&psv_head->reduction_vals, 
				 rv_iter->reduction_operator, 
				 rv_iter->tree_reduction_var_list);

      if (local_simd_values.reduction_vals != NULL)
	psv_head->rvars_OK = false;
      else
	psv_head->rvars_OK = true;
    
      psv_head->ptr_next = NULL;
      return psv_head->index;
    }
  
  for (ps_iter = psv_head; ps_iter->ptr_next != NULL;
       ps_iter = ps_iter->ptr_next)
    ;

  ps_iter->ptr_next = (struct pragma_simd_values *)
    xcalloc (1, sizeof (struct pragma_simd_values));

  // FIXME: There are a bunch of fields not initialized here:
  // i.e. vlength_OK, pvars_OK, linear_steps_size
  ps_iter->ptr_next->pragma_encountered = local_simd_values.pragma_encountered;
  ps_iter->ptr_next->loc = local_simd_values.loc;
  ps_iter->ptr_next->index = ps_iter->index + 1;
  ps_iter->ptr_next->types = local_simd_values.types;
  ps_iter->ptr_next->vectorlength = local_simd_values.vectorlength;
  ps_iter->ptr_next->private_vars = local_simd_values.private_vars;
  ps_iter->ptr_next->linear_vars = local_simd_values.linear_vars;
  ps_iter->ptr_next->linear_steps = local_simd_values.linear_steps;
  ps_iter->ptr_next->ptr_next = NULL;

  ps_iter->ptr_next->reduction_vals = NULL;
  for (rv_iter = local_simd_values.reduction_vals; rv_iter; 
       rv_iter = rv_iter->ptr_next) 
    insert_reduction_values (&ps_iter->ptr_next->reduction_vals, 
			     rv_iter->reduction_operator, 
			     rv_iter->tree_reduction_var_list);

  return ps_iter->ptr_next->index;
}

/* this function wil check if the user requsted the assert command. That is,
   the user requested the compiler to halt if the loop is not vectorized.  */

bool
pragma_simd_assert_requested_p (int ps_index)
{
  struct pragma_simd_values *ps_iter = NULL;

  if (ps_index == 0) 
    return 0;

  // FIXME: Why not use psv_find_node.
  for (ps_iter = psv_head; ps_iter; ps_iter = ps_iter->ptr_next)
    {
      if ((ps_iter->pragma_encountered == true) && (ps_iter->index == ps_index))
	{
	  if ((ps_iter->types & P_SIMD_ASSERT))
	    return true;
	  else
	    return false;
	}
    }
  return false;
}

/* This function will check to see if the vectorlength found by the 
   vectorizer falls in the list of acceptable vectorlengths provided by the 
   user.  */

bool
pragma_simd_acceptable_vlength_p (int ps_index, 
				  int possible_vectorization_factor, 
				  tree scalar_type)
{
  tree size = NULL_TREE;
  struct pragma_simd_values *ps_iter = NULL;
  int possible_vector_length = 0;
  tree ii_tree = NULL_TREE;
  tree ii_value = NULL_TREE;
  tree vl_tree = NULL_TREE;
  size = size_in_bytes (scalar_type);

  if (ps_index <= INVALID_PRAGMA_SIMD_SLOT) 
    return true; 
  if (TREE_CODE (size) == INTEGER_CST) 
    ;
  else 
    error ("Unable to determine size of scalar type!");

  possible_vector_length = possible_vectorization_factor;

  vl_tree = build_int_cst (integer_type_node, possible_vector_length);

  // FIXME: Why not use psv_find_node?
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
    {
      if ((ps_iter->pragma_encountered == true) && (ps_iter->index == ps_index))
	break;
    }

  gcc_assert (ps_iter != NULL);

  if (list_length (ps_iter->vectorlength) == 0)
    /* the user has not given any vectorlength, so whatever the vectorizer
       the vectorizer found is good enough for us.  */ 
    return true;

  for (ii_tree = ps_iter->vectorlength; ii_tree; ii_tree = TREE_CHAIN (ii_tree))
    {
      ii_value = TREE_VALUE (ii_tree);
      if ((TREE_CODE (ii_value) == INTEGER_CST)
	  && tree_int_cst_equal ((const_tree) ii_value, (const_tree) vl_tree))
	  /* We found a match, so we return TRUE.  */ 
	return true;
    }

  /* A match was not found, hence we return a false.  */
  return false;
}

/* this function will check if we wanted a loop to be vectorized.  */

bool
pragma_simd_vectorize_loop_p (int ps_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  
  if (ps_index <= INVALID_PRAGMA_SIMD_SLOT) 
    return false;

  // FIXME: Why not use psv_find_node?
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next) 
    if (ps_iter->index == ps_index) 
      return ps_iter->pragma_encountered;

  return false;
}

/* This function will insert the appropriate reduction values asked by pragma
   simd reduction into the internal pragma simd list.  */

void
insert_reduction_values (struct reduction_values **reduction_val_head,
			 enum tree_code reduction_op, tree var_list)
{
  struct reduction_values *red_iter = NULL;
  bool found = false;
  if (reduction_val_head == NULL)
    {
      reduction_val_head = (struct reduction_values **)
	xmalloc (sizeof (struct reduction_values *));
      gcc_assert (reduction_val_head != NULL);
    }
  if (*reduction_val_head == NULL)
    {
      *reduction_val_head = (struct reduction_values *)
	xmalloc (sizeof (struct reduction_values));
      gcc_assert (*reduction_val_head != NULL);

      (*reduction_val_head)->reduction_operator = reduction_op;
      (*reduction_val_head)->tree_reduction_var_list  = var_list;
      (*reduction_val_head)->not_reduced = list_length (var_list);
      (*reduction_val_head)->ptr_next = NULL;
      return;
    }
  else
    {
      for (red_iter = *reduction_val_head; red_iter->ptr_next != NULL;
	   red_iter = red_iter->ptr_next)
	{
	  if (red_iter->reduction_operator == reduction_op)
	    {
	      found = true;
	      break;
	    }
	}

      if (found)
	{
	  red_iter->tree_reduction_var_list =
	    tree_cons (NULL_TREE, var_list, red_iter->tree_reduction_var_list);
	  red_iter->not_reduced +=
	    list_length (red_iter->tree_reduction_var_list);
	}
      else
	{
	  red_iter->ptr_next = (struct reduction_values *)
	    xmalloc (sizeof (struct reduction_values));
	  gcc_assert (red_iter->ptr_next != NULL);

	  red_iter->ptr_next->reduction_operator = reduction_op;
	  red_iter->ptr_next->tree_reduction_var_list = var_list;
	  red_iter->ptr_next->not_reduced =
	    list_length (red_iter->tree_reduction_var_list);
	  red_iter->ptr_next->ptr_next = NULL;
	}
      return;
    }
}

/* this function will check if a same variable is in multiple pragma clause 
   for example, the following pragma is illegal and this function should flag 
   them as true: 
	  #pragma simd reduction (+:p) linear(p:1)
 */

bool
same_var_in_multiple_lists_p (struct pragma_simd_values *ps_values)
{
  tree vl_tree = NULL_TREE, vl_value = NULL_TREE;
  tree pr_tree = NULL_TREE, pr_value = NULL_TREE;
  tree rd_tree = NULL_TREE, rd_value = NULL_TREE;
  struct reduction_values *rd_list = NULL;

  if (ps_values->reduction_vals != NULL)
    {
      for (rd_list = ps_values->reduction_vals; rd_list != NULL;
	   rd_list = rd_list->ptr_next)
	{
	  rd_tree = rd_list->tree_reduction_var_list;
	  vl_tree = ps_values->vectorlength;
	  pr_tree = ps_values->private_vars;
	
	  while ((vl_tree != NULL_TREE) || (rd_tree != NULL_TREE)
		 || (pr_tree != NULL_TREE))
	    {
	      if (vl_tree != NULL_TREE)
		vl_value = TREE_VALUE (vl_tree);
	      else
		vl_value = NULL_TREE;
    
	      if (pr_tree != NULL_TREE)
		pr_value = TREE_VALUE (pr_tree);
	      else
		pr_value = NULL_TREE;

	      if (rd_tree != NULL_TREE)
		rd_value = TREE_VALUE (rd_tree);
	      else
		rd_value = NULL_TREE;

	      if (((vl_value != NULL_TREE || pr_value != NULL_TREE)
		   && (simple_cst_equal (vl_value, pr_value) == 1))
		  || ((vl_value != NULL_TREE || rd_value != NULL_TREE)
		      && (simple_cst_equal (vl_value, rd_value) == 1))
		  || ((pr_value != NULL_TREE || rd_value != NULL_TREE)
		      && (simple_cst_equal (pr_value, rd_value) == 1))) 
		return true;

	      if (vl_tree != NULL_TREE)
		vl_tree = TREE_CHAIN (vl_tree);

	      if (pr_tree != NULL_TREE)
		pr_tree = TREE_CHAIN (pr_tree);

	      if (rd_tree != NULL_TREE)
		rd_tree = TREE_CHAIN (rd_tree);
	    }
	}
    }
  else
    {    
      while ((vl_tree != NULL_TREE) || (rd_tree != NULL_TREE)
	     || (pr_tree != NULL_TREE))
	{
	  if (vl_tree != NULL_TREE)
	    vl_value = TREE_VALUE (vl_tree);
	  else
	    vl_value = NULL_TREE;
    
	  if (pr_tree != NULL_TREE)
	    pr_value = TREE_VALUE (pr_tree);
	  else
	    pr_value = NULL_TREE;

	  if (rd_tree != NULL_TREE)
	    rd_value = TREE_VALUE (rd_tree);
	  else
	    rd_value = NULL_TREE;

	  /* If either one is not NULL and the trees are equal, then we 
	     say we have found a duplicate.  */
	  if (((vl_value != NULL_TREE || pr_value != NULL_TREE)
	       && (simple_cst_equal (vl_value, pr_value) == 1))
	      || ((vl_value != NULL_TREE || rd_value != NULL_TREE)
	       && (simple_cst_equal (vl_value, rd_value) == 1))
	      || ((pr_value != NULL_TREE || rd_value != NULL_TREE)
	       && (simple_cst_equal (pr_value, rd_value) == 1))) 
	    return true;
	  if (vl_tree != NULL_TREE)
	    vl_tree = TREE_CHAIN (vl_tree);

	  if (pr_tree != NULL_TREE)
	    pr_tree = TREE_CHAIN (pr_tree);

	  if (rd_tree != NULL_TREE)
	    rd_tree = TREE_CHAIN (rd_tree);
	}
    }
  return false;
  
}

/* This function will check to see if a variable is reduced, if so then it will 
   mark it as being reduced.  */

void
check_off_reduction_var (gimple reduc_stmt, int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  struct reduction_values *rv_iter = NULL;
  enum tree_code op_code = NOP_EXPR;
  tree def = NULL_TREE;
  tree ii_iter = NULL_TREE;
  tree ii_value = NULL_TREE;
  
  
  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return;

  if (psv_head == NULL)
    return;

  if (!reduc_stmt || gimple_code (reduc_stmt) != GIMPLE_ASSIGN)
    return;
  else
    {
      def = gimple_assign_lhs (reduc_stmt);
      gcc_assert (def != NULL_TREE);

      if (TREE_CODE (def) == SSA_NAME) 
	def = SSA_NAME_VAR (def);
      op_code = gimple_assign_rhs_code (reduc_stmt);
    }


  // FIXME: Why not use psv_find_node?
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next) 
    if (ps_iter->pragma_encountered && (ps_iter->index == pragma_simd_index)) 
      break;

  gcc_assert (ps_iter != NULL);

  for (rv_iter = ps_iter->reduction_vals; rv_iter != NULL;
       rv_iter = rv_iter->ptr_next)
    {
      if (rv_iter->reduction_operator == op_code)
	{
	  for (ii_iter = rv_iter->tree_reduction_var_list; ii_iter != NULL_TREE;
	       ii_iter = TREE_CHAIN (ii_iter))
	    {
	      ii_value = TREE_VALUE (ii_iter);
	      if (simple_cst_equal (ii_value, DECL_NAME (def))) 
		rv_iter->not_reduced--;
	    }
	}
    }
  return;
}
      
/* This function will find the step size from the pragma simd linear clause.  */

HOST_WIDE_INT
find_linear_step_size (int pragma_simd_index, tree var)
{
  tree ii_var_iter   = NULL_TREE;
  tree ii_var_value  = NULL_TREE;
  tree ii_step_iter  = NULL_TREE;
  tree ii_step_value = NULL_TREE;
  tree real_var      = NULL_TREE;
  HOST_WIDE_INT step = 0;
  struct pragma_simd_values *ps_iter = NULL;
  
  if (psv_head == NULL)
    return 0;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return 0;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
    {
      if (ps_iter->pragma_encountered && (ps_iter->index == pragma_simd_index))
	{
	  ii_var_iter  = ps_iter->linear_vars;
	  ii_step_iter = ps_iter->linear_steps;

	  while ((ii_var_iter  != NULL_TREE) && (ii_step_iter != NULL_TREE))
	    {
	      ii_var_value  = TREE_VALUE (ii_var_iter);
	      ii_step_value = TREE_VALUE (ii_step_iter);
	      if (TREE_CODE (var) == SSA_NAME)
		real_var = SSA_NAME_VAR (var);
	      else
		real_var = var;

	      if (simple_cst_equal (ii_var_value, DECL_NAME (real_var)))
		{
		  step = int_cst_value (ii_step_value);
		  return step;
		}
	    }
	}
    }
  return 0;
}

/* Goes through the private vars of pragma simd structure pointed by PS_INDEX
   and checks of DEF_VAR is in the list and if so returns true.  */

bool
pragma_simd_is_private_var (int ps_index, tree def_var)
{
  struct pragma_simd_values *ps_node = NULL;
  tree variable, ii_priv_list;
  
  if (def_var == NULL_TREE)
    return false;

  ps_node = psv_find_node (ps_index);
  if (!ps_node)
    return false;

  /* If it is a SSA_NAME, then this will extract the original variable name.  */
  if (TREE_CODE (def_var) == SSA_NAME)
    {
      if (SSA_NAME_VAR (def_var) != NULL_TREE)
	{
	  def_var = SSA_NAME_VAR (def_var);
	  if (TREE_CODE (def_var) == VAR_DECL
	      || TREE_CODE (def_var) == PARM_DECL)
	    {
	      tree variable = DECL_NAME (def_var);
	      if (variable == NULL_TREE)
		return false;
	      for (ii_priv_list = ps_node->private_vars; ii_priv_list;
		   ii_priv_list = TREE_CHAIN (ii_priv_list))
		{
		  tree ii_priv_value = TREE_VALUE (ii_priv_list);
		  if (simple_cst_equal (ii_priv_value, variable) == 1)
		    return true;
		}
	    }
	}
      else if (SSA_NAME_DEF_STMT (def_var))
	{
	  bool found = false;
	  gimple def_stmt = SSA_NAME_DEF_STMT (def_var);

	  /* We go through all the ops in the def_stmt's RHS.  */
	  for (size_t ii = 1; ii < gimple_num_ops (def_stmt); ii++)
	    {
	      tree var_name, var = gimple_op (def_stmt, ii);
	      if (!var)
		continue;
	      else if (TREE_CODE (var) == SSA_NAME && SSA_NAME_VAR (var))
		var = SSA_NAME_VAR (var);

	      if (TREE_CODE (var) == VAR_DECL || TREE_CODE (var) == PARM_DECL)
		var_name = DECL_NAME (var);
	      else
		var_name = var; /* It should not get here.  */

	      /* Here we go through all the variables in the private list.
		 If we have a match, then we set found to true.  If we didn't
		 have a match, then found will be false and then we return that.
		 This means that the variable we are looking in def is dependent
		 on variables that are not on the private list.  */
	      for (ii_priv_list = ps_node->private_vars; ii_priv_list;
		   ii_priv_list = TREE_CHAIN (ii_priv_list))
		{
		  tree ii_priv_value = TREE_VALUE (ii_priv_list);
		  if (simple_cst_equal (ii_priv_value, var_name) == 1)
		    found = true;
		}
	      if (found == false)
		return false;
	    }
	  return found;
	}
    }
  else if (TREE_CODE (def_var) == VAR_DECL || TREE_CODE (def_var) == PARM_DECL)
    {
      variable = DECL_NAME (def_var);
      if (variable == NULL_TREE)
	return false;

      for (ii_priv_list = ps_node->private_vars; ii_priv_list;
	   ii_priv_list = TREE_CHAIN (ii_priv_list))
	{
	  tree ii_priv_value = TREE_VALUE (ii_priv_list);
	  if (simple_cst_equal (ii_priv_value, variable) == 1)
	    return true;
	}
    }
  return false;
}
