/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains C/C++ specific functions for elemental
   functions.
   
   Copyright (C) 2012  Free Software Foundation, Inc.
   Written by Balaji V. Iyer <balaji.v.iyer@intel.com>,
              Intel Corporation

   Many Thanks to Karthik Kumar for advice on the basic technique
   about cloning functions.
   
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
#include "langhooks.h"
#include "cilk.h"
#include "basic-block.h"
#include "output.h"
#include "c-family/c-common.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "tree-pass.h"
#include "timevar.h"
#include "flags.h"
#include "c/c-tree.h"
#include "tree-inline.h"
#include "cgraph.h"
#include "ipa-prop.h"
#include "opts.h"
#include "tree-iterator.h"
#include "options.h"
#include "intl.h"
#include "vec.h"
#include "target.h"

static tree create_optimize_attribute (int);
static tree create_processor_attribute (elem_fn_info *, tree *);
static tree elem_fn_build_array (tree base_var, tree index);

enum elem_fn_parm_size {
  SCALAR = 0,
  VECTOR_SEGMENTED = 1,
  VECTOR_NONSEGMENTED = 2
};

typedef struct
{
  tree arg;
  tree replacement;
} args_data;

typedef struct
{
  tree var_name;
  vec<tree, va_gc> *substitute_vars;
} var_expand_struct;

/* Creates the appropriate __target__ attribute for the processor information
   given in ELEM_FN_VALUES->proc_type.  The function also returns the opposite
   attribute through OPPOSITE_ATTR for the scalar function.  */

static tree
create_processor_attribute (elem_fn_info *elem_fn_values, tree *opposite_attr)
{
  if (elem_fn_values)
    return targetm.cilkplus.builtin_map_processor_to_attr
      (elem_fn_values->proc_type, opposite_attr);
  else
    {
      /* We should never get here.  If we get here, something wrong has
	 happened, so we reset the whole proc. attribute.  */
      *opposite_attr = NULL_TREE;
      return NULL_TREE;
    }
}

/* Goes through all the uniform and linear variables in the ELEM_FN_VALUES and
   if those variables are set to VECTOR_TYPE in FNDECL by the
   tree_elem_fn_versioning function then we convert it back to scalar.  */

static void
scalarize_uniform_linear_params (tree fndecl, elem_fn_info *elem_fn_values)
{
  size_t ii = 0;
  tree ii_tree;
  if (!elem_fn_values || !fndecl)
    return;

  for (ii_tree = DECL_ARGUMENTS (fndecl); ii_tree; 
       ii_tree = DECL_CHAIN (ii_tree))
    {
      for (ii = 0; ii < (size_t) elem_fn_values->no_uvars; ii++)
	if (!strcmp (elem_fn_values->uniform_vars[ii],
		     IDENTIFIER_POINTER (DECL_NAME (ii_tree))))
	  {
	    tree type_t = TREE_TYPE (TREE_TYPE (ii_tree));
	    TREE_TYPE (ii_tree) = type_t;
	    DECL_ARG_TYPE (ii_tree) = type_t;
	  }
      for (ii = 0; ii < (size_t) elem_fn_values->no_lvars; ii++)
	if (!strcmp (elem_fn_values->linear_vars[ii],
		     IDENTIFIER_POINTER (DECL_NAME (ii_tree))))
	  {
	    tree type_t = TREE_TYPE (TREE_TYPE (ii_tree));
	    TREE_TYPE (ii_tree) = type_t;
	    DECL_ARG_TYPE (ii_tree) = type_t;
	  }
    }
  return;
}
  

/* Returns an optimize attribute for the Optimization level given by OPTION.  */

static tree
create_optimize_attribute (int option)
{
  tree opt_attr;
  vec<tree, va_gc> *opt_vec;
  char optimization[2];

  optimization[0] = 'O';
  vec_alloc (opt_vec, 4);
  
  if (option == 3)
    optimization[1] = '3';
  else if (option == 2)
    optimization[1] = '2';
  else if (option == 1)
    optimization[1] = '1';
  else if (option == 0)
    optimization[1] = '0';
  
  vec_safe_push (opt_vec, build_string (2, optimization));
  opt_attr = build_tree_list_vec (opt_vec);
  vec_safe_truncate (opt_vec, 0);
  opt_attr = build_tree_list (get_identifier ("optimize"), opt_attr);
  return opt_attr;
}

