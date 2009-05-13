/* Translation of CLAST (CLooG AST) to Gimple.
   Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@amd.com>.

This file is part of GCC.

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
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_GRAPHITE_CLAST_TO_GIMPLE_H
#define GCC_GRAPHITE_CLAST_TO_GIMPLE_H

/* Data structure for CLooG program representation.  */

typedef struct cloog_prog_clast {
  CloogProgram *prog;
  struct clast_stmt *stmt;
} cloog_prog_clast;

extern bool gloog (scop_p);
extern cloog_prog_clast scop_to_clast (scop_p);
extern void debug_clast_stmt (struct clast_stmt *);
extern void debug_clast_name_indexes (htab_t);


#endif
