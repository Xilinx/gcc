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

VEC(tree,gc) * gpy_decl_fold_primitive (const gpy_symbol_obj * const sym)
{
  VEC(tree,gc) * retval = VEC_alloc(tree,gc,0);
  switch( sym->op_a_t )
    {
    case TYPE_INTEGER:
      {
	tree address = build_decl (sym->loc, VAR_DECL, create_tmp_var_name("P"),
				   gpy_object_type_ptr);
	VEC(tree,gc) * bc = gpy_builtin_get_fold_int_call (sym->op_a.integer);
	VEC_safe_push (tree, gc, retval,
		       build2 (MODIFY_EXPR, gpy_object_type_ptr,
			       address, VEC_index(tree,bc,0))
		       );
	VEC_safe_push (tree, gc, retval, address);
      }
      break;

    default:
      fatal_error("invalid primitive type <0x%x>!\n", sym->op_a_t );
      break;
    }

  return retval;
}

VEC(tree,gc) * gpy_decl_process_assign (gpy_symbol_obj ** op_a,
					gpy_symbol_obj ** op_b,
					VEC(gpy_ctx_t,gc) * context)
{
  VEC(tree,gc) * retval = NULL; 
  gpy_symbol_obj *opa, *opb;

  if( op_a && op_b ) { opa= *op_a; opb= *op_b; }
  else {
    fatal_error("operands A or B are undefined!\n");
    return NULL;
  }

  if( opa->type == SYMBOL_REFERENCE )
    {
      int l = (VEC_length( gpy_ctx_t, context));
      VEC(tree,gc) * rhs_tree_vec = NULL;
      
      tree decl = gpy_ctx_lookup_decl (context, opa->op_a.string);

      if (!decl)
	{
	  gpy_ctx_t x = VEC_index (gpy_ctx_t, context, (l-1));
	  
	  decl = build_decl (opa->loc, VAR_DECL,
			     get_identifier (opa->op_a.string),
			     gpy_object_type_ptr);
	  
	  if (!gpy_ctx_push_decl (decl, opa->op_a.string, x))
	    fatal_error("error pushing var decl <%s>!\n", opa->op_a.string );
	}

      rhs_tree_vec = gpy_process_expression (opb, context);
      tree rhs_tree_res_decl = VEC_pop (tree, rhs_tree_vec);

      retval = rhs_tree_vec;
      
      VEC_safe_push (tree, gc, retval, build2 (MODIFY_EXPR, gpy_object_type_ptr,
					       decl,rhs_tree_res_decl));

      VEC(tree,gc) * bc =  gpy_builtin_get_incr_ref_call (decl);
      GPY_VEC_stmts_append(retval,bc);

      bc = gpy_builtin_get_set_decl_call (decl);
      GPY_VEC_stmts_append(retval,bc);

      VEC_safe_push (tree, gc, retval, decl);
    }
  else
    fatal_error("Invalid accessor for assignment <0x%x>!\n", opa->type );

  return retval;
}

VEC(tree,gc) *
gpy_decl_process_bin_expression (gpy_symbol_obj ** op_a, gpy_symbol_obj ** op_b,
				 gpy_opcode_t operation, VEC(gpy_ctx_t,gc) * context)
{
  VEC(tree,gc) * retval = NULL;
  VEC(tree,gc) * t1 = NULL, * t2 = NULL;
  
  VEC(tree,gc) *  op = NULL;
  gpy_symbol_obj *opa, *opb; 
  if( op_a && op_b ) { opa= *op_a; opb= *op_b; }
  else {
    fatal_error("operands A or B are undefined!\n");
    return NULL;
  }

  t1 = gpy_process_expression (opa, context);
  t2 = gpy_process_expression (opb, context);

  tree t1_res_decl = VEC_pop (tree, t1);
  tree t2_res_decl = VEC_pop (tree, t2);

  switch (operation)
    {
    case OP_BIN_ADDITION:
      op = gpy_builtin_get_eval_expression_call (t1_res_decl, t2_res_decl,
						 operation);
      break;

    default:
      fatal_error("unhandled symbol type <0x%x>!\n", operation);
      break;
    }

  if( op )
    {
      gcc_assert (VEC_length(tree, op) == 1);
      retval = t2;

      int idx = 0; tree itx = NULL_TREE;
      for ( ; VEC_iterate(tree,t1,idx,itx); ++idx )
	{
	  VEC_safe_push (tree,gc,retval,itx);
	}
      
      tree address = build_decl (opa->loc, VAR_DECL, create_tmp_var_name("T"),
				 gpy_object_type_ptr);
      VEC_safe_push (tree, gc, retval,
		     build2 (MODIFY_EXPR, gpy_object_type_ptr, address, VEC_index(tree,op,0)));
      VEC_safe_push (tree,gc,retval,address);
    }

  return retval;
}

