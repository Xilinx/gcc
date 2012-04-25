/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains the functions for Elemental functions.
   
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
#include "rtl.h"
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
#include "cfgloop.h"
#include "flags.h"
#include "tree-inline.h"
#include "cgraph.h"
#include "ipa-prop.h"
#include "opts.h"
#include "tree-iterator.h"
#include "toplev.h"
#include "options.h"
#include "intl.h"
#include "vec.h"

#define MAX_VARS 50

enum mask_options {
  USE_MASK = 12345,
  USE_NOMASK,
  USE_BOTH
};

typedef struct
{
  char *proc_type;
  enum mask_options mask;
  int vectorlength[MAX_VARS];
  int no_vlengths;
  char *uniform_vars[MAX_VARS];
  int no_uvars;
  int uniform_location[MAX_VARS]; /* their location in parm list */
  char *linear_vars[MAX_VARS];
  int linear_steps[MAX_VARS];
  int linear_location[MAX_VARS]; /* their location in parm list */
  int no_lvars;
  int private_location[MAX_VARS]; /* parm not in uniform or linear list */
  int no_pvars;
  char *func_prefix;
  int total_no_args;
} elem_fn_info;

static elem_fn_info *extract_elem_fn_values (tree);
static tree create_optimize_attribute (int);
static tree create_processor_attribute (elem_fn_info *, tree *);

/* this is an helper function for find_elem_fn_param_type */
static enum elem_fn_parm_type
find_elem_fn_parm_type_1 (tree fndecl, int parm_no)
{
  int ii = 0;
  elem_fn_info *elem_fn_values;

  elem_fn_values = extract_elem_fn_values (fndecl);
  if (!elem_fn_values)
    return TYPE_NONE;

  for (ii = 0; ii < elem_fn_values->no_lvars; ii++)
    if (elem_fn_values->linear_location[ii] == parm_no)
      return TYPE_LINEAR;
    
  for (ii = 0; ii < elem_fn_values->no_uvars; ii++)
    if (elem_fn_values->uniform_location[ii] == parm_no)
      return TYPE_UNIFORM;
    
  return TYPE_NONE;
}
  
  
/* this function will return the type of a parameter in elemental function.
   The choices are UNIFORM or LINEAR. */
enum elem_fn_parm_type
find_elem_fn_parm_type (gimple stmt, tree op)
{
  tree fndecl, parm = NULL_TREE;
  int ii, nargs;
  enum elem_fn_parm_type return_type = TYPE_NONE;
  
  if (gimple_code (stmt) != GIMPLE_CALL)
    return TYPE_NONE;

  fndecl = gimple_call_fndecl (stmt);
  gcc_assert (fndecl);

  nargs = gimple_call_num_args (stmt);

  for (ii = 0; ii < nargs; ii++)
    {
      parm = gimple_call_arg (stmt, ii);
      if (op == parm)
	{
	  return_type = find_elem_fn_parm_type_1 (fndecl, 1);
	  return return_type;
	}
    }
  return return_type;
}
  
/* this function will concatinate the suffix to the existing function decl */
static tree
rename_elem_fn (tree decl, const char *suffix)
{
  int length = 0;
  const char *fn_name = IDENTIFIER_POINTER (DECL_NAME (decl));
  char *new_fn_name;
  tree new_decl = NULL_TREE;
  
  if (!suffix || !fn_name)
    return decl;
  else
    new_decl = decl;

  length = strlen (fn_name) + strlen (suffix) + 1;
  new_fn_name = (char *)xmalloc (length);
  strcpy (new_fn_name, fn_name);
  strcat (new_fn_name, suffix);

  DECL_NAME (new_decl) = get_identifier (new_fn_name);
  return new_decl;
}

/* this function will check to see if the node is part of an function that
 * needs to be converted to its vector equivalent. */
bool
is_elem_fn (tree fndecl)
{
  tree ii_tree;

  for (ii_tree = DECL_ATTRIBUTES (fndecl); ii_tree;
       ii_tree = TREE_CHAIN (ii_tree))
    {
      tree ii_value = TREE_PURPOSE (ii_tree);
      if (TREE_CODE (ii_value) == IDENTIFIER_NODE
	  && !strcmp (IDENTIFIER_POINTER (ii_value), "vector"))
	return true;
    }

  /* If we are here, then we didn't find a vector keyword, so it is false */
  return false;
}

