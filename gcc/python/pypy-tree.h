/* This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>. */

#ifndef __PYPY_TREE_H__
#define __PYPY_TREE_H__

extern void gpy_ident_vec_init( gpy_ident_vector_t * const v )
     __attribute__((nonnull));

extern void gpy_ident_vec_push( gpy_ident_vector_t * const v, void * s )
     __attribute__((nonnull));

extern void * gpy_ident_vec_pop( gpy_ident_vector_t * const v )
     __attribute__((nonnull));

extern gpy_hashval_t gpy_dd_hash_string( const char * );

extern gpy_hash_entry_t * gpy_dd_hash_lookup_table( gpy_hash_tab_t *, gpy_hashval_t );

extern void ** gpy_dd_hash_insert( gpy_hashval_t, void *, gpy_hash_tab_t * );

extern void gpy_dd_hash_grow_table( gpy_hash_tab_t * );

extern void gpy_dd_hash_init_table( gpy_hash_tab_t ** );

extern void gpy_gg_invoke_garbage( void );

extern void gpy_garbage_free_obj( gpy_symbol_obj ** );

/* expression.c */
extern VEC(tree,gc) * gpy_process_assign( gpy_symbol_obj ** , gpy_symbol_obj ** );

extern VEC(tree,gc) * gpy_process_bin_expression( gpy_symbol_obj ** , gpy_symbol_obj **,
						  gpy_opcode_t );

extern VEC(tree,gc) * gpy_fold_primitive( const gpy_symbol_obj * const );

/* ------- */

extern VEC(tree,gc) * gpy_process_expression( const gpy_symbol_obj * const );

extern VEC(tree,gc) * gpy_process_class( gpy_symbol_obj * const );

extern VEC(tree,gc) * gpy_process_functor( const gpy_symbol_obj * const, const char * );

extern VEC(tree,gc) * gpy_get_tree( gpy_symbol_obj * );

extern VEC(tree,gc) * gpy_process_print( gpy_symbol_obj * );
/* ------- */

extern tree gpy_main_method_decl( VEC(tree,gc) * , gpy_context_branch * );

extern tree gpy_ctx_lookup_decl( const char *, enum DECL_T );

extern void gpy_process_decl( gpy_symbol_obj * );

extern bool gpy_ctx_push_decl( tree, const char *, gpy_context_branch *,
			       enum DECL_T );

extern gpy_symbol_obj * gpy_symbol_obj_clone( gpy_symbol_obj *, bool );

extern void gpy_symbol_init_ctx( gpy_symbol_obj * const )
  __attribute__((nonnull));

extern void gpy_init_ctx_branch( gpy_context_branch * const * )
  __attribute__((nonnull));

#endif //__PYPY_TREE_H__
