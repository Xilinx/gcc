/* Build meta information from clast data structures for GRAPHITE-OpenCL.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

/* This file implements building meta infromation about data
   references, supported types and operations and dependencies.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "toplev.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "domwalk.h"
#include "value-prof.h"
#include "pointer-set.h"
#include "gimple.h"
#include "sese.h"
#include "output.h"
#include "hashtab.h"
#include "tree.h"
#include "gimple-pretty-print.h"

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "graphite-ppl.h"
#include "graphite.h"
#include "graphite-poly.h"
#include "graphite-scop-detection.h"
#include "graphite-clast-to-gimple.h"
#include "graphite-dependences.h"
#include "dyn-string.h"
#include "graphite-opencl.h"

/* Something like C++ std::map<int, int>.  */

struct opencl_pair_def
{
  int id;
  int val;
};

typedef struct opencl_pair_def *opencl_pair;

/* Hash function for opencl_pair.  */

static hashval_t
opencl_pair_to_hash (const void *data)
{
  const struct opencl_pair_def *obj = (const struct opencl_pair_def *) data;

  return (hashval_t) (obj->id);
}

/* Compare function for opencl_pair.  */

static int
opencl_pair_cmp (const void *e1, const void *e2)
{
  const struct opencl_pair_def *obj1 = (const struct opencl_pair_def *) e1;
  const struct opencl_pair_def *obj2 = (const struct opencl_pair_def *) e2;

  return obj1->id == obj2->id;
}

/* Create new opencl_pair with NEW_ID as id and NEW_VAL as val.  */

static opencl_pair
opencl_pair_create (int new_id, int new_val)
{
  opencl_pair tmp = XNEW (struct opencl_pair_def);

  tmp->id = new_id;
  tmp->val = new_val;
  return tmp;
}

/* Delete opencl_pair DATA.  */

static void
opencl_pair_delete (opencl_pair data)
{
  free (data);
}

/* Create new opencl_clast_meta structure with PARENT as parent,
   DEPTH as out_depth.  If ACCESS_INIT is true, then init access bitmaps.  */

static opencl_clast_meta
opencl_clast_meta_create (int depth, opencl_clast_meta parent,
                          bool access_init)
{
  opencl_clast_meta tmp = XNEW (struct opencl_clast_meta_def);

  tmp->out_depth = depth;
  tmp->in_depth = 0;
  tmp->next = NULL;
  tmp->body = NULL;
  tmp->parent = parent;
  tmp->on_device = false;
  tmp->modified_on_host = BITMAP_ALLOC (NULL);
  tmp->modified_on_device = BITMAP_ALLOC (NULL);
  tmp->access_unsupported = false;

  if (access_init)
    {
      tmp->can_be_private = BITMAP_ALLOC (NULL);
      tmp->access = BITMAP_ALLOC (NULL);
    }
  else
    {
      tmp->access = NULL;
      tmp->can_be_private = NULL;
    }

  return tmp;
}

/* Check whether type TYPE is supported by current graphite-opencl
   implementation.  If PTR or ARRAY is true, then TYPE can not be pointer
   type (because only one level of pointers is supported). Also if PTR is
   true, TYPE can not be array (because pointers to arrays are not
   supported yet).  */

static bool
opencl_supported_type_p (tree type, bool ptr, bool array)
{
  switch (TREE_CODE (type))
    {
    case POINTER_TYPE:
      {
	if (ptr || array)
	  return false;
	return opencl_supported_type_p (TREE_TYPE (type), true, false);
      }

    case ARRAY_TYPE:
      {
	if (ptr)
	  return false;
	return opencl_supported_type_p (TREE_TYPE (type), false, true);
      }

    case FUNCTION_DECL:
    case FUNCTION_TYPE:
    case COMPLEX_TYPE:
    case RECORD_TYPE:
    case ENUMERAL_TYPE:
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
    case METHOD_TYPE:
    case REFERENCE_TYPE:
      return false;

    case BOOLEAN_TYPE:
    case INTEGER_TYPE:
    case REAL_TYPE:
    case VOID_TYPE:
      return true;

    case OFFSET_TYPE:
    case FIXED_POINT_TYPE:
    case VECTOR_TYPE:
    case LANG_TYPE:
    default:
      debug_tree (type);
      gcc_unreachable ();
    }
}

/* Check whether expression ARG is supported by current graphite-opencl
   implementation.  */

