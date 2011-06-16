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

#include <gmp.h>
#include <mpfr.h>

#include "vec.h"
#include "hashtab.h"

#include "gpython.h"
#include "py-dot-codes.def"
#include "py-dot.h"
#include "py-vec.h"
#include "py-tree.h"
#include "py-types.h"
#include "py-runtime.h"

VEC(tree,gc) * gpy_builtin_get_init_call (void)
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

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr( gpy_rr_init, 0, NULL_TREE ));

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_cleanup_final_call( void )
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

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr( gpy_rr_cleanup, 0, NULL_TREE ));

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_push_context_call( void )
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

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr( gpy_rr_push, 0, NULL_TREE )
		 );

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_pop_context_call( void )
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

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr( gpy_rr_pop, 0, NULL_TREE ));

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_fold_int_call( int val )
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

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr (gpy_eval_expr_decl, 1,
				  build_int_cst (integer_type_node,
						 val)
				  )
			    );
  
  return retval;
}

VEC(tree,gc) * gpy_builtin_get_eval_expression_call( tree t1, tree t2, gpy_opcode_t op )
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

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		  build_call_expr( gpy_eval_expr_decl, 3, t1, t2,
				   build_int_cst( integer_type_node, op ))
		 );

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_incr_ref_call( tree x )
{
  tree params = NULL_TREE;

  chainon( params, tree_cons (NULL_TREE, ptr_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, void_type_node, NULL_TREE) );

  tree fntype = build_function_type( ptr_type_node, params );
  tree gpy_incr_ref_decl = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
					get_identifier("gpy_rr_incr_ref_count"),
					fntype );
  tree restype = TREE_TYPE( gpy_incr_ref_decl );
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT( resdecl ) = gpy_incr_ref_decl;
  DECL_RESULT( gpy_incr_ref_decl ) = resdecl;
  DECL_EXTERNAL( gpy_incr_ref_decl ) = 1;
  TREE_PUBLIC( gpy_incr_ref_decl ) = 1;

  VEC(tree,gc) * retval = VEC_alloc (tree, gc, 0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr( gpy_incr_ref_decl, 1, x )
		 );

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_decr_ref_call( tree x )
{
  tree params = NULL_TREE;

  chainon( params, tree_cons (NULL_TREE, ptr_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, void_type_node, NULL_TREE) );

  tree fntype = build_function_type( ptr_type_node, params );
  tree gpy_decr_ref_decl = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
					get_identifier("gpy_rr_decr_ref_count"),
					fntype );
  tree restype = TREE_TYPE( gpy_decr_ref_decl );
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT( resdecl ) = gpy_decr_ref_decl;
  DECL_RESULT( gpy_decr_ref_decl ) = resdecl;
  DECL_EXTERNAL( gpy_decr_ref_decl ) = 1;
  TREE_PUBLIC( gpy_decr_ref_decl ) = 1;

  VEC(tree,gc) * retval = VEC_alloc (tree, gc, 0);
  VEC_safe_push(tree,gc,retval,
		build_call_expr( gpy_decr_ref_decl, 1, x )
		);

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_print_call( int n, tree * args )
{
  tree params = NULL_TREE;

  chainon( params, tree_cons (NULL_TREE, integer_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, integer_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, va_list_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, void_type_node, NULL_TREE) );

  tree fntype = build_function_type( ptr_type_node, params );
  tree gpy_eval_print_decl = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
					get_identifier("gpy_rr_eval_print"),
					fntype );
  tree restype = TREE_TYPE(gpy_eval_print_decl);
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );
  DECL_CONTEXT( resdecl ) = gpy_eval_print_decl;
  DECL_RESULT( gpy_eval_print_decl ) = resdecl;
  DECL_EXTERNAL( gpy_eval_print_decl ) = 1;
  TREE_PUBLIC( gpy_eval_print_decl ) = 1;

  tree * vec = XNEWVEC( tree, n+2 );

  printf("n = <%i>!!\n\n", n );

  vec[0] = build_int_cst( integer_type_node, 1 );
  vec[1] = build_int_cst( integer_type_node, n );

  int idx = 2, idy = 0;
  for( ; idy<n; ++idy )
    {
      vec[idx] = args[idy];
      idx++;
    }
  VEC(tree,gc) * retval = VEC_alloc (tree, gc, 0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr_loc_array (UNKNOWN_LOCATION,
					    gpy_eval_print_decl,
					    n+2, vec)
		 );
  return retval;
}

