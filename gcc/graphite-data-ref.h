/* Graphite polyhedral data dependence analysis.
   Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Konrad Trifunovic <konrad.trifunovic@inria.fr>.

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

/* Associate a POLYHEDRON dependence description to two data
   references A and B.  */

#ifndef GCC_GRAPHITE_DATA_REF_H
#define GCC_GRAPHITE_DATA_REF_H

struct data_dependence_polyhedron
{
  struct data_reference *a;
  struct data_reference *b;
  bool reversed_p;
  bool loop_carried;
  signed level;
  CloogDomain *polyhedron;  
};

#define RDGE_DDP(E)   ((struct data_dependence_polyhedron*) ((E)->data))

typedef struct data_dependence_polyhedron *ddp_p;

DEF_VEC_P(ddp_p);
DEF_VEC_ALLOC_P(ddp_p,heap);

extern void graphite_dump_dependence_graph (FILE *, struct graph *);
extern struct graph *graphite_build_rdg_all_levels (scop_p);
extern struct data_dependence_polyhedron *
graphite_test_dependence (scop_p scop, poly_bb_p pbb1, poly_bb_p pbb2,
                          struct data_reference *a, struct data_reference *b);

#endif