/* This function will find the appropriate processor code in the function
 * mangling vector function
 */
static char *
find_processor_code (elem_fn_info *elem_fn_values)
{
  if (!elem_fn_values || !elem_fn_values->proc_type)
    return xstrdup ("B");

  if (!strcmp (elem_fn_values->proc_type, "pentium_4"))
    return xstrdup ("B");
  else if (!strcmp (elem_fn_values->proc_type, "pentium4_sse3"))
    return xstrdup ("D");
  else if (!strcmp (elem_fn_values->proc_type, "core2_duo_ssse3"))
    return xstrdup ("E");
  else if (!strcmp (elem_fn_values->proc_type, "core2_duo_sse_4_1"))
    return xstrdup ("F");
  else if (!strcmp (elem_fn_values->proc_type, "core_i7_sse4_2"))
    return xstrdup ("H");
  else
    gcc_unreachable ();

  return NULL; /* should never get here */
}

/* this function will return vectorlength, if specified, in string format -OR-
 * it will give the default vector length for the specified architecture. */
static char *
find_vlength_code (elem_fn_info *elem_fn_values)
{
  char *vlength_code = (char *) xmalloc (sizeof (char) * 10);
  if (!elem_fn_values)
    { 
      sprintf (vlength_code, "4");
      return vlength_code;
    }

  memset (vlength_code, 10, 0);
  
  if (elem_fn_values->no_vlengths != 0)
    sprintf(vlength_code,"%d", elem_fn_values->vectorlength[0]);
  else
    {
      if (!strcmp (elem_fn_values->proc_type, "pentium_4"))
	sprintf (vlength_code,"4");
      else if (!strcmp (elem_fn_values->proc_type, "pentium4_sse3"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "core2_duo_ssse3"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "core2_duo_sse_4_1"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "core_i7_sse4_2"))
	sprintf (vlength_code, "4");
      else
	gcc_unreachable ();
    }
  return vlength_code;
}

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
  else if (!strcmp (elem_fn_values->proc_type, "pentium4_sse3"))
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
  else if (!strcmp (elem_fn_values->proc_type, "core2_duo_ssse3"))
    {
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("arch=core2"), "arch=core2"));
      VEC_safe_push (tree, gc, proc_vec_list,
		     build_string (strlen ("ssse3"), "ssse3"));
      if (opposite_attr)
	{
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("arch=core2"), "arch=core2"));
	  VEC_safe_push (tree, gc, opp_proc_vec_list,
			 build_string (strlen ("no-ssse3"), "no-ssse3"));
	}
    }
  else if (!strcmp (elem_fn_values->proc_type, "core2_duo_sse_4_1"))
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
  sprintf(&optimization[1], "%1d", option);
  VEC_safe_push (tree, gc, opt_vec, build_string (2, optimization));
  opt_attr = build_tree_list_vec (opt_vec);
  VEC_truncate (tree, opt_vec, 0);
  opt_attr = build_tree_list (get_identifier ("optimize"), opt_attr);
  return opt_attr;
}

/* this function will find the appropriate mangling suffix for the vector
 * function */
static char *
find_suffix (elem_fn_info *elem_fn_values, bool masked)
{
  char *suffix = (char*)xmalloc (100);
  char tmp_str[10];
  int arg_number, ii_pvar, ii_uvar, ii_lvar;
  strcpy (suffix, "._simdsimd_");
  strcat (suffix, find_processor_code (elem_fn_values));
  strcat (suffix, find_vlength_code (elem_fn_values));
  if (masked)
    strcpy (suffix, "m");
  else
    strcat (suffix, "n");

  for (arg_number = 1; arg_number <= elem_fn_values->total_no_args;
       arg_number++)
    {
      for (ii_lvar = 0; ii_lvar < elem_fn_values->no_lvars; ii_lvar++)
	{
	  if (elem_fn_values->linear_location[ii_lvar] == arg_number)
	    {
	      strcat (suffix, "_l");
	      sprintf(tmp_str, "%d", elem_fn_values->linear_steps[ii_lvar]);
	      strcat (suffix, tmp_str);
	    }
	}
      for (ii_uvar = 0; ii_uvar < elem_fn_values->no_uvars; ii_uvar++)
	{
	  if (elem_fn_values->uniform_location[ii_uvar] == arg_number)
	    strcat (suffix, "_s1");
	}
      for (ii_pvar = 0; ii_pvar < elem_fn_values->no_pvars; ii_pvar++)
	{
	  if (elem_fn_values->private_location[ii_pvar] == arg_number)
	    strcat (suffix, "_v1");
	}
    } 
  return suffix;
}

