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

/*
  typedef gpy_object_t * (*gpy_std_callable)
  (gpy_object_t **);

  typedef gpy_callable__t {
  char * ident;
  gpy_std_callable call;
  int n;
  } gpy_callable_t ;
*/
static
tree gpy_build_callable_record_type (void)
{
  tree callable_struct_Type = make_node (RECORD_TYPE);
  
  tree name = get_identifier("ident");
  tree field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name,
			  build_pointer_type(char_type_node));
  DECL_CONTEXT(field) = callable_struct_Type;
  TYPE_FIELDS(callable_struct_Type) = field;
  tree last_field = field;

  name = get_identifier("call");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name, ptr_type_node);
  DECL_CONTEXT(field) = callable_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  name = get_identifier("n");
  field = build_decl(BUILTINS_LOCATION, FIELD_DECL, name,
		     integer_type_node);
  DECL_CONTEXT(field) = callable_struct_Type;
  DECL_CHAIN(last_field) = field;
  last_field = field;

  layout_type(callable_struct_Type);

  name = get_identifier("gpy_callable_t");
  tree gpy_callable_type_decl = build_decl(BUILTINS_LOCATION, TYPE_DECL, name,
					   callable_struct_Type);
  DECL_ARTIFICIAL(gpy_callable_type_decl) = 1;
  TYPE_NAME(callable_struct_Type) = name;
  gpy_preserve_from_gc(gpy_callable_type_decl);
  rest_of_decl_compilation(gpy_callable_type_decl, 1, 0);

  return build_pointer_type (callable_struct_Type);
}

void gpy_initilize_types (void)
{
  gpy_builtin_types_vec = VEC_alloc(tree,gc,0);

  VEC_safe_push (tree,gc,gpy_builtin_types_vec,
		 gpy_build_py_object_type ());

  VEC_safe_push (tree,gc,gpy_builtin_types_vec,
		 build_pointer_type (gpy_object_type_ptr));

  VEC_safe_push (tree,gc,gpy_builtin_types_vec,
		 gpy_build_callable_record_type ());
}
