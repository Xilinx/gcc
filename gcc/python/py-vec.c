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
#include "py-runtime.h"

/*
  This is just a basic garbage collection to free all IR symbols
  created from the parser...
 */
VEC(gpy_sym,gc) * gpy_garbage_decls;
VEC(tree,gc) * gpy_function_decls;
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

void gpy_garbage_free_obj (gpy_symbol_obj ** sym)
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

gpy_hashval_t gpy_dd_hash_string (const char * s)
{
  const unsigned char *str = (const unsigned char *) s;
  gpy_hashval_t r = 0;
  unsigned char c;

  while ((c = *str++) != 0)
    r = r * 67 + c - 113;

  return r;
}

gpy_hash_entry_t *
gpy_dd_hash_lookup_table (gpy_hash_tab_t * tbl, gpy_hashval_t h)
{
  gpy_hash_entry_t* retval = NULL;
  if( tbl->array )
    {
      int size= tbl->size, idx= (h % size);
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

void ** gpy_dd_hash_insert (gpy_hashval_t h, void * obj,
			    gpy_hash_tab_t *tbl)
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
void gpy_dd_hash_init_table (gpy_hash_tab_t * tbl)
{
  gpy_hash_tab_t *tb= tbl;
  tb->size= 0; tb->length= 0;
  tb->array= NULL;
}

bool gpy_ctx_push_decl (tree decl, const char * s,
			gpy_hash_table_t * tbl)
{
  bool retval = true;
  gpy_hashval_t h = 0;

  h = gpy_dd_hash_string (s);
  void ** slot = gpy_dd_hash_insert (h,decl,tbl);

  if (!slot)
    {
      debug ("pushed decl <%s> into context!\n", s);
    }
  else
    {
      debug ("error pushing decl <%s>!\n", s);
    }
  
  return retval;
}

tree gpy_ctx_lookup_decl (VEC(gpy_ctx_t,gc) * context, const char * s)
{
  tree retval = NULL_TREE;

  gpy_hashval_t = h = gpy_dd_hash_string (s);
  gpy_ctx_t it;

  int i;
  int l = VEC_length (gpy_ctx_t,context);
  for (i = (l-1); i>=0; --i)
    {
      it = VEC_index (gpy_ctx_t, context, i);

      gpy_hash_entry_t * o = NULL;
      o = gpy_dd_hash_lookup_table (it, h);
      if (o)
	{
	  if (o->data)
	    {
	      debug ("found decl <%s>!\n", s);
	      retval = (tree) o->data;
	      break;
	    }
	}
    }

  return retval;
}

void __gpy_debug__ (const char * file, unsigned int lineno,
		    const char * fmt, ...)
{
  va_list args;
  fprintf( stderr, "debug: <%s:%i> -> ",
           file, lineno );
  va_start( args, fmt );
  vfprintf( stderr, fmt, args );
  va_end( args );
}
