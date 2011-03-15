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

/* Used for stack-traces ... */
gpy_vector_t * gpy_call_stack;

void gpy_rr_init_primitives (void)
{
  gpy_primitives = (gpy_vector_t *)
    gpy_malloc( sizeof(gpy_vector_t) );
  gpy_vec_init( gpy_primitives );

  gpy_obj_integer_mod_init( gpy_primitives );
}

void gpy_dump_current_stack_trace (void)
{
  return;
}

void gpy_rr_register_callable (gpy_std_callable call, int nargs,
			       char * ident)
{
  gpy_object_t * c = NULL_OBJECT;

  gpy_callable_t * call_ = (gpy_callable_t *)
    gpy_malloc (sizeof(gpy_callable_t));

  call_->ident = ident;
  call_->n = nargs;
  call_->call = call;

  c = (gpy_object_t *)
    gpy_malloc (sizeof(gpy_object_t));
  c->T = TYPE_CALLABLE;
  c->o.call = call_;

  gpy_hashval_t h = gpy_dd_hash_string (ident);

  gpy_hash_entry_t * f = gpy_rr_lookup_decl (h, gpy_namespace_vec);
  if (f)
    {
      gpy_object_t * prev = f->data;
      gpy_free (prev); // fix later
      f->data = c;
    }
  else
    {
      void ** r = gpy_dd_hash_insert (h, c, Gpy_Vec_Head (gpy_namespace_vec,
							  gpy_hash_tab_t *));
      if (r)
	fatal ("error registering decl <%s>!\n", ident);
    }
}

void gpy_rr_register_decl (char * ident)
{
  gpy_hashval_t h = gpy_dd_hash_string (ident);
  gpy_hash_entry_t * f = gpy_rr_lookup_decl (h, gpy_namespace_vec);
  if (!f)
    {
      gpy_object_t * o = (gpy_object_t *)
	gpy_malloc (sizeof (gpy_object_t));
      o->T = TYPE_NULL;
      o->o.object_state = NULL;

      void ** r = gpy_dd_hash_insert (h, o, Gpy_Vec_Head (gpy_namespace_vec,
							  gpy_hash_tab_t *));
      if (r)
	fatal ("error registering decl <%s>!\n", ident);
    }
}

void gpy_rr_set_decl_val (char *ident, gpy_object_t *o)
{
  gpy_hashval_t h = gpy_dd_hash_string (ident);
  gpy_hash_entry_t * f = gpy_rr_lookup_decl (h, gpy_namespace_vec);

  if (f)
    {
      gpy_object_t * prev = f->data;
      gpy_rr_decr_ref_count (prev);
      f->data = o;
    }
  else
    {
      void ** r = gpy_dd_hash_insert (h, o, Gpy_Vec_Head (gpy_namespace_vec,
							  gpy_hash_tab_t *));
      if (r)
	fatal ("error registering decl <%s>!\n", ident);
    }
}

void gpy_rr_init_runtime (void)
{
  /* 
     Setup runtime namespace Init builtin's
  */
  gpy_rr_init_primitives ();

  gpy_namespace_vec = (gpy_vector_t*)
    gpy_malloc( sizeof(gpy_vector_t) );
  gpy_vec_init( gpy_namespace_vec );

  gpy_call_stack = (gpy_vector_t*)
    gpy_malloc(sizeof(gpy_vector_t));
  gpy_vec_init( gpy_call_stack );

  gpy_hash_tab_t * head = (gpy_hash_tab_t *)
    gpy_malloc (sizeof(gpy_hash_tab_t));
  gpy_dd_hash_init_table (&head);
  gpy_vec_push( gpy_namespace_vec, head );
}

void gpy_rr_cleanup_final( void )
{
  gpy_rr_pop_context ();

  if( gpy_namespace_vec->length > 0 )
    error( "<%i> un-free'd conexts!\n", gpy_namespace_vec->length );

  gpy_vec_free (gpy_namespace_vec);
  gpy_vec_free (gpy_primitives);
  gpy_vec_free (gpy_call_stack);
}

gpy_object_t * gpy_rr_fold_integer (int x)
{
  gpy_object_t * retval = NULL_OBJECT;

  gpy_object_t ** args = (gpy_object_t **)
    gpy_calloc(2, sizeof(gpy_object_t*));

  gpy_literal_t i;
  i.type = TYPE_INTEGER;
  i.literal.integer = x;

  gpy_object_t a1 = { .T = TYPE_OBJECT_LIT, .o.literal = &i };
  gpy_object_t a2 = { .T = TYPE_NULL, .o.literal = NULL };

  args[0] = &a1;
  args[1] = &a2;

  gpy_typedef_t * Int_def = (gpy_typedef_t *)
    gpy_primitives->vector[ 0 ];
  gpy_assert (Int_def);

  retval = Int_def->init_hook (Int_def, args);
  gpy_free(args);

  debug("initilized integer object <%p> to <%i>!\n",
	(void*)retval, x );
  gpy_assert (retval->T == TYPE_OBJECT_STATE);

  return retval;
}

/**
 * int fd: we could use bit masks to represent:
 *   stdout/stderr ...
 **/