/* This function will replace parameter stored in DATA->arg with
   DATA->replacement in *TP. If *WALK_SUBTREES is set to 0, then the subtrees
   of *TP will not be stepped through.  */

static tree
replace_parm_decl (tree *tp, int *walk_subtrees, void *data)
{
  if (!tp)
    return NULL_TREE;

  if (TREE_CODE (*tp) == PARM_DECL)
    {
      args_data *value = (args_data *) data;
      if (DECL_NAME (*tp) == DECL_NAME (value->arg))
	*tp = value->replacement;

      *walk_subtrees = 0;
    }
  return NULL_TREE;
}
  

/* Stores the return expression to a temporary var in DATA (typecasted to tree)
   in a set of *TP tree.  If *WALK_SUBTREES is set to 1, then we walk through
   the subtrees of *TP.  */

static tree
replace_return_with_new_var (tree *tp, int *walk_subtrees, void *data)
{
  tree mod_expr = NULL_TREE, return_var = NULL_TREE, ret_expr = NULL_TREE;
  
  if (!*tp)
    return NULL_TREE;

  if (TREE_CODE (*tp) == RETURN_EXPR)
    {
      return_var = (tree) data;
      ret_expr = TREE_OPERAND (TREE_OPERAND (*tp, 0), 1);
      mod_expr = build2 (MODIFY_EXPR, TREE_TYPE (return_var), return_var,
			 ret_expr);
      *tp = mod_expr;
      *walk_subtrees = 0;
    }
  return NULL_TREE;
}

/* Creates an ARRAY_REF expression for BASE_VAR array with INDEX as the
   appropriate index.  */

static tree
elem_fn_build_array (tree base_var, tree index)
{
  return build_array_ref (EXPR_LOCATION (base_var), base_var, index);
}

/* Replaces all the vector references in *TP with array references stored in
   DATA (type casted to fn_vect_elements that stores this info).  If
   *WALK_SUBTREES is set to 1, then we recurse through all the subtrees of
   *TP.  */

static tree
replace_array_ref_for_vec (tree *tp, int *walk_subtrees, void *data)
{
  size_t ii = 0;
  tree ii_var, add_expr = NULL_TREE, mult_expr = NULL_TREE;
  fn_vect_elements *func_data;
  if (!*tp)
    return NULL_TREE;

  if (TREE_CODE (*tp) == VAR_DECL || TREE_CODE (*tp) == PARM_DECL)
    {
      func_data = (fn_vect_elements *) data;
      gcc_assert (func_data->induction_var);
      for (ii_var = func_data->arguments; ii_var; ii_var = DECL_CHAIN (ii_var))
	{
	  if (DECL_NAME (ii_var) == DECL_NAME (*tp))
	    {
	      /* If the TREE_CODE of the TREE_TYPE is not a vector, then it
		 means that the variable is a UNIFORM or LINEAR and thus we do
		 not need to break it up into array.  */
	      if (TREE_CODE (TREE_TYPE (*tp)) == VECTOR_TYPE)
		*tp =  elem_fn_build_array (*tp, func_data->induction_var);
	      else
		/* Now we go through all the linear variable list and
		   if we have a hit, then we multiply the induction var with
		   step-size and add it to the variable.  */		     
		for (ii = 0; ii < (size_t) func_data->no_lvars; ii++)
		  if (!strcmp (IDENTIFIER_POINTER (DECL_NAME (*tp)),
			       func_data->linear_vars[ii]))
		    {
		      mult_expr = fold_build2
			(MULT_EXPR, TREE_TYPE (func_data->induction_var),
			 func_data->induction_var,
			 build_int_cst (integer_type_node,
					func_data->linear_steps[ii]));
		      mult_expr = build_c_cast (EXPR_LOCATION (*tp),
						TREE_TYPE (*tp), mult_expr);
		      add_expr = fold_build2 (PLUS_EXPR, TREE_TYPE (*tp),
					      *tp, mult_expr);
		      *tp = add_expr;
		    }
		
	      *walk_subtrees = 0;
	      return NULL_TREE;
	    }
	}
      if (func_data->return_var 
	  && (DECL_NAME (*tp) == DECL_NAME (func_data->return_var)))
	{
	  *tp = elem_fn_build_array (*tp, func_data->induction_var);
	  *walk_subtrees = 0;
	}
    }
  return NULL_TREE;
}

