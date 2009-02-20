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

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "sese.h"
#include "graphite-ppl.h"
#include "graphite.h"
#include "graphite-poly.h"
#include "graphite-data-ref.h"


/* Compares two poly bbs and returns an integer less than, equal to, or
   greater than zero if the first argument is considered to be respectively
   less than, equal to, or greater than the second. 
   We compare using the lexicographic order of the static schedules.  */

static int 
pbb_compare (const void *p_1, const void *p_2)
{
  const struct poly_bb *const pbb_1
    = *(const struct poly_bb *const*) p_1;
  const struct poly_bb *const pbb_2
    = *(const struct poly_bb *const*) p_2;

  return ppl_lexico_compare_linear_expressions (PBB_STATIC_SCHEDULE (pbb_1),
						PBB_STATIC_SCHEDULE (pbb_2));
}

/* Sort poly bbs in SCOP.  */

static void
graphite_sort_pbbs (scop_p scop)
{
  VEC (poly_bb_p, heap) *bbs = SCOP_BBS (scop);

  qsort (VEC_address (poly_bb_p, bbs),
         VEC_length (poly_bb_p, bbs),
         sizeof (poly_bb_p), pbb_compare);
}

/* Move the loop at index SRC and insert it before index DEST.
   This transformartion is only valid, if the loop nest between i and k is
   perfectly nested. Therefore we do not need to change the static schedule.

   Example:

   for (i = 0; i < 50; i++)
     for (j ...)
       for (k = 5; k < 100; k++)
         A

   To move k before i use:

   graphite_trans_bb_move_loop (A, 2, 0)

   for (k = 5; k < 100; k++)
     for (i = 0; i < 50; i++)
       for (j ...)
         A

   And to move k back:

   graphite_trans_bb_move_loop (A, 0, 2)

   This function does not check the validity of interchanging loops.
   This should be checked before calling this function.  */

static void
graphite_trans_bb_move_loop (poly_bb_p pbb, int src, int dest)
{
  loop_p tmp_loop_p;
  ppl_dimension_type dim, *map;
  int i;

  gcc_assert (src < pbb_nb_loops (pbb)
	      && dest < pbb_nb_loops (pbb));

  tmp_loop_p = VEC_index (loop_p, PBB_LOOPS (pbb), src);
  VEC_ordered_remove (loop_p, PBB_LOOPS (pbb), src);
  VEC_safe_insert (loop_p, heap, PBB_LOOPS (pbb), dest, tmp_loop_p);

  ppl_Polyhedron_space_dimension (PBB_DOMAIN (pbb), &dim);
  map = (ppl_dimension_type *) XNEWVEC (ppl_dimension_type, dim);

  for (i = 0; i < (int) dim; i++)
    map[i] = i;

  /* | "x SRC a b c DEST y" is transformed to
     | "x a b c DEST SRC y".  */
  for (i = src + 1; i <= dest; i++)
    map[i] = i - 1;

  /* | "x DEST a b c SRC y" is transformed to
     | "x SRC DEST a b c y".  */
  for (i = dest; i < src; i++)
    map[i] = i + 1;

  map[src] = dest;

  ppl_Polyhedron_map_space_dimensions (PBB_DOMAIN (pbb), map, dim);
  free (map);
}

/* Strip mines the loop at dimension LOOP of BB with STRIDE.  This
   transform is always valid but not always a performance gain.  */
  
static void
graphite_trans_bb_strip_mine (poly_bb_p pbb, ppl_dimension_type loop,
			      int stride)
{
  ppl_Polyhedron_t ph = ppl_strip_loop (PBB_DOMAIN (pbb), loop, stride);  
  ppl_delete_Polyhedron (PBB_DOMAIN (pbb));
  PBB_DOMAIN (pbb) = ph;

  gcc_assert ((int) loop <= pbb_nb_loops (pbb) - 1);

  /* Update the loops vector.  */
  VEC_safe_insert (loop_p, heap, PBB_LOOPS (pbb), loop, NULL);

  /* Update static schedule.  */
  {
    ppl_dimension_type i, nb_loops = pbb_nb_loops (pbb);
    ppl_Linear_Expression_t new_schedule;
    ppl_Coefficient_t c;

    ppl_new_Coefficient (&c);
    ppl_new_Linear_Expression_with_dimension (&new_schedule, nb_loops + 1);

    for (i = 0; i <= loop; i++)
      {
	ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb), i, c);
	ppl_Linear_Expression_add_to_coefficient (new_schedule, i, c);
      }

    for (i = loop + 1; i <= nb_loops - 2; i++)
      {
	ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb), i, c);
	ppl_Linear_Expression_add_to_coefficient (new_schedule, i + 2, c);
      }

    ppl_delete_Linear_Expression (PBB_STATIC_SCHEDULE (pbb));
    PBB_STATIC_SCHEDULE (pbb) = new_schedule;

    ppl_delete_Coefficient (c);
  }
}

