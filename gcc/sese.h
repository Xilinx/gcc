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
extern void sese_insert_phis_for_liveouts (sese, basic_block, edge, edge);
extern void sese_adjust_phis_for_liveouts (sese, basic_block, edge, edge);
extern int parameter_index_in_region (tree, sese);
extern bool build_sese_loop_nests (sese);
extern tree oldiv_for_loop (sese, loop_p);
extern edge copy_bb_and_scalar_dependences (basic_block, sese, edge, htab_t);
extern struct loop *outermost_loop_in_sese (sese, basic_block);
extern void insert_loop_close_phis (sese, basic_block);
extern void insert_guard_phis (sese, basic_block, edge, edge, htab_t);

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


/* Calculate the number of loops around LOOP in the SCOP.
   FIXME: The same as sese_loop_depth.  */

static inline int
nb_loops_around_loop_in_sese (struct loop *l, sese region)
{
  int d = 0;

  for (; loop_in_sese_p (l, region); d++, l = loop_outer (l));

  return d;
}

/* Returns the block preceding the entry of a SESE.  */

static inline basic_block
block_before_sese (sese sese)
{
  return SESE_ENTRY (sese)->src;
}

/* A single entry single exit specialized for conditions.  */

typedef struct ifsese {
  sese region;
  sese true_region;
  sese false_region;
} *ifsese;

extern void if_region_set_false_region (ifsese, sese);
extern ifsese create_if_region_on_edge (edge, tree);
extern ifsese move_sese_in_condition (sese);
extern edge get_true_edge_from_guard_bb (basic_block);
extern edge get_false_edge_from_guard_bb (basic_block);

static inline edge
if_region_entry (ifsese if_region)
{
  return SESE_ENTRY (if_region->region);
}

static inline edge
if_region_exit (ifsese if_region)
{
  return SESE_EXIT (if_region->region);
}

static inline basic_block
if_region_get_condition_block (ifsese if_region)
{
  return if_region_entry (if_region)->dest;
}

/* Structure containing the mapping between the old names and the new
   names used after block copy in the new loop context.  */
typedef struct rename_map_elt
{
  tree old_name, new_name;
} *rename_map_elt;

extern void debug_rename_map (htab_t);
extern hashval_t rename_map_elt_info (const void *);
extern int eq_rename_map_elts (const void *, const void *);

/* Constructs a new SCEV_INFO_STR structure for VAR and INSTANTIATED_BELOW.  */

static inline rename_map_elt
new_rename_map_elt (tree old_name, tree new_name)
{
  rename_map_elt res;
  
  res = XNEW (struct rename_map_elt);
  res->old_name = old_name;
  res->new_name = new_name;

  return res;
}

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

extern void debug_oldivs (sese);
extern void debug_loop_iv_stack (loop_iv_stack);
extern void loop_iv_stack_insert_constant (loop_iv_stack, int, tree);
extern tree loop_iv_stack_get_iv_from_name (loop_iv_stack, const char *);
extern void loop_iv_stack_push_iv (loop_iv_stack, tree, const char *);
extern tree loop_iv_stack_get_iv (loop_iv_stack, int);
extern void loop_iv_stack_remove_constants (loop_iv_stack);
extern void loop_iv_stack_pop (loop_iv_stack);
extern void free_loop_iv_stack (loop_iv_stack);

/* Structure containing the mapping between the CLooG's induction
   variable and the type of the old induction variable.  */
typedef struct ivtype_map_elt
{
  tree type;
  const char *cloog_iv;
} *ivtype_map_elt;

extern void debug_ivtype_map (htab_t);
extern hashval_t ivtype_map_elt_info (const void *);
extern int eq_ivtype_map_elts (const void *, const void *);

/* Constructs a new SCEV_INFO_STR structure for VAR and INSTANTIATED_BELOW.  */

static inline ivtype_map_elt
new_ivtype_map_elt (const char *cloog_iv, tree type)
{
  ivtype_map_elt res;
  
  res = XNEW (struct ivtype_map_elt);
  res->cloog_iv = cloog_iv;
  res->type = type;

  return res;
}

/* Free and compute again all the dominators information.  */

static inline void
recompute_all_dominators (void)
{
  mark_irreducible_loops ();
  free_dominance_info (CDI_DOMINATORS);
  free_dominance_info (CDI_POST_DOMINATORS);
  calculate_dominance_info (CDI_DOMINATORS);
  calculate_dominance_info (CDI_POST_DOMINATORS);
}

#endif
