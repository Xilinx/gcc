/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains the language independent functions for
   Elemental functions.
   
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
#include "cilk.h"

#define MAX_VARS 50

enum elem_fn_parm_type find_elem_fn_parm_type (gimple, tree, tree *);
bool is_elem_fn (tree);
tree find_elem_fn_name (tree old_fndecl, tree vectype_out, tree vectype_in);
elem_fn_info *extract_elem_fn_values (tree decl);

/* This function will find the appropriate processor code in the function
 * mangling vector function
 */
char *
find_processor_code (elem_fn_info *elem_fn_values)
{
  if (!elem_fn_values || !elem_fn_values->proc_type)
    return xstrdup ("B");

  if (!strcmp (elem_fn_values->proc_type, "pentium_4"))
    return xstrdup ("B");
  else if (!strcmp (elem_fn_values->proc_type, "pentium_4_sse3"))
    return xstrdup ("D");
  else if (!strcmp (elem_fn_values->proc_type, "core2_duo_sse3"))
    return xstrdup ("E");
  else if (!strcmp (elem_fn_values->proc_type, "core_2_duo_sse_4_1"))
    return xstrdup ("F");
  else if (!strcmp (elem_fn_values->proc_type, "core_i7_sse4_2"))
    return xstrdup ("H");
  else
    gcc_unreachable ();

  return NULL; /* should never get here */
}

/* this function will return vectorlength, if specified, in string format -OR-
 * it will give the default vector length for the specified architecture. */
char *
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
      if (!elem_fn_values->proc_type)
	sprintf(vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "pentium_4"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "pentium_4_sse3"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "core2_duo_sse3"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "core_2_duo_sse_4_1"))
	sprintf (vlength_code, "4");
      else if (!strcmp (elem_fn_values->proc_type, "core_i7_sse4_2"))
	sprintf (vlength_code, "4");
      else
	gcc_unreachable ();
    }
  return vlength_code;
}


/* this function will concatinate the suffix to the existing function decl */
tree
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


/* this function will find the appropriate mangling suffix for the vector
 * function */
char *
find_suffix (elem_fn_info *elem_fn_values, bool masked)
{
  char *suffix = (char*)xmalloc (100);
  char tmp_str[10];
  int arg_number, ii_pvar, ii_uvar, ii_lvar;
  strcpy (suffix, "._simdsimd_");
  strcat (suffix, find_processor_code (elem_fn_values));
  strcat (suffix, find_vlength_code (elem_fn_values));

  if (masked)
    strcat (suffix, "m");
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


/* this is an helper function for find_elem_fn_param_type */
static enum elem_fn_parm_type
find_elem_fn_parm_type_1 (tree fndecl, int parm_no, tree *step_size)
{
  int ii = 0;
  elem_fn_info *elem_fn_values;

  elem_fn_values = extract_elem_fn_values (fndecl);
  if (!elem_fn_values)
    return TYPE_NONE;

  for (ii = 0; ii < elem_fn_values->no_lvars; ii++)
    if (elem_fn_values->linear_location[ii] == parm_no)
      {
	if (step_size != NULL)
	  *step_size = build_int_cst (integer_type_node,
				      elem_fn_values->linear_steps[ii]);
	return TYPE_LINEAR;
      }
    
  for (ii = 0; ii < elem_fn_values->no_uvars; ii++)
    if (elem_fn_values->uniform_location[ii] == parm_no)
      return TYPE_UNIFORM;
    
  return TYPE_NONE;
}
  
  
/* this function will return the type of a parameter in elemental function.
   The choices are UNIFORM or LINEAR. */
enum elem_fn_parm_type
find_elem_fn_parm_type (gimple stmt, tree op, tree *step_size)
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
	  return_type = find_elem_fn_parm_type_1 (fndecl, ii, step_size);
	  return return_type;
	}
    }
  return return_type;
}
/* this function will return the appropriate cloned named for the function */
tree
find_elem_fn_name (tree old_fndecl, tree vectype_out, 
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

/* this function will extract the elem. function values from a vector and store
 * it in a data structure and return that */
elem_fn_info *
extract_elem_fn_values (tree decl)
{
  elem_fn_info *elem_fn_values = NULL;
  int x = 0; /* this is a dummy variable */
  int arg_number = 0, ii = 0;
  tree ii_tree, jj_tree, kk_tree;
  tree decl_attr = DECL_ATTRIBUTES (decl);
  tree decl_ret_type;
  if (!decl_attr)
    return NULL;

  elem_fn_values = (elem_fn_info *)xmalloc (sizeof (elem_fn_info));
  gcc_assert (elem_fn_values);

  decl_ret_type = TREE_TYPE (decl);
  if (decl_ret_type)
    decl_ret_type = TREE_TYPE (decl_ret_type);
  
  elem_fn_values->proc_type = NULL;
  elem_fn_values->mask = USE_BOTH;
  elem_fn_values->no_vlengths = 0;
  elem_fn_values->no_uvars = 0;
  elem_fn_values->no_lvars = 0;
  elem_fn_values->no_pvars = 0;
  if (decl_ret_type && COMPLETE_TYPE_P (decl_ret_type)
      && !VOID_TYPE_P (decl_ret_type))
    switch (compare_tree_int (TYPE_SIZE (decl_ret_type), 64))
      {
      case 0: /* means they are equal */
	elem_fn_values->vectorlength[0] = 2;
	break;
      case -1: /* means it is less than 64 */
	elem_fn_values->vectorlength[0] = 4;
	break;
      default:
	elem_fn_values->vectorlength[0] = 1;
      }
  

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
	      tree jj_purpose = NULL_TREE, jj_value = TREE_VALUE (jj_tree);

	      /* this means we have a mask/nomask */
	      if (TREE_CODE (jj_value) == IDENTIFIER_NODE)
		{ 
		  if (!strcmp (IDENTIFIER_POINTER (jj_value), "mask"))
		    elem_fn_values->mask = USE_MASK;		    
		  else if (!strcmp (IDENTIFIER_POINTER (jj_value), "nomask"))
		    elem_fn_values->mask = USE_NOMASK;
		  continue;
		}
	      else
		jj_purpose = TREE_PURPOSE (jj_value);
	      
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
	    }
	}
    }

  for (ii_tree = DECL_ARGUMENTS (decl); ii_tree;
       ii_tree = DECL_CHAIN (ii_tree))
    {
      bool already_found = false;
      arg_number++;
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
  if (elem_fn_values->no_vlengths == 0)
    elem_fn_values->no_vlengths = 1; /* we have a default value if none is
				      * given */
  return elem_fn_values;
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
