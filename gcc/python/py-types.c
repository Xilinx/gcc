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

tree gpy_get_callable_record_type( void )
{
  return VEC_index(tree,gpy_builtin_types_vec,0);
}

tree gpy_init_callable_record( const char * identifier, int args,
			       bool c, tree fndecl )
{
  VEC(constructor_elt,gc) *struct_data_cons = NULL;

  /*
  tree ident = build_string(strlen(identifier), identifier);
  TREE_TYPE(ident) = build_pointer_type( char_type_node );
  */

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

void gpy_initilize_types( void )
{
  VEC_safe_push(tree,gc,gpy_builtin_types_vec, gpy_build_callable_record_type());
}
