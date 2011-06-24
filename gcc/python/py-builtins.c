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

VEC(tree,gc) * gpy_builtin_types_vec;

static tree gpy_build_py_object_type (void);

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

static
tree gpy_build_py_object_type (void)
{
  tree object_state_struct_Type = make_node (RECORD_TYPE);
  
  tree name = get_identifier("obj_t_ident");
  tree field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name,
			  build_pointer_type(char_type_node));
  DECL_CONTEXT(field) = object_state_struct_Type;
  TYPE_FIELDS(object_state_struct_Type) = field;
  tree last_field = field;

  name = get_identifier("ref_count");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name, integer_type_node);
  DECL_CONTEXT(field) = object_state_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  name = get_identifier("self");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name,
		     build_pointer_type (void_type_node));
  DECL_CONTEXT(field) = object_state_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;
  
  name = get_identifier("definition");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name,
		     build_pointer_type (void_type_node));
  DECL_CONTEXT(field) = object_state_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  layout_type(object_state_struct_Type);

  // Give the struct a name for better debugging info.
  name = get_identifier("gpy_object_state_t");
  tree object_state_type_decl = build_decl(BUILTINS_LOCATION, TYPE_DECL, name,
					   object_state_struct_Type);
  DECL_ARTIFICIAL(object_state_type_decl) = 1;
  TYPE_NAME(object_state_struct_Type) = object_state_type_decl;
  gpy_preserve_from_gc(object_state_type_decl);
  rest_of_decl_compilation(object_state_type_decl, 1, 0);

  debug_tree (object_state_type_decl);
  tree object_state_ptr_type = build_pointer_type (object_state_struct_Type);
  gpy_preserve_from_gc (object_state_ptr_type);

  //....................

  tree union_type__ = make_node (UNION_TYPE);

  name = get_identifier ("object_state");
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, name,
		      object_state_ptr_type);
  DECL_CONTEXT(field) = union_type__;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  name = get_identifier ("literal");
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, name,
		      ptr_type_node);
  DECL_CONTEXT(field) = union_type__;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  layout_type (union_type__);
  
  name = get_identifier("o");
  tree union_type_decl = build_decl(BUILTINS_LOCATION, TYPE_DECL, name,
				    union_type__);
  DECL_ARTIFICIAL(union_type_decl) = 1;
  TYPE_NAME(union_type__) = union_type_decl;
  gpy_preserve_from_gc(union_type_decl);
  rest_of_decl_compilation(union_type_decl, 1, 0);

  //.........................

  tree gpy_object_struct_Type = make_node (RECORD_TYPE);
  
  name = get_identifier("type");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name, integer_type_node);
  DECL_CONTEXT(field) = gpy_object_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  name = get_identifier("o");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name,
		     union_type_decl);
  DECL_CONTEXT(field) = gpy_object_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  layout_type (object_state_struct_Type);
  
  name = get_identifier("gpy_object_t");
  tree gpy_object_type_decl = build_decl(BUILTINS_LOCATION, TYPE_DECL, name,
					 gpy_object_struct_Type);
  DECL_ARTIFICIAL(gpy_object_type_decl) = 1;
  TYPE_NAME(gpy_object_struct_Type) = name;
  gpy_preserve_from_gc(gpy_object_type_decl);
  rest_of_decl_compilation(gpy_object_type_decl, 1, 0);

  return build_pointer_type (gpy_object_struct_Type);
}

void gpy_initilize_types (void)
{
  gpy_builtin_types_vec = VEC_alloc(tree,gc,0);

  tree const_char_type = build_qualified_type(unsigned_char_type_node,
					      TYPE_QUAL_CONST);
  tree ctype = build_pointer_type(const_char_type);

  VEC_safe_push (tree,gc,gpy_builtin_types_vec,
		 gpy_build_py_object_type ());
  gpy_preserve_from_gc (gpy_object_type_ptr);

  VEC_safe_push (tree,gc,gpy_builtin_types_vec,
		 build_pointer_type (gpy_object_type_ptr));
  gpy_preserve_from_gc (gpy_object_type_ptr_ptr);
  
  VEC_safe_push (tree,gc,gpy_builtin_types_vec,ctype);
  gpy_preserve_from_gc (gpy_const_char_ptr);
}
