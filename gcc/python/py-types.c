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

static VEC(tree,gc) * gpy_builtin_types_vec;

/* @see coverage.c build_fn_info_type ( unsigned int )
   for more examples on RECORD_TYPE's

   Better still @see fortran/trans-types.c - gfc_get_desc_dim_type

   Or even better @see go/types.cc - 2027

typedef gpy_object_state_t * (*__callable)( void );

typedef struct gpy_callable_def_t {
  char * ident; int n_args;
  bool class; __callable call;
} gpy_callable_def_t;

 */

/* go/gofrontend/types.cc 4517 && 5066  for making an array type */

tree gpy_build_callable_record_type( void )
{
  tree type = make_node(RECORD_TYPE);
  tree field_trees = NULL_TREE;
  tree *chain = &field_trees;

  tree field = build_decl(BUILTINS_LOCATION, FIELD_DECL, get_identifier ("ident"),
			  build_pointer_type( char_type_node ));
  DECL_CONTEXT(field) = type;
  *chain = field;
  chain = &TREE_CHAIN(field);

  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, get_identifier ("n_args"),
		     integer_type_node);
  DECL_CONTEXT(field) = type;
  *chain = field;
  chain = &TREE_CHAIN(field);

  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, get_identifier ("class"),
		     boolean_type_node);
  DECL_CONTEXT(field) = type;
  *chain = field;
  chain = &TREE_CHAIN(field);

  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, get_identifier ("call"),
		     ptr_type_node);
  DECL_CONTEXT(field) = type;
  *chain = field;
  chain = &TREE_CHAIN(field);

  TYPE_FIELDS(type) = field_trees;
  layout_type(type);

  return type;
}

/*
  typedef struct gpy_rr_object_state_t {
  char * obj_t_ident;
  signed long ref_count;
  void * self;
  struct gpy_typedef_t * definition;
} gpy_object_state_t ;

typedef struct gpy_object_t {
  enum GPY_OBJECT_T T;
  union{
    gpy_object_state_t * object_state;
    gpy_literal_t * literal;
  } o ;
} gpy_object_t ;

*/

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

  object_state_ptr_type = build_pointer_type (object_state_type_decl);
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

  

}

tree gpy_get_callable_record_type( void )
{
  return VEC_index(tree,gpy_builtin_types_vec,0);
}

tree gpy_init_callable_record( const char * identifier, int args,
			       bool c, tree fndecl )
{
  VEC(constructor_elt,gc) *struct_data_cons = NULL;

  tree ident = build_string(strlen(identifier), identifier);
  TREE_TYPE(ident) = build_pointer_type( char_type_node );

  CONSTRUCTOR_APPEND_ELT (struct_data_cons, build_decl (BUILTINS_LOCATION, FIELD_DECL,
							get_identifier("ident"),
							build_pointer_type( char_type_node )),
			  build_int_cst( build_pointer_type(char_type_node), 0)
			  );
  
  CONSTRUCTOR_APPEND_ELT (struct_data_cons, build_decl (BUILTINS_LOCATION, FIELD_DECL,
							get_identifier("n_args"),
							integer_type_node),
			  build_int_cst(integer_type_node, args )
			  );
  
  CONSTRUCTOR_APPEND_ELT (struct_data_cons, build_decl (BUILTINS_LOCATION, FIELD_DECL,
							get_identifier("class"),
							boolean_type_node),
			  build_int_cst(boolean_type_node,c)
			  );

  CONSTRUCTOR_APPEND_ELT (struct_data_cons, build_decl (BUILTINS_LOCATION, FIELD_DECL,
							get_identifier("call"),
							ptr_type_node),
			  build_int_cst(ptr_type_node,0)
			  );

  return (build_constructor(gpy_get_callable_record_type(), struct_data_cons));
}

void gpy_initilize_types (void)
{
  VEC_safe_push(tree,gc,gpy_builtin_types_vec, gpy_build_callable_record_type());
}
