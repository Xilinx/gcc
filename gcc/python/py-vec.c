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
#include "opcodes.def"
#include "symbols.h"
#include "pypy-tree.h"
#include "runtime.h"

/*
  This is just a basic garbage collection to free all IR symbols
  created from the parser...
 */
VEC( gpy_sym,gc ) * gpy_garbage_decls;
#define threshold_alloc(x) (((x)+16)*3/2)

void gpy_gg_invoke_garbage( void )
{
  int idx = 0; gpy_symbol_obj * it = NULL;
  for( ; idx<VEC_iterate(gpy_sym,gpy_garbage_decls,idx,it ); ++idx )
    {
      gpy_garbage_free_obj( &it );
      it = NULL;
    }
}

void gpy_garbage_free_obj( gpy_symbol_obj ** sym )
{
  if( sym )
    {
      debug("deleting object <%p>!\n", (void *) (*sym) );

      if( (*sym)->identifier )
        {
          debug("garbage symbol identifier '%s'\n", (*sym)->identifier );
          free( (*sym)->identifier );
        }

      switch( (*sym)->op_a_t )
	{
	case TYPE_STRING:
	  if( (*sym)->op_a.string )
	    {
	      char *xstr= (char*) (*sym)->op_a.string;
	      free( xstr );
	    }
	  break;

	case TYPE_SYMBOL:
	  gpy_garbage_free_obj( &((*sym)->op_a.symbol_table) );
	  break;

	default:
	  break;
	}

      switch( (*sym)->op_b_t )
	{

	case TYPE_STRING:
	  if( (*sym)->op_b.string )
	    {
	      char *xstr= (char*) (*sym)->op_b.string;
	      free( xstr );
	    }
	  break;

	case TYPE_SYMBOL:
	  gpy_garbage_free_obj( &((*sym)->op_b.symbol_table) );
	  break;	

	default:
	  break;
	}

      if( (*sym)->next )
        gpy_garbage_free_obj( &((*sym)->next) );

      free( (*sym) );
      (*sym) = NULL;
    }
}

/**
 *  Hash function in use is a 32bit FNV-1
 *
 *  * http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
 *  * http://isthe.com/chongo/tech/comp/fnv/#FNV-1
 *
 *  Eventualy look at using a sha1 may work better but may be too big
 *  a Digest.., but should avoid conflicts better
 **/
gpy_hashval_t gpy_dd_hash_string( const char * s )
{
  gpy_hashval_t hash =  0x811C9DC5;
  unsigned char ch = '\0';
  unsigned int idx = 0;

  for( ; idx < strlen( s ); ++idx )
    {
      ch = (*s + idx);
      hash ^= ch;
      hash *= 0x1000193;
    }

  return hash;
}

gpy_hash_entry_t *
gpy_dd_hash_lookup_table( gpy_hash_tab_t * tbl, gpy_hashval_t h )
{
  gpy_hash_entry_t* retval = NULL;
  if( tbl->array )
    {
      gpy_hashval_t size= tbl->size, idx= (h % size);
      gpy_hash_entry_t *array= tbl->array;

      while( array[idx].data )
        {
          if( array[idx].hash == h )
            break;

          idx++;
          if( idx >= size )
            idx= 0;
        }
      retval= (array+idx);
    }
  else
    retval= NULL;

  return retval;
}

void ** gpy_dd_hash_insert( gpy_hashval_t h, void * obj,
			    gpy_hash_tab_t *tbl )
{
  void **retval = NULL;
  gpy_hash_entry_t *entry = NULL;
  if( tbl->length >= tbl->size )
    gpy_dd_hash_grow_table( tbl );

  entry= gpy_dd_hash_lookup_table( tbl, h );
  if( entry->data )
    retval= &( entry->data );
  else
    {
      entry->data= obj;
      entry->hash= h;
      tbl->length++;
    }
  return retval;
}

