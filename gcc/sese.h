/* Single entry single exit control flow regions.
   Copyright (C) 2008, 2009  Free Software Foundation, Inc.
   Contributed by Jan Sjodin <jan.sjodin@amd.com> and
   Sebastian Pop <sebastian.pop@amd.com>.

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

#ifndef GCC_SESE_H
#define GCC_SESE_H

typedef struct name_tree
{
  tree t;
  const char *name;
  struct loop *loop;
} *name_tree;

DEF_VEC_P(name_tree);
DEF_VEC_ALLOC_P (name_tree, heap);

/* A Single Entry, Single Exit region is a part of the CFG delimited
   by two edges.  */
typedef struct sese
{
  /* Single ENTRY and single EXIT from the SESE region.  */
  edge entry, exit;

  /* REGION_BASIC_BLOCKS contains the set of all the basic blocks
     belonging to the SESE region.  */
  struct pointer_set_t *region_basic_blocks;

  /* An SSA_NAME version is flagged in the LIVEOUT bitmap if the
     SSA_NAME is defined inside and used outside the SESE region.  */
  bitmap liveout;

  /* The overall number of SSA_NAME versions used to index LIVEIN.  */
  int num_ver;

  /* For each SSA_NAME version VER in LIVEOUT, LIVEIN[VER] contains
     the set of basic blocks indices that contain a use of VER.  */
  bitmap *livein;

  /* Parameters used within the SCOP.  */
  VEC (name_tree, heap) *params;

  /* A collection of old induction variables*/ 
  VEC (name_tree, heap) *old_ivs;

  /* Loops completely contained in the SCOP.  */
  bitmap loops;
  VEC (loop_p, heap) *loop_nest;

  /* LIVEOUT_RENAMES registers the rename mapping that has to be
     applied after code generation.  */
  htab_t liveout_renames;

  /* Are we allowed to add more params?  This is for debugging purpose.  We
     can only add new params before generating the bb domains, otherwise they
     become invalid.  */
  bool add_params;
} *sese;

#define SESE_ENTRY(S) (S->entry)
#define SESE_EXIT(S) (S->exit)
#define SESE_REGION_BBS(S) (S->region_basic_blocks)
#define SESE_LIVEOUT(S) (S->liveout)
#define SESE_LIVEIN(S) (S->livein)
#define SESE_LIVEIN_VER(S, I) (S->livein[I])
#define SESE_NUM_VER(S) (S->num_ver)
#define SESE_PARAMS(S) (S->params)
#define SESE_LOOPS(S) (S->loops)
#define SESE_LOOP_NEST(S) (S->loop_nest)
#define SESE_ADD_PARAMS(S) (S->add_params)
#define SESE_PARAMS(S) (S->params)
#define SESE_OLDIVS(S) (S->old_ivs)
#define SESE_LIVEOUT_RENAMES(S) (S->liveout_renames)

extern sese new_sese (edge, edge);
extern void free_sese (sese);
extern void sese_build_livein_liveouts (sese);
void sese_build_livein_liveouts (sese);
int parameter_index_in_region (tree, sese);
bool build_sese_loop_nests (sese);

/* Checks, if SESE contains LOOP.  */

static inline bool
sese_contains_loop (sese sese, struct loop *loop)
{
  return bitmap_bit_p (SESE_LOOPS (sese), loop->num);
}

/* Returns the loop depth of LOOP in SESE.
   FIXME: The same as nb_loops_around_loop_in_sese.  */

static inline unsigned int
sese_loop_depth (sese sese, loop_p loop)
{
  unsigned int depth = 0;

  loop = loop_outer (loop);

  while (sese_contains_loop (sese, loop))
    {
      depth++;
      loop = loop_outer (loop);
    }

  return depth;
}

/* The number of parameters in REGION. */

static inline unsigned
sese_nb_params (sese region)
{
  return VEC_length (name_tree, SESE_PARAMS (region));
}

/* Returns true when BB is in REGION.  */

static inline bool
bb_in_sese_p (basic_block bb, sese region)
{
  return pointer_set_contains (SESE_REGION_BBS (region), bb);
}

/* Returns true when LOOP is in the SESE region R.  */

static inline bool 
loop_in_sese_p (struct loop *loop, sese r)
{
  return (bb_in_sese_p (loop->header, r)
	  && bb_in_sese_p (loop->latch, r));
}

/* Returns the block preceding the entry of a SESE.  */

static inline basic_block
block_before_sese (sese sese)
{
  return SESE_ENTRY (sese)->src;
}

#endif
