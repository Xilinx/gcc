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
#include "runtime.h"

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
	  gpy_ctx_t x = VEC_index( gpy_ctx_t, gpy_ctx_table, (l-1) );
	  
	  decl = build_decl( opa->loc, VAR_DECL,
			     get_identifier( opa->op_a.string ),
			     build_pointer_type( void_type_node ) );
	  
	  if( !(gpy_ctx_push_decl( decl, opa->op_a.string, x, VAR )) )
	    fatal_error("error pushing var decl <%s>!\n", opa->op_a.string );
	}

      rhs_tree = gpy_process_expression( opb, block );

      tree address = build_decl( opa->loc, VAR_DECL,
				 create_tmp_var_name("A"),
				 build_pointer_type( void_type_node ) );

      append_to_statement_list( build2( MODIFY_EXPR, ptr_type_node, address, rhs_tree ),
				block );

      append_to_statement_list( gpy_builtin_get_incr_ref_call( address ), block );

      retval = build2( MODIFY_EXPR, ptr_type_node, decl, address );
    }
  else
    fatal_error("Invalid accessor for assignment <0x%x>!\n", opa->type );

  return retval;
}

tree gpy_process_bin_expression( gpy_symbol_obj ** op_a, gpy_symbol_obj ** op_b,
				 gpy_opcode_t operation, tree * block )
{
  gpy_symbol_obj *opa, *opb; tree retval = NULL;
  tree t1 = NULL_TREE, t2 = NULL_TREE;

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
      retval = gpy_builtin_get_eval_expression_call( t1, t2, operation );
      break;

    default:
      error("unhandled symbol type <0x%x>!\n", operation);
      retval = NULL;
      break;
    }

  return retval;
}