tree
find_elem_fn_name (tree old_fndecl,
		   tree vectype_out ATTRIBUTE_UNUSED,
		   tree vectype_in ATTRIBUTE_UNUSED)
{
  elem_fn_info *elem_fn_values = NULL;
  tree new_fndecl = NULL_TREE, arg_type = NULL_TREE;
  char *suffix = NULL;
  
  elem_fn_values = extract_elem_fn_values (old_fndecl);
 
  if (elem_fn_values)
    {
      if (elem_fn_values->no_vlengths > 0)
	{
	  if (elem_fn_values->vectorlength[0] ==
	      (int)TYPE_VECTOR_SUBPARTS (vectype_out))
	    suffix = find_suffix (elem_fn_values, false);
	  else
	    return NULL_TREE;
	}
      else
	return NULL_TREE;
    }
  else
    return NULL_TREE;

  new_fndecl = copy_node (rename_elem_fn (old_fndecl, suffix));
  TREE_TYPE (new_fndecl) = copy_node (TREE_TYPE (old_fndecl));

  TYPE_ARG_TYPES (TREE_TYPE (new_fndecl)) =
    copy_list (TYPE_ARG_TYPES (TREE_TYPE (new_fndecl)));
  
  for (arg_type = TYPE_ARG_TYPES (TREE_TYPE (new_fndecl));
       arg_type && arg_type != void_type_node;
       arg_type = TREE_CHAIN (arg_type))
    TREE_VALUE (arg_type) = vectype_out;
  
  if (TREE_TYPE (TREE_TYPE (new_fndecl)) != void_type_node)
    {
      TREE_TYPE (TREE_TYPE (new_fndecl)) =
	copy_node (TREE_TYPE (TREE_TYPE (new_fndecl)));
      TREE_TYPE (TREE_TYPE (new_fndecl)) = vectype_out;
      DECL_MODE (new_fndecl) = TYPE_MODE (vectype_out);
    }
  
  return new_fndecl;
}

/* this function wil create the elemental vector function node */
static struct cgraph_node *
create_elem_fn_nodes (struct cgraph_node *node)
{
  tree new_decl, old_decl, new_decl_name, opt_attr;
  tree proc_attr, opp_proc_attr = NULL_TREE;
  struct cgraph_node *new_node;
  elem_fn_info *elem_fn_values = NULL;
  char *suffix = NULL;
  
  old_decl = node->symbol.decl;
  new_decl = copy_node (old_decl);
  TREE_TYPE (new_decl) = copy_node (TREE_TYPE (old_decl));
  elem_fn_values = extract_elem_fn_values (old_decl);

  if (elem_fn_values)
    {
      suffix = find_suffix (elem_fn_values, false);
    }
  else
    return NULL;
  
  new_decl_name = rename_elem_fn (new_decl, suffix);

  SET_DECL_ASSEMBLER_NAME (new_decl, DECL_NAME(new_decl_name));
  SET_DECL_RTL (new_decl, NULL);
  TREE_SYMBOL_REFERENCED (DECL_NAME (new_decl_name)) = 1;
  
  new_node = cgraph_copy_node_for_versioning (node, new_decl, NULL, NULL);
  new_node->symbol.externally_visible = node->symbol.externally_visible;
  new_node->lowered = true;

  tree_elem_fn_versioning (old_decl, new_decl, NULL, false, NULL, false, NULL,
			   NULL, elem_fn_values->vectorlength[0]);
  cgraph_call_function_insertion_hooks (new_node);
  DECL_STRUCT_FUNCTION (new_decl)->elem_fn_already_cloned = true;
  DECL_STRUCT_FUNCTION (new_decl)->curr_properties = cfun->curr_properties;
  DECL_ATTRIBUTES (cfun->decl) =
    remove_attribute ("vector", DECL_ATTRIBUTES (cfun->decl));
  DECL_ATTRIBUTES (new_node->symbol.decl) =
    remove_attribute ("vector", DECL_ATTRIBUTES (new_node->symbol.decl));

  proc_attr = create_processor_attribute (elem_fn_values, &opp_proc_attr);
  
  if (proc_attr)
    decl_attributes (&new_node->symbol.decl, proc_attr, 0);
  if (opp_proc_attr)
    decl_attributes (&cfun->decl, opp_proc_attr, 0);

  opt_attr = create_optimize_attribute (3); /* this will turn vectorizer on */
  if (opt_attr)
    decl_attributes (&new_node->symbol.decl, opt_attr, 0);
  
  return new_node;
}

