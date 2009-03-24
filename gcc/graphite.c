/* Gimple Represented as Polyhedra.
   Copyright (C) 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
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

/* This pass converts GIMPLE to GRAPHITE, performs some loop
   transformations and then converts the resulting representation back
   to GIMPLE.  

   An early description of this pass can be found in the GCC Summit'06
   paper "GRAPHITE: Polyhedral Analyses and Optimizations for GCC".
   The wiki page http://gcc.gnu.org/wiki/Graphite contains pointers to
   the related work.  

   One important document to read is CLooG's internal manual:
   http://repo.or.cz/w/cloog-ppl.git?a=blob_plain;f=doc/cloog.texi;hb=HEAD
   that describes the data structure of loops used in this file, and
   the functions that are used for transforming the code.  */

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

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "graphite-ppl.h"
#include "graphite.h"
#include "graphite-poly.h"
#include "graphite-scop-detection.h"
#include "graphite-data-ref.h"
#include "graphite-clast-to-gimple.h"
#include "graphite-sese-to-poly.h"

/* Initialize graphite: when there are no loops returns false.  */

static bool
graphite_initialize (void)
{
  if (number_of_loops () <= 1)
    return false;

  recompute_all_dominators ();
  initialize_original_copy_tables ();
  cloog_initialize ();

  if (dump_file && dump_flags)
    dump_function_to_file (current_function_decl, dump_file, dump_flags);

  return true;
}

/* Finalize graphite: perform cleanup when TRANSFORM_DONE.  */

static void
graphite_finalize (bool transform_done)
{
  if (transform_done)
    cleanup_tree_cfg ();

  cloog_finalize ();
  free_original_copy_tables ();

  if (dump_file && dump_flags)
    dump_function_to_file (current_function_decl, dump_file, dump_flags);
}

/* Perform a set of linear transforms on the loops of the current
   function.  */

void
graphite_transform_loops (void)
{
  int i;
  scop_p scop;
  bool transform_done = false;
  VEC (scop_p, heap) *scops = NULL;

  if (!graphite_initialize ())
    return;

  build_scops (&scops);

  for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
    if (build_poly_scop (scop))
      {
	if (apply_poly_transforms (scop))
	  transform_done |= gloog (scop);
	else  
	  check_poly_representation (scop);
      }

  free_scops (scops);
  graphite_finalize (transform_done);
}

#else /* If Cloog is not available: #ifndef HAVE_cloog.  */

void
graphite_transform_loops (void)
{
  sorry ("Graphite loop optimizations cannot be used");
}

#endif
