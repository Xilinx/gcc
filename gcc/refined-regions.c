/* Refined Regions
   Copyright (C) 2010 Free Software Foundation, Inc.
   Contributed by Tobias Grosser <grosser@fim.uni-passau.de>.
	          Antoniu Pop  <antoniu.pop@gmail.com>

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "basic-block.h"
#include "timevar.h"
#include "bitmap.h"

#include "refined-regions.h"
#include "domwalk.h"

/* Print REGION to F with indention level INDENT.  */

void
print_refined_region (FILE *F, refined_region_p region, int indent)
{
  int ix, i;
  refined_region_p subregion;

  for (i = 0; i < indent * 2; ++i)
    fprintf (F, " ");

  if (region->exit)
    fprintf (F, "%d => %d \n", region->entry->index, region->exit->index);
  else
    fprintf (F, "%d => End\n", region->entry->index);

  for (ix = 0; VEC_iterate (refined_region_p, region->children, ix, subregion);
       ix++)
    print_refined_region (F, subregion, indent + 1);
}

/* Print REGION and all its subregions to stderr.  */

void
debug_refined_region (refined_region_p region)
{
  fprintf (stderr, "\n");
  print_refined_region (stderr, region, 0);
}

/* Check that BB is contained in REGION.  */

bool
refined_region_contains_bb_p (refined_region_p region, basic_block bb)
{
  return dominated_by_p (CDI_DOMINATORS, bb, region->entry)
    && !(dominated_by_p (CDI_DOMINATORS, bb, region->exit)
	 && dominated_by_p (CDI_DOMINATORS, region->exit, region->entry));
}

/* Check that the INNER region is contained in the OUTER region.  */

bool
refined_region_contains_region_p (refined_region_p outer,
				  refined_region_p inner)
{
  return refined_region_contains_bb_p (outer, inner->entry)
    && (refined_region_contains_bb_p (outer, inner->exit)
        || outer->exit == inner->exit);
}

/* Check that BB is part of the dominance frontier of ENTRY, because it was
   inherited from the dominance frontier of EXIT.  */

static bool is_common_df (basic_block bb, basic_block entry, basic_block exit)
{
  int i;
  edge e;

  for (i = 0; VEC_iterate (edge, bb->preds, i, e); i++)
    if (dominated_by_p (CDI_DOMINATORS, e->src, entry)
	&& !dominated_by_p (CDI_DOMINATORS, e->src, exit))
      return false;

  return true;
}

/* Data needed to calculate the refined regions.  */

struct find_regions_global_data {

  /* The shortcut table keeps track of existing regions and saves shortcuts
     from the beginning of each region to the end of the largest possible
     region.  Canonical regions are attached to represent the largest possible
     region.  */
  htab_t shortcut;

  /* A map from each basic block that is a region header to its surrounding
     region.  */
  htab_t bbmap;

  /* An array that contains for each basic block its dominance frontier.  */
  bitmap *dfs;
};

/* Check that ENTRY and EXIT form a refined region.  DFS contains the dominance
   frontier for all basic blocks.  */

static bool
is_region (basic_block entry, basic_block exit, bitmap *dfs)
{
  bitmap df_entry = BITMAP_ALLOC (NULL);
  bitmap df_exit = BITMAP_ALLOC (NULL);
  bitmap_iterator bi;
  unsigned bit = 0;

  bitmap_copy (df_entry, dfs[entry->index]);

  bitmap_clear_bit (df_entry, entry->index);
  bitmap_clear_bit (df_entry, exit->index);

  if (entry == exit)
    return false;

  if (!dominated_by_p (CDI_POST_DOMINATORS, entry, exit))
    return false;

  if (!dominated_by_p (CDI_DOMINATORS, exit, entry))
    return bitmap_count_bits (df_entry) == 0;

  bitmap_copy (df_exit, dfs[exit->index]);
  bitmap_clear_bit (df_exit, entry->index);
  bitmap_clear_bit (df_exit, exit->index);

  if (bitmap_intersect_compl_p (df_entry, df_exit))
    return false;

  EXECUTE_IF_SET_IN_BITMAP (df_entry, 0, bit, bi)
    if (!is_common_df (BASIC_BLOCK (bit), entry, exit))
      return false;

  EXECUTE_IF_SET_IN_BITMAP (df_exit, 0, bit, bi)
    if (dominated_by_p (CDI_DOMINATORS, BASIC_BLOCK (bit), entry))
      return false;

  bitmap_clear (df_entry);
  bitmap_clear (df_exit);

  return true;
}