/* This function will extact the vector attribute and store the data in the
 * elem_fn_info structure.
 */
static elem_fn_info *
extract_elem_fn_values (tree decl)
{
  elem_fn_info *elem_fn_values = NULL;
  int x = 0; /* this is a dummy variable */
  int arg_number = 0, ii = 0;
  tree ii_tree, jj_tree, kk_tree;
  tree decl_attr = DECL_ATTRIBUTES (decl);
  
  if (!decl_attr)
    return NULL;

  elem_fn_values = (elem_fn_info *)xmalloc (sizeof (elem_fn_info));
  gcc_assert (elem_fn_values);

  elem_fn_values->proc_type = NULL;
  elem_fn_values->mask = USE_BOTH;
  elem_fn_values->no_vlengths = 0;
  elem_fn_values->no_uvars = 0;
  elem_fn_values->no_lvars = 0;
  

  for (ii_tree = decl_attr; ii_tree; ii_tree = TREE_CHAIN (ii_tree))
    {
      tree ii_purpose = TREE_PURPOSE (ii_tree);
      tree ii_value = TREE_VALUE (ii_tree);
      if (TREE_CODE (ii_purpose) == IDENTIFIER_NODE
	  && !strcmp (IDENTIFIER_POINTER (ii_purpose), "vector"))
	{
	  for (jj_tree = ii_value; jj_tree;
	       jj_tree = TREE_CHAIN (jj_tree))
	    {
	      tree jj_value = TREE_VALUE (jj_tree);
	      tree jj_purpose = TREE_PURPOSE (jj_value);
	      if (TREE_CODE (jj_purpose) == IDENTIFIER_NODE
		  && !strcmp (IDENTIFIER_POINTER (jj_purpose), "processor"))
		{
		  for (kk_tree = TREE_VALUE (jj_value); kk_tree;
		       kk_tree = TREE_CHAIN (kk_tree))
		    {
		      tree kk_value = TREE_VALUE (kk_tree);
		      if (TREE_CODE (kk_value) == STRING_CST)
			elem_fn_values->proc_type =
			  xstrdup (TREE_STRING_POINTER (kk_value));
		    }
		}
	      else if (TREE_CODE (jj_purpose) == IDENTIFIER_NODE
		       && !strcmp (IDENTIFIER_POINTER (jj_purpose),
				  "vectorlength"))
		{
		  for (kk_tree = TREE_VALUE (jj_value); kk_tree;
		       kk_tree = TREE_CHAIN (kk_tree))
		    {
		      tree kk_value = TREE_VALUE (kk_tree);
		      if (TREE_CODE (kk_value) == INTEGER_CST)
			{
			  x = elem_fn_values->no_vlengths;
			  elem_fn_values->vectorlength[x] =
			    (int) TREE_INT_CST_LOW (kk_value);
			  elem_fn_values->no_vlengths++;
			}
		    }
		}
	      else if (TREE_CODE (jj_purpose) == IDENTIFIER_NODE
		       && !strcmp (IDENTIFIER_POINTER (jj_purpose), "uniform"))
		{
		  for (kk_tree = TREE_VALUE (jj_value); kk_tree;
		       kk_tree = TREE_CHAIN (kk_tree))
		    {
		      tree kk_value = TREE_VALUE (kk_tree);
		      elem_fn_values->uniform_vars[elem_fn_values->no_uvars] =
			xstrdup (TREE_STRING_POINTER (kk_value));
		      elem_fn_values->no_uvars++;
		    }
		}
	      else if (TREE_CODE (jj_purpose) == IDENTIFIER_NODE
		       && !strcmp (IDENTIFIER_POINTER (jj_purpose), "linear"))
		{
		  for (kk_tree = TREE_VALUE (jj_value); kk_tree;
		       kk_tree = TREE_CHAIN (kk_tree))
		    {
		      tree kk_value = TREE_VALUE (kk_tree);
		      elem_fn_values->linear_vars[elem_fn_values->no_lvars] =
			xstrdup (TREE_STRING_POINTER (kk_value));
		      kk_tree = TREE_CHAIN (kk_tree);
		      kk_value = TREE_VALUE (kk_tree);
		      elem_fn_values->linear_steps[elem_fn_values->no_lvars] =
			TREE_INT_CST_LOW (kk_value);
		      elem_fn_values->no_lvars++;
		    }
		}
	      else if (TREE_CODE (jj_purpose) == IDENTIFIER_NODE
		       && !strcmp (IDENTIFIER_POINTER (jj_purpose), "mask"))
		elem_fn_values->mask = USE_MASK;
	      else if (TREE_CODE (jj_purpose) == IDENTIFIER_NODE
		       && !strcmp (IDENTIFIER_POINTER (jj_purpose), "nomask"))
		elem_fn_values->mask = USE_NOMASK;
	    }
	}
    }

  for (ii_tree = DECL_ARGUMENTS (decl); ii_tree; ii_tree = DECL_CHAIN (ii_tree))
    {
      arg_number++;
      bool already_found = false;
      for (ii = 0; ii < elem_fn_values->no_uvars; ii++)
	{
	  if (DECL_NAME (ii_tree)
	      && !strcmp (IDENTIFIER_POINTER (DECL_NAME (ii_tree)),
			  elem_fn_values->uniform_vars[ii]))
	    {
	      already_found = true;
	      elem_fn_values->uniform_location[ii] = arg_number;
	    }
	}
      for (ii = 0; ii < elem_fn_values->no_lvars; ii++)
	{
	  if (DECL_NAME (ii_tree)
	      && !strcmp (IDENTIFIER_POINTER (DECL_NAME (ii_tree)),
			  elem_fn_values->linear_vars[ii]))
	    {
	      if (already_found)
		  fatal_error
		    ("variable %s defined in both uniform and linear clause",
		     elem_fn_values->linear_vars[ii]);
	      else
		{
		  already_found = true;
		  elem_fn_values->linear_location[ii] = arg_number;
		}
	    }
	}
      if (!already_found) /* this means this variable is a private */
	elem_fn_values->private_location[elem_fn_values->no_pvars++] =
	  arg_number;
    }

  elem_fn_values->total_no_args = arg_number;
  
  return elem_fn_values;
}  

