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

#include <gmp.h>
#include <mpfr.h>

#include "vec.h"
#include "hashtab.h"

#include "gpy.h"
#include "symbols.h"
#include "opcodes.def"
#include "y.py.h"

DEF_VEC_P( gpy_sym );
DEF_VEC_ALLOC_P( gpy_sym,gc );
static VEC( gpy_sym,gc ) * gpy_decls;

VEC(gpy_ctx_t,gc) * gpy_ctx_table;

#define threshold_alloc(x) (((x)+16)*3/2)

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

inline
void gpy_init_ctx_branch( gpy_context_branch * const * o )
{
  if( o )
    {
      (*o)->var_decls = htab_create_alloc( 512, &htab_hash_string,
					   tree_decl_map_eq, 0,
					   xcalloc, free );

      (*o)->fnc_decls = htab_create_alloc( 512, &htab_hash_string,
					   tree_decl_map_eq, 0,
					   xcalloc, free );

      (*o)->var_decl_t = VEC_alloc(gpy_ident,gc,0);
      (*o)->fnc_decl_t = VEC_alloc(gpy_ident,gc,0);
    }
}

void gpy_init_tbls( void )
{
  gpy_context_branch *o = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );

  gpy_init_ctx_branch( &o );

  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, o );
}

void gpy_ident_vec_init( gpy_ident_vector_t * const v )
{
  v->size = threshold_alloc( 0 );
  v->vector = (void**) xcalloc( v->size, sizeof(void*) );
  v->length = 0;
}

void gpy_ident_vec_push( gpy_ident_vector_t * const v,  void * s )
{
  if( s )
    {
      if( v->length >= v->size )
	{
	  signed long size = threshold_alloc( v->size );
	  v->vector = (void**) xrealloc( v->vector, size*sizeof(void*) );
	  v->size = size;
	}
      v->vector[ v->length ] = s;
      v->length++;
    }
}

void * gpy_ident_vec_pop( gpy_ident_vector_t * const v )
{
  void * retval = v->vector[ v->length-1 ];
  v->length--;
  return retval;
}

bool gpy_ctx_push_decl( tree decl, const char * s,
			gpy_context_branch * ctx,
			enum DECL_T T )
{
  bool retval = true; htab_t t = NULL;
  void ** slot = NULL;  VEC(gpy_ident,gc) * st;

  if( T == VAR )
    {
      t = ctx->var_decls;
      st = ctx->var_decl_t;
    }
  else
    {
      t = ctx->fnc_decls;
      st = ctx->fnc_decl_t;
    }

  debug("trying to push decl <%s>!\n", s );

  slot = htab_find_slot_with_hash( t, s, htab_hash_string( s ), INSERT );
  if( slot )
    {
      gpy_ident o = (gpy_ident) xmalloc( sizeof(gpy_ident_t) );
      o->ident = xstrdup( s );

      debug("ident <%s> at <%p>!\n", s, (void*)o );

      if( !o )
	fatal_error("whoop!\n");

      (*slot) = decl;
      
      VEC_safe_push( gpy_ident, gc, st, o );
    }
  else
    retval = false;

  return retval;
}

