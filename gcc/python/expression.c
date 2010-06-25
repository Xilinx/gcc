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
#include "cgraph.h"

#include "vec.h"

#include "gpy.h"
#include "symbols.h"
#include "opcodes.def"
#include "y.py.h"

#include <gmp.h>
#include <mpfr.h>

tree gpy_process_assign( gpy_symbol_obj ** op_a,
			 gpy_symbol_obj ** op_b )
{
  tree retval = NULL; gpy_symbol_obj *opa, *opb;

  if( op_a && op_b ) { opa= *op_a; opb= *op_b; }
  else {
    fatal_error("operands A or B are undefined!\n");
    return NULL;
  }

  if( opa->type == SYMBOL_REFERENCE )
    {
      tree reference = NULL_TREE;
      tree rhs_tree = NULL_TREE;

      tree decl = gpy_ctx_lookup_decl( opa->op_a.string, VAR );
      if( !decl )
	{
	  tree decl = build_decl( UNKNOWN_LOCATION, VAR_DECL,
				  get_identifier( opa->op_a.string ),
				  integer_type_node );
	  
	  gpy_ctx_t x = VEC_index( gpy_ctx_t, gpy_ctx_table,
				   (VEC_length( gpy_ctx_t, gpy_ctx_table)-1) );
	  
	  if( !(gpy_ctx_push_decl( decl, opa->op_a.string, x, VAR )) )
	    fatal_error("error pushing var decl <%s>!\n", opa->op_a.string );
	  
	  debug( "built the VAR_DECL <%p> for <%s>!\n", (void*)decl,
		 opa->op_a.string );
	}
      reference = gpy_process_expression( opa );
      rhs_tree = gpy_process_expression( opb );
      
      retval = build2( MODIFY_EXPR, integer_type_node,
		       reference, rhs_tree );
      debug("built assignment for <%s>!\n", opa->op_a.string );
    }
  else
    {
      fatal_error("Invalid accessor for assignment <0x%x>!\n", opa->type );
    }
  return retval;
}

tree gpy_process_bin_expression( gpy_symbol_obj ** op_a,
				 gpy_symbol_obj ** op_b )
{
  gpy_symbol_obj *opa, *opb; tree retval = NULL;
  if( op_a && op_b ) { opa= *op_a; opb= *op_b; }
  else {
    fatal_error("operands A or B are undefined!\n");
    return NULL;
  }

  if( opa->type == SYMBOL_PRIMARY )
    {
      if( opb->type == SYMBOL_PRIMARY )
        {
	  tree t1 = gpy_process_expression( opa );
          tree t2 = gpy_process_expression( opb );

          retval = build2( PLUS_EXPR, integer_type_node, t1, t2 );
        }
      else
        {
	  tree t1 = gpy_process_expression( opa );
          tree t2 = gpy_process_expression( opb );

	  retval = build2( PLUS_EXPR, integer_type_node, t1, t2 );
        }
    }
  else
    {
      if( opb->type == SYMBOL_PRIMARY )
	{
	  tree t1 = gpy_process_expression( opa );
          tree t2 = gpy_process_expression( opb );

	  retval = build2( PLUS_EXPR, integer_type_node, t1, t2 );
	}
      else
	{
	  tree t1 = gpy_process_expression( opa );
          tree t2 = gpy_process_expression( opb );

	  retval = build2( PLUS_EXPR, integer_type_node, t1, t2 );
	}
    }

  return retval;
}

