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

extern tree gpy_builtin_get_init_call (void);
extern tree gpy_builtin_get_cleanup_final_call (void);
extern tree gpy_builtin_get_fold_int_call (int);

extern tree gpy_builtin_get_incr_ref_call (tree);
extern tree gpy_builtin_get_decr_ref_call (tree);

extern tree gpy_builtin_get_print_call (int, tree *);
extern tree gpy_builtin_get_eval_expression_call (tree, tree, 
						  opcode_t);

#endif /* __RUNTIME_H__ */
