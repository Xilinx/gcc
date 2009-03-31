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
#include "output.h"
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
#include "params.h"

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "sese.h"
#include "graphite-ppl.h"
#include "graphite.h"
#include "graphite-poly.h"
#include "graphite-data-ref.h"

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

/* Extend the scattering matrix of PBB to MAX_SCATTERING scattering
   dimensions.  */

static void
extend_scattering (poly_bb_p pbb, int max_scattering)
{
  ppl_dimension_type nb_old_dims, nb_new_dims;
  int nb_added_dims, i;

  nb_added_dims = max_scattering - pbb_nb_scattering (pbb); 

  gcc_assert (nb_added_dims >= 0);

  nb_old_dims = pbb_nb_scattering (pbb) + pbb_nb_loops (pbb)
	    + scop_nb_params (PBB_SCOP (pbb));
  nb_new_dims = nb_old_dims + nb_added_dims;

  ppl_insert_dimensions (PBB_TRANSFORMED_SCATTERING (pbb),
			 pbb_nb_scattering (pbb), nb_added_dims);

  for (i = max_scattering - nb_added_dims; i < max_scattering; i++)
    {
      ppl_Constraint_t cstr;
      ppl_Coefficient_t coef;
      ppl_Linear_Expression_t expr;
      Value v;

      value_init (v);
      value_set_si (v, 1);
      ppl_new_Coefficient (&coef);
      ppl_new_Linear_Expression_with_dimension (&expr, nb_new_dims);
      ppl_assign_Coefficient_from_mpz_t (coef, v);
      ppl_Linear_Expression_add_to_coefficient (expr, i, coef);
      ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_EQUAL);
      ppl_Polyhedron_add_constraint (PBB_TRANSFORMED_SCATTERING (pbb), cstr);
      ppl_delete_Coefficient (coef);
      ppl_delete_Linear_Expression (expr);
    }
}

/* All scattering matrices in SCOP will have the same number of scattering
   dimensions.  */

int
unify_scattering_dimensions (scop_p scop)
{
  int i;
  poly_bb_p pbb;
  graphite_dim_t max_scattering = 0;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    max_scattering = MAX (pbb_nb_scattering (pbb), max_scattering);
  
  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    extend_scattering (pbb, max_scattering);

  return max_scattering;
}

/* Prints to FILE the scattering function of PBB.  */

void
print_scattering_function (FILE *file, poly_bb_p pbb)
{
  ppl_print_polyhedron_matrix (file, PBB_TRANSFORMED_SCATTERING (pbb));
}

/* Prints to FILE the scattering functions of every PBB of SCOP.  */

void
print_scattering_functions (FILE *file, scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    print_scattering_function (file, pbb);
}

/* Prints to STDERR the scattering function of PBB.  */

void
debug_scattering_function (poly_bb_p pbb)
{
  print_scattering_function (stderr, pbb);
}

/* Prints to STDERR the scattering functions of every PBB of SCOP.  */

void
debug_scattering_functions (scop_p scop)
{
  print_scattering_functions (stderr, scop);
}

/* Write to file_name.graphite the transforms for SCOP.  */

static void
graphite_write_transforms (scop_p scop)
{
  print_scattering_functions (graphite_out_file, scop);
}

/* Read transforms from file_name.graphite and set the transforms on
   SCOP.  */

static bool 
graphite_read_transforms (scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      ppl_Polyhedron_t newp;
      ppl_read_polyhedron_matrix (&newp, graphite_in_file);
      PBB_TRANSFORMED_SCATTERING (pbb) = newp;
    }

  return true;
}

/* Apply graphite transformations to all the basic blocks of SCOP.  */

bool
apply_poly_transforms (scop_p scop)
{
  bool transform_done = false;

  if (flag_graphite_read)
    transform_done |= graphite_read_transforms (scop);

  /* Generate code even if we did not apply any real transformation.
     This also allows to check the performance for the identity
     transformation: GIMPLE -> GRAPHITE -> GIMPLE
     Keep in mind that CLooG optimizes in control, so the loop structure
     may change, even if we only use -fgraphite-identity.  */ 
  if (flag_graphite_identity)
    transform_done = true;

  if (flag_loop_block)
    gcc_unreachable (); /* Not yet supported.  */

  if (flag_loop_interchange)
    gcc_unreachable (); /* Not yet supported.  */

  if (flag_loop_strip_mine)
    gcc_unreachable (); /* Not yet supported.  */

  if (flag_graphite_write)
    graphite_write_transforms (scop);

  return transform_done;
}

/* Create a new polyhedral black box.  */

void
new_poly_bb (scop_p scop, void *black_box)
{
  poly_bb_p pbb = XNEW (struct poly_bb);

  ppl_new_NNC_Polyhedron_from_space_dimension (&PBB_DOMAIN (pbb), 0, 0);
  PBB_SCOP (pbb) = scop;
  pbb_set_black_box (pbb, black_box);
  PBB_TRANSFORMED_SCATTERING (pbb) = NULL;
  PBB_ORIGINAL_SCATTERING (pbb) = NULL;
  PBB_DRS (pbb) = VEC_alloc (poly_dr_p, heap, 3);
  VEC_safe_push (poly_bb_p, heap, SCOP_BBS (scop), pbb);
}

/* Free polyhedral black box.  */

void
free_poly_bb (poly_bb_p pbb)
{
  ppl_delete_Polyhedron (PBB_DOMAIN (pbb));

  if (PBB_TRANSFORMED_SCATTERING (pbb))
    ppl_delete_Polyhedron (PBB_TRANSFORMED_SCATTERING (pbb));

  if (PBB_ORIGINAL_SCATTERING (pbb))
    ppl_delete_Polyhedron (PBB_ORIGINAL_SCATTERING (pbb));

  VEC_free (poly_dr_p, heap, PBB_DRS (pbb));
  XDELETE (pbb);
}

/* Creates a new SCOP containing REGION.  */

scop_p
new_scop (void *region)
{
  scop_p scop = XNEW (struct scop);

  SCOP_DEP_GRAPH (scop) = NULL;
  scop_set_region (scop, region);
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

/* Print to FILE the domain of PBB.  */

void
print_pbb_domain (FILE *file, poly_bb_p pbb)
{
  ppl_print_polyhedron_matrix (file, PBB_DOMAIN (pbb));
}

/* Print to FILE the domain and scattering function of PBB.  */

void
print_pbb (FILE *file, poly_bb_p pbb)
{
  print_pbb_domain (file, pbb);
  print_scattering_function (file, pbb);
}

/* Print to FILE the SCOP.  */

void
print_scop (FILE *file, scop_p scop)
{
  int i;
  poly_bb_p pbb;

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    print_pbb (file, pbb);
}

/* Print to STDERR the domain of PBB.  */

void
debug_pbb_domain (poly_bb_p pbb)
{
  print_pbb_domain (stderr, pbb);
}

/* Print to FILE the domain and scattering function of PBB.  */

void
debug_pbb (poly_bb_p pbb)
{
  print_pbb (stderr, pbb);
}

/* Print to STDERR the SCOP.  */

void
debug_scop (scop_p scop)
{
  print_scop (stderr, scop);
}


#endif

