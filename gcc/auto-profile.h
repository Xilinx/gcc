/* auto-profile.h - Defines data exported from auto-profile.c
   Copyright (C) 2012. Free Software Foundation, Inc.
   Contributed by Dehao Chen (dehao@google.com)

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

#ifndef AUTO_PROFILE_H
#define AUTO_PROFILE_H

/* Read, process, finalize AutoFDO data structures.  */
extern void init_auto_profile (void);
extern void end_auto_profile (void);
extern void process_auto_profile (void);

/* Annotate function's count and total count.  */
extern void afdo_set_current_function_count (void);

/* Add the assembly_name to bfd_name mapping.  */
extern void afdo_add_bfd_name_mapping (const char *, const char *);

/* Add copy scale for an inlined edge to stack_scale_map.  */
extern void afdo_add_copy_scale (struct cgraph_edge *);

/* Calculate branch probability in both AutoFDO pass and after inlining.  */
extern void afdo_calculate_branch_prob (void);

/* Calculate total sample count of an inlined callsite.  */
extern bool afdo_get_callsite_count (struct cgraph_edge *, gcov_type *,
				     gcov_type *, bool);

/* Calculate basic block count.  */
extern gcov_type afdo_get_bb_count (basic_block);
#endif /* AUTO_PROFILE_H */
