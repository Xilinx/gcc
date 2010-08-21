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
#include <gpython/objects.h>
#include <gpython/vectors.h>
#include <gpython/garbage.h>

gpy_vector_t * gpy_garbage_vec;

void gpy_garbage_invoke( void )
{
  if( gpy_garbage_vec )
    {
      debug("garbage collector running...\n");
      gpy_object_state_t * p_obj = NULL_OBJ_STATE;

      while( (p_obj= (gpy_object_state_t *)
	      gpy_vec_pop( gpy_garbage_vec )) )
	{
	  gpy_garbage_free_obj( p_obj );
	}
      gpy_vec_free( gpy_garbage_vec );
      gpy_garbage_vec = NULL;
    }
}

void gpy_garbage_mark_obj__( gpy_object_state_t * const sym )
{
  if( sym )
    {
      if( gpy_garbage_vec )
        {
	  gpy_vec_push( gpy_garbage_vec, sym );
        }
      else
        {
          gpy_garbage_vec = (gpy_vector_t *)
            gpy_malloc( sizeof(gpy_vector_t) );
          gpy_vec_init( gpy_garbage_vec );

          gpy_vec_push( gpy_garbage_vec, sym );
        }
    }
}

void gpy_garbage_invoke_sweep( gpy_vector_t * const context )
{
  signed long ctx_l = context->length;
  if( context )
    {
      debug("sweeping context table for garbage length <%i>...\n", ctx_l);
      gpy_context_t * ctx_idx = NULL; signed long idx = (ctx_l - 1);

      while( idx >= 0 )
	{
	  ctx_idx = context->vector[ idx ];
	  void ** s_arr = ctx_idx->symbols->vector;

	  int i = 0; int len = (ctx_idx->symbols->length);
	  debug("vector length <%i>!\n", len );
	  for( ; i<len; ++i )
	    {
	      gpy_object_state_t * o = (gpy_object_state_t *) s_arr[ i ];
	      if( o )
		{
		  debug( "object <%p> has ref count <%i>!\n",
			 (void *) o, o->ref_count );

		  // If no references remain
		  if( o->ref_count <= 0 )
		    {
		      gpy_garbage_mark_obj( o );
		      s_arr[ i ] = NULL;
		    }
		}
	    }
	  idx--;
	}
    }
  gpy_garbage_invoke( );
}

void gpy_garbage_free_obj( gpy_object_state_t * x )
{
  debug("deleting garbage object <%p>!\n", (void*)x );
  if( x )
    {
      gpy_free( x->obj_t_ident );
      (*x->definition).destroy_hook( x->self );
      gpy_free( x );
    }
}

/* Cleanup the program for exit! */
void gpy_cleanup( void )
{
  debug("cleanup.......\n");

  gpy_garbage_invoke_sweep( gpy_namespace_vec );

  gpy_vec_free( gpy_primitives );
  gpy_vec_free( gpy_namespace_vec );

  mpfr_free_cache( );
}
