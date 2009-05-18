/* Copyright (C) 2009. Free Software Foundation, Inc.
   Contributed by Xinliang David Li (davidxl@google.com) and
                  Raksit Ashok  (raksit@google.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_L_IPO_H
#define GCC_L_IPO_H

/* Primary module's id (non-zero). If no module-info was read in, this will
   be zero.  */
extern unsigned primary_module_id;

#define L_IPO_COMP_MODE (primary_module_id != 0)
#define RESET_L_IPO_COMP_MODE primary_module_id = 0;
#define IS_PRIMARY_MODULE (current_module_id == primary_module_id)
#define IS_AUXILIARY_MODULE (L_IPO_COMP_MODE && current_module_id \
                             && current_module_id != primary_module_id)

/* Current module id.  */
extern unsigned current_module_id;
extern struct gcov_module_info **module_infos;
extern int is_last_module (unsigned mod_id);

extern unsigned num_in_fnames;
extern int at_eof;
extern bool parsing_start;

void push_module_scope (void);
void pop_module_scope (void);
tree lipo_save_decl (tree src);
void lipo_restore_decl (tree, tree);
void add_decl_to_current_module_scope (tree decl, void *b);
int cmp_type_arg (tree t1, tree t2);
tree get_type_or_decl_name (tree);
int equivalent_struct_types_for_tbaa (tree t1, tree t2);

#endif
