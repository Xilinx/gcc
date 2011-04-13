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

#ifndef __PY_TYPES_H__
#define __PY_TYPES_H__

extern VEC(tree,gc) * gpy_builtin_types_vec;

#define gpy_object_type_ptr     VEC_index(tree,gpy_builtin_types_vec,0)

#define gpy_object_type_ptr_ptr VEC_index(tree,gpy_builtin_types_vec,1)

#define gpy_const_char_ptr      VEC_index(tree,gpy_builtin_types_vec,4)

extern tree gpy_init_callable_record (tree, int, tree);

extern void gpy_initilize_types (void);

#endif /* __PY_TYPES_H__ */
