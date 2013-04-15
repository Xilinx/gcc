
/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains routines to handle Pragma SIMD expression
   handling routines in the C Compiler.
   Copyright (C) 2013  Free Software Foundation, Inc.
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
#include "cp-tree.h"
#include "diagnostic-core.h"


/* This function is passed in as a function pointer to walk_tree.  *TP is
   the current tree pointer, *WALK_SUBTREES is set to 0 by this function if
   recursing into TP's subtrees is unnecessary. *DATA is a bool variable that
   is set to false if an error has occured.  */

static tree
find_invalid_stmts (tree *tp, int *walk_subtrees, void *data)
{
  bool *valid = (bool *) data;
  location_t loc = EXPR_HAS_LOCATION (*tp) ? EXPR_LOCATION (*tp) :
    UNKNOWN_LOCATION;
  if (!tp || !*tp)
    return NULL_TREE;
  else if (TREE_CODE (*tp) == GOTO_EXPR)
    {
      error_at (loc, "goto statments are not allowed inside "
		"loops marked with #pragma simd");
      *valid = false;
      *walk_subtrees = 0;
    }
  else if (TREE_CODE (*tp) == BREAK_STMT)
    {
      error_at (loc, "break statements are not allowed inside loop marked "
		"with pragma simd");
      *valid = false;
      *walk_subtrees = 0;
    }
  else if (TREE_CODE (*tp) == CONTINUE_STMT)
    {
      error_at (loc, "continue statements are not allowed inside the loop "
		"marked with pragma simd");
      *walk_subtrees = 0;
      *valid = false;
    }
  else if (TREE_CODE (*tp) == THROW_EXPR)
    {
      error_at (loc, "throw expressions are not allowed inside the loop "
		"marked with pragma simd");
      *walk_subtrees = 0;
      *valid = false;
    }
  else if (TREE_CODE (*tp) == TRY_BLOCK)
    {
      error_at (loc, "try statements are not allowed inside loop marked with "
		"#pragma simd");
      *valid = false;
      *walk_subtrees = 0;
    }
  else if (TREE_CODE (*tp) == CILK_FOR_STMT)
    {
      error_at (loc, "_Cilk_for statements are not allowed inside loop marked "
		"with #pragma simd");
      *valid = false;
      *walk_subtrees = 0;
    }
  else if (TREE_CODE (*tp) == CALL_EXPR)
    {
      tree fndecl = CALL_EXPR_FN (*tp);

      if (TREE_CODE (fndecl) == ADDR_EXPR)
	fndecl = TREE_OPERAND (fndecl, 0);
      if (TREE_CODE (fndecl) == FUNCTION_DECL)
	{
	  if (setjmp_call_p (fndecl))
	    {
	      error_at (loc, "setjmps are not allowed inside loops marked with"
			" #pragma simd");
	      *valid = false;
	      *walk_subtrees = 0;
	    }
	}
    }
  return NULL_TREE;
}  


/* Walks through all the subtrees of BODY using walk_tree to make sure invalid
   statements/expressions are not found inside BODY.  Returns false if any
   invalid statements are found.  */

bool
p_simd_valid_stmts_in_body_p (tree body)
{
  bool valid = true;
  cp_walk_tree (&body, find_invalid_stmts, (void *) &valid, NULL);
  return valid;
}
