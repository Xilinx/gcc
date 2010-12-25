/* Translation of CLAST (CLooG AST) to Gimple.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@amd.com>.

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

#ifndef GCC_GRAPHITE_CLAST_TO_GIMPLE_H
#define GCC_GRAPHITE_CLAST_TO_GIMPLE_H

#include "graphite-cloog-util.h"
/* Data structure for CLooG program representation.  */

typedef struct cloog_prog_clast {
  CloogProgram *prog;
  struct clast_stmt *stmt;
} cloog_prog_clast;

/* Stores BB's related PBB.  */

typedef struct bb_pbb_def
{
  basic_block bb;
  poly_bb_p pbb;
}bb_pbb_def;

/* From graphite-clast-to-gimple.c  */
extern bool gloog (scop_p, htab_t);
extern cloog_prog_clast scop_to_clast (scop_p, CloogState *);
extern void debug_clast_stmt (struct clast_stmt *);
extern void print_clast_stmt (FILE *, struct clast_stmt *);
extern void build_iv_mapping (VEC (tree, heap) *, sese, VEC (tree, heap) *,
                              htab_t, struct clast_user_stmt *, htab_t);
extern struct loop *graphite_create_new_loop (sese, edge, struct clast_for *,
                                              loop_p,  VEC (tree, heap) **,
                                              htab_t, htab_t, int);
extern edge graphite_create_new_loop_guard (sese, edge, struct clast_for *,
                                            VEC (tree, heap) *, htab_t, htab_t);
extern edge graphite_create_new_guard (sese, edge, struct clast_guard *,
                                       VEC (tree, heap) *, htab_t, htab_t);
extern tree clast_to_gcc_expression (tree, struct clast_expr *, sese,
                                     VEC (tree, heap) *, htab_t, htab_t);
extern tree gcc_type_for_iv_of_clast_loop (struct clast_for *, int, tree, tree);

extern tree gcc_type_for_clast_expr (struct clast_expr *, sese,
                                     VEC (tree, heap) *, htab_t, htab_t);
extern tree clast_name_to_gcc (clast_name_p, sese, VEC (tree, heap) *,
			       htab_t, htab_t);
extern void save_clast_name_index (htab_t, const char *, int);

/* Hash function for data base element BB_PBB.  */

static inline hashval_t
bb_pbb_map_hash (const void *bb_pbb)
{
  return (hashval_t)(((const bb_pbb_def *)bb_pbb)->bb->index);
}

/* Compare data base element BB_PBB1 and BB_PBB2.  */

static inline int
eq_bb_pbb_map (const void *bb_pbb1, const void *bb_pbb2)
{
  const bb_pbb_def *bp1 = (const bb_pbb_def *) bb_pbb1;
  const bb_pbb_def *bp2 = (const bb_pbb_def *) bb_pbb2;
  return (bp1->bb->index == bp2->bb->index);
}

/* Returns the scattering dimension for STMTFOR.

   The relationship between dimension in scattering matrix
   and the DEPTH of the loop is:
   DIMENSION = 2*DEPTH - 1
*/

static inline int get_scattering_level (int depth)
{
  return 2 * depth - 1;
}

/* Stores the INDEX in a vector for a given clast NAME.  */

typedef struct clast_name_index
{
  int index;
  const char *name;
} *clast_name_index_p;

/* Computes a hash function for database element ELT.  */

static inline hashval_t
clast_name_index_elt_info (const void *elt)
{
  return htab_hash_pointer (((const struct clast_name_index *) elt)->name);
}

/* Compares database elements E1 and E2.  */

static inline int
eq_clast_name_indexes (const void *e1, const void *e2)
{
  const struct clast_name_index *elt1 = (const struct clast_name_index *) e1;
  const struct clast_name_index *elt2 = (const struct clast_name_index *) e2;

  return (elt1->name == elt2->name);
}

/* For a given clast NAME, returns -1 if it does not correspond to any
   parameter, or otherwise, returns the index in the PARAMS or
   SCATTERING_DIMENSIONS vector.  */

static inline int
clast_name_to_index (clast_name_p name, htab_t index_table)
{
  struct clast_name_index tmp;
  PTR *slot;

  tmp.name = clast_name_to_str (name);
  slot = htab_find_slot (index_table, &tmp, NO_INSERT);

  if (slot && *slot)
    return ((struct clast_name_index *) *slot)->index;

  return -1;
}

#endif
