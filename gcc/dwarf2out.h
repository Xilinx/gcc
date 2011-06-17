/* dwarf2out.h - Various declarations for functions found in dwarf2out.c
   Copyright (C) 1998, 1999, 2000, 2003, 2007, 2010, 2011
   Free Software Foundation, Inc.

This file is part of GCC.

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

struct dw_cfi_struct;
/* In dwarf2out.c */
/* Interface of the DWARF2 unwind info support.  */

/* Generate a new label for the CFI info to refer to.  */

extern void dwarf2out_maybe_emit_cfi_label (void);

/* Entry point to update the canonical frame address (CFA).  */

extern void dwarf2out_def_cfa (unsigned, HOST_WIDE_INT);

/* Add the CFI for saving a register window.  */

extern void dwarf2out_window_save (void);

/* Entry point for saving a register to the stack.  */

extern void dwarf2out_reg_save (unsigned, HOST_WIDE_INT);

/* Entry point for saving the return address in the stack.  */

extern void dwarf2out_return_save (HOST_WIDE_INT);

/* Entry point for saving the return address in a register.  */

extern void dwarf2out_return_reg (unsigned);

/* Entry point for saving the first register into the second.  */

extern void dwarf2out_reg_save_reg (rtx, rtx);

extern void dwarf2out_decl (tree);
extern void dwarf2out_emit_cfi (struct dw_cfi_struct *);

extern void debug_dwarf (void);
struct die_struct;
extern void debug_dwarf_die (struct die_struct *);
extern void dwarf2out_set_demangle_name_func (const char *(*) (const char *));
#ifdef VMS_DEBUGGING_INFO
extern void dwarf2out_vms_debug_main_pointer (void);
#endif

struct array_descr_info
{
  int ndimensions;
  tree element_type;
  tree base_decl;
  tree data_location;
  tree allocated;
  tree associated;
  struct array_descr_dimen
    {
      tree lower_bound;
      tree upper_bound;
      tree stride;
    } dimen[10];
};
