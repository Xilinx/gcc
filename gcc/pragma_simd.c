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
#include "tree.h"
#include "gimple.h"
#include "diagnostic-core.h"
#include "cilk.h"

vec<struct pragma_simd_values, va_gc> *p_simd_info_list;

/* Traverses the global vector P_SIMD_INFO_LIST and returns the node whose 
   field "INDEX" as PRAGMA_SIMD_INDEX.  If no match is found, returns NULL.  */

static struct pragma_simd_values *
psv_find_node (unsigned int pragma_simd_index)
{
  unsigned int ii = 0;
  struct pragma_simd_values *return_value = NULL; 
  for (ii = 0; ii < vec_safe_length (p_simd_info_list); ii++)
    if (pragma_simd_index == (*p_simd_info_list)[ii].index)
      return_value = &(*p_simd_info_list)[ii];
  return return_value;
}

static vec<struct reduction_node, va_gc> *
psv_find_reduction_list (unsigned int pragma_simd_index)
{
  unsigned int ii = 0;
  vec<struct reduction_node, va_gc> *return_value = NULL;

  for (ii = 0; ii < vec_safe_length (p_simd_info_list); ii++)
    if (pragma_simd_index == (*p_simd_info_list)[ii].index)
      return_value = ((*p_simd_info_list)[ii].reduction_list);
  return return_value;
}

/* Returns true of all the clauses of KIND in the node with the matching 'index'
   field to PRAGMA_SIMD_INDEX.  */

static bool
clause_resolved_p (enum pragma_simd_kind kind, unsigned int pragma_simd_index)
{
  struct pragma_simd_values *p_simd_node;
  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return true;

  p_simd_node = psv_find_node (pragma_simd_index);
  if (!p_simd_node)
    return true;
 
  switch (kind)
    {
    case P_SIMD_VECTORLENGTH:
      if (!vec_safe_is_empty (p_simd_node->vec_length_list)) 
	return p_simd_node->vlength_OK;
      else
	return true;
    case P_SIMD_PRIVATE:
      if (!vec_safe_is_empty (p_simd_node->priv_var_list))
	return p_simd_node->private_OK;
      else
	return true;
    case P_SIMD_LINEAR:
      if (!vec_safe_is_empty (p_simd_node->linear_var_list))
	return p_simd_node->linear_OK;
      else
	return true;
    case P_SIMD_REDUCTION:
      if (!vec_safe_is_empty (p_simd_node->reduction_list))
	return p_simd_node->reduction_OK;
      else
	return true;
    default:
      return false;
    }

  /* If we can't find a certain pragma, then just return tur to make sure we
     do not assert/halt.  */
  return true;
}

/* Returns true of the node whose field 'index' matches PRAGMA_SIMD_INDEX
   has the assert clause.  */

bool
pragma_simd_assert_requested_p (unsigned int pragma_simd_index)
{
  if (!psv_find_node (pragma_simd_index))
    return false;
  else
    return psv_find_node (pragma_simd_index)->assert_requested;
}

/* Verify that the <#pragma simd> clauses have been properly resolved.
   INDEX is the pragma_simd_index into the global table.  */

void
pragma_simd_verify_clauses (unsigned int index)
{
  struct pragma_simd_values *vals = psv_find_node (index);
  location_t loc = vals ? vals->loc : UNKNOWN_LOCATION;

  if ((!clause_resolved_p (P_SIMD_VECTORLENGTH, index)))
    {
      if (pragma_simd_assert_requested_p (index))
	error_at (loc, "vectorlength in pragma simd not picked from list");
      else 
	warning_at (loc, 0,
		    "vectorlength in pragma simd not picked from list");
    }
  if (!clause_resolved_p (P_SIMD_PRIVATE, index))
    { 
      if (pragma_simd_assert_requested_p (index))
	error_at (loc, "unable to make all variables private");
      else
	warning_at (loc, 0,
		    "unable to make all variables private in pragma simd");
    }     
  if (!clause_resolved_p (P_SIMD_LINEAR, index))
    {
      if (pragma_simd_assert_requested_p (index))
	error_at (loc, "unable to pick requested step-size in pragma simd");
      else
	warning (loc, "unable to pick requested step-size in pragma simd");
    }
  if (!clause_resolved_p (P_SIMD_REDUCTION, index))
    {
      if (pragma_simd_assert_requested_p (index))
	error_at (loc, "unable to satisfy all reductions in pragma simd");
      else
	warning_at (loc, 0, "unable to satisfy all reductions in pragma simd");
    }
}


/* Assigns the "OK" field of a certain clause (indicated by KIND) in a node
   whose 'index' field matches pragma_simd_index to SET_VALUE.  */
   
