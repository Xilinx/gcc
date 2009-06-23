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
#ifndef GCC_GRAPHITE_PPL_H
#define GCC_GRAPHITE_PPL_H

CloogMatrix *new_Cloog_Matrix_from_ppl_Polyhedron (ppl_const_Polyhedron_t);
CloogDomain *new_Cloog_Domain_from_ppl_Polyhedron (ppl_const_Polyhedron_t);
CloogDomain * new_Cloog_Domain_from_ppl_Pointset_Powerset (
  ppl_Pointset_Powerset_NNC_Polyhedron_t);
void new_NNC_Polyhedron_from_Cloog_Matrix (ppl_Polyhedron_t *, CloogMatrix *);
void insert_constraint_into_matrix (CloogMatrix *, int, ppl_const_Constraint_t);
ppl_Polyhedron_t ppl_strip_loop (ppl_Polyhedron_t, ppl_dimension_type, int);
int ppl_lexico_compare_linear_expressions (ppl_Linear_Expression_t,
					   ppl_Linear_Expression_t);

void ppl_print_polyhedron_matrix (FILE *, ppl_const_Polyhedron_t);
void ppl_print_powerset_matrix (FILE *, ppl_Pointset_Powerset_NNC_Polyhedron_t);
void debug_ppl_polyhedron_matrix (ppl_Polyhedron_t);
void debug_ppl_powerset_matrix (ppl_Pointset_Powerset_NNC_Polyhedron_t);
void ppl_read_polyhedron_matrix (ppl_Polyhedron_t *, FILE *);
void ppl_insert_dimensions (ppl_Polyhedron_t, int, int);
void ppl_insert_dimensions_pointset (ppl_Pointset_Powerset_NNC_Polyhedron_t, int, int);
void ppl_set_inhomogeneous (ppl_Linear_Expression_t, int);
void ppl_set_coef (ppl_Linear_Expression_t, ppl_dimension_type, int);

/* Sets RES to the max of V1 and V2.  */

static inline void
value_max (Value res, Value v1, Value v2)
{
  if (value_compare (v1, v2) < 0)
    value_assign (res, v2);
  value_assign (res, v1);
}

#endif

