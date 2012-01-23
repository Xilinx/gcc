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
#include "tm.h"
#include "tree.h"
#include "langhooks.h"
#include "expr.h"
#include "optabs.h" /* for sync_lock_release in detach */
#include "recog.h"
#include "cilk.h"
#include "gimple.h"
#include "tree-iterator.h"
#include "c-family/c-common.h"
#include "toplev.h" /* only correct declaration of warning() */
#include "output.h"
#include "dwarf2out.h"

static void
mark_cold (tree fndecl)
{
  DECL_ATTRIBUTES (fndecl) = tree_cons (get_identifier ("cold"), NULL_TREE,
					DECL_ATTRIBUTES (fndecl));
}

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
  new_func = build_fn_decl ("__sec_reduce_all_zeros", func_type);
  mark_cold (new_func);
  new_func = lang_hooks.decls.pushdecl (new_func);

  func_type = build_function_type_list (integer_type_node, ptr_type_node,
					NULL_TREE);
  new_func = build_fn_decl ("__sec_reduce_any_zeros", func_type);
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
  return;
}
