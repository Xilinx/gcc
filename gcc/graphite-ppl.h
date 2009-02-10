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

CloogMatrix *
new_Cloog_Matrix_from_ppl_Constraint_System (ppl_Constraint_System_t);
void new_Constraint_System_from_Cloog_Matrix (ppl_Constraint_System_t *,
					      CloogMatrix *);
ppl_Constraint_System_t ppl_move_dimension (ppl_Constraint_System_t,
					    ppl_dimension_type,
					    ppl_dimension_type);

