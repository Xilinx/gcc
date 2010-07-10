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

static VEC( gpy_sym,gc ) * gpy_decls;
VEC(gpy_ctx_t,gc) * gpy_ctx_table;

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
gpy_symbol_obj * gpy_process_AST( gpy_symbol_obj ** sym )
{
  gpy_symbol_obj *nn = NULL;
  gpy_symbol_obj *retval = NULL;
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
		      fatal_error("error processing the expression AST!\n");
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
      tree decl = gpy_ctx_lookup_decl( sym->op_a.string, VAR );
      gcc_assert( sym->op_a_t == TYPE_STRING );
      if( decl )
	{
	  debug("tree reference <%s>!\n", sym->op_a.string);
	  retval = decl;
	}
      else
	{
	  error("undeclared symbol reference <%s>!\n", sym->op_a.string );
	}
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
	  res = gpy_process_bin_expression( &opa, &opb, sym->type );
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

tree gpy_process_functor( const gpy_symbol_obj * const  functor )
{
  gpy_symbol_obj * o = functor->op_a.symbol_table;
  tree fntype = build_function_type(void_type_node, void_list_node);
  tree retval = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
			    get_identifier( functor->identifier ),
			    fntype );

  tree declare_vars = NULL_TREE;
  tree bind = NULL_TREE;
  tree block = NULL_TREE;
  tree resdecl = NULL_TREE;
  tree restype = TREE_TYPE(retval);

  unsigned int idx = 0;
  gpy_context_branch *co = NULL;
  gpy_ident it = NULL;

  SET_DECL_ASSEMBLER_NAME(retval, get_identifier(functor->identifier));

  TREE_PUBLIC(retval) = 1;
  TREE_STATIC(retval) = 1;

  resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			restype );
  DECL_CONTEXT(resdecl) = retval;
  DECL_RESULT(retval) = resdecl;

  DECL_INITIAL(retval) = block;

  /* push a new context for local symbols */
  co = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );
  co->var_decls = NULL;
  co->fnc_decls = NULL;
  gpy_init_ctx_branch( &co );
  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, co );

  while( o )
    {
      /* looping over the gpy_symbol_obj block of function statements
	 and getting the respective tree's and creating the GENERIC block
       */
      tree x = gpy_get_tree( o );
      gcc_assert( x );
      append_to_statement_list( x, &block );
      o = o->next;
    }
  
  for( ; VEC_iterate( gpy_ident,co->var_decl_t, idx, it ); ++idx )
    {
      /* get all block var_decls */
      tree x = gpy_ctx_lookup_decl( it->ident, VAR );
      gcc_assert( TREE_CODE( x ) == VAR_DECL );
      debug("got var decl <%p>:<%s> within func <%s>!\n", (void*)x,
	    it->ident, functor->identifier );
      debug_tree( x );
      TREE_CHAIN( x ) = declare_vars;
      declare_vars = x;
    }
 
  if( declare_vars != NULL_TREE )
    {
      tree bl = make_node(BLOCK);
      BLOCK_SUPERCONTEXT(bl) = retval;
      DECL_INITIAL(retval) = bl;
      BLOCK_VARS(bl) = declare_vars;
      TREE_USED(bl) = 1;
      bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		    NULL_TREE, bl);
      TREE_SIDE_EFFECTS(bind) = 1;
    }
  BIND_EXPR_BODY(bind) = block;
  block = bind;
  DECL_SAVED_TREE(retval) = block;
   
  VEC_pop( gpy_ctx_t, gpy_ctx_table );

  gimplify_function_tree( retval );

  cgraph_add_new_function(retval, false);
  cgraph_finalize_function(retval, true);
    
  return retval;
}

