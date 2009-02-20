/* Conversion of SESE regions to Polyhedra.
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

#ifndef GCC_GRAPHITE_SESE_TO_POLY_H
#define GCC_GRAPHITE_SESE_TO_POLY_H

extern void build_bb_loops (scop_p);
extern void build_sese_conditions_1 (VEC (gimple, heap) **,
				     VEC (gimple, heap) **,
				     basic_block, sese);
extern bool scop_contains_non_iv_scalar_phi_nodes (scop_p);
extern void build_sese_conditions (sese);
extern void find_scop_parameters (scop_p);
extern void build_scop_iteration_domain (scop_p);
extern void add_conditions_to_constraints (scop_p);
extern void build_scop_canonical_schedules (scop_p);
extern void build_scop_data_accesses (scop_p);

#endif