/* Functions to map from one basic block to another.  */

typedef struct bb_bb_def
{
  basic_block key;
  basic_block value;
} bb_bb_def;

/* Allocate a new bb->bb object mapping from KEY to VALUE.  */

static bb_bb_def *
new_bb_bb_def (basic_block key, basic_block value)
{
  bb_bb_def *bb_bb_p;
  bb_bb_p = XNEW (bb_bb_def);
  bb_bb_p->key = key;
  bb_bb_p->value = value;
  return bb_bb_p;
}

/* Calculate the hash code of the bb->bb object BB_BB.  */

static inline hashval_t
bb_bb_map_hash (const void *bb_bb)
{
  return (hashval_t)(((const bb_bb_def *)bb_bb)->key->index);
}

/* Check that two bb->bb objects BB_BB1 and BB_BB2 are equal.  */

static inline int
eq_bb_bb_map (const void *bb_bb1, const void *bb_bb2)
{
  const bb_bb_def *bb1 = (const bb_bb_def *) bb_bb1;
  const bb_bb_def *bb2 = (const bb_bb_def *) bb_bb2;
  return (bb1->key->index == bb2->key->index);
}

/* Find the value saved in BB_MAP at KEY.  If key is not available return NULL.
   */

static inline basic_block
find_new_bb (basic_block key, htab_t bb_map)
{
  bb_bb_def tmp;
  PTR *x;

  tmp.key = key;
  x = htab_find_slot (bb_map, &tmp, NO_INSERT);
  if (!x || !*x)
    return NULL;

  return ((bb_bb_def *) (*x))->value;
}

/* Functions to map from a basic block to a region.  */

typedef struct bb_reg_def
{
  basic_block bb;
  refined_region_p region;
} bb_reg_def;

/* Create a bb to region map object mapping from BB to REGION.  */

static bb_reg_def *
new_bb_reg_def (basic_block bb, refined_region_p region)
{
  bb_reg_def *bb_reg_p;
  bb_reg_p = XNEW (bb_reg_def);
  bb_reg_p->bb = bb;
  bb_reg_p->region = region;
  return bb_reg_p;
}

/* Calculate the hash value for BB_REG.  */

static inline hashval_t
bb_reg_map_hash (const void *bb_reg)
{
  return (hashval_t)(((const bb_reg_def *)bb_reg)->bb->index);
}

/* Check that BB_REG1 and BB_REG2 are equal.  */

static inline int
eq_bb_reg_map (const void *bb_reg1, const void *bb_reg2)
{
  const bb_reg_def *bb1 = (const bb_reg_def *) bb_reg1;
  const bb_reg_def *bb2 = (const bb_reg_def *) bb_reg2;
  return (bb1->bb->index == bb2->bb->index);
}

/* Search in BB_MAP for the region saved for BB.  If available return the
   region otherwise NULL.  */

static inline refined_region_p
find_new_region (basic_block bb, htab_t bb_map)
{
  bb_reg_def tmp;
  PTR *x;

  tmp.bb = bb;
  x = htab_find_slot (bb_map, &tmp, NO_INSERT);
  if (!x || !*x)
    return NULL;

  return ((bb_reg_def *) (*x))->region;
}

/* Insert an object mapping from BB to REG into REGMAP.  Overwrite
   the old value if the BB already exists.  */

static inline void
insert_new_reg (basic_block bb, refined_region_p reg, htab_t regmap)
{
  bb_reg_def tmp;
  PTR *x;

  tmp.bb = bb;
  x = htab_find_slot (regmap, &tmp, INSERT);
  if (x && !*x)
    *x = new_bb_reg_def (bb, reg);
  else if (x)
    {
      free (*x);
      *x = new_bb_reg_def (bb, reg);
    }
}

