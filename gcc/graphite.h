/* Gimple Represented as Polyhedra.
   Copyright (C) 2006, 2007, 2008  Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@inria.fr>.

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

#ifndef GCC_GRAPHITE_H
#define GCC_GRAPHITE_H

#include "graphite-ppl.h"
#include "tree-data-ref.h"
#include "sese.h"

int ref_nb_loops (data_reference_p);

typedef struct graphite_bb *graphite_bb_p;
DEF_VEC_P(graphite_bb_p);
DEF_VEC_ALLOC_P (graphite_bb_p, heap);

DEF_VEC_P(scop_p);
DEF_VEC_ALLOC_P (scop_p, heap);

static inline unsigned scop_nb_params (scop_p scop);

typedef struct graphite_bb
{
  basic_block bb;
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

  /* Lists containing the restrictions of the conditional statements
     dominating this bb.  This bb can only be executed, if all conditions
     are true.
 
     Example:
 
     for (i = 0; i <= 20; i++)
     {
       A
 
       if (2i <= 8)
         B
     }
 
     So for B there is an additional condition (2i <= 8).
 
     TODO: Add these restrictions to the domain matrix.
      
     List of COND_EXPR and SWITCH_EXPR.  A COND_EXPR is true only if the
     corresponding element in CONDITION_CASES is not NULL_TREE.  For a
     SWITCH_EXPR the corresponding element in CONDITION_CASES is a
     CASE_LABEL_EXPR.  */
  VEC (gimple, heap) *conditions;
  VEC (gimple, heap) *condition_cases;

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

  VEC (data_reference_p, heap) *data_refs;
  htab_t cloog_iv_types;
} *gbb_p;

#define GBB_BB(GBB) GBB->bb
#define GBB_SCOP(GBB) GBB->scop
#define GBB_STATIC_SCHEDULE(GBB) GBB->static_schedule
#define GBB_DATA_REFS(GBB) GBB->data_refs
#define GBB_DOMAIN(GBB) GBB->domain
#define GBB_CONDITIONS(GBB) GBB->conditions
#define GBB_CONDITION_CASES(GBB) GBB->condition_cases
#define GBB_LOOPS(GBB) GBB->loops
#define GBB_CLOOG_IV_TYPES(GBB) GBB->cloog_iv_types

/* Return the loop that contains the basic block GBB.  */

static inline struct loop *
gbb_loop (struct graphite_bb *gbb)
{
  return GBB_BB (gbb)->loop_father;
}

int nb_loops_around_gbb (graphite_bb_p);

/* Calculate the number of loops around GB in the current SCOP.  Only
   works if GBB_DOMAIN is built.  */

static inline int
gbb_nb_loops (const struct graphite_bb *gb)
{
  scop_p scop = GBB_SCOP (gb);
  ppl_dimension_type dim;

  ppl_Polyhedron_space_dimension (GBB_DOMAIN (gb), &dim);
  return dim - scop_nb_params (scop);
}

/* Returns the gimple loop, that corresponds to the loop_iterator_INDEX.  
   If there is no corresponding gimple loop, we return NULL.  */

static inline loop_p
gbb_loop_at_index (graphite_bb_p gb, int index)
{
  return VEC_index (loop_p, GBB_LOOPS (gb), index);
}

/* Returns the index of LOOP in the loop nest around GB.  */

static inline int
gbb_loop_index (graphite_bb_p gb, loop_p loop)
{
  int i;
  loop_p l;

  for (i = 0; VEC_iterate (loop_p, GBB_LOOPS (gb), i, l); i++)
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
  VEC (graphite_bb_p, heap) *bbs;

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

extern void debug_scop (scop_p, int);
extern void debug_scops (int);
extern void print_graphite_bb (FILE *, graphite_bb_p, int, int);
extern void debug_gbb (graphite_bb_p, int);
extern void dot_scop (scop_p);
extern void dot_all_scops (void);
extern void debug_clast_stmt (struct clast_stmt *);
extern void debug_rename_map (htab_t);
extern void debug_ivtype_map (htab_t);
extern void debug_loop_vec (graphite_bb_p);
extern void debug_oldivs (scop_p);
extern void print_generated_program (FILE *, scop_p);
extern void debug_generated_program (scop_p);

/* Describes the type of an iv stack entry.  */
typedef enum {
  iv_stack_entry_unknown = 0,
  iv_stack_entry_iv,
  iv_stack_entry_const
} iv_stack_entry_kind;

/* Data contained in an iv stack entry.  */
typedef union iv_stack_entry_data_union
{
  name_tree iv;
  tree constant;
} iv_stack_entry_data;

/* Datatype for loop iv stack entry.  */
typedef struct iv_stack_entry_struct
{
  iv_stack_entry_kind kind;
  iv_stack_entry_data data;
} iv_stack_entry;

typedef iv_stack_entry *iv_stack_entry_p;

DEF_VEC_P(iv_stack_entry_p);
DEF_VEC_ALLOC_P(iv_stack_entry_p,heap);

typedef VEC(iv_stack_entry_p, heap) **loop_iv_stack;
extern void debug_loop_iv_stack (loop_iv_stack);

/* Return the old induction variable of the LOOP that is in normal
   form in SCOP.  */

static inline tree
oldiv_for_loop (scop_p scop, loop_p loop)
{
  int i;
  name_tree iv;

  if (!loop)
    return NULL_TREE;

  for (i = 0; VEC_iterate (name_tree, SCOP_OLDIVS (scop), i, iv); i++)
    if (iv->loop == loop)
      return iv->t;

  return NULL_TREE;
}

/* Associate a POLYHEDRON dependence description to two data
   references A and B.  */
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
graphite_test_dependence (scop_p, graphite_bb_p, graphite_bb_p,
			  struct data_reference *, struct data_reference *);
#endif  /* GCC_GRAPHITE_H  */