/* Moves the return values of function FNDECL toward the end of the function.
   The return is replaced with INDUCTION_VAR.  */
   
static void
fix_elem_fn_return_value (tree fndecl, elem_fn_info *elem_fn_values,
			  tree induction_var)
{
  size_t ii = 0;
  fn_vect_elements data;
  tree old_fndecl;
  tree new_var, new_var_init,  new_body = NULL_TREE;
  tree ret_expr, ret_stmt = NULL_TREE;
  if (!fndecl || !DECL_SAVED_TREE (fndecl))
    return;

  if (TREE_TYPE (DECL_RESULT (fndecl)) == void_type_node)
    return;

  old_fndecl = current_function_decl;
  push_cfun (DECL_STRUCT_FUNCTION (fndecl));
  current_function_decl = fndecl;
  
  new_var = create_tmp_var (TREE_TYPE (DECL_RESULT (fndecl)), "elem_fn_ret");
  new_var_init =
    build_vector_from_val
    (TREE_TYPE (DECL_RESULT (fndecl)),
     build_zero_cst (TREE_TYPE (TREE_TYPE (DECL_RESULT (fndecl)))));
  DECL_INITIAL (new_var) = new_var_init;
  walk_tree (&DECL_SAVED_TREE (fndecl), replace_return_with_new_var,
	     (void *) new_var, NULL);
  data.return_var = new_var;
  data.arguments = DECL_ARGUMENTS (fndecl);
  data.induction_var = induction_var;
  for (ii = 0; ii < (size_t) elem_fn_values->no_lvars; ii++)
    {
      data.linear_vars[ii] = xstrdup (elem_fn_values->linear_vars[ii]);
      data.linear_steps[ii] = elem_fn_values->linear_steps[ii];
    }
  data.no_lvars = elem_fn_values->no_lvars;
  walk_tree (&DECL_SAVED_TREE (fndecl), replace_array_ref_for_vec,
	     (void *) &data, NULL);
  ret_expr = build2 (MODIFY_EXPR, TREE_TYPE (new_var),
		     DECL_RESULT (fndecl), new_var);
  
  ret_stmt = build1 (RETURN_EXPR, TREE_TYPE (ret_expr), ret_expr);
  if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
    {
      if (!BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)))
        ;
      else if (TREE_CODE (BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl))) !=
	       TREE_LIST)
	{
	  append_to_statement_list_force
	    (BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)), &new_body);
	  append_to_statement_list_force (ret_stmt, &new_body);
	}
      else
	{
	  new_body = BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl));
	  append_to_statement_list_force (ret_stmt, &new_body);
	}
      BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)) = new_body;
    }
  
  pop_cfun ();
  current_function_decl = old_fndecl;
  return;
}

/* Converts the vector value in FNDECL to a scalar one with a for-loop that
   goes from 0->(VLENGTH-1).  */