static bool
opencl_supported_arg_p (opencl_main code_gen, tree arg)
{
  switch (TREE_CODE (arg))
    {
    case SSA_NAME:
      return opencl_supported_arg_p (code_gen, SSA_NAME_VAR (arg));

    case ARRAY_REF:
    case INDIRECT_REF:
    case ADDR_EXPR:
      return opencl_supported_arg_p (code_gen, TREE_OPERAND (arg, 0));

    case VAR_DECL:
    case PARM_DECL:
      {
	tree type = TREE_TYPE (arg);

	if (TREE_CODE (type) == POINTER_TYPE
	    && !opencl_get_data_by_tree (code_gen, arg))
	  return false;

	return opencl_supported_type_p (type, false, false);
      }

    case INTEGER_CST:
    case REAL_CST:
    case POINTER_PLUS_EXPR:
      return true;

    case FIELD_DECL:
    case COMPONENT_REF:
    case MEM_REF:
    case REALPART_EXPR:
    case IMAGPART_EXPR:
    case COMPLEX_EXPR:
    case CALL_EXPR:
    case RESULT_DECL:
      return false;

    default:
      debug_tree (arg);
      gcc_unreachable ();
    }
}

/* Check whether gimple assignment statement GMP is supported by current
   graphite-opencl implementation.  CODE_GEN holds information about non
   scalar arguments.  */

static bool
opencl_gimple_assign_with_supported_types_p (opencl_main code_gen, gimple gmp)
{
  tree curr_tree;
  int num_of_ops = gimple_num_ops (gmp);

  gcc_assert (gimple_code (gmp) == GIMPLE_ASSIGN
	      && (num_of_ops == 2 || num_of_ops == 3));

  curr_tree = gimple_assign_lhs (gmp);

  if (!opencl_supported_arg_p (code_gen, curr_tree))
    return false;

  curr_tree = gimple_assign_rhs1 (gmp);

  if (!opencl_supported_arg_p (code_gen, curr_tree))
    return false;

  if (num_of_ops == 3)
    {
      curr_tree = gimple_assign_rhs2 (gmp);

      if (!opencl_supported_arg_p (code_gen, curr_tree))
        return false;
    }

  return true;
}

/* Check whether all statements in basic block BB are supported by current
   graphite-opencl implementation.  CODE_GEN holds information about non
   scalar arguments.  */

static bool
opencl_supported_type_access_p (opencl_main code_gen, basic_block bb)
{
  gimple_stmt_iterator gsi;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);

      if (!stmt)
        continue;

      switch (gimple_code (stmt))
        {
        case GIMPLE_DEBUG:
        case GIMPLE_COND:
        case GIMPLE_PHI:
        case GIMPLE_LABEL:
          continue;

        case GIMPLE_ASSIGN:
          if (!opencl_gimple_assign_with_supported_types_p (code_gen, stmt))
            {
              if (dump_file && (dump_flags & TDF_DETAILS))
                {
                  fprintf (dump_file, "opencl_supported_type_access_p:"
                           " bad types in assignment\n");
                  print_gimple_stmt (dump_file, stmt, 0, TDF_VOPS|TDF_MEMSYMS);
                }
              return false;
            }
          continue;

        case GIMPLE_CALL:
          return false;

        default:
          debug_gimple_stmt (stmt);
          gcc_unreachable ();
        }
    }

  return true;
}

/* Mark variable, represented by tree OBJ as visited in bitmap VISITED.
   If DEF is true and given variable can be privatized, mark it as
   privatized in META.  CODE_GEN holds information about non
   scalar arguments.  */

static void
opencl_def_use_data (opencl_main code_gen, tree obj, bitmap visited,
                     opencl_clast_meta meta, bool def)
{
  opencl_data data;

  if (obj == NULL)
    return;

  data = opencl_get_data_by_tree (code_gen,
                                  opencl_get_base_object_by_tree (obj));
  if (data == NULL)
    return;

  if (!data->can_be_private)
    return;

  if (!bitmap_set_bit (visited, data->id))
    return;

  if (!def)
    return;

  bitmap_set_bit (meta->can_be_private, data->id);
}

/* Mark data in META, corresponding to basic block BB, which can be
   privatized.  CODE_GEN holds information about non
   scalar arguments.  */

static void
opencl_calc_bb_privatization (opencl_main code_gen, basic_block bb,
                              opencl_clast_meta meta)
{
  gimple_stmt_iterator gsi;
  bitmap visited = BITMAP_ALLOC (NULL);

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gimple stmt = gsi_stmt (gsi);

      if (gimple_code (stmt) != GIMPLE_ASSIGN)
        continue;

      opencl_def_use_data (code_gen, gimple_assign_lhs (stmt),
                           visited, meta, true);

      opencl_def_use_data (code_gen, gimple_assign_rhs1 (stmt),
                           visited, meta, false);

      opencl_def_use_data (code_gen, gimple_assign_rhs2 (stmt),
                           visited, meta, false);
    }

  BITMAP_FREE (visited);
}