/* Entry point function for creating the vector elemental function */
static unsigned int
create_elem_vec_fn (void)
{
  struct cgraph_node *ii_node, *copied_node;
  
  for (ii_node = cgraph_nodes; ii_node != NULL; ii_node = ii_node->next)
    {
      tree node_decl = ii_node->symbol.decl;
      if (is_elem_fn (node_decl)
	  && DECL_STRUCT_FUNCTION (node_decl) 
	  && !DECL_STRUCT_FUNCTION (node_decl)->elem_fn_already_cloned)
	{
       	  copied_node = create_elem_fn_nodes (ii_node);
	  if (DECL_RTL (ii_node->symbol.decl))
	    {
	      SET_DECL_RTL (copied_node->symbol.decl,
			    copy_rtx (DECL_RTL (ii_node->symbol.decl)));
	      XEXP (DECL_RTL (copied_node->symbol.decl), 0) =
		gen_rtx_SYMBOL_REF
		(GET_MODE (XEXP (DECL_RTL (ii_node->symbol.decl), 0)),
		 IDENTIFIER_POINTER (DECL_NAME (copied_node->symbol.decl)));
	    }
	  
	}
    }
  return 0;
}
 

struct gimple_opt_pass pass_elem_fn =
  {
    {
      GIMPLE_PASS,
      "tree_elem_fn",			/* name */
      0,				/* gate */
      create_elem_vec_fn,		/* execute */
      NULL,				/* sub */
      NULL,				/* next */
      0,				/* static_pass_number */
      TV_NONE,				/* tv_id */
      PROP_gimple_any| PROP_cfg, 	/* properties_required */
      0,				/* properties_provided */
      0,				/* properties_destroyed */
      0,				/* todo_flags_start */
      TODO_verify_flow,			/* todo_flags_finish */
    }
  };