static tree
add_elem_fn_loop (tree fndecl, int vlength)
{
  tree exit_label = NULL_TREE, if_label = NULL_TREE, body_label = NULL_TREE;
  tree fn_body, loop = NULL_TREE, loop_var, mod_var, incr_expr, cond_expr;
  tree cmp_expr, old_fndecl;
  
  if (!fndecl)
    return NULL_TREE; 

  if (!DECL_SAVED_TREE (fndecl))
    return NULL_TREE;

  old_fndecl = current_function_decl;
  push_cfun (DECL_STRUCT_FUNCTION (fndecl));
  current_function_decl = fndecl;
  
  if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
    fn_body = BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl));
  else
    fn_body = DECL_SAVED_TREE (fndecl);

  loop = alloc_stmt_list ();
  
  loop_var = create_tmp_var (size_type_node, "ii_elem_fn_vec_val");
  mod_var = build2 (MODIFY_EXPR, void_type_node, loop_var,
		    build_int_cst (size_type_node, 0));
  append_to_statement_list_force (mod_var, &loop);
  
  if_label = build_decl (EXPR_LOCATION (fndecl), LABEL_DECL,
			 get_identifier ("if_lab"), void_type_node);
  DECL_CONTEXT (if_label) = fndecl;
  DECL_ARTIFICIAL (if_label) = 0;
  DECL_IGNORED_P (if_label) = 1;

  exit_label = build_decl (EXPR_LOCATION (fndecl), LABEL_DECL,
			   get_identifier ("exit_label"), void_type_node);
  DECL_CONTEXT (exit_label) = fndecl;
  DECL_ARTIFICIAL (exit_label) = 0;
  DECL_IGNORED_P (exit_label) = 1;

  body_label = build_decl (EXPR_LOCATION (fndecl), LABEL_DECL,
			   get_identifier ("body_label"), void_type_node);
  DECL_CONTEXT (body_label) = fndecl;
  DECL_ARTIFICIAL (body_label) = 0;
  DECL_IGNORED_P (body_label) = 1;
  append_to_statement_list_force (build1 (LABEL_EXPR, void_type_node,
					  if_label), &loop);
  cmp_expr = build2 (LT_EXPR, boolean_type_node, loop_var,
		     build_int_cst (size_type_node, vlength));
  cond_expr = build3 (COND_EXPR, void_type_node, cmp_expr,
		      build1 (GOTO_EXPR, void_type_node, body_label),
		      build1 (GOTO_EXPR, void_type_node, exit_label));

  append_to_statement_list_force (cond_expr, &loop);
  append_to_statement_list_force (build1 (LABEL_EXPR, void_type_node,
					  body_label), &loop);
  append_to_statement_list_force (fn_body, &loop);

  incr_expr = build2 (MODIFY_EXPR, void_type_node, loop_var,
		      build2 (PLUS_EXPR, TREE_TYPE (loop_var), loop_var,
			      build_int_cst (size_type_node, 1)));

  append_to_statement_list_force (incr_expr, &loop);
  append_to_statement_list_force (build1 (GOTO_EXPR, void_type_node, if_label),
				  &loop);
  append_to_statement_list_force (build1 (LABEL_EXPR, void_type_node,
					  exit_label), &loop);
  
  if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
    BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)) = loop;
  else
    DECL_SAVED_TREE (fndecl) = loop;

  pop_cfun ();
  current_function_decl = old_fndecl;
  
  return loop_var;
}

/* Adds a mask if-statement for FNDECL function.  */ 

static void
add_elem_fn_mask (tree fndecl)
{
  tree ii_arg;
  tree cond_expr, cmp_expr, old_fndecl;
  tree fn_body = NULL_TREE;

  old_fndecl = current_function_decl;
  push_cfun (DECL_STRUCT_FUNCTION (fndecl));
  current_function_decl = fndecl;
  
  if (!DECL_SAVED_TREE (fndecl))
    return;
  
  for (ii_arg = DECL_ARGUMENTS (fndecl); DECL_CHAIN (ii_arg);
       ii_arg = DECL_CHAIN (ii_arg))
    {
      ;
    }
  if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
    fn_body = BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl));
  else
    fn_body = DECL_SAVED_TREE (fndecl); /* Not sure if we ever get here.  */

  gcc_assert (DECL_NAME (ii_arg) == get_identifier ("__elem_fn_mask"));

  cmp_expr = fold_build2 (NE_EXPR, TREE_TYPE (ii_arg), ii_arg,
			  build_int_cst (TREE_TYPE (TREE_TYPE (ii_arg)), 0));
  cond_expr = fold_build3 (COND_EXPR, void_type_node, cmp_expr, fn_body,
			   build_empty_stmt (EXPR_LOCATION (fndecl)));

  if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
    BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)) = cond_expr;
  else
    DECL_SAVED_TREE (fndecl) = cond_expr;

  pop_cfun ();
  current_function_decl = old_fndecl;
  
  return;
 
}

/* Inserts the tree expression EXPR as the first statement for the function
   FNDECL.  */

static void
insert_as_first_stmt (tree expr, tree fndecl)
{
  tree body = NULL_TREE, new_body = NULL_TREE;
  if (fndecl == NULL_TREE)
    return;
  if (expr == NULL_TREE)
    return;

  body = DECL_SAVED_TREE (fndecl);
  if (!body)
    return;

  if (TREE_CODE (body) == BIND_EXPR)
    body = BIND_EXPR_BODY (body);

  if (TREE_CODE (body) == STATEMENT_LIST)
    {
      tree_stmt_iterator tsi = tsi_start (body);
      tsi_link_before (&tsi, expr, TSI_CONTINUE_LINKING);
    }
  else
    {
      new_body = alloc_stmt_list ();
      append_to_statement_list_force (expr, &new_body);
      append_to_statement_list_force (body, &new_body);
      if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
	BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)) = new_body;
    }
  return;
}