/* Analyse clast_user_stmt STMT and set read/write flags for each data
   reference in this statement in clast meta corresponding to this
   statement.  If some data references in statement are unsupported,
   then mark META as accessing unsupported.  CODE_GEN holds
   information about non scalar arguments.  */

static void
opencl_set_meta_rw_flags (opencl_clast_meta meta,
                          struct clast_user_stmt * stmt,
                          opencl_main code_gen)
{
  CloogStatement *cs = stmt->statement;
  poly_bb_p pbb = (poly_bb_p) cloog_statement_usr (cs);
  VEC (poly_dr_p, heap) *drs = PBB_DRS (pbb);
  gimple_bb_p gbb = PBB_BLACK_BOX (pbb);
  basic_block bb = GBB_BB (gbb);
  int i;
  poly_dr_p curr;

  if (!opencl_supported_type_access_p (code_gen, bb))
    {
      if (dump_file && (dump_flags & TDF_DETAILS))
        {
          fprintf (dump_file, "Basic block contains unsupported "
		   "types in graphite-opencl\n");
          dump_bb (bb, dump_file, 0);

        }

      meta->access_unsupported = true;
    }

  opencl_calc_bb_privatization (code_gen, bb, meta);

  for (i = 0; VEC_iterate (poly_dr_p, drs, i, curr); i++)
    {
      data_reference_p d_ref = (data_reference_p) PDR_CDR (curr);
      tree data_ref_tree = dr_outermost_base_object (d_ref);
      opencl_data data;

      if (!opencl_supported_arg_p (code_gen, data_ref_tree))
        {
          meta->access_unsupported = true;

          if (dump_file && (dump_flags & TDF_DETAILS))
            {
              fprintf (dump_file,
		       "Unsupported in graphite-opencl data reference\n");
              dump_data_reference (dump_file, d_ref);

            }

          continue;
        }

      data = opencl_get_data_by_data_ref (code_gen, d_ref);
      gcc_assert (data);

      if (!data->supported)
        meta->access_unsupported = true;

      if (!graphite_outer_subscript_bound (curr, false))
        {
          meta->access_unsupported = true;

          if (dump_file && (dump_flags & TDF_DETAILS))
            {
              fprintf (dump_file, "Can not determine subscript bound "
		       "for data reference\n");
              dump_data_reference (dump_file, d_ref);

            }

          continue;
        }

      if (data->size_value == NULL)
        {
          meta->access_unsupported = true;

          if (dump_file && (dump_flags & TDF_DETAILS))
            {
              fprintf (dump_file,
		       "Can not determine size for data reference\n");
              dump_data_reference (dump_file, d_ref);
            }
        }

      bitmap_set_bit (meta->access, data->id);
    }
}

/* Update META access bitmap by union of access bitmaps of it's children.  */

static void
opencl_collect_definitions_info (opencl_clast_meta meta)
{
  opencl_clast_meta curr = meta->body->next;
  bitmap tmp_access = BITMAP_ALLOC (NULL);

  bitmap_copy (tmp_access, meta->body->access);
  meta->can_be_private = BITMAP_ALLOC (NULL);
  bitmap_copy (meta->can_be_private, meta->body->can_be_private);

  while (curr)
    {
      bitmap new_defs = BITMAP_ALLOC (NULL);
      bitmap_and_compl (new_defs, curr->can_be_private, tmp_access);
      bitmap_ior_into (tmp_access, curr->access);
      bitmap_ior_into (meta->can_be_private, new_defs);
      curr = curr->next;
      BITMAP_FREE (new_defs);
    }

  meta->access = tmp_access;
}

/* Build meta structure from clast structure.
   BODY - base clast statement.
   DEPTH - depth of BODY in whole clast structure.
   PARENT - parent meta node.
   CODE_GEN - data structure, which holds information
   about non scalar arguments.  */

