/* This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "coretypes.h"
#include "opts.h"
#include "tree.h"
#include "tree-iterator.h"
#include "gimple.h"
#include "toplev.h"
#include "debug.h"
#include "options.h"
#include "flags.h"
#include "convert.h"
#include "diagnostic-core.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "target.h"
#include "cgraph.h"

#include <gmp.h>
#include <mpfr.h>

#include "vec.h"
#include "hashtab.h"

#include "gpy.h"
#include "symbols.h"
#include "runtime.h"
#include "opcodes.def"

tree gpy_builtin_get_init_call( void )
{
  tree fntype = build_function_type( void_type_node, void_list_node );
  tree gpy_rr_init = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
				 get_identifier("gpy_rr_init_runtime"),
				 fntype );
  tree restype = TREE_TYPE(gpy_rr_init);
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT(resdecl) = gpy_rr_init;
  DECL_RESULT(gpy_rr_init) = resdecl;
  DECL_EXTERNAL( gpy_rr_init ) = 1;
  TREE_PUBLIC( gpy_rr_init ) = 1;

  return ( build_call_expr( gpy_rr_init, 0, NULL_TREE ) );
}

tree gpy_builtin_get_cleanup_final_call( void )
{
  tree fntype = build_function_type( void_type_node, void_list_node );
  tree gpy_rr_cleanup = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
				    get_identifier("gpy_rr_cleanup_final"),
				    fntype );
  tree restype = TREE_TYPE( gpy_rr_cleanup );
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT(resdecl) = gpy_rr_cleanup;
  DECL_RESULT(gpy_rr_cleanup) = resdecl;
  DECL_EXTERNAL( gpy_rr_cleanup ) = 1;
  TREE_PUBLIC( gpy_rr_cleanup ) = 1;

  return ( build_call_expr( gpy_rr_cleanup, 0, NULL_TREE ) );
}

tree gpy_builtin_get_push_context_call( void )
{
  tree fntype = build_function_type( void_type_node, void_list_node );
  tree gpy_rr_push = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
				 get_identifier("gpy_rr_push_context"),
				 fntype );
  tree restype = TREE_TYPE( gpy_rr_push );
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT(resdecl) = gpy_rr_push;
  DECL_RESULT(gpy_rr_push) = resdecl;
  DECL_EXTERNAL( gpy_rr_push ) = 1;
  TREE_PUBLIC( gpy_rr_push ) = 1;

  return ( build_call_expr( gpy_rr_push, 0, NULL_TREE ) );
}

tree gpy_builtin_get_pop_context_call( void )
{
  tree fntype = build_function_type( void_type_node, void_list_node );
  tree gpy_rr_pop = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
				 get_identifier("gpy_rr_pop_context"),
				 fntype );
  tree restype = TREE_TYPE( gpy_rr_pop );
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT(resdecl) = gpy_rr_pop;
  DECL_RESULT(gpy_rr_pop) = resdecl;
  DECL_EXTERNAL( gpy_rr_pop ) = 1;
  TREE_PUBLIC( gpy_rr_pop ) = 1;

  return ( build_call_expr( gpy_rr_pop, 0, NULL_TREE ) );
}

tree gpy_builtin_get_fold_int_call( int val )
{
  tree params = NULL_TREE;

  chainon( params, tree_cons (NULL_TREE, integer_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, void_type_node, NULL_TREE) );
  
  tree fntype = build_function_type( ptr_type_node, params );
  tree gpy_eval_expr_decl = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
					get_identifier("gpy_rr_fold_integer"),
					fntype );
  tree restype = TREE_TYPE(gpy_eval_expr_decl);
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT(resdecl) = gpy_eval_expr_decl;
  DECL_RESULT(gpy_eval_expr_decl) = resdecl;
  DECL_EXTERNAL( gpy_eval_expr_decl ) = 1;
  TREE_PUBLIC( gpy_eval_expr_decl ) = 1;
  
  return ( build_call_expr( gpy_eval_expr_decl, 1,
			    build_int_cst( integer_type_node,
					   val )
			    )
	   );
}

tree gpy_builtin_get_eval_expression_call( tree t1, tree t2,
					   gpy_opcode_t op )
{
  tree params = NULL_TREE;

  chainon( params, tree_cons (NULL_TREE, ptr_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, ptr_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, integer_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, void_type_node, NULL_TREE) );

  tree fntype = build_function_type( ptr_type_node, params );
  tree gpy_eval_expr_decl = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
					get_identifier("gpy_rr_eval_expression"),
					fntype );
  tree restype = TREE_TYPE(gpy_eval_expr_decl);
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT( resdecl ) = gpy_eval_expr_decl;
  DECL_RESULT( gpy_eval_expr_decl ) = resdecl;
  DECL_EXTERNAL( gpy_eval_expr_decl ) = 1;
  TREE_PUBLIC( gpy_eval_expr_decl ) = 1;

  return ( build_call_expr( gpy_eval_expr_decl, 3, t1, t2,
			    build_int_cst( integer_type_node, op ))
	   );
}

tree gpy_builtin_get_eval_accessor_call( tree t1, tree t2 )
{
  fatal_error("Accessor's not implemented yet!\n");
}
