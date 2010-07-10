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
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include <gpython/gpython.h>
#include <gpython/objects.h>
#include <gpython/vectors.h>
#include <gpython/garbage.h>

struct gpy_obj_integer_t {
  long int Int;
};

void * gpy_obj_integer_init( gpy_literal_t * lit )
{
  struct gpy_obj_integer_t *retval = (struct gpy_obj_integer_t *)
    gpy_malloc( sizeof(struct gpy_obj_integer_t) );

  gpy_assert( lit->type == TYPE_INTEGER );
  retval->Int = lit->literal.integer;

  return ((void*) retval);
}

void gpy_obj_integer_destroy( void * self )
{
  if( self )
    gpy_free( self );
}

void gpy_obj_integer_print( void * self, FILE * fd, bool newline )
{
  struct gpy_obj_integer_t * si = (struct gpy_obj_integer_t *) self;
  fprintf( fd, "%li", si->Int );
  if( newline )
    fprintf( fd, "\n" );
}

gpy_object_state_t *
gpy_obj_integer_add( gpy_object_state_t * o1, gpy_object_state_t * o2 )
{

  gpy_object_state_t * retval = NULL_OBJ_STATE;
  debug("Integer addition!\n");

  if( !strcmp( o1->obj_t_ident, "Int" ) )
    {
      if( !strcmp( o2->obj_t_ident, "Int") )
	{
	  struct gpy_obj_integer_t *t1 = (struct gpy_obj_integer_t*) o1->self;
	  struct gpy_obj_integer_t *t2 = (struct gpy_obj_integer_t*) o2->self;

	  mpfr_t x,y,z;
	  mpfr_init( z );
	  mpfr_init_set_si( x, t1->Int, GMP_RNDU );
	  mpfr_init_set_si( y, t2->Int, GMP_RNDU );

	  if( mpfr_add( z, x, y, GMP_RNDU ) )
	    {
	      fatal("overflow in integer addition!\n");
	    }

	  retval = gpy_rr_fold_integer( mpfr_get_si( z, GMP_RNDU ) );
	  mpfr_clears( x, y, z, (mpfr_ptr)0 );
	}
      else
	{
	  fatal("invalid object type <%s>!\n", o2->obj_t_ident );
	}
    }
  else
    {
      fatal("invalid object type <%s>!\n", o1->obj_t_ident );
    }

  return retval;
}

struct gpy_number_prot_t integer_binary_ops = {
  true,
  &gpy_obj_integer_add,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

struct gpy_type_obj_def_t integer_obj = {
  "Int",
  sizeof(struct gpy_obj_integer_t),
  &gpy_obj_integer_init,
  &gpy_obj_integer_destroy,
  &gpy_obj_integer_print,
  &integer_binary_ops,
};

void gpy_obj_integer_mod_init( gpy_vector_t * const vec )
{
  gpy_vec_push( vec, &integer_obj );
}
