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
#include <gpython/vectors.h>
#include <gpython/garbage.h>

gpy_hash_tab_t * gpy_primitives;

void gpy_rr_init_primitives( gpy_hash_tab_t * const p )
{
  return;
}

void gpy_rr_init_runtime ( void ) 
{
  /* Setup runtime namespace */   
  /* Init builtin's */
  gpy_rr_init_primitives( gpy_primitives );

  
  
}

gpy_object_state_t gpy_rr_fold_integer( int x )
{
  gpy_object_state_t retval = NULL_OBJ_STATE;
  Gpy_Object_State_Init( retval );

  gpy_literal_t i;
  i.type = TYPE_INTEGER;
  i.literal.integer = x;

  
  return retval;
}
