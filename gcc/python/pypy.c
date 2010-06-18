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

#include "hashtab.h"

#include <gmp.h>
#include <mpfr.h>

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
      debug("tree primary!\n");
      gcc_assert( sym->op_a_t == TYPE_INTEGER );

      retval = build_int_cst( integer_type_node, sym->op_a.integer );
    }
  else if( sym->type == SYMBOL_REFERENCE )
    {
      gcc_assert( sym->op_a_t == TYPE_STRING );
      debug("tree reference <%s>!\n", sym->op_a.string);

      retval = get_identifier( sym->op_a.string );
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

	case OP_BIN_ADDITION:
	  res = gpy_process_bin_expression( &opa, &opb );
	  break;

	default:
	  fatal_error( "invalid expression evaluation symbol type <0x%x>!\n",
		       sym->type );
	  break;
	}
      if( res ) { retval = res; }
      else { fatal_error("error evaluating expression!\n"); }
    }

  /* gpy_preserve_from_gc( retval ); */

  return retval;
}

/*
          void foo()
          {
            int a;
            {
              int b;
            }
            int c;
          }
     

you would get the following:

          tree foo = FUNCTION_DECL;
          tree decl_a = VAR_DECL;
          tree decl_b = VAR_DECL;
          tree decl_c = VAR_DECL;
          tree block_a = BLOCK;
          tree block_b = BLOCK;
          tree block_c = BLOCK;
          BLOCK_VARS(block_a) = decl_a;
          BLOCK_SUBBLOCKS(block_a) = block_b;
          BLOCK_CHAIN(block_a) = block_c;
          BLOCK_SUPERCONTEXT(block_a) = foo;
          BLOCK_VARS(block_b) = decl_b;
          BLOCK_SUPERCONTEXT(block_b) = block_a;
          BLOCK_VARS(block_c) = decl_c;
          BLOCK_SUPERCONTEXT(block_c) = foo;
          DECL_INITIAL(foo) = block_a;
*/

tree gpy_process_functor( const gpy_symbol_obj * const  functor )
{
  gpy_symbol_obj * o = functor->op_a.symbol_table;
  tree block = NULL;
  tree fntype = build_function_type(void_type_node, void_list_node);
  tree retval = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
			    get_identifier( functor->identifier ),
			    fntype );

  SET_DECL_ASSEMBLER_NAME(retval, get_identifier(functor->identifier));

  TREE_PUBLIC(retval) = 1;
  DECL_EXTERNAL(retval) = 1;

  block = make_node( BLOCK );

  while( o )
    {
      tree decl = gpy_get_tree( o );
      TREE_CHAIN( decl ) = block;

      o = o->next;
    }

  TREE_USED(block) = 1;
  BLOCK_SUPERCONTEXT(block) = retval;
  DECL_INITIAL(retval) = block;
  
  return retval;
}

tree gpy_get_tree( gpy_symbol_obj * sym )
{
  tree retval_decl = NULL; gpy_symbol_obj * o = sym;
  
  debug( "processing decl of type <0x%X> object <%p>\n",
	 o->type, (void*) o );

  if( o->exp == OP_EXPRESS )
    {
      /* Should already be pre-processed ... */
      retval_decl = gpy_process_expression( o );
    }
  else
    {
      switch( o->type )
	{
	case STRUCTURE_FUNCTION_DEF:
	  retval_decl = gpy_process_functor( o );
	  break;
	  
	default:
	  fatal_error("unhandled symbol type <0x%x>\n", o->type );
	  break;
	}
    }

  return retval_decl;
}

void gpy_write_globals( void )
{
  tree *vec;
  unsigned decl_len = 0;
  unsigned int idx = 0; gpy_symbol_obj *it = NULL;

  decl_len = VEC_length( gpy_sym, gpy_decls );
  vec = XNEWVEC( tree, decl_len );

  debug("decl_len <%u>!\n", decl_len);
  for( ; VEC_iterate(gpy_sym,gpy_decls,idx,it); ++idx )
    {
      debug("decl <%p>!\n", (void*)it );
      vec[ idx ] = gpy_get_tree( it );
      debug("decl addr <%p>!\n", (void*) vec[idx] );
      debug("decl <%p> processed!\n", (void*)it );
    }

  wrapup_global_declarations( vec, decl_len );

  check_global_declarations( vec, decl_len );
  emit_debug_global_declarations( vec, decl_len );

  cgraph_finalize_compilation_unit( );

  free( vec );
}
