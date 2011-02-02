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

#ifndef __GCC_GARBAGE_H__
#define __GCC_GARBAGE_H__

extern gpy_vector_t * gpy_primitives;
extern gpy_vector_t * gpy_namespace_vec;
extern gpy_vector_t * gpy_garbage_vec;

extern void gpy_rr_cleanup_final( void );

extern void gpy_rr_pop_context( void );

extern void gpy_rr_push_context( void );

extern void gpy_garbage_inoke( void );

extern void gpy_garbage_mark_obj__( gpy_object_t * const );

extern void gpy_garbage_invoke_sweep( gpy_vector_t * const );

extern void gpy_garbage_free_obj( gpy_object_t * );

#define gpy_garbage_mark_obj( x )			\
  gpy_assert( x );					\
  debug("marking garbage <%p> ref count <%i>!\n",	\
	(void*) x, x->ref_count );			\
  gpy_garbage_mark_obj__( x );

#endif //__GCC_GARBAGE_H__
