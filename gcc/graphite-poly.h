/* Graphite polyhedral representation.
   Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@amd.com> and
   Tobias Grosser <grosser@fim.uni-passau.de>.

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

#ifndef GCC_GRAPHITE_POLY_H
#define GCC_GRAPHITE_POLY_H

typedef struct poly_bb *poly_bb_p;
DEF_VEC_P(poly_bb_p);
DEF_VEC_ALLOC_P (poly_bb_p, heap);

typedef struct scop *scop_p;
DEF_VEC_P(scop_p);
DEF_VEC_ALLOC_P (scop_p, heap);

/* POLY_BB represents a blackbox in the polyhedral model.  */

struct poly_bb 
{
  gimple_bb_p black_box;

  scop_p scop;

  /* The static schedule contains the textual order for every loop layer.
    
     Example:

     S0
     for (i ...)
       {
         S1
         for (j ...)
           {
             S2
             S3
           }
         S4
       }
     S5
     for (k ...)
       {
         S6
         S7
         for (l ...)
           {
             S8
           }
         S9
       }
     S10

     Schedules:
  
        | Depth       
     BB | 0  1  2 
     ------------
     S0 | 0
     S1 | 1, 0
     S2 | 1, 1, 0
     S3 | 1, 1, 1
     S4 | 1, 2
     S5 | 2
     S6 | 3, 0
     S7 | 3, 1
     S8 | 3, 2, 0
     S9 | 3, 3
     S10| 4

   Normalization rules:
     - One SCoP can never contain two bbs with the same schedule timestamp.
     - All bbs at the same loop depth have a consecutive ordering (no gaps). */
  ppl_Linear_Expression_t static_schedule;

  /* The iteration domain of this bb.
     Example:

     for (i = a - 7*b + 8; i <= 3*a + 13*b + 20; i++)
       for (j = 2; j <= 2*i + 5; j++)
         for (k = 0; k <= 5; k++)
           S (i,j,k)

     Loop iterators: i, j, k 
     Parameters: a, b
      
     | i >=  a -  7b +  8
     | i <= 3a + 13b + 20
     | j >= 2
     | j <= 2i + 5
     | k >= 0 
     | k <= 5

     The number of variables in the DOMAIN may change and is not
     related to the number of loops in the original code.  */
  ppl_Polyhedron_t domain;

  /* LOOPS contains for every column in the graphite domain the corresponding
     gimple loop.  If there exists no corresponding gimple loop LOOPS contains
     NULL. 
  
     Example:

     Original code:

     for (i = 0; i <= 20; i++) 
       for (j = 5; j <= 10; j++)
         A

     Original domain:

     |  i >= 0
     |  i <= 20
     |  j >= 0
     |  j <= 10

     This is a two dimensional domain with "Loop i" represented in
     dimension 0, and "Loop j" represented in dimension 1.  Original
     loops vector:

     | 0         1 
     | Loop i    Loop j

     After some changes (Exchange i and j, strip-mine i), the domain
     is:

     |  i >= 0
     |  i <= 20
     |  j >= 0
     |  j <= 10
     |  ii <= i
     |  ii + 1 >= i 
     |  ii <= 2k
     |  ii >= 2k 

     Iterator vector:
     | 0        1        2         3
     | Loop j   NULL     Loop i    NULL
    
     Means the original loop i is now on dimension 2 of the domain and
     loop j in the original loop nest is now on dimension 0.
     Dimensions 1 and 3 represent the newly created loops.  */
  VEC (loop_p, heap) *loops;

};

#define PBB_SCOP(PBB) PBB->scop
#define PBB_STATIC_SCHEDULE(PBB) PBB->static_schedule
#define PBB_DOMAIN(PBB) PBB->domain
#define PBB_BLACK_BOX(PBB) PBB->black_box
#define PBB_LOOPS(PBB) PBB->loops

void new_poly_bb (scop_p, gimple_bb_p);
void free_poly_bb (poly_bb_p);
extern void debug_loop_vec (poly_bb_p);

static inline unsigned scop_nb_params (scop_p);

/* Calculate the number of loops around GB in the current SCOP.  Only
   works if GBB_DOMAIN is built.  */

static inline int
pbb_nb_loops (const struct poly_bb *pbb)
{
  scop_p scop = PBB_SCOP (pbb);
  ppl_dimension_type dim;

  ppl_Polyhedron_space_dimension (PBB_DOMAIN (pbb), &dim);
  return dim - scop_nb_params (scop);
}

/* Returns the gimple loop, that corresponds to the loop_iterator_INDEX.  
   If there is no corresponding gimple loop, we return NULL.  */

static inline loop_p
pbb_loop_at_index (poly_bb_p pbb, int index)
{
  return VEC_index (loop_p, PBB_LOOPS (pbb), index);
}

/* Returns the index of LOOP in the loop nest around GB.  */

static inline int
pbb_loop_index (poly_bb_p pbb, loop_p loop)
{
  int i;
  loop_p l;

  for (i = 0; VEC_iterate (loop_p, PBB_LOOPS (pbb), i, l); i++)
    if (loop == l)
      return i;

  gcc_unreachable();
}

/* A SCOP is a Static Control Part of the program, simple enough to be
   represented in polyhedral form.  */
struct scop
{
  /* A SCOP is defined as a SESE region.  */
  sese region;

  /* All the basic blocks in this scop that contain memory references
     and that will be represented as statements in the polyhedral
     representation.  */
  VEC (poly_bb_p, heap) *bbs;

  /* Data dependence graph for this SCoP.  */
  struct graph *dep_graph;
};

#define SCOP_BBS(S) S->bbs
#define SCOP_REGION(S) S->region
/* SCOP_ENTRY bb dominates all the bbs of the scop.  SCOP_EXIT bb
   post-dominates all the bbs of the scop.  SCOP_EXIT potentially
   contains non affine data accesses, side effect statements or
   difficult constructs, and thus is not considered part of the scop,
   but just a boundary.  SCOP_ENTRY is considered part of the scop.  */
#define SCOP_ENTRY(S) (SESE_ENTRY (SCOP_REGION (S))->dest)
#define SCOP_EXIT(S) (SESE_EXIT (SCOP_REGION (S))->dest)
#define SCOP_REGION_BBS(S) (SESE_REGION_BBS (SCOP_REGION (S)))
#define SCOP_DEP_GRAPH(S) (S->dep_graph)
#define SCOP_PARAMS(S) (SCOP_REGION (S)->params)
#define SCOP_LOOP_NEST(S) (SCOP_REGION (S)->loop_nest)
#define SCOP_PARAMS(S) (SCOP_REGION (S)->params)
#define SCOP_OLDIVS(S) (SCOP_REGION (S)->old_ivs)
#define SCOP_LIVEOUT_RENAMES(S) (SCOP_REGION (S)->liveout_renames)

/* Returns the number of parameters for SCOP.  */

static inline unsigned
scop_nb_params (scop_p scop)
{
  return sese_nb_params (SCOP_REGION (scop));
}

extern scop_p new_scop (sese);
extern void free_scop (scop_p);
extern void dot_all_scops (void);
extern void print_generated_program (FILE *, scop_p);
extern void debug_generated_program (scop_p);
extern int scop_max_loop_depth (scop_p);

bool graphite_apply_transformations (scop_p);

#endif
