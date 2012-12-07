/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains the builtin functions for Array
   notations.
   Copyright (C) 2012  Free Software Foundation, Inc.
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
#include "langhooks.h" 
#include "tree-iterator.h"
#include "diagnostic-core.h"

int extract_sec_implicit_index_arg (location_t, tree);
bool is_sec_implicit_index_fn (tree);
void array_notation_init_builtins (void);

/* Mark the FNDECL as cold, meaning that the function specified by FNDECL is
   not run as is.  */

static void
mark_cold (tree fndecl)
{
  DECL_ATTRIBUTES (fndecl) = tree_cons (get_identifier ("cold"), NULL_TREE,
					DECL_ATTRIBUTES (fndecl));
}

/* This function inititializes array notation specific builtin information.  */


void
array_notation_init_builtins (void)
{
  tree func_type = NULL_TREE;
  tree new_func = NULL_TREE;
  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_add", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_mul", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_all_zero", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_any_zero", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_max", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);
  
  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_min", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_min_ind", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_max_ind", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
				       NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_any_nonzero", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_all_nonzero", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);
  
  func_type = build_function_type_list (integer_type_node, integer_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_implicit_index", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					ptr_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce", func_type);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (ptr_type_node, ptr_type_node,
					ptr_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_mutating", func_type);
  new_func = lang_hooks.decls.pushdecl (new_func);
  return;
}

/* Returns true if the function call specified in FUNC_NAME is
   __sec_implicit_index.  */

bool
is_sec_implicit_index_fn (tree func_name)
{
  const char *function_name = NULL;

  if (!func_name)
    return false;

  if (TREE_CODE (func_name) == FUNCTION_DECL)
    func_name = DECL_NAME (func_name);
  
  if (TREE_CODE (func_name) == IDENTIFIER_NODE)
    function_name = IDENTIFIER_POINTER (func_name);
  else if (TREE_CODE (func_name) == ADDR_EXPR)
    {
      func_name = TREE_OPERAND (func_name, 0);
      if (TREE_CODE (func_name) == FUNCTION_DECL)
	if (DECL_NAME (func_name))
	  function_name = IDENTIFIER_POINTER (DECL_NAME (func_name));
    }

  if (!function_name)
    return false;
  else if (!strcmp (function_name, "__sec_implicit_index"))
    return true;
  else
    return false;
}

/* Returns the first and only argument for FN, which should be a
   sec_implicit_index function.  FN's location in the source file is is 
   indicated by LOCATION.  */

int
extract_sec_implicit_index_arg (location_t location, tree fn)
{
  tree fn_arg;
  HOST_WIDE_INT return_int = 0;
  if (!fn)
    return -1;

  if (TREE_CODE (fn) == CALL_EXPR)
    {
      fn_arg = CALL_EXPR_ARG (fn, 0);
      if (really_constant_p (fn_arg))
	return_int = (int) int_cst_value (fn_arg);
      else
	{
	  if (location == UNKNOWN_LOCATION && EXPR_HAS_LOCATION (fn))
	    location = EXPR_LOCATION (fn);
	  error_at (location, "__sec_implicit_index parameter must be a " 
		    "constant integer expression");
	  return -1;
	}
    }
  return return_int;
}