void
set_OK_for_certain_clause (enum pragma_simd_kind kind, bool set_value,
			   unsigned int pragma_simd_index)
{
  unsigned int ii = 0;
  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return;
  
  for (ii = 0; ii < vec_safe_length (p_simd_info_list); ii++)
    if ((*p_simd_info_list)[ii].index == pragma_simd_index)
      {
	if (kind == P_SIMD_VECTORLENGTH)
	  (*p_simd_info_list)[ii].vlength_OK = set_value;
	else if (kind == P_SIMD_PRIVATE)
	  (*p_simd_info_list)[ii].private_OK = set_value;
	else if (kind == P_SIMD_REDUCTION)
	  (*p_simd_info_list)[ii].reduction_OK = set_value;
	else if (kind == P_SIMD_LINEAR)
	  (*p_simd_info_list)[ii].linear_OK = set_value;
	else
	  gcc_unreachable ();
	break;
      }
}
      

/* Clears all the node in *p_simd_info_list.  */

void
p_simd_nodes_clear (void)
{
  vec_free (p_simd_info_list);
}

/* Inserts *P_SIMD_NODE into P_SIMD_INFO_LIST and assigns index of P_SIMD_NODE
   to the P_SIMD_INFO_LIST->head->index + 1.  Returns the P_SIMD_NODE->index of
   type unsigned int.  */

unsigned int
p_simd_head_insert (struct pragma_simd_values *p_simd_node)
{
  if (p_simd_node)
    {
      if (vec_safe_is_empty (p_simd_info_list))
	p_simd_node->index = INVALID_PRAGMA_SIMD_SLOT + 1;
      else
	{
	  unsigned int vec_length = vec_safe_length (p_simd_info_list);
	  p_simd_node->index = (*p_simd_info_list)[vec_length-1].index + 1;
	}

      vec_safe_push (p_simd_info_list, *p_simd_node);

      return p_simd_node->index;
    }
  return INVALID_PRAGMA_SIMD_SLOT;
}



/* Finds the pragma_simd_node whose index matches the pragma_simd_index and
   goes through all the vectorlengths specified and returns TRUE if there is
   a match.  Otherwise, the function returns false.  */

bool
pragma_simd_acceptable_vlength_p (unsigned int pragma_simd_index,
				  int possible_vect_factor,
				  tree scalar_type ATTRIBUTE_UNUSED)
{
  unsigned int ii = 0, jj = 0;
  tree vl_value;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return true;
  
  vl_value = build_int_cst (integer_type_node, possible_vect_factor);

  for (ii = 0; ii < vec_safe_length (p_simd_info_list); ii++)
    if (pragma_simd_index == (*p_simd_info_list)[ii].index)
      {
	vec<tree, va_gc> *vl_vec = (*p_simd_info_list)[ii].vec_length_list;
	if (vec_safe_length (vl_vec) == 0)
	  return true;
	for (jj = 0; jj < vec_safe_length (vl_vec); jj++)
	  {
	    if (tree_int_cst_equal ((const_tree) (*vl_vec)[jj],
				    (const_tree) (vl_value)))
	      return true;
	  }
      }

  /* A match was not found, hence return false.  */
  return false;
}

/* Goes through the private vars of pragma simd structure pointed by PS_INDEX
   and checks of DEF_VAR is in the list and if so returns true.  */

