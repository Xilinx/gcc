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

#ifndef __PYVEC_H__
#define __PYVEC_H__

typedef struct gpy_vector_t {
  void ** vector;
  signed long size, length;
} gpy_ident_vector_t;

enum OPERATOR_T {
  LESS_OP, GREATER_OP, EQ_EQ_OP,
  GREATER_EQ_OP, LESS_EQ_OP
};

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

typedef int gpy_int;
DEF_VEC_I( gpy_int );
DEF_VEC_ALLOC_I( gpy_int,gc );

DEF_VEC_P( gpy_ident );
DEF_VEC_ALLOC_P( gpy_ident,gc );

typedef struct GTY(()) gpy_context_branch_t {
  gpy_hash_tab_t * var_decls;
  VEC(gpy_ident,gc) *var_decl_t;
} gpy_context_branch ;

enum DECL_T { VAR, FUNC };

typedef gpy_symbol_obj *gpy_sym;
typedef gpy_context_branch *gpy_ctx_t;

DEF_VEC_P( gpy_sym );
DEF_VEC_ALLOC_P( gpy_sym,gc );

DEF_VEC_P( gpy_ctx_t );
DEF_VEC_ALLOC_P( gpy_ctx_t,gc );

extern VEC(gpy_ctx_t,gc) * gpy_ctx_table;
extern VEC(gpy_sym,gc) * gpy_garbage_decls;
extern VEC(tree,gc) * global_decls;

#endif /*__PY_VEC_H__*/
