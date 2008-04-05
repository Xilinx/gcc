/* Bounds checking.
   Copyright (C) 2007 Free Software Foundation, Inc.
   Contributed by Alexander Lamaison <awl03@doc.ic.ac.uk>
   based on Mudflap.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#ifndef TREE_BOUNDS_H
#define TREE_BOUNDS_H

/* Instrumentation.  */
extern void bounds_init (void);
extern void bounds_enqueue_decl (tree);
extern void bounds_enqueue_constant (tree);
extern void bounds_finish_file (void);

/* Tree node marking.  */
extern int bounds_marked_seen_p (tree);
extern tree bounds_mark_seen (tree);

#endif /* TREE_BOUNDS_H */
