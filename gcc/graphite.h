/* Gimple Represented as Polyhedra.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@inria.fr>.

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

#include "tree-data-ref.h"

typedef struct graphite_bb *graphite_bb_p;
DEF_VEC_P(graphite_bb_p);
DEF_VEC_ALLOC_P (graphite_bb_p, heap);

DEF_VEC_P(scop_p);
DEF_VEC_ALLOC_P (scop_p, heap);

struct graphite_bb
{
  basic_block bb;
  scop_p scop;

  lambda_vector static_schedule;
  VEC (data_reference_p, heap) *data_refs;
};

#define GBB_BB(GBB) GBB->bb
#define GBB_SCOP(GBB) GBB->scop
#define GBB_STATIC_SCHEDULE(GBB) GBB->static_schedule
#define GBB_DATA_REFS(GBB) GBB->data_refs


/* A SCoP is a Static Control Part of the program, simple enough to be
   represented in polyhedral form.  */
struct scop
{
  /* The entry bb dominates all the bbs of the scop.  The exit bb
     post-dominates all the bbs of the scop.  These entry and exit bbs
     potentially contain non affine data accesses, side effect
     statements or difficult constructs, and thus they are not
     considered part of the scop, but just boundaries.  */
  basic_block entry, exit;
  VEC (graphite_bb_p, heap) *bbs;

  lambda_vector static_schedule;

  /* Parameters used within the SCOP.  */
  VEC (tree, heap) *params;

  /* Loops contained in the scop.  */
  VEC (loop_p, heap) *loop_nest;

  VEC (lambda_vector, heap) *iteration_domain;
};

#define SCOP_BBS(S) S->bbs
#define SCOP_ENTRY(S) S->entry
#define SCOP_EXIT(S) S->exit
#define SCOP_STATIC_SCHEDULE(S) S->static_schedule
#define SCOP_LOOP_NEST(S) S->loop_nest
#define SCOP_PARAMS(S) S->params

#define SCOP_DOMAIN(S) S->iteration_domain

extern void debug_scop (scop_p, int);
extern void debug_scops (int);
extern void print_graphite_bb (FILE *, graphite_bb_p, int, int);

/* Return the number of parameters used in SCOP.  */

static inline int
scop_nb_params (scop_p scop)
{
  return VEC_length (tree, SCOP_PARAMS (scop));
}

/* Return the number of loops contained in SCOP.  */

static inline int
scop_nb_loops (scop_p scop)
{
  return VEC_length (loop_p, SCOP_LOOP_NEST (scop));
}

/* Return the dimension of the domains for SCOP.  */

static inline int
scop_dim_domain (scop_p scop)
{
  return scop_nb_loops (scop) + scop_nb_params (scop) + 1;
}

/* Return the dimension of the domains for GB.  */

static inline int
gbb_dim_domain (graphite_bb_p gb)
{
  return scop_dim_domain (GBB_SCOP (gb));
}

/* Returns the index of LOOP in the domain matrix for the SCOP.  */

static inline int
scop_loop_index (scop_p scop, struct loop *loop)
{
  unsigned i;
  struct loop *l;

  for (i = 0; VEC_iterate (loop_p, SCOP_LOOP_NEST (scop), i, l); i++)
    if (l == loop)
      return i;

  gcc_unreachable ();
  return -1;
}