/* Returns true when the strip mining of LOOP_INDEX by STRIDE is
   profitable or undecidable.  GB is the statement around which the
   loops will be strip mined.  */

static bool
strip_mine_profitable_p (poly_bb_p pbb, int stride,
			 int loop_index)
{
  bool res = true;
  edge exit = NULL;
  tree niter;
  loop_p loop;
  long niter_val;

  loop = VEC_index (loop_p, PBB_LOOPS (pbb), loop_index);
  exit = single_exit (loop);

  niter = find_loop_niter (loop, &exit);
  if (niter == chrec_dont_know 
      || TREE_CODE (niter) != INTEGER_CST)
    return true;
  
  niter_val = int_cst_value (niter);

  if (niter_val < stride)
    {
      res = false;
      if (dump_file && (dump_flags & TDF_DETAILS))
	{
	  fprintf (dump_file, "\nStrip Mining is not profitable for loop %d:",
		   loop->num);
	  fprintf (dump_file, "number of iterations is too low.\n");
	}
    }
  
  return res;
}
 
/* Determines when the interchange of LOOP_A and LOOP_B belonging to
   SCOP is legal.  DEPTH is the number of loops around.  */

static bool
is_interchange_valid (scop_p scop ATTRIBUTE_UNUSED, int loop_a ATTRIBUTE_UNUSED,
		      int loop_b ATTRIBUTE_UNUSED, int depth ATTRIBUTE_UNUSED)
{
  return false;
}

/* Loop block the LOOPS innermost loops of GB with stride size STRIDE. 

   Example

   for (i = 0; i <= 50; i++) 
     for (k = 0; k <= 100; k++) 
       for (l = 0; l <= 200; l++) 
         A

   To strip mine the two innermost loops with stride = 4 call:

   graphite_trans_bb_block (A, 4, 2) 

   for (i = 0; i <= 50; i++) 
     for (kk = 0; kk <= 100; kk+=4) 
       for (ll = 0; ll <= 200; ll+=4) 
         for (k = kk; k <= min (100, kk + 3); k++) 
           for (l = ll; l <= min (200, ll + 3); l++) 
             A
*/

static bool
graphite_trans_bb_block (poly_bb_p pbb, int stride, int loops)
{
  int i, j;
  int nb_loops = pbb_nb_loops (pbb);
  int start = nb_loops - loops;
  scop_p scop = PBB_SCOP (pbb);

  gcc_assert (sese_contains_loop (SCOP_REGION (scop),
				  gbb_loop (PBB_BLACK_BOX (pbb))));

  for (i = start ; i < nb_loops; i++)
    for (j = i + 1; j < nb_loops; j++)
      if (!is_interchange_valid (scop, i, j, nb_loops))
	{
	  if (dump_file && (dump_flags & TDF_DETAILS))
	    fprintf (dump_file,
		     "\nInterchange not valid for loops %d and %d:\n", i, j);
	  return false;
	}
      else if (dump_file && (dump_flags & TDF_DETAILS))
	fprintf (dump_file,
		 "\nInterchange valid for loops %d and %d:\n", i, j);

  /* Check if strip mining is profitable for every loop.  */
  for (i = 0; i < nb_loops - start; i++)
    if (!strip_mine_profitable_p (pbb, stride, start + i))
      return false;

  /* Strip mine loops.  */
  for (i = 0; i < nb_loops - start; i++)
    graphite_trans_bb_strip_mine (pbb, start + 2 * i, stride);

  /* Interchange loops.  */
  for (i = 1; i < nb_loops - start; i++)
    graphite_trans_bb_move_loop (pbb, start + 2 * i, start + i);

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "\nLoops containing BB %d will be loop blocked.\n",
	     GBB_BB (PBB_BLACK_BOX (pbb))->index);

  return true;
}

