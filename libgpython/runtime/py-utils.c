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

bool gpy_args_check_fmt (gpy_object_t ** args, const char * fmt)
{
  bool retval = true;

  size_t idx = 0, l = strlen (fmt);
  for ( ; args[idx]->T != TYPE_NULL; ++idx )
    {
      gpy_assert (idx < l);
      char c = fmt[idx];
      
      switch (c)
	{
	  /* Literal Integer ... */
	case 'i':
	  {
	    gpy_assert (args[idx]->T == TYPE_OBJECT_LIT);
	    gpy_assert (args[idx]->o.literal->type == TYPE_INTEGER);
	    debug ("integer check pass!\n");
	  }
	  break;

	default:
	  error ("unhandled format option <%c>!\n", c);
	  break;
	}
    }

  return retval;
}


int gpy_args_lit_parse_int (gpy_object_t * arg)
{
  int retval = -1;
  gpy_assert (arg->T == TYPE_OBJECT_LIT);
  gpy_assert (arg->o.literal->type == TYPE_INTEGER);

  retval = arg->o.literal->literal.integer;
  debug ("parsed int <%i>!\n", retval);

  return retval;
}

gpy_object_t * gpy_create_object_state (gpy_typedef_t * type,
					const void * self)
{
  gpy_object_state_t * state = (gpy_object_state_t *)
    gpy_malloc (sizeof(gpy_object_state_t));
  state->obj_t_ident = strdup(type->identifier);
  state->ref_count = 0;
  state->self = self;
  state->definition = type;

  gpy_object_t * retval = (gpy_object_t *)
    gpy_malloc (sizeof(gpy_object_t));
  retval->T = TYPE_OBJECT_STATE;
  retval->o.object_state = state;

  return retval;
}
