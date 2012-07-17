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
#include "tm.h"
#include "tree.h"
#include "langhooks.h"
#include "cilk.h"
#include "tm_p.h"
#include "hard-reg-set.h"
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
#include "toplev.h"
#include "options.h"
#include "intl.h"
#include "vec.h"


static tree create_optimize_attribute (int);
static tree create_processor_attribute (elem_fn_info *, tree *);
static tree elem_fn_build_array (tree base_var, tree index);


/* This function will create the appropriate __target__ attribute for the
 * processor */
static tree
create_processor_attribute (elem_fn_info *elem_fn_values, tree *opposite_attr)
{
  /* you need the opposite attribute for the scalar code part */
  tree proc_attr, opp_proc_attr;
  VEC(tree,gc) *proc_vec_list = VEC_alloc (tree, gc, 4);
  VEC(tree,gc) *opp_proc_vec_list = VEC_alloc (tree, gc, 4);
  
  if (!elem_fn_values || !elem_fn_values->proc_type)
    return NULL_TREE;

  if (!strcmp (elem_fn_values->proc_type, "pentium_4"))
    {
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("arch=pentium4"), "arch=pentium4"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("mmx"), "mmx"));
      if (opposite_attr)
	{
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("no-mmx"), "no-mmx"));
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("arch=pentium4"),
				       "arch=pentium4"));
	}
    }
  else if (!strcmp (elem_fn_values->proc_type, "pentium_4_sse3"))
    {
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("arch=pentium4"), "arch=pentium4"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("sse3"), "sse3"));
      if (opposite_attr)
	{
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("arch=pentium4"),
				       "arch=pentium4"));
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("no-sse3"), "no-sse3"));
	}
    }
  else if (!strcmp (elem_fn_values->proc_type, "core2_duo_sse3"))
    {
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("arch=core2"), "arch=core2"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("sse3"), "sse3"));
      if (opposite_attr)
	{
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("arch=core2"), "arch=core2"));
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("no-sse3"), "no-sse3"));
	}
    }
  else if (!strcmp (elem_fn_values->proc_type, "core_2_duo_sse_4_1"))
    {
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("arch=core2"), "arch=core2"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("sse4.1"), "sse4.1"));
      if (opposite_attr)
	{
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("arch=core2"), "arch=core2"));
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("no-sse4.1"), "no-sse4.1"));
	}
    }
  else if (!strcmp (elem_fn_values->proc_type, "core_i7_sse4_2"))
    {
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("arch=corei7"), "arch=corei7"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("sse4.2"), "sse4.2"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("avx"), "avx"));
      if (opposite_attr)
	{
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("arch=corei7"), "arch=corei7"));
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("no-sse4.2"), "no-sse4.2"));
	}
    }
  else
    sorry ("Processor type not supported.");

  proc_attr = build_tree_list_vec (proc_vec_list);
  VEC_truncate (tree, proc_vec_list, 0);
  proc_attr = build_tree_list (get_identifier ("__target__"), proc_attr);

  if (opposite_attr)
    {
      opp_proc_attr = build_tree_list_vec (opp_proc_vec_list);
      VEC_truncate (tree, opp_proc_vec_list, 0);
      opp_proc_attr = build_tree_list (get_identifier ("__target__"),
				       opp_proc_attr);
      *opposite_attr = opp_proc_attr;
    }
  return proc_attr;
}

/* this will create an optimize attribute for the vector function, to make sure
 * the vectorizer is turned on and has its full capabilities */
static tree
create_optimize_attribute (int option)
{
  tree opt_attr;
  VEC(tree,gc) *opt_vec = VEC_alloc (tree,gc, 4);
  char optimization[2];
  optimization[0] = 'O';
  
  if (option == 3)
    optimization[1] = '3';
  else if (option == 2)
    optimization[1] = '2';
  else if (option == 1)
    optimization[1] = '1';
  else if (option == 0)
    optimization[1] = '0';
  
  VEC_safe_push (tree, gc, opt_vec, build_string (2, optimization));
  opt_attr = build_tree_list_vec (opt_vec);
  VEC_truncate (tree, opt_vec, 0);
  opt_attr = build_tree_list (get_identifier ("optimize"), opt_attr);
  return opt_attr;
}


/* this function will store return expression to a temporary var */
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


/* This function will create a vector access as a array access */
static tree
elem_fn_build_array (tree base_var, tree index)
{
  return build_array_ref (UNKNOWN_LOCATION, base_var, index);
}

/* this function wil replace all vector references with array references. */
static tree
replace_array_ref_for_vec (tree *tp, int *walk_subtrees, void *data)
{
  tree ii_var;
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
	      *tp =  elem_fn_build_array (*tp, func_data->induction_var);
	      *walk_subtrees = 0;
	      return NULL_TREE;
	    }
	}
      if (func_data->return_var &&
	  (DECL_NAME (*tp) == DECL_NAME (func_data->return_var)))
	{
	  *tp = elem_fn_build_array (*tp, func_data->induction_var);
	  *walk_subtrees = 0;
	}
    }
  return NULL_TREE;
}

/* this function will move return values to the end of the function */
static void
fix_elem_fn_return_value (tree fndecl, tree induction_var)
{
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
	     (void *)new_var, NULL);
  data.return_var = new_var;
  data.arguments = DECL_ARGUMENTS (fndecl);
  data.induction_var = induction_var;

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