/* Loop block LOOPS innermost loops of a loop nest.  BBS represent the
   basic blocks that belong to the loop nest to be blocked.  */

static bool
graphite_trans_loop_block (VEC (poly_bb_p, heap) *bbs, int loops)
{
  poly_bb_p pbb;
  int i;
  bool transform_done = false;

  /* TODO: - Calculate the stride size automatically.  */
  int stride_size = 64;

  for (i = 0; VEC_iterate (poly_bb_p, bbs, i, pbb); i++)
    transform_done |= graphite_trans_bb_block (pbb, stride_size, loops);

  return transform_done;
}

/* Return the maximal loop depth in SCOP.  */

int
scop_max_loop_depth (scop_p scop)
{
  int i;
  poly_bb_p pbb;
  int max_nb_loops = 0;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++) 
    {    
      int nb_loops = pbb_nb_loops (pbb);
      if (max_nb_loops < nb_loops)
        max_nb_loops = nb_loops;
    }    

  return max_nb_loops;
}

/* Loop block all basic blocks of SCOP.  Return false when the
   transform is not performed.  */

static bool
graphite_trans_scop_block (scop_p scop)
{
  poly_bb_p pbb;
  int i, j;
  int last_nb_loops;
  int nb_loops;
  bool perfect = true;
  bool transform_done = false;
  VEC (poly_bb_p, heap) *bbs = VEC_alloc (poly_bb_p, heap, 3);
  int max_schedule = scop_max_loop_depth (scop) + 1;
  ppl_Linear_Expression_t last_schedule;
  ppl_Coefficient_t c;
  Value v0, v1;

  if (VEC_length (poly_bb_p, SCOP_BBS (scop)) == 0)
    return false;

  value_init (v0);
  value_init (v1);
  ppl_new_Coefficient (&c);

  /* Get the data of the first bb.  */
  pbb = VEC_index (poly_bb_p, SCOP_BBS (scop), 0);
  last_nb_loops = pbb_nb_loops (pbb);

  ppl_new_Linear_Expression_with_dimension (&last_schedule, max_schedule);
  ppl_assign_Linear_Expression_from_Linear_Expression
    (last_schedule, PBB_STATIC_SCHEDULE (pbb));

  VEC_safe_push (poly_bb_p, heap, bbs, pbb);
  
  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      /* We did the first bb before.  */
      if (i == 0)
        continue;

      nb_loops = pbb_nb_loops (pbb);

      /* If the number of loops is unchanged and only the last element of the
         schedule changes, we stay in the loop nest.  */
      if (nb_loops == last_nb_loops)
	{
	  ppl_Linear_Expression_coefficient (last_schedule, nb_loops + 1, c);
	  ppl_Coefficient_to_mpz_t (c, v0);
	  ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb),
					     nb_loops + 1, c);
	  ppl_Coefficient_to_mpz_t (c, v1);

	  if (value_ne (v0, v1))
	    {
	      VEC_safe_push (poly_bb_p, heap, bbs, pbb);
	      continue;
	    }
	}

      /* Otherwise, we left the innermost loop. So check, if the last bb was in
         a perfect loop nest and how many loops are contained in this perfect
         loop nest. 
         
         Count the number of zeros from the end of the schedule. They are the
         number of surrounding loops.

         Example:
         last_bb  2 3 2 0 0 0 0 3
         bb       2 4 0
	 <------  j = 4
        
         last_bb  2 3 2 0 0 0 0 3
         bb       2 3 2 0 1
	 <--  j = 2

         If there is no zero, there were other bbs in outer loops and the loop
         nest is not perfect.  */
      for (j = last_nb_loops - 1; j >= 0; j--)
        {
	  ppl_Linear_Expression_coefficient (last_schedule, j, c);
	  ppl_Coefficient_to_mpz_t (c, v0);
	  value_set_si (v1, 0);

          if (value_ne (v0, v1))
	    {
	      ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb),
						 j, c);
	      ppl_Coefficient_to_mpz_t (c, v0);
	      value_set_si (v1, 1);

	      if (j <= nb_loops && value_eq (v0, v1))
		{
		  j--;
		  break;
		}
	    }
        }
      
      j++;

      /* Found perfect loop nest.  */
      if (perfect && last_nb_loops - j >= 2)
        transform_done |= graphite_trans_loop_block (bbs, last_nb_loops - j);
 
      /* Check if we start with a new loop.

         Example:
  
         last_bb  2 3 2 0 0 0 0 3
         bb       2 3 2 0 0 1 0
        
         Here we start with the loop "2 3 2 0 0 1" 

         last_bb  2 3 2 0 0 0 0 3
         bb       2 3 2 0 0 1 

         But here not, so the loop nest can never be perfect.  */

      ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb),
					 nb_loops, c);
      ppl_Coefficient_to_mpz_t (c, v0);
      value_set_si (v1, 0);
      perfect = value_eq (v0, v1);

      /* Update the last_bb infos.  We do not do that for the bbs in the same
         loop, as the data we use is not changed.  */
      last_nb_loops = nb_loops;

      ppl_assign_Linear_Expression_from_Linear_Expression
	(last_schedule, PBB_STATIC_SCHEDULE (pbb));

      VEC_truncate (poly_bb_p, bbs, 0);
      VEC_safe_push (poly_bb_p, heap, bbs, pbb);
    }

  /* Check if the last loop nest was perfect.  It is the same check as above,
     but the comparison with the next bb is missing.  */
  for (j = last_nb_loops - 1; j >= 0; j--)
    {
      ppl_Linear_Expression_coefficient (last_schedule, j, c);
      ppl_Coefficient_to_mpz_t (c, v0);
      value_set_si (v1, 0);

      if (value_ne (v0, v1))
	{
	  j--;
	  break;
	}
    }

  j++;

  /* Found perfect loop nest.  */
  if (last_nb_loops - j >= 2)
    transform_done |= graphite_trans_loop_block (bbs, last_nb_loops - j);
  VEC_free (poly_bb_p, heap, bbs);

  ppl_delete_Linear_Expression (last_schedule);
  ppl_delete_Coefficient (c);
  value_clear (v0);
  value_clear (v1);
  return transform_done;
}

