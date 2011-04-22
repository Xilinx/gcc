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

#ifndef __PY_TREE_H__
#define __PY_TREE_H__

extern VEC(tree,gc) * gpy_decl_process_assign (gpy_symbol_obj ** , gpy_symbol_obj **,
					       VEC(gpy_ctx_t,gc) *);
extern VEC(tree,gc) * gpy_decl_process_bin_expression (gpy_symbol_obj ** , gpy_symbol_obj **,
						       gpy_opcode_t, VEC(gpy_ctx_t,gc) *);
					
extern VEC(tree,gc) * gpy_decl_fold_primitive (const gpy_symbol_obj * const);

extern VEC(tree,gc) * gpy_stmt_get_tree_1 (gpy_symbol_obj *, VEC(gpy_ctx_t,gc) *);
extern VEC(tree,gc) * gpy_stmt_get_tree_2 (gpy_symbol_obj *, tree, VEC(gpy_ctx_t,gc) *);

extern VEC(tree,gc) * gpy_stmt_process_expression (const gpy_symbol_obj * const,
						   VEC(gpy_ctx_t,gc) *);

extern VEC(tree,gc) * gpy_stmt_process_print (gpy_symbol_obj *, VEC(gpy_ctx_t,gc) *);

extern tree gpy_stmt_process_functor_1 (gpy_symbol_obj * const, const char *,
					VEC(gpy_ctx_t,gc) *);

extern tree gpy_stmt_process_functor_2 (gpy_symbol_obj * const, const char *,
					tree, VEC(gpy_ctx_t,gc) *);

extern void gpy_stmt_process_decl (gpy_symbol_obj * const);

extern tree gpy_ctx_lookup_decl (VEC(gpy_ctx_t,gc) *, const char *);

extern bool gpy_ctx_push_decl (tree, const char *, gpy_hash_tab_t *);
	    
#define GPY_VEC_stmts_append(x,y)			\
  do {							\
    int x_ = 0; tree t_ = NULL_TREE;			\
    for (; VEC_iterate(tree,y,x_,t_); ++x_)		\
      {							\
        VEC_safe_push(tree,gc,x,t_);			\
      }							\
  } while (0);						\

#endif //__PYPY_TREE_H__
