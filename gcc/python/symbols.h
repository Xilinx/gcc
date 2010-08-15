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
  location_t loc;
  union {
    /* literal primitive semantic types! */
    int integer;
    char * string;
    bool boolean;
    struct gpy_symbol_table_t * symbol_table;
  } op_a;
  union {
    int integer;
    char * string;
    bool boolean;
    struct gpy_symbol_table_t * symbol_table;
  } op_b;
  struct gpy_symbol_table_t *next;
} gpy_symbol_obj ;

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
  gpy_hash_tab_t * fnc_decls;
  VEC(gpy_ident,gc) *fnc_decl_t;
  VEC(tree,gc) * ctx_init;
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

#define Gpy_Symbol_Init_Ctx( x )		\
  x->identifier = NULL;				\
  x->exp = TYPE_SYMBOL_NIL;			\
  x->type = SYMBOL_PRIMARY;			\
  x->loc = UNKNOWN_LOCATION;			\
  x->op_a_t = TYPE_SYMBOL_NIL;			\
  x->op_b_t = TYPE_SYMBOL_NIL;			\
  x->op_a.symbol_table = NULL;			\
  x->op_b.symbol_table = NULL;			\
  x->next = NULL;

#define Gpy_Symbol_Init( x )				\
  x = (gpy_symbol_obj*)					\
    xmalloc( sizeof(gpy_symbol_obj) );			\
  debug("object created at <%p>!\n", (void*)x );	\
  Gpy_Symbol_Init_Ctx( x );

#define Gpy_Mark_Garbage_obj( x )			\
  debug("marking object <%p> as garbage!\n", x );	\
  VEC_safe_push( gpy_sym,gc, gpy_garbage_decls, x );

#endif /* __SYMBOLS_H_ */