/* Apply graphite transformations to all the basic blocks of SCOP.  */

bool
graphite_apply_transformations (scop_p scop)
{
  bool transform_done = false;

  /* Sort the list of bbs.  Keep them always sorted.  */

  graphite_sort_pbbs (scop);

  if (flag_loop_block)
    transform_done = graphite_trans_scop_block (scop);

  /* Generate code even if we did not apply any real transformation.
     This also allows to check the performance for the identity
     transformation: GIMPLE -> GRAPHITE -> GIMPLE
     Keep in mind that CLooG optimizes in control, so the loop structure
     may change, even if we only use -fgraphite-identity.  */ 
  if (flag_graphite_identity)
    transform_done = true;

  return transform_done;
}

/* Create a new polyhedral black box.  */

void
new_poly_bb (scop_p scop, gimple_bb_p black_box)
{
  poly_bb_p pbb = XNEW (struct poly_bb);

  PBB_LOOPS (pbb) = NULL;
  PBB_STATIC_SCHEDULE (pbb) = NULL;
  ppl_new_NNC_Polyhedron_from_space_dimension (&PBB_DOMAIN (pbb), 0, 0);
  PBB_SCOP (pbb) = scop;
  PBB_BLACK_BOX (pbb) = black_box;
  VEC_safe_push (poly_bb_p, heap, SCOP_BBS (scop), pbb);
}

/* Free polyhedral black box.  */

void
free_poly_bb (poly_bb_p pbb)
{
  ppl_delete_Polyhedron (PBB_DOMAIN (pbb));

  if (PBB_STATIC_SCHEDULE (pbb))
    ppl_delete_Linear_Expression (PBB_STATIC_SCHEDULE (pbb));

  VEC_free (loop_p, heap, PBB_LOOPS (pbb));

  XDELETE (pbb);
}

/* Creates a new SCOP containing REGION.  */

