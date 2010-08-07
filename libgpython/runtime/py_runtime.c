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
#include <stdarg.h>

#include <gpython/gpython.h>
#include <gpython/objects.h>
#include <gpython/vectors.h>
#include <gpython/garbage.h>

gpy_vector_t * gpy_primitives;
gpy_vector_t * gpy_namespace_vec;

void gpy_rr_init_primitives( void )
{
  gpy_primitives = (gpy_vector_t *)
    gpy_malloc( sizeof(gpy_vector_t) );
  gpy_vec_init( gpy_primitives );

  gpy_obj_integer_mod_init( gpy_primitives );
}

void gpy_rr_init_runtime( void ) 
{
  /* 
     Setup runtime namespace Init builtin's
  */
  gpy_rr_init_primitives( );

  gpy_namespace_vec = (gpy_vector_t*)
    gpy_malloc( sizeof(gpy_vector_t) );
  gpy_vec_init( gpy_namespace_vec );

  gpy_context_t * head = (gpy_context_t *)
    gpy_malloc( sizeof(gpy_context_t) );
  head->symbols = (gpy_vector_t*)
    gpy_malloc( sizeof(gpy_vector_t) );
  gpy_vec_init( head->symbols );

  gpy_vec_push( gpy_namespace_vec, head );
}

void gpy_rr_cleanup_final( void )
{
  gpy_rr_pop_context( );

  if( gpy_namespace_vec->length > 0 )
    error( "<%i> un-free'd conexts!\n", gpy_namespace_vec->length );

  gpy_vec_free( gpy_namespace_vec );
  gpy_vec_free( gpy_primitives );
}

gpy_object_state_t * gpy_rr_fold_integer( int x )
{
  gpy_object_state_t * retval = NULL_OBJ_STATE;
  Gpy_Object_State_Init_Ctx( retval, gpy_namespace_vec );

  gpy_literal_t i;
  i.type = TYPE_INTEGER;
  i.literal.integer = x;

  gpy_type_obj_def_t * Int_def = (gpy_type_obj_def_t *)
    gpy_primitives->vector[ 0 ];
  
  gpy_assert( Int_def );
  retval->obj_t_ident = gpy_strdup( Int_def->identifier );
  retval->ref_count++;
  retval->self = Int_def->init_hook( &i );
  retval->definition = Int_def;

  debug("initilized integer object <%p> to <%i>!\n",
	(void*)retval, x );

  return retval;
}

void gpy_rr_eval_print( int fd, int count, ...  )
{
  va_list vl; int idx;
  va_start( vl,count );

  /* gpy_object_t is a typedef of gpy_object_state_t *
     to keep stdarg.h happy
  */
  gpy_object_state_t * it = NULL;
  for( idx = 0; idx<count; ++idx )
    {
      it = va_arg( vl, gpy_object_t );
      gpy_assert( it );
      (*it->definition).print_hook( it, stdout, false );
    }
  fprintf( stdout, "\n" );
  va_end(vl);
}

inline
void gpy_rr_incr_ref_count( gpy_object_state_t * x )
{
  gpy_assert( x );
  debug("incrementing ref count on <%p>:<%i> to <%i>!\n",
	(void*) x, x->ref_count, (x->ref_count + 1) );
  x->ref_count++;
}

inline
void gpy_rr_decr_ref_count( gpy_object_state_t * x )
{
  gpy_assert( x );
  debug("decrementing ref count on <%p>:<%i> to <%i>!\n",
	(void*) x, x->ref_count, (x->ref_count - 1) );
  x->ref_count--;
}

void gpy_rr_push_context( void )
{
  gpy_context_t * ctx = (gpy_context_t *)
    gpy_malloc( sizeof(gpy_context_t) );
  ctx->symbols = (gpy_vector_t*)
    gpy_malloc( sizeof(gpy_vector_t) );
  gpy_vec_init( ctx->symbols );

  gpy_vec_push( gpy_namespace_vec, ctx );
}

void gpy_rr_pop_context( void )
{
  gpy_context_t * head = Gpy_Namespace_Head;
  void ** vec = head->symbols->vector;

  unsigned int idx = 0;
  for( ; idx<(head->symbols->length); ++idx )
    {
      gpy_object_state_t * i = (gpy_object_state_t *) vec[ idx ];
      Gpy_Decr_Ref( i );
    }

  gpy_garbage_invoke_sweep( gpy_namespace_vec );

  /* Loop over for stragglers like returns which need pushed up a
     context soo they can still be garbage collected....
     --
     straggler is something which will have a (ref_count > 0)
  */

  gpy_context_t * popd = gpy_vec_pop( gpy_namespace_vec );
  gpy_vec_free( popd->symbols );
  gpy_free( popd );
}

gpy_object_state_t *
gpy_rr_eval_dot_operator( gpy_object_state_t * x, gpy_object_state_t * y )
{
  return NULL;
}

gpy_object_state_t *
gpy_rr_eval_expression( gpy_object_state_t * x,	gpy_object_state_t * y,
			gpy_opcode_t op )
{
  char * op_str = NULL;
  gpy_object_state_t * retval = NULL;

  struct gpy_type_obj_def_t * def = x->definition;
  struct gpy_number_prot_t * binops = (*def).binary_protocol;
  struct gpy_number_prot_t binops_l = (*binops);

  if( binops_l.init )
    {
      binary_op o = NULL;
      switch( op )
	{
	case OP_BIN_ADDITION:
	  o = binops_l.n_add;
	  op_str = " + ";
	  break;

	default:
	  fatal("unhandled binary operation <%x>!\n", op );
	  break;
	}

#ifdef DEBUG
      x->definition->print_hook( x->self, stdout, false );
      fprintf(stdout, "%s", op_str );
      y->definition->print_hook( y->self, stdout, true );
#endif
      
      retval = o( x,y );
      
#ifdef DEBUG
      if( retval )
	{
	  fprintf(stdout, "evaluated to: ");
	  retval->definition->print_hook( retval->self, stdout, false );
	  fprintf(stdout, "!\n");
	}
#endif
    }
  else
    {
      fatal("object type <%s> has no binary protocol!\n",
	    x->obj_t_ident );
    }
 
  return retval;
}
