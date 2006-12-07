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

typedef struct scop *scop_p;
DEF_VEC_P(scop_p);
DEF_VEC_ALLOC_P (scop_p, heap);

struct graphite_bb
{
  basic_block bb;

  lambda_vector static_schedule;
  VEC (lambda_vector, heap) *iteration_domain;
  VEC (data_reference_p, heap) *data_refs;
};

#define GBB_BB(GBB) GBB->bb
#define GBB_SCOP(GBB) GBB->scop
#define GBB_STATIC_SCHEDULE(GBB) GBB->static_schedule
#define GBB_DOMAIN(GBB) GBB->iteration_domain
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
  int nb_params;
  VEC (tree, heap) *params;

  /* Loops contained in the scop.  */
  int nb_loops;
  VEC (loop_p, heap) *loop_nest;

  int dim_domain;
  VEC (lambda_vector, heap) *iteration_domain;
};

#define SCOP_BBS(S) S->bbs
#define SCOP_ENTRY(S) S->entry
#define SCOP_EXIT(S) S->exit
#define SCOP_STATIC_SCHEDULE(S) S->static_schedule
#define SCOP_LOOP_NEST(S) S->loop_nest
#define SCOP_NB_LOOPS(S) S->nb_loops
#define SCOP_PARAMS(S) S->params
#define SCOP_NB_PARAMS(S) S->nb_params
#define SCOP_DIM_DOMAIN(S) S->dim_domain
#define SCOP_DOMAIN(S) S->iteration_domain

extern void debug_scop (scop_p, int);
extern void debug_scops (int);
extern void print_graphite_bb (FILE *, graphite_bb_p, int, int);

#define SCOP_STMTS(S) (S)->stmts
