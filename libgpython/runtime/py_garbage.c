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

#include <gmp.h>
#include <mpfr.h>

#include <gpython/gpython.h>
#include <gpython/vectors.h>
#include <gpython/garbage.h>

gpy_vector_t * gpy_garbage_vec;

void gpy_garbage_invoke( void )
{
  if( gpy_garbage_vec )
    {
      debug("garbage collector running...\n");
      gpy_object_state_t p_obj = NULL_OBJ_STATE;

      while( (p_obj= gpy_vec_pop( gpy_garbage_vec )) )
	{
	  gpy_garbage_free_obj( p_obj );
	}
      gpy_vec_free( gpy_garbage_vec );
      gpy_garbage_vec = NULL;
    }
}

void gpy_garbage_mark_obj__( gpy_object_state_t const sym )
{
  if( sym )
    {
      if( gpy_garbage_vec )
        {
	  gpy_vec_push( gpy_garbage_vec, sym );
        }
      else
        {
          gpy_garbage_vec= (gpy_vector_t *)
            gpy_malloc(sizeof(gpy_vector_t));
          gpy_vec_init( gpy_garbage_vec );

          gpy_vec_push( gpy_garbage_vec, sym );
        }
    }
}

void gpy_garbage_invoke_sweep( gpy_vector_t * const context )
{
  unsigned int ctx_l = gpy_rr_context_get_table_size( context );
  if( context )
    {
      debug("sweeping context table for garbage length <%u>...\n", ctx_l);
      gpy_branch_context* ctx_idx = NULL; signed int idx = (ctx_l - 1);

      while( idx >= 0 )
	{
	  ctx_idx = context->array[ idx ];
	  void ** s_arr = ctx_idx->symbol_stack->array;

	  int i = 0; unsigned int len = (ctx_idx->symbol_stack->length);
	  debug("stack length = <%u>!\n", len );
	  for( ; i<len; ++i )
	    {
	      gpy_symbol_obj * o = (gpy_symbol_obj *) s_arr[ i ];
	      if( o )
		{
		  debug( "object <%p> has ref count <%u>!\n",
			 (void *) o, o->n_ref );

		  // If no references remain
		  if( o->n_ref <= 0 )
		    {
		      gpy_garbage_mark_obj( &o );
		      s_arr[ i ] = NULL;
		    }
		}
	    }
	  idx--;
	}
    }
  gpy_garbage_invoke( );
}

/* Cleanup the program for exit! */
void gpy_cleanup( void )
{
  debug("cleanup.......\n");

  mpfr_free_cache( );
}