bool
pragma_simd_is_private_var_p (unsigned int ps_index, tree def_var)
{
  struct pragma_simd_values *ps_node = NULL;
  tree variable;
  unsigned int ii = 0, jj = 0;
  
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
	      for (ii = 0; ii < vec_safe_length (ps_node->priv_var_list); ii++)
		{
		  tree priv_value = (*(ps_node->priv_var_list))[ii];
		  if (simple_cst_equal (priv_value, variable) == 1)
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
	      for (jj = 0; jj < vec_safe_length (ps_node->priv_var_list); jj++)
		{
		  tree priv_value = (*(ps_node->priv_var_list))[jj];
		  if (simple_cst_equal (priv_value, var_name) == 1)
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

      for (ii = 0; ii < vec_safe_length (ps_node->priv_var_list); ii++)
	{
	  tree priv_value = (*(ps_node->priv_var_list))[ii];
	  if (simple_cst_equal (priv_value, variable) == 1)
	    return true;
	}
    }
  return false;
}

/* Goes through the linear vars of pragma simd structure pointed by PS_INDEX
   and checks of DEF_VAR is in the list and if so returns true.  */

bool
pragma_simd_is_linear_var_p (unsigned int ps_index, tree def_var)
{
  struct pragma_simd_values *ps_node = NULL;
  tree variable;
  unsigned int ii = 0, jj = 0;
  
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
	      for (ii = 0; ii < vec_safe_length (ps_node->linear_var_list);
		   ii++)
		{
		  tree priv_value = (*(ps_node->linear_var_list))[ii];
		  if (simple_cst_equal (priv_value, variable) == 1)
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
	      for (jj = 0; jj < vec_safe_length (ps_node->linear_var_list);
		   jj++)
		{
		  tree priv_value = (*(ps_node->linear_var_list))[jj];
		  if (simple_cst_equal (priv_value, var_name) == 1)
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

      for (ii = 0; ii < vec_safe_length (ps_node->priv_var_list); ii++)
	{
	  tree priv_value = (*(ps_node->priv_var_list))[ii];
	  if (simple_cst_equal (priv_value, variable) == 1)
	    return true;
	}
    }
  return false;
}

/* Outputs all the index fields in P_SIMD_INFO_LIST global vector to the
   file pointed by *FPTR.  */

void
dump_pragma_simd_values_vec (FILE *fptr)
{
  unsigned int ii = 0;

  if (!fptr)
    fptr = stderr;
  for (ii = 0; ii < vec_safe_length (p_simd_info_list); ii++)
    {
      fprintf (fptr, "index           =  %2d\n", (*p_simd_info_list)[ii].index);
      fprintf (fptr, "assert_requested = %s\n",
	       (*p_simd_info_list)[ii].assert_requested ? "true" : "false");
    }
  return;
}

/* Returns the step size for VAR that is in the linear clause stored in the
   pragma_simd_index data vector pointed by pragma_simd_index.  */

HOST_WIDE_INT
find_linear_step_size (unsigned int pragma_simd_index, tree var)
{
  unsigned int ii = 0;
  tree real_var = NULL_TREE;
  struct pragma_simd_values *ps_node = psv_find_node (pragma_simd_index);
  
  if (!ps_node)
    return 0;

  if (TREE_CODE (var) == SSA_NAME)
    real_var = SSA_NAME_VAR (var);
  else
    real_var = var;
  
  for (ii = 0; ii < vec_safe_length (ps_node->linear_var_list); ii++)
    if (real_var 
	&& simple_cst_equal (DECL_NAME (real_var), 
			     (*(ps_node->linear_var_list))[ii]))
      return int_cst_value ((*(ps_node->linear_steps_list))[ii]);
  
  return 0;
}

/* Returns true when all the reductions_var in the pragma simd data structure
   with index field matching PRAGMA_SIMD_INDEX is NULL or has a length of 0.  */

bool
all_reductions_satisfied_p (unsigned int pragma_simd_index)
{
  unsigned int ii;
  bool satisfied = true;
  vec <struct reduction_node, va_gc> *red_list =
    psv_find_reduction_list (pragma_simd_index);

  if (!red_list || vec_safe_length (red_list) == 0)
    return true;

  for (ii = 0; ii < vec_safe_length (red_list); ii++)
    if (vec_safe_length ((*red_list)[ii].reduction_vars) != 0)
      satisfied = false;

  return satisfied;
}

/* If definition variable of REDUC_STMT is marked as a reduction variable
   in the pragma simd data structure whose index field matches PS_INDEX, the
   variable is removed from the list.  */

void
check_off_reduction_var (gimple reduc_stmt, unsigned int ps_index)
{
  enum tree_code op_code = NOP_EXPR;
  tree def;
  bool found = false;
  unsigned int ii, jj;
  vec<struct reduction_node, va_gc> *red_list =
    psv_find_reduction_list (ps_index);

  if (!red_list || vec_safe_length (red_list) == 0)
    return;
  
  if (!reduc_stmt || gimple_code (reduc_stmt) != GIMPLE_ASSIGN)
    return;

  def = gimple_assign_lhs (reduc_stmt);
  gcc_assert (def != NULL_TREE);

  if (TREE_CODE (def) == SSA_NAME)
    def = SSA_NAME_VAR (def);
  if (TREE_CODE (def) == VAR_DECL || TREE_CODE (def) == PARM_DECL)
    def = DECL_NAME (def);
  op_code = gimple_assign_rhs_code (reduc_stmt);

  for (ii = 0; ii < vec_safe_length (red_list); ii++)
    if ((*red_list)[ii].reduction_type == (int) op_code)
      {
	unsigned int length = vec_safe_length ((*red_list)[ii].reduction_vars);
	for (jj = 0; jj < length; jj++)
	  {
	    tree var = (*((*red_list)[ii].reduction_vars))[jj];
	    if (simple_cst_equal (var, def))
	      {
		found = true;
		break;
	      }
	  }
	if (found)
	  (*red_list)[ii].reduction_vars->unordered_remove (jj);
      }
}
