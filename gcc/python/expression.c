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
#include "opcodes.def"
#include "y.py.h"

tree gpy_process_assign( gpy_symbol_obj ** op_a, gpy_symbol_obj ** op_b,
			 tree * block)
{
  tree retval = NULL; gpy_symbol_obj *opa, *opb;

  if( op_a && op_b ) { opa= *op_a; opb= *op_b; }
  else {
    fatal_error("operands A or B are undefined!\n");
    return NULL;
  }

  if( opa->type == SYMBOL_REFERENCE )
    {
      int l = (VEC_length( gpy_ctx_t, gpy_ctx_table));

      tree rhs_tree = NULL_TREE;
      tree decl = gpy_ctx_lookup_decl( opa->op_a.string, VAR );

      if( !decl )
	{
	  gpy_ctx_t x = VEC_index( gpy_ctx_t, gpy_ctx_table,
				   (l-1) );
	  
	  decl = build_decl( opa->loc, VAR_DECL,
			     get_identifier( opa->op_a.string ),
			     build_pointer_type( void_type_node ) );
	  
	  if( !(gpy_ctx_push_decl( decl, opa->op_a.string, x, VAR )) )
	    fatal_error("error pushing var decl <%s>!\n", opa->op_a.string );
	  
	  debug( "built the VAR_DECL <%p> for <%s>!\n", (void*)decl,
		 opa->op_a.string );
	}
      rhs_tree = gpy_process_expression( opb, block );
      
      retval = build2( MODIFY_EXPR, ptr_type_node,
		       decl, rhs_tree );

      debug("built assignment for <%s>!\n", opa->op_a.string );
    }
  else
    {
      fatal_error("Invalid accessor for assignment <0x%x>!\n", opa->type );
    }

  debug_tree( retval );

  return retval;
}

tree gpy_process_bin_expression( gpy_symbol_obj ** op_a, gpy_symbol_obj ** op_b,
				 gpy_opcode_t operation, tree * block )
{
  gpy_symbol_obj *opa, *opb; tree retval = NULL;
  tree t1 = NULL_TREE, t2 = NULL_TREE;

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
  DECL_CONTEXT(resdecl) = gpy_eval_expr_decl;
  DECL_RESULT(gpy_eval_expr_decl) = resdecl;
  DECL_EXTERNAL( gpy_eval_expr_decl ) = 1;
  TREE_PUBLIC( gpy_eval_expr_decl ) = 1;

  if( op_a && op_b ) { opa= *op_a; opb= *op_b; }
  else {
    fatal_error("operands A or B are undefined!\n");
    return NULL;
  }

  t1 = gpy_process_expression( opa, block );
  t2 = gpy_process_expression( opb, block );

  switch( operation )
    {
    case OP_BIN_ADDITION:
      retval =  build_call_expr( gpy_eval_expr_decl, 3, t1, t2,
				 build_int_cst( integer_type_node,
						OP_BIN_ADDITION )
				 );
      break;

    default:
      error("unhandled symbol type <0x%x>!\n", operation);
      retval = NULL;
      break;
    }

  debug_tree( retval );

  return retval;
}

