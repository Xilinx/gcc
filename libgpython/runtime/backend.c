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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <stdbool.h>

#include <gpython/gpython.h>
#include <gpython/vectors.h>

/**
 *  Hash function in use is a 32bit FNV-1
 *
 *  * http://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
 *  * http://isthe.com/chongo/tech/comp/fnv/#FNV-1
 *
 *  Eventualy look at using a sha1 may work better but may be too big
 *  a Digest.., but should avoid conflicts better
 **/
inline
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
  gpy_hash_entry_t * retval = NULL;
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
			    gpy_hash_tab_t * tbl )
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
      entry->data = obj;
      entry->hash = h;
      tbl->length++;
    }
  return retval;
}

void gpy_dd_hash_grow_table( gpy_hash_tab_t * tbl )
{
  unsigned int prev_size= tbl->size, size= 0, i= 0;
  gpy_hash_entry_t *prev_array= tbl->array, *array;

  size = gpy_threshold_alloc( prev_size );
  array = (gpy_hash_entry_t*)
    gpy_calloc( size, sizeof(gpy_hash_entry_t) );

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
    gpy_free( prev_array );
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

void gpy_vec_init( gpy_vector_t * const v )
{
  v->size = gpy_threshold_alloc( 0 );
  v->vector = (void **) gpy_calloc( v->size, sizeof(void *) );
  v->length = 0;
}

void gpy_vec_push( gpy_vector_t * const v, void * const s )
{
  if( s )
    {
      if( v->length >= v->size )
	{
	  signed long size = gpy_threshold_alloc( v->size );
	  v->vector = (void**) gpy_realloc( v->vector, size*sizeof(void*) );
	  v->size = size;
	}
      v->vector[ v->length ] = s;
      v->length++;
    }
}

inline
void * gpy_ident_vec_pop( gpy_vector_t * const v )
{
  register void * retval = v->vector[ v->length-1 ];
  v->length--;
  return retval;
}

/* --- DIAGNOSTIC'S --- */

inline
void gpy_assertion_failed( const char * expr, unsigned line,
			   const char * file, const char * func )
{
  fprintf( stderr, "assertion of <%s> failed at <%s:%s:%u>!\n",
	   expr, file, func, line );
  /* Call cleanups .... */

  exit( EXIT_FAILURE );
}

#ifdef DEBUG
inline
void __gpy_debug__( const char * file, unsigned line,
                    const char * functor, const char * fmt, ... )
{
  fprintf( stderr, "debug: <%s:%s:%u> -> ",
           file, functor, line );
  va_list args;
  va_start( args, fmt );
  vfprintf( stderr, fmt, args );
  va_end( args );
}
#endif

inline
void __gpy_error__( const char * file, unsigned line,
                    const char * functor, const char * fmt, ... )
{
  fprintf( stderr, "error: <%s:%s:%u> -> ",
           file, functor, line );
  va_list args;
  va_start( args, fmt );
  vfprintf( stderr, fmt, args );
  va_end( args );
}

inline
void __gpy_fatal__( const char * file, unsigned line,
		    const char * functor, const char * fmt, ... )
{
  fprintf( stderr, "fatal: <%s:%s:%u> -> ",
           file, functor, line );
  va_list args;
  va_start( args, fmt );
  vfprintf( stderr, fmt, args );
  va_end( args );

  /* Call cleanups .... */

  exit( EXIT_FAILURE );
}

/* --- memory allocators --- */

inline
void * gpy_malloc( size_t s )
{
  register void * retval = malloc( s );
  if( !retval )
    fatal("virtual memory exhausted!\n");

  return retval;
}

inline
void * gpy_realloc( void * p, size_t s )
{
  register void * retval = realloc( p, s );
  if( !retval )
    fatal("virtual memory exhausted!\n");

  return retval;
}

inline
void * gpy_calloc( size_t n, size_t s )
{
  register void * retval = calloc( n , s );
  if( !retval )
    fatal("virtual memory exhausted!\n");

  return retval;
}
