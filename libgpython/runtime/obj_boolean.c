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

struct gpy_obj_boolean_t {
  bool boolean;
} ;

void * gpy_obj_boolean_init( gpy_literal_t * lit )
{
  return NULL;
}

void gpy_obj_boolean_destroy( void * self )
{
  if( self )
    gpy_free( self )
}

void gpy_obj_boolean_print( void * self, FILE * fd, bool newline )
{
  struct gpy_obj_boolean_t * si = (struct gpy_obj_boolean_t *) self;

  if( si->boolean )
    fprintf( fd, "True " );
  else
    fprintf( fd, "False " );

  if( newline )
    fprintf( fd, "\n" );
}

/*
  The member methods table
   - member fields can be handle'd in a similar fashion
*/
static gpy_method_def_t gpy_obj_boolean_methods[] = {
  { NULL, NULL, 0 }
};

/* The binary protocol handles */
static struct gpy_number_prot_t boolean_binary_ops = {
  false,
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
  NULL,
};

static struct gpy_type_obj_def_t boolean_obj = {
  "Bool",
  sizeof(struct gpy_obj_boolean_t),
  gpy_obj_boolean_init,
  gpy_obj_boolean_destroy,
  gpy_obj_boolean_print,
  &boolean_binary_ops,
  gpy_obj_boolean_methods
};

/*
  Should be used for handling any Field initilizers!
*/
void gpy_obj_boolean_mod_init( gpy_vector_t * const vec )
{
  gpy_vec_push( vec, &boolean_obj );
}