void gpy_dd_hash_grow_table( gpy_hash_tab_t * tbl )
{
  unsigned int prev_size= tbl->size, size= 0, i= 0;
  gpy_hash_entry_t *prev_array= tbl->array, *array;

  size = threshold_alloc( prev_size );
  array = (gpy_hash_entry_t*)
    xcalloc( size, sizeof(gpy_hash_entry_t) );

  tbl->length = 0; tbl->size= size;
  tbl->array= array;

  for ( ; i<prev_size; ++i )
    {
      gpy_hashval_t h= prev_array[i].hash;
      void *s= prev_array[i].data;

      if( s )
        gpy_dd_hash_insert( h, s, tbl );
    }
  if( prev_array )
    free( prev_array );
}

inline
void gpy_dd_hash_init_table( gpy_hash_tab_t ** tbl )
{
  if( tbl )
    {
      gpy_hash_tab_t *tb= *tbl;
      tb->size= 0; tb->length= 0;
      tb->array= NULL;
    }
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

inline
void gpy_init_ctx_branch( gpy_context_branch * const * o )
{
  if( o )
    {
      (*o)->var_decls = (gpy_hash_tab_t *)
	xmalloc( sizeof(gpy_hash_tab_t) );
      (*o)->fnc_decls = (gpy_hash_tab_t *)
	xmalloc( sizeof(gpy_hash_tab_t) );

      gpy_dd_hash_init_table( &((*o)->var_decls) );
      gpy_dd_hash_init_table( &((*o)->fnc_decls) );

      (*o)->var_decl_t = VEC_alloc(gpy_ident,gc,0);
      (*o)->fnc_decl_t = VEC_alloc(gpy_ident,gc,0);
      (*o)->ctx_init = VEC_alloc(tree,gc,0);
    }
}

void gpy_init_tbls( void )
{
  gpy_context_branch *o = (gpy_context_branch *)
    xmalloc( sizeof(gpy_context_branch) );

  gpy_init_ctx_branch( &o );

  VEC_safe_push( gpy_ctx_t, gc, gpy_ctx_table, o );
}

bool gpy_ctx_push_decl( tree decl, const char * s,
			gpy_context_branch * ctx,
			enum DECL_T T )
{
  bool retval = true; gpy_hash_tab_t *t = NULL;
  gpy_hashval_t h = 0;

  void ** slot = NULL;
  const char * type;
  gpy_ident o = (gpy_ident) xmalloc( sizeof(gpy_ident_t) );
  o->ident = xstrdup( s );
  
  debug("ident <%s> at <%p>!\n", s, (void*)o );

  if( T == VAR )
    {
      t = ctx->var_decls;
      type = "VAR";
    }
  else fatal_error("unhandled decl type!\n");

  debug("trying to push decl <%s>!\n", s );

  h = gpy_dd_hash_string( o->ident );
  slot = gpy_dd_hash_insert( h, decl, t );
  if( !slot )
    {
      debug("successfully pushed DECL <%s>:<%s>!\n", s,type );
      if( T == VAR )
	{
	  VEC_safe_push( gpy_ident, gc, ctx->var_decl_t, o );
	}
    }
  else
    {
      debug("decl <%s> already pushed!\n", s );
      retval = false;
    }

  return retval;
}

tree gpy_ctx_lookup_decl( VEC(gpy_ctx_t,gc) * context, const char * s, enum DECL_T T )
{
  tree retval = NULL;
  unsigned int n_ctx = VEC_length( gpy_ctx_t,context );
  int idx = 0; gpy_context_branch * it = NULL;

  gpy_hashval_t h = gpy_dd_hash_string( s );
  debug( "trying to lookup <%s> : context table length = <%i>!\n",
	 s, n_ctx );

  for( idx=(n_ctx-1); idx>=0; --idx )
    {
      it = VEC_index( gpy_ctx_t, context, idx );

      gpy_hash_entry_t * o = NULL;
      gpy_hash_tab_t * decl_table = NULL;

      if( T == VAR )
	decl_table = it->var_decls;
      else
	decl_table = it->fnc_decls;

      o = gpy_dd_hash_lookup_table( decl_table, h );
      if( o )
	{
	  if( o->data )
	    {
	      debug("found symbol <%s> in context <%p>!\n", s, (void*)it );
	      retval = (tree) (o->data) ;
	      break;
	    }
	}
    }
  return retval;
}