/* Insert an object mapping from basic block KEY to VALUE into REGMAP.
   If KEY is already used overwrite the old value.  */

static inline void
insert_new_bb (basic_block key, basic_block value, htab_t bb_map)
{
  bb_bb_def tmp;
  PTR *x;

  tmp.key = key;
  x = htab_find_slot (bb_map, &tmp, INSERT);
  if (x && !*x)
    *x = new_bb_bb_def (key, value);
  else if (x)
    *x = new_bb_bb_def (key, value);
}

/* Insert a shortcut from ENTRY to EXIT into SHORTCUT.  If there is already
   a shortcut starting at EXIT, extend the shortcut to point to the same
   basic block EXIT is already pointing.  */

static void
insert_shortcut (basic_block entry, basic_block exit, htab_t shortcut)
{
  basic_block bb = find_new_bb (exit, shortcut);

  if (!bb)
    insert_new_bb (entry, exit, shortcut);
  else
    insert_new_bb (entry, bb, shortcut);
}

/* Get the next post dominator of BB, but skip all existing regions by looking
   at SHORTCUT.  */

static basic_block
get_next_postdom (basic_block bb, htab_t shortcut)
{
  basic_block bb_shortcut = find_new_bb (bb, shortcut);

  if (bb_shortcut)
    bb = bb_shortcut;

  return get_immediate_dominator (CDI_POST_DOMINATORS, bb);
}

/* Create a new region starting at ENTRY and finishing at EXIT.  */

static refined_region_p
create_region (basic_block entry, basic_block exit)
{
  refined_region_p r = XNEW (struct refined_region);
  r->entry = entry;
  r->exit = exit;
  r->parent = 0;
  r->children = VEC_alloc (refined_region_p, heap, 16);

  return r;
}

/* Find all regions that start at ENTRY.  DFS is an array containing
   the dominance frontier for each basic block.  GD the global dom walk data
   storing some further information.  */

static void
find_regions_with_entry (basic_block entry, struct find_regions_global_data *gd)
{
  basic_block exit = entry;
  refined_region_p last_region = 0;
  basic_block last_exit = entry;

  while ((exit = get_next_postdom (exit, gd->shortcut)))
    {

      if (is_region (entry, exit, gd->dfs))
	{
	  refined_region_p new_region = create_region (entry, exit);

	  /* new_region becomes the parent of last_region.  */
	  if (last_region)
	    {
	      VEC_safe_push (refined_region_p, heap, new_region->children,
			     last_region);
	      last_region->parent = new_region;
	    }
	  else
	    insert_new_reg (entry, new_region, gd->bbmap);

	  last_region = new_region;
	  last_exit = exit;
	}

      /* This can never be a region, so stop the search.  */
      if (!dominated_by_p (CDI_DOMINATORS, exit, entry))
	break;
    }

  /* Tried to create regions from entry to last_exit.  Next time take a
     shortcut from entry to last_exit.  */
  if (last_exit != entry)
    insert_shortcut (entry, last_exit, gd->shortcut);
}

/* Execute this code after each element that was visited by the
   dominance tree walk.  It will find the regions starting
   at this element (BB),  while taking into account the global
   dom walk data DWD.  */

static void
find_regions_adc (struct dom_walk_data *dwd, basic_block bb)
{
  struct find_regions_global_data *gd =
    (struct find_regions_global_data *) dwd->global_data;
  find_regions_with_entry (bb, gd);
}

/* Find all regions in the current function.  DFS is an array
   containing the dominance frontiers of all basic blocks.  BBMAP will contain
   the surrounding regions for all region header basic blocks.  */