tree gpy_ctx_lookup_decl( const char * s, enum DECL_T T )
{
  tree retval = NULL;
  unsigned int n_ctx = VEC_length( gpy_ctx_t,gpy_ctx_table );
  unsigned int idx = 0; gpy_context_branch * it = NULL;

  debug( "trying to lookup <%s> : context table length = <%i>!\n",
	 s, n_ctx );

  for( ; VEC_iterate(gpy_ctx_t,gpy_ctx_table,idx,it); ++idx )
    {
      htab_t decl_table = NULL; void ** o = NULL;
      if( T == VAR )
	decl_table = it->fnc_decls;
      else
	decl_table = it->var_decls;

      o = htab_find_slot_with_hash( decl_table, s, htab_hash_string( s ), NO_INSERT );
      if( o )
	{
	  debug("found symbol <%s> in context <%p>!\n", s, (void*)it );
	  retval = (tree)(*o) ;
	}
    }
  return retval;
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
      tree decl = gpy_ctx_lookup_decl( sym->op_a.string, VAR );

      gcc_assert( sym->op_a_t == TYPE_STRING );
      debug("tree reference <%s>!\n", sym->op_a.string);

      retval = get_identifier( sym->op_a.string );

      if( !decl )
	{
	  tree decl = build_decl( UNKNOWN_LOCATION, VAR_DECL,
				  get_identifier( sym->op_a.string ),
				  integer_type_node );

	  gpy_ctx_t x = VEC_index( gpy_ctx_t, gpy_ctx_table,
				   (VEC_length( gpy_ctx_t, gpy_ctx_table)-1) );
	  
	  if( !(gpy_ctx_push_decl( decl, sym->op_a.string, x, VAR )) )
	    fatal_error("error pushing var decl <%s>!\n", sym->op_a.string );
	  
	  debug( "built the VAR_DECL <%p> for <%s>!\n", (void*)decl,
		 sym->op_a.string );
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

tree gpy_process_functor( const gpy_symbol_obj * const  functor )
{
  gpy_symbol_obj * o = functor->op_a.symbol_table;
  tree block = NULL; tree block_decl = NULL; tree t = NULL;


  tree fntype = build_function_type(void_type_node, void_list_node);
  tree retval = build_decl( UNKNOWN_LOCATION, FUNCTION_DECL,
			    get_identifier( functor->identifier ),
			    fntype );
  unsigned int idx = 0;
  gpy_context_branch *co = NULL; gpy_ident it = NULL;

  SET_DECL_ASSEMBLER_NAME(retval, get_identifier(functor->identifier));

  TREE_PUBLIC(retval) = 1;
  DECL_EXTERNAL(retval) = 1;

  gpy_preserve_from_gc(retval);

  block = make_node( BLOCK );
  block_decl = make_node( BLOCK );
  DECL_INITIAL(retval) = block_decl;

  /* push a new context for local symbols */
  co = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );

  co->var_decls = NULL;
  co->fnc_decls = NULL;
  gpy_init_ctx_branch( &co );
  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, co );

  while( o )
    {
      tree de = NULL;
 
      de = gpy_get_tree( o );
      TREE_CHAIN( block ) = de;

      o = o->next;
    }
  
  for( ; VEC_iterate( gpy_ident,co->var_decl_t, idx, it ); ++idx )
    {
      t = gpy_ctx_lookup_decl( it->ident, VAR );
      TREE_CHAIN( t ) = block_decl;
    }

  TREE_CHAIN( block_decl ) = block;

  TREE_USED(block_decl) = 1;
  BLOCK_SUPERCONTEXT(block_decl) = retval;
 
  VEC_pop( gpy_ctx_t, gpy_ctx_table );

  gimplify_function_tree(retval);

  cgraph_add_new_function(retval, false);
  cgraph_finalize_function(retval, true);
    
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
  tree *vec; tree *decl_vec; unsigned long decl_len = 0, tmp = 0;
  unsigned int idx, idy;

  gpy_ctx_t x = VEC_index( gpy_ctx_t, gpy_ctx_table,
			   (VEC_length( gpy_ctx_t, gpy_ctx_table)-1) );

  gpy_symbol_obj * it = NULL; gpy_ident i = NULL;
  
  tmp = decl_len = VEC_length( gpy_sym, gpy_decls );
  vec = XNEWVEC( tree, decl_len );

  debug("decl_len <%lu>!\n", decl_len);
  for( idx=0; VEC_iterate(gpy_sym,gpy_decls,idx,it); ++idx )
    {
      debug("decl AST <%p>!\n", (void*)it );
      vec[ idx ] = gpy_get_tree( it ); 
      gpy_preserve_from_gc( vec[idx] );
      debug("finished processing <%p> got decl tree addr <%p>!\n",
	    (void*)it, (void*) vec[idx] );
    }

  decl_len += VEC_length( gpy_ident, x->var_decl_t );
  decl_vec = XNEWVEC( tree, decl_len );

  for( idx=0; VEC_iterate( gpy_ident,x->var_decl_t, idx, i); ++idx )
    {
      decl_vec[ idx ] = gpy_ctx_lookup_decl( i->ident, VAR );
      gpy_preserve_from_gc( decl_vec[idx] );
    }
  for( idy=0; idy<tmp; ++idy )
    {
      decl_vec[ idx ] = vec[ idy ];
      idx++;
    }

  debug("Finished processing!\n\n");

  wrapup_global_declarations( decl_vec, decl_len );

  check_global_declarations( decl_vec, decl_len );
  emit_debug_global_declarations( decl_vec, decl_len );

  cgraph_finalize_compilation_unit( );

  debug("finished passing to middle-end!\n\n");

  free( vec ); free( decl_vec );
}