VEC(tree,gc) * gpy_builtin_get_finalize_block_call (int n, tree * args)
{
  tree params = NULL_TREE;

  chainon( params, tree_cons (NULL_TREE, integer_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, va_list_type_node, NULL_TREE) );
  chainon( params, tree_cons (NULL_TREE, void_type_node, NULL_TREE) );

  tree fntype = build_function_type( ptr_type_node, params );
  tree gpy_finalize_block_decl = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
					     get_identifier("gpy_rr_finalize_block_decls"),
					     fntype );

  tree restype = TREE_TYPE( gpy_finalize_block_decl );
  tree resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype );

  DECL_CONTEXT( resdecl ) = gpy_finalize_block_decl;
  DECL_RESULT( gpy_finalize_block_decl ) = resdecl;
  DECL_EXTERNAL( gpy_finalize_block_decl ) = 1;
  TREE_PUBLIC( gpy_finalize_block_decl ) = 1;

  tree * vec = XNEWVEC (tree, n+1);

  vec[0] = build_int_cst (integer_type_node, n);

  int idx = 1, idy = 0;
  for( ; idy<n; ++idy )
    {
      vec[idx] = args[idy];
      idx++;
    }

  VEC(tree,gc) * retval = VEC_alloc (tree, gc, 0);
  VEC_safe_push (tree, gc, retval,
		 build_call_expr_loc_array (UNKNOWN_LOCATION,
					    gpy_finalize_block_decl,
					    n+1, vec)
		 );

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_set_decl_call (tree decl)
{
  tree params = NULL_TREE;

  chainon (params, tree_cons (NULL_TREE, gpy_const_char_ptr, NULL_TREE));
  chainon (params, tree_cons (NULL_TREE, gpy_object_type_ptr, NULL_TREE));
  chainon (params, tree_cons (NULL_TREE, void_type_node, NULL_TREE));

  tree fntype = build_function_type (void_type_node, params);
  tree gpy_rr_decl = build_decl (UNKNOWN_LOCATION, FUNCTION_DECL,
				 get_identifier("gpy_rr_set_decl_val"),
				 fntype);

  tree restype = TREE_TYPE (gpy_rr_decl);
  tree resdecl = build_decl (UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype);

  DECL_CONTEXT (resdecl ) = gpy_rr_decl;
  DECL_RESULT (gpy_rr_decl ) = resdecl;
  DECL_EXTERNAL (gpy_rr_decl ) = 1;
  TREE_PUBLIC (gpy_rr_decl ) = 1;

  const char * c_ident = IDENTIFIER_POINTER (DECL_NAME(decl));
  debug ("c_ident = <%s>!\n", c_ident);

  tree type = build_array_type (char_type_node,
				build_index_type (size_int (
							    IDENTIFIER_LENGTH (DECL_NAME(decl))
							    )));
  type = build_qualified_type (type, TYPE_QUAL_CONST);
  gpy_preserve_from_gc (type);
  
  tree str = build_string (IDENTIFIER_LENGTH (DECL_NAME(decl)), c_ident);
  TREE_TYPE (str) = type;
  tree ident = str; //build_fold_addr_expr (str);

  tree address = build_decl (BUILTINS_LOCATION, VAR_DECL,
			     create_tmp_var_name("R"),
			     type);
  TREE_STATIC (address) = 1;
  TREE_READONLY (address) = 1;
  DECL_ARTIFICIAL (address) = 1;

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree, gc, retval,
		 build2 (MODIFY_EXPR, type, address, ident));

  VEC_safe_push (tree, gc, retval,
		 build_call_expr (gpy_rr_decl, 2,
				  address, decl));
  return retval;
}