/* Segments all the vector parameters of FNDECL into the sizes of largest
   vector register possible.  */

static void
segment_params_for_reg_size (tree fndecl)
{
  size_t ii = 0, jj = 0, kk = 0, ll = 0, ii_narg = 0, nargs_reqd = 0;
  enum elem_fn_parm_size *param_array;
  unsigned HOST_WIDE_INT biggest_vec_reg_size = BIGGEST_ALIGNMENT;
  tree p = NULL_TREE, new_expr = NULL_TREE;
  tree *param_var_array = NULL, *parm_type_array = NULL, *new_parm_var = NULL;
  unsigned HOST_WIDE_INT *param_index = NULL;
  unsigned HOST_WIDE_INT param_no = 0;
  int *param_length = NULL;
  struct function *f = DECL_STRUCT_FUNCTION (fndecl);
  
  gcc_assert (f);
  push_cfun (f);

  for (p = DECL_ARGUMENTS (fndecl); p; p = DECL_CHAIN (p))
    param_no++;

  param_length = XNEWVEC (int, param_no);
  gcc_assert (param_length);
  ii = 0;
  for (p = DECL_ARGUMENTS (fndecl); p; p = DECL_CHAIN (p))
    {
      unsigned HOST_WIDE_INT p_size = 0;
      if (TREE_TYPE (p) && TREE_CODE (TREE_TYPE (p)) == VECTOR_TYPE)
	p_size = tree_low_cst (TYPE_SIZE (TREE_TYPE (p)), 1);
      if (p_size > biggest_vec_reg_size)
	{
	  nargs_reqd += p_size / biggest_vec_reg_size;
	  param_length[ii] = p_size / biggest_vec_reg_size;
	}
      else
	{
	  nargs_reqd++;
	  param_length[ii] = 1;
	}
      ii++;
    }
  if (nargs_reqd == 0)
    return;

  param_array = XNEWVEC (enum elem_fn_parm_size, nargs_reqd);
  gcc_assert (param_array != NULL);

  param_var_array = XNEWVEC (tree, nargs_reqd);
  gcc_assert (param_var_array != NULL);

  parm_type_array = XNEWVEC (tree, nargs_reqd);
  gcc_assert (parm_type_array != NULL);

  param_index = XNEWVEC (unsigned HOST_WIDE_INT, nargs_reqd);
  gcc_assert (param_index != NULL);
  param_no = 0;
  for (p = DECL_ARGUMENTS (fndecl); p; p = DECL_CHAIN (p))
    {
      unsigned HOST_WIDE_INT p_size = 0;
      if (TREE_TYPE (p) && TREE_CODE (TREE_TYPE (p)) == VECTOR_TYPE)
	{
	  p_size = tree_low_cst (TYPE_SIZE (TREE_TYPE (p)), 1);
	  if (p_size > biggest_vec_reg_size)
	    {
	      for (ii = 0; ii < (size_t) ((int) (p_size/biggest_vec_reg_size));
		   ii++)
		{
		  unsigned HOST_WIDE_INT var_size = 0;
		  param_array[ii + ii_narg] = VECTOR_SEGMENTED;
		  var_size = tree_low_cst (TYPE_SIZE_UNIT (TREE_TYPE (p)),
					   1);
		  parm_type_array[ii + ii_narg] =
		    build_vector_type (TREE_TYPE (TREE_TYPE (p)),
				       (biggest_vec_reg_size / var_size));
		  param_index[ii + ii_narg] = param_no;
		}
	      ii_narg += (int) (p_size / biggest_vec_reg_size);
	    }
	  else
	    {
	      param_array[ii_narg] = VECTOR_NONSEGMENTED;
	      parm_type_array[ii_narg] = TREE_TYPE (p);
	      param_index[ii_narg] = param_no;
	      ii_narg++;
	    }
	}
      else
	{
	  param_array[ii_narg] = SCALAR;
	  parm_type_array[ii_narg] = TREE_TYPE (p);
	  param_index[ii_narg] = param_no;
	  ii_narg++;
	}
      param_no++;
    }

  new_parm_var = XNEWVEC (tree, param_no);
  gcc_assert (new_parm_var != NULL);

  ii = 0;
  for (p = DECL_ARGUMENTS (fndecl); p; p = DECL_CHAIN (p), ii++)
    new_parm_var[ii] = create_tmp_var (TREE_TYPE (p),
				       IDENTIFIER_POINTER (DECL_NAME (p)));
    
      
  for (ii = 0; ii < (size_t) nargs_reqd; ii++)
    {
      param_var_array[ii] = build_decl (EXPR_LOCATION (fndecl), PARM_DECL,
					NULL_TREE, parm_type_array[ii]);
      DECL_ARG_TYPE (param_var_array[ii]) = parm_type_array[ii];
      DECL_CONTEXT (param_var_array[ii]) = fndecl;
      DECL_ARTIFICIAL (param_var_array[ii]) = 1;
      lang_hooks.dup_lang_specific_decl (param_var_array[ii]);
    }

  for (ii = 1; ii < nargs_reqd; ii++)
    TREE_CHAIN (param_var_array[ii-1]) = param_var_array[ii];

  ii = 0;
  for (p = DECL_ARGUMENTS (fndecl); p; p = DECL_CHAIN (p))
    {
      args_data d;
      d.arg = p;
      d.replacement = new_parm_var[ii];
      walk_tree (&DECL_SAVED_TREE (fndecl), replace_parm_decl, (void *) &d,
		 NULL);
      ii++;
    }

  DECL_ARGUMENTS (fndecl) = param_var_array[0];

  /* Now we have to do group the split up register value into one big variable.
     For Example, let's say we split vector(8) int x into:
        vector(4) int D124 and vector(4) int D123
	We have to regroup them into the following:
	
	x (8, 7, 6, 5) = D124
	x (4, 3, 2, 1) = D123

	So, the final thing will look something like this:

	data_type foo (vec(8) int x)
	       |
	       |
	       V
	data_type foo (vec (4) int D124, vec (4) int D123)
	{
	   x = {D124, D123}
           < REST OF FUNCTION BODY >
	}
  */
  
  ii = 0;
  while (ii < nargs_reqd)
    {
      if (param_array[ii] == SCALAR || param_array[ii] == VECTOR_NONSEGMENTED)
	{
	  new_expr = build2 (MODIFY_EXPR, parm_type_array[ii],
			     new_parm_var[jj], param_var_array[ii]);
	  insert_as_first_stmt (new_expr, fndecl);
	  ii++;
	}
      else
	for (kk = 0; kk < (size_t) param_length[jj]; kk++)
	  {
	    for (ll = 0; ll < (size_t) param_length[jj]; ll++)
	      {
		tree m_type = TREE_TYPE (TREE_TYPE (new_parm_var[jj]));
		tree lhs_array =
		  elem_fn_build_array
		  (new_parm_var[jj], build_int_cst
		   (size_type_node, kk * param_length[jj] + ll));
		tree rhs_array =
		  elem_fn_build_array (param_var_array[ii],
				       build_int_cst (size_type_node, ll));
		new_expr = build2 (MODIFY_EXPR, m_type, lhs_array, rhs_array);
		insert_as_first_stmt (new_expr, fndecl);
	      }
	    ii++;
	  }
      jj++;
    }
  pop_cfun ();
  return;
}

