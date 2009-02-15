/* Gimple Represented as Polyhedra.
   Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@inria.fr>
   and Tobias Grosser <grosser@fim.uni-passau.de>.

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

#include <ppl_c.h>
#include <cloog/cloog.h>

CloogMatrix *new_Cloog_Matrix_from_ppl_Polyhedron (ppl_const_Polyhedron_t);
void new_NNC_Polyhedron_from_Cloog_Matrix (ppl_Polyhedron_t *, CloogMatrix *);
void insert_constraint_into_matrix (CloogMatrix *, int, ppl_const_Constraint_t);
ppl_Polyhedron_t ppl_strip_loop (ppl_Polyhedron_t, ppl_dimension_type, int);
