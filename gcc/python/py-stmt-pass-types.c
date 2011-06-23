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
#include "tm.h"
#include "opts.h"
#include "tree.h"
#include "tree-iterator.h"
#include "tree-pass.h"
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

#include "gpython.h"
#include "py-dot-codes.def"
#include "py-dot.h"
#include "py-vec.h"
#include "py-tree.h"
#include "py-types.h"
#include "py-runtime.h"

static tree gpy_stmt_pass_process_toplevel_decls (VEC(gpydot,gc) *);

/*
  Takes the toplevel decls and generates the type example:

  x = 1
  y = 2
  class foobar:
    def __init__ (self):
      self.attrib = x

  we dont need to generate types for normal things like local blocks
  as these will go out of scope anyways because of the python standard
  so we can leave these address's on the stack, this also goes for functions
  for functions it will just be a case of knowing which objects it has
  access to and passing the correct instances so proper scoping is preserved
  or we can pass them all as a naive way as normal static analysis should
  preserve the scoping standard.

  Will only care about the x and y expressions on the toplevel,
  leaving the class
*/
static
tree gpy_stmt_pass_process_toplevel_decls (VEC(gpydot,gc) * decls)
{
  tree retval = make_node (RECORD_TYPE);
  int idx = 0;
  gpy_dot_tree_t * itx = NULL_DOT;

  gpy_hash_tab_t context;
  gpy_dd_hash_init_table (&context);

  for (; VEC_iterate (gpydot, decls, idx, itx); ++idx)
    {
      /* 
	 Lets not worry about un-defined objects within expressions
	 example:

	 x = 1
	 y = x + z  # where z is un-defined

	 We will let that be handled by the 2nd pass.
       */
      if (DOT_FIELD (itx) == D_D_EXPR)
	{
	  itx = gpy_stmt_process_AST_Align (&itx);

	  if (DOT_TYPE(itx) == D_MODIFY_EXPR)
	    {
	      gcc_assert ((DOT_lhs_T(itx) == D_TD_DOT)
			  && (DOT_rhs_T(itx) == D_TD_DOT)
			  );

	      gpy_dot_type_t * target = DOT_lhs_TT (itx);
	      do {
		gpy_hashval_t h = gpy_dd_hash_string (DOT_IDENTIFIER_POINTER (target));
		gpy_dd_hash_insert (h, target, &context);
	      } while (target = DOT_CHAIN (target));
	    }
	}
    }

  if (context.length > 0)
    {
      const char * ident = "main.main";
      tree field = NULL_TREE, last_field = NULL_TREE;

      gpy_hash_entry_t *array = context.array;
      for (idx = 0; idx<context.size; ++idx)
	{
	  gpy_dot_tree_t * d = array[idx].data;
	  if (d)
	    {
	      field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
				  get_identifier (DOT_IDENTIFIER_POINTER (d)),
				  gpy_object_type_ptr);
	      DECL_CONTEXT(field) = retval;
	      if (idx>0)
		DECL_CHAIN (last_field) = field;
	      else
		TYPE_FIELDS (retval) = field;
	      last_field = field;
	    }
	}

      // free (context.array);

      finish_builtin_struct (retval, ident, field, NULL_TREE);
      TYPE_NAME (retval) = get_identifier (ident);

      /*
	DECL_ARTIFICIAL (retval) = 1;
	gpy_preserve_from_gc (retval);
	rest_of_decl_compilation (retval, 1, 0);
      */
    }
  else
    retval = error_mark_node;
  
  return retval;
}

/*
  The first initial pass over the IR to generate the types for each
  declared object within this module:

  example:

  x = 1
  y = 2
  class foobar:
    def __init__ (self):
      self.attrib = x

  This is all actually 2 different objects which need to be setup,
  and we need to generate the types for GCC before as, in the class
  you dont have to pre-declare these attributes to a class to have them
  in that different scope of the entire class

  struct main.main {
    gpy_object_t * x, * y;
  }

  struct main.foobar {
    gpy_object_t * attrib;
  }

  These structs for each class may eventaully be fleshed out with more
  information like member functions and eventually the base attributes
  like polymorphism stuff but we ignore this for now.
*/
VEC(tree,gc) * gpy_stmt_pass_generate_types (VEC(gpydot,gc) * decls)
{
  VEC(tree,gc) * retval = VEC_alloc (tree,gc,0);

  VEC_safe_push (tree, gc,
		 gpy_stmt_pass_process_toplevel_decls (decls),
		 retval);

  /*
    now to iterate over the class declarations and generate their types...
   */
  
  return retval;
}