opencl_clast_meta
opencl_create_meta_from_clast (opencl_main code_gen,
                               struct clast_stmt *body, int depth,
                               opencl_clast_meta parent)
{
  int max_depth = 0;
  opencl_clast_meta result = NULL;
  opencl_clast_meta curr = NULL;
  struct clast_stmt *curr_stmt;

  for (curr_stmt = body; curr_stmt; curr_stmt = curr_stmt->next)
    {
      opencl_clast_meta tmp_result = NULL;

      if (CLAST_STMT_IS_A (curr_stmt, stmt_root))
        continue;

      if (CLAST_STMT_IS_A (curr_stmt, stmt_user))
        {
          tmp_result = opencl_clast_meta_create (depth, parent, true);
          opencl_set_meta_rw_flags (tmp_result,
                                    (struct clast_user_stmt*) curr_stmt,
                                    code_gen);
        }

      if (CLAST_STMT_IS_A (curr_stmt, stmt_guard))
        {
          struct clast_guard *if_stmt = (struct clast_guard *) curr_stmt;
          /* For guard (if) statement create meta for it's body and just
             append it to current list.  */
          tmp_result = opencl_create_meta_from_clast (code_gen, if_stmt->then,
                                                      depth, parent);
        }

      if (CLAST_STMT_IS_A (curr_stmt, stmt_block))
        {
          struct clast_block *bl_stmt = (struct clast_block *) curr_stmt;

          tmp_result = opencl_create_meta_from_clast (code_gen, bl_stmt->body,
                                                      depth, parent);
        }

      if (CLAST_STMT_IS_A (curr_stmt, stmt_for))
        {
          struct clast_for *for_stmt = (struct clast_for *) curr_stmt;

          tmp_result = opencl_clast_meta_create (depth, parent, false);
          tmp_result->body
	    = opencl_create_meta_from_clast (code_gen, for_stmt->body,
					     depth + 1, tmp_result);

          max_depth = (max_depth > tmp_result->in_depth + 1)
	    ? max_depth : tmp_result->in_depth + 1;
          opencl_collect_definitions_info (tmp_result);
        }

      if (!result)
        curr = result = tmp_result;
      else
        curr->next = tmp_result;

      while (curr->next)
	curr = curr->next;
    }

  if (parent)
    parent->in_depth = max_depth;

  return result;
}

/* For all opencl_data referenced in META calculate depth  of innermost
   reference.  DEPTH is the depth of the loop, represented be META
   in current loop nest.  DATE holds intermediate resuls.
   Function returns false iff there is access to unsupported types in
   given META.
   Consider an example:

   |         Meta_1
   |          / \
   |         /   \
   |        /     \
   |       /       \
   |    Meta_2      Meta_3
   |    {D1,D2}     /  \
   |               /    \
   |              /      \
   |           Meta_4   Meta_5
   |           {D2,D4}    |
   |                      |
   |                      |
   |                   Meta_6
   |                    {D3}

   In this example D1_depth = 1, D2_depth = 2, D3_depth = 3, D4_depth = 2.  */

static bool
opencl_calc_max_depth_tab (opencl_clast_meta meta, htab_t data, int depth)
{
  while (meta)
    {
      if (meta->body)
        {
          if (!opencl_calc_max_depth_tab (meta->body, data, depth + 1))
            return false;
        }
      else
        {
          /* User stmt.  Analyze data access.  */
          bitmap stmt_access = meta->access;
          unsigned i;
          bitmap_iterator bi;

          if (meta->access_unsupported)
            return false;

          EXECUTE_IF_SET_IN_BITMAP (stmt_access, 0, i, bi)
            {
              opencl_pair curr_pair = opencl_pair_create (i, depth);
              struct opencl_pair_def **slot
		= (struct opencl_pair_def **) htab_find_slot (data, curr_pair,
							      INSERT);

              if (*slot == NULL)
                *slot = curr_pair;
              else
                {
                  opencl_pair old_pair = *slot;

                  if (old_pair->val > curr_pair->val)
                    opencl_pair_delete (curr_pair);
                  else
                    {
                      *slot = curr_pair;
                      opencl_pair_delete (old_pair);
                    }
                }
            }
        }

      meta = meta->next;
    }

  return true;
}

/* Check whether it's reasonable to pass data, represented by OBJ,
   to device based on information from META.
   This function helps to avoid kernels like this.

   | host_use_a_and_b ();
   | for (int i = 0; i < N; i++)
   |   a[i] = b[i];

   host_use_a_and_b ();

   We have no dependency here, but memory transfer from host to devices and
   from device to host aren't reasonable here.

   We have 2 situations when memory transfer is reasonable.

   1.

   | for (int j = 0; j < N; j++)
   |   {
   |     host_use_c ();
   |     for (int i = 0; i < N; i ++)
   |       a[i] = b [i] + j;
   |   }

   In this case we can put memory transfer befor first loop, so
   we will avoid situation, when all benefits from parallel execution
   can be eliminated by memory transfer.

   2.

   | for (int i = 0; i < N; i ++)
   |   for (int j = 0; j < N; j ++)
   |     {
   |       c[i][j] = 0;
   |       for (int k = 0; k < N; k ++)
   |         c[i][j] += a[i][k] * b [k][j];
   |     }

   In this case each element of a,b or c is used N time on device,
   so memory transfer is reasonable.  */

