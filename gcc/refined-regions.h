/* Refined Regions
   Copyright (C) 2010 Free Software Foundation, Inc.
   Contributed by Tobias Grosser <grosser@fim.uni-passau.de>

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

#ifndef GCC_REFINED_REGIONS_H
#define GCC_REFINED_REGIONS_H

typedef struct refined_region *refined_region_p;
DEF_VEC_P (refined_region_p);
DEF_VEC_ALLOC_P (refined_region_p, heap);

/* What is a Region?
   ================

   A Region is a connected subgraph of a control flow graph that has exactly
   two connections to the remaining graph.  It can be used to analyze or
   optimize parts of the control flow graph.

   A simple Region is connected to the remaining graph by just two edges.  One
   edge entering the Region and another one leaving the Region.

   A refined Region (or just Region) is a subgraph that can be transformed
   into a simple Region.  The transformation is done by adding basic blocks
   that merge several entry or exit edges so that after the merge just one
   entry and one exit edge exist.

   The Entry of a Region is the first BasicBlock that is passed after entering
   the Region.  It is an element of the Region.  The entry BasicBlock dominates
   all basic blocks in the Region.

   The Exit of a Region is the first BasicBlock that is passed after leaving
   the Region.  It is not an element of the Region.  The exit basic block
   postdominates all basic blocks in the region.

   A canonical Region cannot be constructed by combining smaller Regions.

   Region A is the parent of Region B, if B is completely contained in A.

   Two canonical Regions either do not intersect at all or one is the parent of
   the other.

   The (Refined) Program Structure Tree is a graph (V, E) where V is the set of
   (Refined) Regions in the control flow graph and E is the parent relation of
   these.

   What does this analysis?
   ========================

   This analysis calculates the Refined Program Structure Tree.

   The basic ideas are taken from "The Program Structure Tree - Richard
   Johnson, David Pearson, Keshav Pingali - 1994", however enriched with ideas
   from "The Refined Process Structure Tree - Jussi Vanhatalo, Hagen Voelyer,
   Jana Koehler - 2009".
   The algorithm to calculate these data structures however is completely
   different, as it takes advantage of existing information already available
   in (Post)dominance tree and dominance frontier passes.  This leads to a
   simpler and in practice hopefully better performing algorithm.
   The runtime of the algorithms described in the paper above are both linear
   in graph size, O(V+E), whereas this algorithm is not, as the dominance
   frontier information itself is not, but in practice runtime is in the order
   of magnitude of dominance tree calculation.  */

struct refined_region {
  basic_block entry;
  basic_block exit;
  struct refined_region *parent;
  VEC (refined_region_p, heap) *children;
};

extern refined_region_p calculate_region_tree (void);
extern void free_region_tree (refined_region_p);
extern bool refined_region_contains_bb_p (refined_region_p, basic_block);
extern bool refined_region_contains_region_p (refined_region_p,
					      refined_region_p);
extern void print_refined_region (FILE*, refined_region_p, int, bool);
extern void debug_refined_region (refined_region_p);
extern void get_bbs_in_region (refined_region_p, VEC (basic_block, heap) **);

#endif  /* GCC_REFINED_REGIONS_H */
