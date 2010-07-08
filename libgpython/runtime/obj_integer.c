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

#include <gpython/gpython.h>

struct gpy_obj_integer_t {
  long int Int;
};

void * gpy_obj_integer_init( gpy_object_state_t lit )
{
  return NULL;
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

gpy_object_state_t
gpy_obj_integer_add( gpy_object_state_t x, gpy_object_state_t y )
{
  return NULL;
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

const struct gpy_type_obj_def_t integer_obj = {
  "Int",
  sizeof(struct gpy_obj_integer_t),
  &gpy_obj_integer_init,
  &gpy_obj_integer_destroy,
  &gpy_obj_integer_print,
  &integer_binary_ops,
};

void gpy_obj_integer_mod_init( void )
{
  
}
