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

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

extern VEC(tree,gc) * gpy_builtin_get_init_call (void);

extern VEC(tree,gc) * gpy_builtin_get_cleanup_final_call (void);

extern VEC(tree,gc) * gpy_builtin_get_push_context_call (void);

extern VEC(tree,gc) * gpy_builtin_get_pop_context_call (void);

extern VEC(tree,gc) * gpy_builtin_get_fold_int_call (int);

extern VEC(tree,gc) * gpy_builtin_get_eval_accessor_call (tree, tree);

extern VEC(tree,gc) * gpy_builtin_get_incr_ref_call (tree);

extern VEC(tree,gc) * gpy_builtin_get_decr_ref_call (tree);

extern VEC(tree,gc) * gpy_builtin_get_set_callable_call (tree);

extern VEC(tree,gc) * gpy_builtin_get_print_call (int, tree *);

extern VEC(tree,gc) * gpy_builtin_get_finalize_block_call (int, tree *);

extern VEC(tree,gc) * gpy_builtin_get_eval_expression_call (tree, tree, gpy_opcode_t);

extern VEC(tree,gc) * gpy_builtin_get_fold_call_call (const char *, int, tree *);

extern VEC(tree,gc) * gpy_builtin_get_set_decl_call (tree);

extern VEC(tree,gc) * gpy_builtin_get_register_decl_call (tree);

extern VEC(tree,gc) * gpy_builtin_get_register_callable_call (tree, int);

#endif /* __RUNTIME_H__ */
