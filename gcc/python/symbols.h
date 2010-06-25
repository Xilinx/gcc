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

#ifndef __SYMBOLS_H_
#define __SYMBOLS_H__

typedef unsigned short gpy_opcode_t;

typedef struct GTY(()) gpy_symbol_table_t {
  char * identifier; gpy_opcode_t exp;
  gpy_opcode_t type, op_a_t, op_b_t;
  union {
    /* literal primitive semantic types! */
    long int integer;
    char * string;
    struct gpy_symbol_table_t * symbol_table;
  } op_a;
  union {
    long int integer;
    char * string;
    struct gpy_symbol_table_t * symbol_table;
  } op_b;
  struct gpy_symbol_table_t *next;
} gpy_symbol_obj ;

typedef struct gpy_vector_t {
  void ** vector;
  signed long size, length;
} gpy_ident_vector_t;

typedef unsigned long gpy_hashval_t;
typedef struct gpy_hash_entry {
  gpy_hashval_t hash;
  void * data;
} gpy_hash_entry_t ;

typedef struct gpy_hash_table_t {
  unsigned int size, length;
  gpy_hash_entry_t * array;
} gpy_hash_tab_t ;

typedef struct gpy_id_t {
  char * ident;
} gpy_ident_t;
typedef gpy_ident_t * gpy_ident;

DEF_VEC_P( gpy_ident );
DEF_VEC_ALLOC_P( gpy_ident,gc );

typedef struct GTY(()) gpy_context_branch_t {
  gpy_hash_tab_t * var_decls;
  VEC(gpy_ident,gc) *var_decl_t;
  gpy_hash_tab_t * fnc_decls;
  VEC(gpy_ident,gc) *fnc_decl_t;
} gpy_context_branch ;

typedef gpy_symbol_obj *gpy_sym;
typedef gpy_context_branch *gpy_ctx_t;

DEF_VEC_P( gpy_sym );
DEF_VEC_ALLOC_P( gpy_sym,gc );

DEF_VEC_P( gpy_ctx_t );
DEF_VEC_ALLOC_P( gpy_ctx_t,gc );

extern VEC(gpy_ctx_t,gc) * gpy_ctx_table;
extern VEC(gpy_sym,gc) * gpy_garbage_decls;
			 
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

extern tree gpy_process_assign( gpy_symbol_obj ** , gpy_symbol_obj ** );

extern tree gpy_process_bin_expression( gpy_symbol_obj ** , gpy_symbol_obj ** );

extern tree gpy_process_expression( const gpy_symbol_obj * const );

extern tree gpy_process_functor( const gpy_symbol_obj * const );

extern tree gpy_get_tree( gpy_symbol_obj * );

extern void gpy_process_decl( gpy_symbol_obj * );

enum DECL_T { VAR, FUNC };

extern tree gpy_ctx_lookup_decl( const char *, enum DECL_T );

extern bool gpy_ctx_push_decl( tree, const char *, gpy_context_branch *,
			       enum DECL_T );

extern void gpy_symbol_init_ctx( gpy_symbol_obj * const )
  __attribute__((nonnull));

extern void gpy_init_ctx_branch( gpy_context_branch * const * )
  __attribute__((nonnull));

#define Gpy_Symbol_Init( x )				\
  x = (gpy_symbol_obj*)					\
    xmalloc( sizeof(gpy_symbol_obj) );			\
  debug("object created at <%p>!\n", (void*)x );	\
  gpy_symbol_init_ctx( x );

#define Gpy_Mark_Garbage_obj( x )			\
  debug("marking object <%p> as garbage!\n", x );	\
  VEC_safe_push( gpy_sym,gc, gpy_garbage_decls, x );

#endif /* __SYMBOLS_H_ */