tree gpy_get_tree( gpy_symbol_obj * sym )
{
  tree retval_decl = NULL;
  
  debug( "processing decl of type <0x%X> object <%p>\n",
	 sym->type, (void*) sym );

  if( sym->exp == OP_EXPRESS )
    {
      sym = gpy_process_AST( &sym );
      retval_decl = gpy_process_expression( sym );
    }
  else
    {
      switch( sym->type )
	{
	case STRUCTURE_FUNCTION_DEF:
	  retval_decl = gpy_process_functor( sym );
	  break;
	  
	default:
	  fatal_error("unhandled symbol type <0x%x>\n", sym->type );
	  break;
	}
    }

  return retval_decl;
}

void gpy_write_globals( void )
{
  tree *vec; 

  tree fntype = build_function_type(void_type_node, void_list_node);
  tree retval = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
			    get_identifier( "main" ),
			    fntype );
  tree declare_vars = NULL_TREE;
  tree bind = NULL_TREE;
  tree block = NULL_TREE;
  tree resdecl = NULL_TREE;
  tree restype = TREE_TYPE(retval);

  unsigned long decl_len = 0, vec_len = 0;
  unsigned int idx;

  gpy_context_branch *co = NULL;
  gpy_symbol_obj * it = NULL;
  gpy_ident itg = NULL;

   /* push a new context for local symbols */
  co = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );
  co->var_decls = NULL;
  co->fnc_decls = NULL;
  gpy_init_ctx_branch( &co );
  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, co );
  
  decl_len = VEC_length( gpy_sym, gpy_decls );
  vec = XNEWVEC( tree, decl_len );

  SET_DECL_ASSEMBLER_NAME(retval, get_identifier("main"));

  TREE_PUBLIC(retval) = 1;
  TREE_STATIC(retval) = 1;

  resdecl = build_decl( UNKNOWN_LOCATION, RESULT_DECL, NULL_TREE,
			restype );
  DECL_CONTEXT(resdecl) = retval;
  DECL_RESULT(retval) = resdecl;

  DECL_INITIAL(retval) = block;

  debug("decl_len <%lu>!\n", decl_len );
  for( idx= 0; VEC_iterate(gpy_sym,gpy_decls,idx,it); ++idx )
    {
      tree x = gpy_get_tree( it );
      gpy_preserve_from_gc( vec[idx] );
      if( TREE_CODE( x ) != FUNCTION_DECL )
	{
	  append_to_statement_list( x, &block );
	}
      else
	{
	  vec[ vec_len ] = x;
	  vec_len++;
	}
    }

  for( ; VEC_iterate( gpy_ident,co->var_decl_t, idx, itg ); ++idx )
    {
      /* get all block var_decls */
      tree x = gpy_ctx_lookup_decl( itg->ident, VAR );
      gcc_assert( TREE_CODE( x ) == VAR_DECL );
      debug("got var decl <%p>:<%s> within func <%s>!\n", (void*)x,
	    itg->ident, "main" );
      debug_tree( x );
      TREE_CHAIN( x ) = declare_vars;
      declare_vars = x;
    }
  if( declare_vars != NULL_TREE )
    {
      tree bl = make_node(BLOCK);
      BLOCK_SUPERCONTEXT(bl) = retval;
      DECL_INITIAL(retval) = bl;
      BLOCK_VARS(bl) = declare_vars;
      TREE_USED(bl) = 1;
      bind = build3(BIND_EXPR, void_type_node, BLOCK_VARS(bl),
		    NULL_TREE, bl);
      TREE_SIDE_EFFECTS(bind) = 1;
    }
  BIND_EXPR_BODY(bind) = block;
  block = bind;
  DECL_SAVED_TREE(retval) = block;

  VEC_pop( gpy_ctx_t, gpy_ctx_table );

  gimplify_function_tree( retval );

  cgraph_add_new_function(retval, false);
  cgraph_finalize_function(retval, true);

  debug("Finished processing!\n\n");

  wrapup_global_declarations( vec, decl_len );

  check_global_declarations( vec, decl_len );
  emit_debug_global_declarations( vec, decl_len );

  cgraph_finalize_compilation_unit( );

  debug("finished passing to middle-end!\n\n");

  free( vec );
}