void gpy_rr_eval_print (int fd, int count, ...)
{
  va_list vl; int idx;
  va_start (vl,count);

  gpy_object_t * it = NULL;
  for( idx = 0; idx<count; ++idx )
    {
      it = va_arg( vl, gpy_object_t* );
      gpy_assert(it->T == TYPE_OBJECT_STATE);
      struct gpy_typedef_t * definition = it->o.object_state->definition;

      switch( fd )
	{
	case 1:
	  (*definition).print_hook( it, stdout, false );
	  break;

	case 2:
	  (*definition).print_hook( it, stderr, false );
	  break;

	default:
	  fatal("invalid print file-descriptor <%i>!\n", fd );
	  break;
	}
    }

  fprintf( stdout, "\n" );
  va_end(vl);
}

inline
void gpy_rr_incr_ref_count (gpy_object_t * x1)
{
  gpy_assert( x1->T == TYPE_OBJECT_STATE );
  gpy_object_state_t * x = x1->o.object_state;

  debug("incrementing ref count on <%p>:<%i> to <%i>!\n",
	(void*) x, x->ref_count, (x->ref_count + 1) );
  x->ref_count++;
}

inline
void gpy_rr_decr_ref_count (gpy_object_t * x1)
{
  gpy_assert( x1->T == TYPE_OBJECT_STATE );
  gpy_object_state_t * x = x1->o.object_state;

  debug("decrementing ref count on <%p>:<%i> to <%i>!\n",
	(void*) x, x->ref_count, (x->ref_count - 1) );
  x->ref_count--;
}

void gpy_rr_push_context (void)
{
  gpy_hash_tab_t * head = (gpy_hash_tab_t *)
    gpy_malloc (sizeof(gpy_hash_tab_t));
  gpy_dd_hash_init_table (&head);
  
  gpy_vec_push (gpy_namespace_vec, head);
}

void gpy_rr_pop_context (void)
{
  gpy_hash_tab_t * head = Gpy_Vec_Head (gpy_namespace_vec,
					gpy_hash_tab_t *);
  gpy_hash_entry_t * array = head->array;

  unsigned int idx = 0;
  for( ; idx<(head->length); ++idx )
    {
      gpy_hash_entry_t i = array[idx];
      if (i.data)
	{
	  gpy_object_t * o = i.data;
	  gpy_rr_decr_ref_count (o);
	}
    }

  gpy_garbage_invoke_sweep (gpy_namespace_vec);

  /* Loop over for stragglers like returns which need pushed up a
     context soo they can still be garbage collected....
     --
     straggler is something which will have a (ref_count > 0) after
     this set of decreasing references...
  */
  //....

  gpy_hash_tab_t * popd = gpy_vec_pop (gpy_namespace_vec);
  gpy_free (popd);
}

void gpy_rr_finalize_block_decls (int n, ...)
{
  va_list vl; int idx;
  va_start (vl,n);

  /* gpy_object_t is a typedef of gpy_object_state_t *
     to keep stdarg.h happy
  */
  gpy_object_t * it = NULL;
  for (idx = 0; idx<n; ++idx)
    {
      it = va_arg (vl, gpy_object_t *);
      gpy_assert (it->T == TYPE_OBJECT_STATE);
      gpy_rr_decr_ref_count (it);
    }
  va_end (vl);
}

gpy_object_t * gpy_rr_fold_call (gpy_callable_t * callables,
				 const char * ident,
				 int n, ...)
{
  gpy_object_t * retval = NULL;

  

  return retval;
}

gpy_object_t * gpy_rr_eval_dot_operator (gpy_object_t * x,
					 gpy_object_t * y)
{
  return NULL;
}

gpy_object_t * gpy_rr_eval_expression (gpy_object_t * x1,
				       gpy_object_t * y1,
				       gpy_opcode_t op)
{
  char * op_str = NULL;
  gpy_object_t * retval = NULL;

  gpy_assert(x1->T == TYPE_OBJECT_STATE);
  gpy_assert(y1->T == TYPE_OBJECT_STATE);
  gpy_object_state_t * x = x1->o.object_state;
  gpy_object_state_t * y = y1->o.object_state;

  struct gpy_typedef_t * def = x1->o.object_state->definition;
  struct gpy_number_prot_t * binops = (*def).binary_protocol;
  struct gpy_number_prot_t binops_l = (*binops);

  debug ("Eval expression!\n");

  if( binops_l.init )
    {
      binary_op o = NULL;
      switch( op )
	{
	case OP_BIN_ADDITION:
	  o = binops_l.n_add;
	  op_str = "+ ";
	  break;

	default:
	  fatal("unhandled binary operation <%x>!\n", op );
	  break;
	}

#ifdef DEBUG
      x->definition->print_hook( x1, stdout, false );
      fprintf(stdout, "%s", op_str );
      y->definition->print_hook( y1, stdout, true );
#endif
      
      retval = o(x1,y1);
      
#ifdef DEBUG
      if( retval )
	{
	  fprintf(stdout, "evaluated to: ");
	  retval->o.object_state->definition->print_hook (retval, stdout, false);
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