VEC(tree,gc) * gpy_builtin_get_register_decl_call (tree decl)
{
  tree params = NULL_TREE;
  gcc_assert (TREE_CODE (decl) == FUNCTION_DECL);

  chainon (params, tree_cons (NULL_TREE, gpy_const_char_ptr, NULL_TREE));
  chainon (params, tree_cons (NULL_TREE, void_type_node, NULL_TREE));

  tree fntype = build_function_type (void_type_node, params);
  tree gpy_rr_decl = build_decl (UNKNOWN_LOCATION, FUNCTION_DECL,
				 get_identifier("gpy_rr_register_decl"),
				 fntype);

  tree restype = TREE_TYPE (gpy_rr_decl);
  tree resdecl = build_decl (UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype);

  DECL_CONTEXT (resdecl ) = gpy_rr_decl;
  DECL_RESULT (gpy_rr_decl ) = resdecl;
  DECL_EXTERNAL (gpy_rr_decl ) = 1;
  TREE_PUBLIC (gpy_rr_decl ) = 1;

  const char * c_ident = IDENTIFIER_POINTER(DECL_NAME(decl));
  debug ("c_ident = <%s>!\n", c_ident);

  tree str = build_string (strlen (c_ident), c_ident);
  TREE_TYPE (str) = gpy_const_char_ptr;
  tree ident = build_fold_addr_expr (str);
  
  VEC(tree,gc) * retval = VEC_alloc(tree,gc,0);
  VEC_safe_push (tree,gc,retval,
		 build_call_expr (gpy_rr_decl, 1, ident)
		 );

  return retval;
}

VEC(tree,gc) * gpy_builtin_get_register_callable_call (tree decl, int n)
{
  tree params = NULL_TREE;
  gcc_assert (TREE_CODE (decl) == FUNCTION_DECL);

  chainon (params, tree_cons (NULL_TREE, ptr_type_node, NULL_TREE));
  chainon (params, tree_cons (NULL_TREE, integer_type_node, NULL_TREE));
  chainon (params, tree_cons (NULL_TREE, gpy_const_char_ptr, NULL_TREE));
  chainon (params, tree_cons (NULL_TREE, void_type_node, NULL_TREE));

  tree fntype = build_function_type (void_type_node, params);
  tree gpy_rr_decl = build_decl (UNKNOWN_LOCATION, FUNCTION_DECL,
				 get_identifier("gpy_rr_register_callable"),
				 fntype);
  
  tree restype = TREE_TYPE (gpy_rr_decl);
  tree resdecl = build_decl (UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			     restype);

  DECL_CONTEXT (resdecl ) = gpy_rr_decl;
  DECL_RESULT (gpy_rr_decl ) = resdecl;
  DECL_EXTERNAL (gpy_rr_decl ) = 1;
  TREE_PUBLIC (gpy_rr_decl ) = 1;

  const char * c_ident = IDENTIFIER_POINTER (DECL_NAME(decl));
  debug ("c_ident = <%s>!\n", c_ident);

  tree type = build_array_type (unsigned_char_type_node,
				build_index_type (size_int (
							    IDENTIFIER_LENGTH (DECL_NAME(decl))
							    )));
  type = build_qualified_type (type, TYPE_QUAL_CONST);
  gpy_preserve_from_gc (type);
  
  tree str = build_string (IDENTIFIER_LENGTH (DECL_NAME(decl)), c_ident);
  TREE_TYPE (str) = type;
  tree ident = build_fold_addr_expr (str);

  tree address = build_decl (BUILTINS_LOCATION, VAR_DECL,
			     create_tmp_var_name("C"),
			     type);

  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);
  VEC_safe_push (tree, gc, retval,
		 build2 (MODIFY_EXPR, type, address, ident));

  VEC_safe_push (tree, gc, retval,
		 build_call_expr (gpy_rr_decl, 3,
				  build_fold_addr_expr (decl),
				  build_int_cst (integer_type_node, n),
				  address));
  return retval;
}


VEC(tree,gc) * gpy_builtin_get_fold_call_call (const char * ident, int n,
					       tree *args)
{
  return NULL;
}

VEC(tree,gc) * gpy_builtin_get_eval_accessor_call (tree t1, tree t2)
{
  fatal_error("Accessor's not implemented yet!\n");
  return NULL;
}
