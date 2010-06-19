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

typedef struct GTY(()) gpy_context_branch_t {
  htab_t var_decls;
  htab_t fnc_decls;
} gpy_context_branch ;

typedef gpy_symbol_obj *gpy_sym;
typedef gpy_context_branch *gpy_ctx_t;

DEF_VEC_P( gpy_ctx_t );
DEF_VEC_ALLOC_P( gpy_ctx_t,gc );
extern VEC_gpy_ctx_t_gc * gpy_ctx_table;

extern tree gpy_process_assign( gpy_symbol_obj ** , gpy_symbol_obj ** );

extern tree gpy_process_bin_expression( gpy_symbol_obj ** , gpy_symbol_obj ** );

extern tree gpy_process_expression( const gpy_symbol_obj * const );

extern tree gpy_process_functor( const gpy_symbol_obj * const );

extern tree gpy_get_tree( gpy_symbol_obj * );

extern void gpy_process_decl( gpy_symbol_obj * );

extern bool gpy_ctx_lookup_var_decl( const char * );

extern bool gpy_ctx_lookup_func_decl( const char * );

extern bool gpy_ctx_push_decl( tree, const char *, htab_t );

extern void gpy_symbol_init_ctx( gpy_symbol_obj * const )
  __attribute__((nonnull));

extern void gpy_init_ctx_branch( gpy_context_branch * const * )
  __attribute__((nonnull));

#define Gpy_Symbol_Init( x )				\
  x = (gpy_symbol_obj*)					\
    xmalloc( sizeof(gpy_symbol_obj) );			\
  debug("object created at <%p>!\n", (void*)x );	\
  gpy_symbol_init_ctx( x );

#endif /* __SYMBOLS_H_ */