/* this function will break a vector value to scalar with a for loop in front */
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
  
  loop_var = create_tmp_var (integer_type_node, "ii_elem_fn_vec_val");
  mod_var = build2 (MODIFY_EXPR, void_type_node, loop_var,
		    build_int_cst (integer_type_node, 0));
  append_to_statement_list_force (mod_var, &loop);
  
  if_label = build_decl (UNKNOWN_LOCATION, LABEL_DECL,
			 get_identifier ("if_lab"), void_type_node);
  DECL_CONTEXT (if_label) = fndecl;
  DECL_ARTIFICIAL (if_label) = 0;
  DECL_IGNORED_P (if_label) = 1;

  exit_label = build_decl (UNKNOWN_LOCATION, LABEL_DECL,
			   get_identifier ("exit_label"), void_type_node);
  DECL_CONTEXT (exit_label) = fndecl;
  DECL_ARTIFICIAL (exit_label) = 0;
  DECL_IGNORED_P (exit_label) = 1;

  body_label = build_decl (UNKNOWN_LOCATION, LABEL_DECL,
			   get_identifier ("body_label"), void_type_node);
  DECL_CONTEXT (body_label) = fndecl;
  DECL_ARTIFICIAL (body_label) = 0;
  DECL_IGNORED_P (body_label) = 1;
  append_to_statement_list_force (build1 (LABEL_EXPR, void_type_node,
					  if_label), &loop);
  cmp_expr = build2 (LT_EXPR, boolean_type_node, loop_var,
		     build_int_cst (integer_type_node, vlength));
  cond_expr = build3 (COND_EXPR, void_type_node, cmp_expr,
		      build1 (GOTO_EXPR, void_type_node, body_label),
		      build1 (GOTO_EXPR, void_type_node, exit_label));

  append_to_statement_list_force (cond_expr, &loop);
  append_to_statement_list_force (build1 (LABEL_EXPR, void_type_node,
					  body_label), &loop);
  append_to_statement_list_force (fn_body, &loop);

  incr_expr = build2 (MODIFY_EXPR, void_type_node, loop_var,
		      build2 (PLUS_EXPR, TREE_TYPE (loop_var), loop_var,
			      build_int_cst (integer_type_node, 1)));

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

/* this function will add the mask if statement for masked clone */
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
    fn_body = DECL_SAVED_TREE (fndecl); /* not sure if we ever get here */

  gcc_assert (DECL_NAME (ii_arg) == get_identifier ("__elem_fn_mask"));

  cmp_expr = fold_build2 (NE_EXPR, TREE_TYPE (ii_arg), ii_arg,
			  build_int_cst (TREE_TYPE (TREE_TYPE (ii_arg)), 0));
  cond_expr = fold_build3 (COND_EXPR, void_type_node, cmp_expr, fn_body,
			   build_empty_stmt (UNKNOWN_LOCATION));

  if (TREE_CODE (DECL_SAVED_TREE (fndecl)) == BIND_EXPR)
    BIND_EXPR_BODY (DECL_SAVED_TREE (fndecl)) = cond_expr;
  else
    DECL_SAVED_TREE (fndecl) = cond_expr;

  pop_cfun ();
  current_function_decl = old_fndecl;
  
  return;
 
}

/* this function will do hacks necessary to recognize the cloned function */
static void
cg_hacks (tree fndecl)
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

/* this function will create clones for function marked with vector attribute */
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
			       NULL, NULL, elem_fn_values->vectorlength[0],
			       true);
      proc_attr = create_processor_attribute (elem_fn_values, &opp_proc_attr);
      if (proc_attr)
	decl_attributes (&new_masked_fn, proc_attr, 0);
      if (opp_proc_attr)
	decl_attributes (&fndecl, opp_proc_attr, 0);
      
      opt_attr = create_optimize_attribute (3); /* will turn vectorizer on */
      if (opt_attr)
	decl_attributes (&new_masked_fn, opt_attr, 0);

      DECL_ATTRIBUTES (new_masked_fn) =
	remove_attribute ("vector", DECL_ATTRIBUTES (new_masked_fn));
	
      add_elem_fn_mask (new_masked_fn);
      induction_var = add_elem_fn_loop (new_masked_fn,
					elem_fn_values->vectorlength[0]);
      fix_elem_fn_return_value (new_masked_fn, induction_var);
      cg_hacks (new_masked_fn);
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
			       elem_fn_values->vectorlength[0], false);
      proc_attr = create_processor_attribute (elem_fn_values, &opp_proc_attr);
      if (proc_attr)
	decl_attributes (&new_unmasked_fn, proc_attr, 0);
      if (opp_proc_attr)
	decl_attributes (&fndecl, opp_proc_attr, 0);
      
      opt_attr = create_optimize_attribute (3); /* will turn vectorizer on */
      if (opt_attr)
	decl_attributes (&new_unmasked_fn, opt_attr, 0);

      DECL_ATTRIBUTES (new_unmasked_fn) =
	remove_attribute ("vector", DECL_ATTRIBUTES (new_unmasked_fn));
      induction_var = add_elem_fn_loop (new_unmasked_fn,
					elem_fn_values->vectorlength[0]);
      fix_elem_fn_return_value (new_unmasked_fn, induction_var);
      cg_hacks (new_unmasked_fn);
      SET_DECL_ASSEMBLER_NAME (new_unmasked_fn, DECL_NAME (new_unmasked_fn));
      DECL_ELEM_FN_ALREADY_CLONED (new_unmasked_fn) = true;
      if (DECL_STRUCT_FUNCTION (new_unmasked_fn))
	DECL_STRUCT_FUNCTION (new_unmasked_fn)->elem_fn_already_cloned = true;
    }
  free (elem_fn_values);
  return;
}
