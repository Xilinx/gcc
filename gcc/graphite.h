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

int ref_nb_loops (data_reference_p, sese);

typedef struct gimple_bb
{
  basic_block bb;

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
 
     List of COND_EXPR and SWITCH_EXPR.  A COND_EXPR is true only if the
     corresponding element in CONDITION_CASES is not NULL_TREE.  For a
     SWITCH_EXPR the corresponding element in CONDITION_CASES is a
     CASE_LABEL_EXPR.  */
  VEC (gimple, heap) *conditions;
  VEC (gimple, heap) *condition_cases;

  VEC (data_reference_p, heap) *data_refs;
  htab_t cloog_iv_types;
} *gimple_bb_p;

#define GBB_BB(GBB) GBB->bb
#define GBB_DATA_REFS(GBB) GBB->data_refs
#define GBB_CONDITIONS(GBB) GBB->conditions
#define GBB_CONDITION_CASES(GBB) GBB->condition_cases
#define GBB_CLOOG_IV_TYPES(GBB) GBB->cloog_iv_types

/* Return the loop that contains the basic block GBB.  */

static inline struct loop *
gbb_loop (struct gimple_bb *gbb)
{
  return GBB_BB (gbb)->loop_father;
}

int nb_loops_around_gbb (gimple_bb_p);
 
extern void print_gimple_bb (FILE *, gimple_bb_p, int, int);
extern void debug_gbb (gimple_bb_p, int);
extern void debug_clast_stmt (struct clast_stmt *);

#endif  /* GCC_GRAPHITE_H  */
