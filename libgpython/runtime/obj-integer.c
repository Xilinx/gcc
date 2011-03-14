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
gpy_object_t * gpy_obj_integer_init (gpy_typedef_t * type,
				     gpy_object_t ** args)
{
  gpy_object_t * retval = NULL_OBJECT;

  bool check = gpy_args_check_fmt (args, "i");
  gpy_assert (check);

  int val = gpy_args_lit_parse_int (args[0]);
  struct gpy_obj_integer_t * self = (struct gpy_obj_integer_t *)
    gpy_malloc (sizeof(struct gpy_obj_integer_t));
  self->Int = val;

  retval = gpy_create_object_state (type,self);

  return retval;
}

/* Destroys self (type) not the object state */
void gpy_obj_integer_destroy (gpy_object_t * self)
{
  gpy_assert (self->T == TYPE_OBJECT_STATE);
  gpy_object_state_t * x = self->o.object_state;
  struct gpy_obj_integer_t *x1 = (struct gpy_obj_integer_t *)
    x->self;

  gpy_free (x1);
}

void gpy_obj_integer_print (gpy_object_t * self, FILE * fd, bool newline)
{
  gpy_assert (self->T == TYPE_OBJECT_STATE);
  gpy_object_state_t * x = self->o.object_state;
  struct gpy_obj_integer_t *x1 = (struct gpy_obj_integer_t *)
    x->self;

  fprintf (fd, "%i ", x1->Int);

  if (newline)
    fprintf (fd, "\n");
}

gpy_object_t *
gpy_obj_integer_whoop_noargs (gpy_object_t * self, gpy_object_t ** args)
{
  printf("inside whoop function!\n\n");
  return NULL_OBJECT;
}

gpy_object_t *
gpy_obj_integer_add (gpy_object_t * o1, gpy_object_t * o2)
{
  gpy_object_t * retval = NULL_OBJECT;

  debug ("Integer Addition!\n");

  gpy_object_state_t * x = o1->o.object_state;
  gpy_object_state_t * y = o2->o.object_state;

  if( !strcmp (x->obj_t_ident, "Int") )
    {
      if( !strcmp (y->obj_t_ident, "Int") )
	{
	  struct gpy_obj_integer_t *t1 = (struct gpy_obj_integer_t*) x->self;
	  struct gpy_obj_integer_t *t2 = (struct gpy_obj_integer_t*) y->self;

	  mpfr_t x,y,z;
	  mpfr_init (z);
	  mpfr_init_set_si (x, t1->Int, GMP_RNDU);
	  mpfr_init_set_si (y, t2->Int, GMP_RNDU);

	  if( mpfr_add( z, x, y, GMP_RNDU ) )
	    {
	      fatal("overflow in integer addition!\n");
	    }

	  retval = gpy_rr_fold_integer( mpfr_get_si( z, GMP_RNDU ) );
	  mpfr_clears( x, y, z, (mpfr_ptr)0 );
	}
      else
	{
	  fatal("invalid object type <%s>!\n", y->obj_t_ident );
	}
    }
  else
    {
      fatal("invalid object type <%s>!\n", x->obj_t_ident );
    }
 
  return retval;
}

/*
  The member methods table
   - member fields could be handle'd in a similar fashion
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

static struct gpy_typedef_t integer_obj = {
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
