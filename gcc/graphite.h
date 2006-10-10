/* Gimple Represented as Polyhedra.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Contributed by Alexandru Plesco <shurikx@gmail.com>
   and Sebastian Pop <pop@cri.ensmp.fr>.

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


/* FIXME: get code from polyhedron.c from autovect branch.  */
struct polyhedron
{
  int foo;
};

/* A SCoP is a Static Control Part of the program, simple enough to be
   represented in polyhedral form.  */
struct scop
{
  basic_block entry, exit;
};

typedef struct scop *scop_p;
DEF_VEC_P(scop_p);
DEF_VEC_ALLOC_P (scop_p, heap);

extern void debug_scop (scop_p);
extern void debug_scops (void);

#define SCOP_STMTS(S) (S)->stmts
