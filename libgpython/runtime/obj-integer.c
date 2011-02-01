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
  int Int;
};

/*
   This Represents:

class foo:
  def __init__ (self, ... )


  Where program wise __init__ is called and the instance
  of the object is created as gpy_object_state_t *
*/
gpy_object_t * gpy_obj_integer_init (gpy_object_t ** args)
{
  gpy_object_t * retval = NULL_OBJECT;

  return retval;
}

/* Destroys self (type) not the object state */
void gpy_obj_integer_destroy (gpy_object_t * self)
{
  return;
}

void gpy_obj_integer_print (gpy_object_t * self, FILE * fd, bool newline)
{
  return;
}

gpy_object_t *
gpy_obj_integer_whoop_noargs (gpy_object_t * self, gpy_object_t ** args )
{
  printf("inside whoop function!\n\n");
  return NULL;
}

gpy_object_t *
gpy_obj_integer_add (gpy_object_t * o1, gpy_object_t * o2)
{
  return;
}

/*
  The member methods table
   - member fields can be handle'd in a similar fashion
*/
static gpy_method_def_t gpy_obj_integer_methods[] = {
  { "whoop_noargs", (gpy_builtin_callback__)
    &gpy_obj_integer_whoop_noargs, METH_NOARGS },
  { NULL, NULL, 0 }
};

/* The binary protocol handles */
static struct gpy_number_prot_t integer_binary_ops = {
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

static struct gpy_type_obj_def_t integer_obj = {
  "Int",
  sizeof(struct gpy_obj_integer_t),
  gpy_obj_integer_init,
  gpy_obj_integer_destroy,
  gpy_obj_integer_print,
  &integer_binary_ops,
  gpy_obj_integer_methods
};

/*
  Should be used for handling any Field initilizers!
*/
void gpy_obj_integer_mod_init( gpy_vector_t * const vec )
{
  gpy_vec_push( vec, &integer_obj );
}