/* Does all the call-graph hacks necessary to make FNDECL a recognized
   function.  */

static void
call_graph_add_fn (tree fndecl)
{
  const tree outer = current_function_decl;
  struct function *f = DECL_STRUCT_FUNCTION (fndecl);

  if (cfun)
    f->curr_properties = cfun->curr_properties;
  push_cfun (f);
  current_function_decl = fndecl;
  
  cgraph_add_new_function (fndecl, false);
  cgraph_finalize_function (fndecl, true);

  pop_cfun ();
  current_function_decl = outer;

  return;
}

/* Clones the function FNDECL to elemental functions (masked and unmasked
   versions, if applicable) since vector attribute is set.  */

void
elem_fn_create_fn (tree fndecl)
{
  tree new_masked_fn = NULL_TREE, new_unmasked_fn = NULL_TREE;
  tree induction_var = NULL_TREE;
  elem_fn_info *elem_fn_values = NULL;
  char *masked_suffix = NULL, *unmasked_suffix = NULL;
  tree proc_attr = NULL_TREE, opp_proc_attr = NULL_TREE, opt_attr = NULL_TREE;

  if (!fndecl)
    return;

  elem_fn_values = extract_elem_fn_values (fndecl);
  if (!elem_fn_values)
    return;

  if (elem_fn_values->mask == USE_MASK)
    masked_suffix = find_suffix (elem_fn_values, true);
  else if (elem_fn_values->mask == USE_NOMASK)
    unmasked_suffix = find_suffix (elem_fn_values, false);
  else
    {
      masked_suffix   = find_suffix (elem_fn_values, true);
      unmasked_suffix = find_suffix (elem_fn_values, false);
    }
  if (masked_suffix)
    {
      new_masked_fn = copy_node (fndecl);
      new_masked_fn = rename_elem_fn (new_masked_fn, masked_suffix);
      SET_DECL_RTL (new_masked_fn, NULL);
      TREE_SYMBOL_REFERENCED (DECL_NAME (new_masked_fn)) = 1;
      tree_elem_fn_versioning (fndecl, new_masked_fn, NULL, false, NULL, false,
			       NULL, NULL, elem_fn_values->vectorlength, true);
      scalarize_uniform_linear_params (new_masked_fn, elem_fn_values);
      proc_attr = create_processor_attribute (elem_fn_values, &opp_proc_attr);
      if (proc_attr)
	decl_attributes (&new_masked_fn, proc_attr, 0);
      if (opp_proc_attr)
	decl_attributes (&fndecl, opp_proc_attr, 0);
      
      opt_attr = create_optimize_attribute (3); /* Turn vectorizer on.  */
      if (opt_attr)
	decl_attributes (&new_masked_fn, opt_attr, 0);

      DECL_ATTRIBUTES (new_masked_fn) =
	remove_attribute ("vector", DECL_ATTRIBUTES (new_masked_fn));
	
      add_elem_fn_mask (new_masked_fn);
      induction_var = add_elem_fn_loop (new_masked_fn,
					elem_fn_values->vectorlength);
      fix_elem_fn_return_value (new_masked_fn, elem_fn_values, induction_var);
      segment_params_for_reg_size (new_masked_fn);
      call_graph_add_fn (new_masked_fn);
      SET_DECL_ASSEMBLER_NAME (new_masked_fn, DECL_NAME (new_masked_fn));
      DECL_ELEM_FN_ALREADY_CLONED (new_masked_fn) = true;
      if (DECL_STRUCT_FUNCTION (new_masked_fn))
	DECL_STRUCT_FUNCTION (new_masked_fn)->elem_fn_already_cloned = true;
    }
  if (unmasked_suffix)
    {
      new_unmasked_fn = copy_node (fndecl);
      new_unmasked_fn = rename_elem_fn (new_unmasked_fn, unmasked_suffix);
      SET_DECL_RTL (new_unmasked_fn, NULL);
      TREE_SYMBOL_REFERENCED (DECL_NAME (new_unmasked_fn)) = 1;
      tree_elem_fn_versioning (fndecl, new_unmasked_fn, NULL, false, NULL,
			       false, NULL, NULL,
			       elem_fn_values->vectorlength, false);
      scalarize_uniform_linear_params (new_unmasked_fn, elem_fn_values);
      proc_attr = create_processor_attribute (elem_fn_values, &opp_proc_attr);
      if (proc_attr)
	decl_attributes (&new_unmasked_fn, proc_attr, 0);
      if (opp_proc_attr)
	decl_attributes (&fndecl, opp_proc_attr, 0);
      
      opt_attr = create_optimize_attribute (3); /* Turn vectorizer on.  */
      if (opt_attr)
	decl_attributes (&new_unmasked_fn, opt_attr, 0);

      DECL_ATTRIBUTES (new_unmasked_fn) =
	remove_attribute ("vector", DECL_ATTRIBUTES (new_unmasked_fn));
      induction_var = add_elem_fn_loop (new_unmasked_fn,
					elem_fn_values->vectorlength);
      fix_elem_fn_return_value (new_unmasked_fn, elem_fn_values,
				induction_var);
      segment_params_for_reg_size (new_unmasked_fn);
      call_graph_add_fn (new_unmasked_fn);
      SET_DECL_ASSEMBLER_NAME (new_unmasked_fn, DECL_NAME (new_unmasked_fn));
      DECL_ELEM_FN_ALREADY_CLONED (new_unmasked_fn) = true;
      if (DECL_STRUCT_FUNCTION (new_unmasked_fn))
	DECL_STRUCT_FUNCTION (new_unmasked_fn)->elem_fn_already_cloned = true;
    }

  XDELETEVEC (elem_fn_values);
  return;
}
