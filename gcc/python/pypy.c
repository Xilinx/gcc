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

typedef gpy_symbol_obj *gpy_sym;

DEF_VEC_P( gpy_sym );
DEF_VEC_ALLOC_P( gpy_sym,gc );
static VEC( gpy_sym,gc ) * gpy_decls;

inline
void gpy_symbol_init_ctx( gpy_symbol_obj * const x )
{
  x->identifier= NULL;
  x->exp = TYPE_SYMBOL_NIL;
  x->type= SYMBOL_PRIMARY;
  x->op_a_t= TYPE_SYMBOL_NIL;
  x->op_b_t= TYPE_SYMBOL_NIL;
  x->op_a.symbol_table= NULL;
  x->op_b.symbol_table= NULL;
  x->next= NULL;
}

/**
 * Fairly Confusing Function to read.
 *
 * example:
 *    >>> x = y = z = 2 + 2 + 2;
 *
 *    --- Currently Yacc parses that expression into this Tree:

                      +
                     / \
                    +   2
		   /
                  =
		 / \
		x   =
		   / \
		  y   =
		     / \
		    z   2

  -- Is converted into the procedure:

  1. z = 2 + 2 + 2;
  2. y = z;
  3. x = y;

  -- Tree structure as so:

                 =
                / \
               x   =
                  / \
                 y   =
		    / \
                   z   +
                      / \
                     +   2
                    / \
                   2   2
 **/
static
gpy_symbol_obj * gpy_process_AST( gpy_symbol_obj **sym )
{
  gpy_symbol_obj *nn = NULL, *retval;
  if( (*sym)->next )
    {
      nn = (*sym)->next;
      (*sym)->next = NULL;
    }
  retval = (*sym);

  if( (*sym)->exp == OP_EXPRESS )
    {
      debug("Processing Expression AST!\n");
      if( retval->type != OP_ASSIGN_EVAL )
	{
	  gpy_symbol_obj *o= retval;
	  gpy_symbol_obj *h= NULL;
	  while( o )
	    {
	      if( o->op_a_t == TYPE_SYMBOL )
		{
		  if( o->op_a.symbol_table->type == OP_ASSIGN_EVAL )
		    {
		      h = o;
		      break;
		    }
		  else
		    {
		      o = o->op_a.symbol_table;
		    }
		}
	      else break;
	    }
	  if( h )
	    {
	      gpy_symbol_obj *head = h->op_a.symbol_table;
	      if( head->op_b.symbol_table->type == OP_ASSIGN_EVAL )
		{
		  gpy_symbol_obj *t = head, *m = NULL;
		  while( t )
		    {
		      if( t->op_b_t == TYPE_SYMBOL )
			{
			  if( t->op_b.symbol_table->type != OP_ASSIGN_EVAL )
			    {
			      m = t;
			      break;
			    }
			  else
			    {
			      t = t->op_b.symbol_table;
			    }
			}
		      else break;
		    }
		  if( m )
		    {
		      h->op_a.symbol_table = m->op_b.symbol_table;
		      m->op_b.symbol_table = retval;
		    }
		  else
		    {
		      error("error processing the expression AST!\n");
		    }
		}
	      else
		{
		  h->op_a.symbol_table = head->op_b.symbol_table;
		  head->op_b.symbol_table = retval;
		}
	      retval = head;
	    }
	}
    }

  if( nn )
    {
      retval->next = nn;
    }
  (*sym) = retval;

  return retval;
}

void gpy_process_decl( gpy_symbol_obj * sym )
{
  debug( "processing declaration <%p> of type: '0x%X'\n",
	 (void*)sym, sym->type );

  if( sym->exp == OP_EXPRESS )
    {
      sym = gpy_process_AST( &sym );
    }
  /* Push the declaration! */
  VEC_safe_push( gpy_sym, gc, gpy_decls, sym );
  debug("decl <%p> was pushed!\n", (void*)sym );
}

tree gpy_process_expression( const gpy_symbol_obj * const sym )
{
  tree retval = NULL;

  if( sym->type == SYMBOL_PRIMARY )
    {
    }
  else if( sym->type == SYMBOL_REFERENCE )
    {
    }
  else
    {
      gpy_symbol_obj *opa= NULL, *opb= NULL; tree res = NULL;
      debug("expression evalution <0x%x>!\n", sym->type );

      opa= sym->op_a.symbol_table;
      opb= sym->op_b.symbol_table;

      debug( "opa->type = <0x%x>, opb->type = <0x%x>!\n",
	     opa->type, opb->type );

      switch( sym->type )
	{
	case OP_ASSIGN_EVAL:
	  res = gpy_process_assign( &opa, &opb );
	  break;

	default:
	  fatal_error( "invalid expression evaluation symbol type <0x%x>!\n",
		       sym->type );
	  break;
	}
      if( res ) { retval = res; }
      else { fatal_error("error evaluating expression!\n"); }
    }

  return retval;
}

tree gpy_get_tree( gpy_symbol_obj * sym )
{
  tree retval_decl = NULL; gpy_symbol_obj * o = sym;
  while( o )
    {
      debug( "processing decl of type <0x%X> object <%p>\n",
	     o->type, (void*) o );

      if( o->exp == OP_EXPRESS )
	{
	  /* Should already be pre-processed ... */
	  retval_decl = gpy_process_expression( o );
	}
      else
	{
	  switch( sym->type )
	    {
	    default:
	      fatal_error("unhandled symbol type <0x%x>\n", o->type );
	      break;
	    }
	}

      o = o->next;
    }

  return retval_decl;
}

void gpy_write_globals( void )
{
  tree *vec;
  unsigned decl_len = VEC_length( gpy_sym, gpy_decls );
  unsigned idx = 0; gpy_symbol_obj *it = NULL;

  cgraph_finalize_compilation_unit( );

  vec = XNEWVEC( tree, decl_len );

  debug("decl_len <%u>!\n", decl_len);
  for( ; VEC_iterate(gpy_sym,gpy_decls,idx,it); ++idx )
    {
      debug("decl <%p>!\n", (void*)it );
      vec[ idx ] = gpy_get_tree( it );
    }

  wrapup_global_declarations( vec, decl_len );
  check_global_declarations( vec, decl_len);
  emit_debug_global_declarations( vec, decl_len );

  free( vec );
}