static bool
opencl_evaluate_data_access_p (opencl_data obj, opencl_clast_meta meta)
{
  int depth = obj->depth;
  int data_id = obj->id;
  opencl_clast_meta parent = meta->parent;

  if (obj->privatized
      || depth < obj->data_dim)
    return false;

  if (parent)
    {
      /* We have outer loop.  */
      bitmap curr_bitmap = parent->modified_on_host;

      /* Memory transfer for this statement has been placed outside
         outer loop, so for one memory transfer will be executing more
         then one kernel (first case).  */
      if (!bitmap_bit_p (curr_bitmap, data_id))
        return true;
    }

  /* Check max depth of memory access (second case).  */
  return (depth > obj->data_dim);
}

/* Find opencl_data object by it's ID in CODE_GEN structures.  */

static opencl_data
opencl_get_data_by_id (opencl_main code_gen, int id)
{
  VEC (opencl_data, heap) *main_data = code_gen->opencl_function_data;
  opencl_data res = VEC_index (opencl_data, main_data, id);

  gcc_assert (res->id == id);
  return res;
}

/* Check whether memory transfer is reasonable if clast statement,
   connected with META, will be replaced by opencl kernel launch.
   ACCESS holds depth of innermost data references
   for all data, references in statement, represented by META.
   CODE_GEN holds information about non scalar arguments.  */

static bool
opencl_analyse_data_access_p (opencl_main code_gen,
                              htab_t access,
                              opencl_clast_meta meta)
{
  htab_iterator h_iter;
  opencl_pair curr;
  int max_dim = 1;
  int i;
  opencl_data curr_data;
  VEC (opencl_data, heap) *data_objs = VEC_alloc (opencl_data, heap,
						  OPENCL_INIT_BUFF_SIZE);

  FOR_EACH_HTAB_ELEMENT (access, curr, opencl_pair, h_iter)
    {
      int id = curr->id;
      opencl_data obj = opencl_get_data_by_id (code_gen, id);

      VEC_safe_push (opencl_data, heap, data_objs, obj);

      if (max_dim < obj->data_dim)
        max_dim = obj->data_dim;

      obj->depth = curr->val;
    }

  for (i = 0; VEC_iterate (opencl_data, data_objs, i, curr_data); i++)
    {
      if (curr_data->data_dim != max_dim)
        continue;

      if (opencl_evaluate_data_access_p (curr_data, meta))
        return true;
    }

  return false;
}

/* Main predicate which checks whether statement, represented by META and
   located on depth DEPTH, should be replaced by opencl kernel launch.
   CODE_GEN holds information about non scalar arguments.  */

bool
opencl_should_be_parallel_p (opencl_main code_gen,
                             opencl_clast_meta meta,
                             int depth)
{
  int i_depth = meta->in_depth;
  htab_t max_access_depth;
  bool dump_p = dump_file && (dump_flags & TDF_DETAILS);

  if (dump_p)
    fprintf (dump_file, "opencl_should_be_parallel_p: ");

  /* Avoid launching a lot of small kernels in a deep loop.  */
  if (!flag_graphite_opencl_no_depth_check
      && depth > i_depth + opencl_base_depth_const)
    {
      if (dump_p)
	fprintf (dump_file, "avoiding small kernel in deep loop\n");

      return false;
    }

  max_access_depth = htab_create (OPENCL_INIT_BUFF_SIZE,
                                  opencl_pair_to_hash,
                                  opencl_pair_cmp, free);

  /* Can't parallelize if statements in loop contain unsupported types.  */
  if (!flag_graphite_opencl_no_types_check
      && !opencl_calc_max_depth_tab (meta, max_access_depth, 0))
    {
      htab_delete (max_access_depth);

      if (dump_p)
	fprintf (dump_file, "unsupported types\n");

      return false;
    }

  /* Can't parallelize if memory transfer is not reasonable.  */
  if (!flag_graphite_opencl_no_memory_transfer_check
      && !flag_graphite_opencl_cpu
      && !opencl_analyse_data_access_p (code_gen, max_access_depth, meta))
    {
      htab_delete (max_access_depth);

      if (dump_p)
	fprintf (dump_file, "avoiding large memory transfer\n");

      return false;
    }

  htab_delete (max_access_depth);

  if (dump_p)
    fprintf (dump_file, "ok\n");

  return true;
}

#endif