static void
find_regions (bitmap *dfs, htab_t bbmap)
{
  struct dom_walk_data dwd;
  struct find_regions_global_data gd;
  gd.shortcut = htab_create (32, &bb_bb_map_hash, &eq_bb_bb_map, free);
  gd.bbmap = bbmap;
  gd.dfs = dfs;

  /* Initialize the domtree walk.  */
  dwd.dom_direction = CDI_DOMINATORS;
  dwd.block_local_data_size = 0;
  dwd.global_data = &gd;
  dwd.after_dom_children = find_regions_adc;
  dwd.before_dom_children = 0;
  dwd.initialize_block_local_data = 0;
  init_walk_dominator_tree (&dwd);

  /* To only get canonical regions we walk the dominator tree post order and
     skip already built regions using the shortcut map.  */
  walk_dominator_tree (&dwd, ENTRY_BLOCK_PTR);
  htab_delete (gd.shortcut);
}

/* Get the topmost parent region of REGION.  */

static refined_region_p
get_topmost_parent (refined_region_p region)
{
  if (!region)
    return NULL;

  while (region->parent)
    region = region->parent;

  return region;
}

/* Initialize the parent/child relations of the regions.  Starting at BB
   that is surrounded by OUTER_REGION.  BBMAP contains the surrounding regions
   for all basic blocks that are region headers.  */

static void
build_regions_tree (basic_block bb, refined_region_p outer_region, htab_t bbmap)
{
  int ix;
  basic_block child_bb;
  refined_region_p region;
  VEC (basic_block, heap) *dominated_bbs = get_dominated_by (CDI_DOMINATORS,
							     bb);
  /* Passed region exit.  */
  while (bb == outer_region->exit)
    outer_region = outer_region->parent;

  region = find_new_region (bb, bbmap);

  if (region)
    {
      refined_region_p topmost_parent = get_topmost_parent (region);
      VEC_safe_push (refined_region_p, heap, outer_region->children,
		     topmost_parent);
      topmost_parent->parent = outer_region;
      outer_region = region;
    }

  for (ix = 0; VEC_iterate (basic_block, dominated_bbs, ix, child_bb); ix++)
    build_regions_tree (child_bb, outer_region, bbmap);

  VEC_free (basic_block, heap, dominated_bbs);
}

/* Calculate the refined region tree and return the root of the region
   tree.  */

refined_region_p
calculate_region_tree (void)
{
  bitmap *dfs;
  basic_block bb;
  refined_region_p outermost_region;
  htab_t bbmap =  htab_create (32, &bb_reg_map_hash, &eq_bb_reg_map, free);
  bool dom_available, postdom_available;

  timevar_push (TV_REFINED_REGIONS);

  /* Initialize dominance frontier.  */
  dfs = XNEWVEC (bitmap, last_basic_block);
  FOR_EACH_BB (bb)
    dfs[bb->index] = BITMAP_ALLOC (NULL);

  /* Required analysis */
  dom_available = dom_info_available_p (CDI_DOMINATORS);
  postdom_available = dom_info_available_p (CDI_POST_DOMINATORS);

  calculate_dominance_info (CDI_DOMINATORS);
  calculate_dominance_info (CDI_POST_DOMINATORS);
  compute_dominance_frontiers (dfs);

  find_regions (dfs, bbmap);
  outermost_region = create_region (ENTRY_BLOCK_PTR, 0);
  build_regions_tree (ENTRY_BLOCK_PTR, outermost_region, bbmap);

  /* Free dominance frontier */
  FOR_EACH_BB (bb)
    BITMAP_FREE (dfs[bb->index]);
  free (dfs);

  if (!dom_available)
    free_dominance_info (CDI_DOMINATORS);

  if (!postdom_available)
    free_dominance_info (CDI_POST_DOMINATORS);

  htab_delete (bbmap);

  timevar_pop (TV_REFINED_REGIONS);
  return outermost_region;
}

/* Free REGION and all its subregions.  */

void
free_region_tree (refined_region_p region)
{
  int ix;
  refined_region_p subregion;

  for (ix = 0; VEC_iterate (refined_region_p, region->children, ix, subregion);
       ix++)
    free_region_tree (subregion);

  VEC_free (refined_region_p, heap, region->children);
  free (region);
}