scop_p
new_scop (sese region)
{
  scop_p scop = XNEW (struct scop);

  SCOP_DEP_GRAPH (scop) = NULL;
  SCOP_REGION (scop) = region;
  SCOP_BBS (scop) = VEC_alloc (poly_bb_p, heap, 3);

  return scop;
}

/* Deletes SCOP.  */

void
free_scop (scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    free_poly_bb (pbb);

  VEC_free (poly_bb_p, heap, SCOP_BBS (scop));

  XDELETE (scop);
}

/* Converts the graphite scheduling function into a cloog scattering
   polyhedron.  The scattering polyhedron is used to add constraints
   that limit the possible cloog output to valid programs in respect
   to the scheduling function.

   SCATTERING_DIMENSIONS specifies the number of scattering
   dimensions.  CLooG 0.15.0 and previous versions require, that all
   scattering functions of one CloogProgram have the same number of
   scattering dimensions, therefore we allow to specify it.  This
   restriction should be removed in future versions of CLooG.

   The scattering polyhedron consists of these dimensions: scattering,
   loop_iterators, parameters.

   Example:

   | scattering_dimensions = 5
   | used_scattering_dimensions = 3
   | nb_iterators = 1 
   | scop_nb_params = 2
   |
   | Schedule:
   |   i
   | 4 5
   |
   | Scattering polyhedron:
   |
   | scattering: {s1, s2, s3, s4, s5}
   | loop_iterators: {i}
   | parameters: {p1, p2}
   |
   | s1  s2  s3  s4  s5  i   p1  p2  1 
   | 1   0   0   0   0   0   0   0  -4  = 0
   | 0   1   0   0   0  -1   0   0   0  = 0
   | 0   0   1   0   0   0   0   0  -5  = 0  */

ppl_Polyhedron_t
schedule_to_scattering (poly_bb_p pbb, int scattering_dimensions) 
{
  int i;
  scop_p scop = PBB_SCOP (pbb);
  int nb_iterators = pbb_nb_loops (pbb);
  int used_scattering_dimensions = nb_iterators * 2 + 1;
  int nb_params = scop_nb_params (scop);
  int col_iter_offset = scattering_dimensions;
  ppl_Polyhedron_t ph;
  ppl_Coefficient_t c;
  ppl_dimension_type dim = scattering_dimensions + nb_iterators + nb_params;
  Value v;

  gcc_assert (scattering_dimensions >= used_scattering_dimensions);

  value_init (v);
  ppl_new_Coefficient (&c);
  ppl_new_NNC_Polyhedron_from_space_dimension (&ph, dim, 0);

  for (i = 0; i < scattering_dimensions; i++)
    {
      ppl_Constraint_t cstr;
      ppl_Linear_Expression_t expr;

      ppl_new_Linear_Expression_with_dimension (&expr, dim);
      value_set_si (v, 1);
      ppl_assign_Coefficient_from_mpz_t (c, v);
      ppl_Linear_Expression_add_to_coefficient (expr, i, c);

      /* Textual order inside this loop.  */
      if (i < used_scattering_dimensions
	  && (i % 2) == 0)
	{
	  ppl_Linear_Expression_coefficient (PBB_STATIC_SCHEDULE (pbb), i / 2, c);
	  ppl_Coefficient_to_mpz_t (c, v);
	  value_oppose (v, v);
	  ppl_assign_Coefficient_from_mpz_t (c, v);
	  ppl_Linear_Expression_add_to_inhomogeneous (expr, c);
	}

      /* Iterations of this loop.  */
      if (i < used_scattering_dimensions
	  && (i % 2) == 1)
	{
	  int loop = (i - 1) / 2;
	  value_set_si (v, -1);
	  ppl_assign_Coefficient_from_mpz_t (c, v);
	  ppl_Linear_Expression_add_to_coefficient (expr,
						    col_iter_offset + loop, c);
	}
      
      ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_EQUAL);
      ppl_Polyhedron_add_constraint (ph, cstr);
      ppl_delete_Linear_Expression (expr);
      ppl_delete_Constraint (cstr);
    }

  value_clear (v);
  ppl_delete_Coefficient (c);
  return ph;
}

#endif

