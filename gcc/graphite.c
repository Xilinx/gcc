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

/* Debug the list of old induction variables for this SCOP.  */

void
debug_oldivs (sese region)
{
  int i;
  name_tree oldiv;

  fprintf (stderr, "Old IVs:");

  for (i = 0; VEC_iterate (name_tree, SESE_OLDIVS (region), i, oldiv); i++)
    {
      fprintf (stderr, "(");
      print_generic_expr (stderr, oldiv->t, 0);
      fprintf (stderr, ", %s, %d)\n", oldiv->name, oldiv->loop->num);
    }
  fprintf (stderr, "\n");
}

/* Debug the loops around basic block GB.  */

void
debug_loop_vec (poly_bb_p pbb)
{
  int i;
  loop_p loop;

  fprintf (stderr, "Loop Vec:");

  for (i = 0; VEC_iterate (loop_p, PBB_LOOPS (pbb), i, loop); i++)
    fprintf (stderr, "%d: %d, ", i, loop ? loop->num : -1);

  fprintf (stderr, "\n");
}

/* Pretty print all SCoPs in DOT format and mark them with different colors.
   If there are not enough colors, paint later SCoPs gray.
   Special nodes:
   - "*" after the node number: entry of a SCoP,
   - "#" after the node number: exit of a SCoP,
   - "()" entry or exit not part of SCoP.  */

static void
dot_all_scops_1 (FILE *file, VEC (scop_p, heap) *scops)
{
  basic_block bb;
  edge e;
  edge_iterator ei;
  scop_p scop;
  const char* color;
  int i;

  /* Disable debugging while printing graph.  */
  int tmp_dump_flags = dump_flags;
  dump_flags = 0;

  fprintf (file, "digraph all {\n");

  FOR_ALL_BB (bb)
    {
      int part_of_scop = false;

      /* Use HTML for every bb label.  So we are able to print bbs
         which are part of two different SCoPs, with two different
         background colors.  */
      fprintf (file, "%d [label=<\n  <TABLE BORDER=\"0\" CELLBORDER=\"1\" ",
                     bb->index);
      fprintf (file, "CELLSPACING=\"0\">\n");

      /* Select color for SCoP.  */
      for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
	if (bb_in_sese_p (bb, SCOP_REGION (scop))
	    || (SCOP_EXIT (scop) == bb)
	    || (SCOP_ENTRY (scop) == bb))
	  {
	    switch (i % 17)
	      {
	      case 0: /* red */
		color = "#e41a1c";
		break;
	      case 1: /* blue */
		color = "#377eb8";
		break;
	      case 2: /* green */
		color = "#4daf4a";
		break;
	      case 3: /* purple */
		color = "#984ea3";
		break;
	      case 4: /* orange */
		color = "#ff7f00";
		break;
	      case 5: /* yellow */
		color = "#ffff33";
		break;
	      case 6: /* brown */
		color = "#a65628";
		break;
	      case 7: /* rose */
		color = "#f781bf";
		break;
	      case 8:
		color = "#8dd3c7";
		break;
	      case 9:
		color = "#ffffb3";
		break;
	      case 10:
		color = "#bebada";
		break;
	      case 11:
		color = "#fb8072";
		break;
	      case 12:
		color = "#80b1d3";
		break;
	      case 13:
		color = "#fdb462";
		break;
	      case 14:
		color = "#b3de69";
		break;
	      case 15:
		color = "#fccde5";
		break;
	      case 16:
		color = "#bc80bd";
		break;
	      default: /* gray */
		color = "#999999";
	      }

	    fprintf (file, "    <TR><TD WIDTH=\"50\" BGCOLOR=\"%s\">", color);
        
	    if (!bb_in_sese_p (bb, SCOP_REGION (scop)))
	      fprintf (file, " ("); 

	    if (bb == SCOP_ENTRY (scop)
		&& bb == SCOP_EXIT (scop))
	      fprintf (file, " %d*# ", bb->index);
	    else if (bb == SCOP_ENTRY (scop))
	      fprintf (file, " %d* ", bb->index);
	    else if (bb == SCOP_EXIT (scop))
	      fprintf (file, " %d# ", bb->index);
	    else
	      fprintf (file, " %d ", bb->index);

	    if (!bb_in_sese_p (bb, SCOP_REGION (scop)))
	      fprintf (file, ")");

	    fprintf (file, "</TD></TR>\n");
	    part_of_scop  = true;
	  }

      if (!part_of_scop)
        {
          fprintf (file, "    <TR><TD WIDTH=\"50\" BGCOLOR=\"#ffffff\">");
          fprintf (file, " %d </TD></TR>\n", bb->index);
        }

      fprintf (file, "  </TABLE>>, shape=box, style=\"setlinewidth(0)\"]\n");
    }

  FOR_ALL_BB (bb)
    {
      FOR_EACH_EDGE (e, ei, bb->succs)
	      fprintf (file, "%d -> %d;\n", bb->index, e->dest->index);
    }

  fputs ("}\n\n", file);

  /* Enable debugging again.  */
  dump_flags = tmp_dump_flags;
}

/* Display all SCoPs using dotty.  */

void
dot_all_scops (VEC (scop_p, heap) *scops)
{
  /* When debugging, enable the following code.  This cannot be used
     in production compilers because it calls "system".  */
#if 1
  FILE *stream = fopen ("/tmp/allscops.dot", "w");
  gcc_assert (stream);

  dot_all_scops_1 (stream, scops);
  fclose (stream);

  system ("dotty /tmp/allscops.dot");
#else
  dot_all_scops_1 (stderr, scops);
#endif
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

  if (number_of_loops () <= 1)
    return;

  recompute_all_dominators ();

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "Graphite loop transformations \n");

  initialize_original_copy_tables ();
  cloog_initialize ();
  build_scops (&scops);

  if (dump_file && (dump_flags & TDF_DETAILS))
    fprintf (dump_file, "\nnumber of SCoPs: %d\n",
	     VEC_length (scop_p, scops));

  for (i = 0; VEC_iterate (scop_p, scops, i, scop); i++)
    {
      build_scop_bbs (scop);
      if (!build_sese_loop_nests (SCOP_REGION (scop)))
	continue;

      if (scop_contains_non_iv_scalar_phi_nodes (scop))
	continue;

      build_bb_loops (scop);
      build_sese_conditions (SCOP_REGION (scop));
      find_scop_parameters (scop);
      build_scop_iteration_domain (scop);
      add_conditions_to_constraints (scop);
      build_scop_canonical_schedules (scop);
      build_scop_data_accesses (scop);
      
      if (graphite_apply_transformations (scop))
	transform_done = gloog (scop);
#ifdef ENABLE_CHECKING
      else
	{
	  cloog_prog_clast pc = scop_to_clast (scop);
	  cloog_clast_free (pc.stmt);
	  cloog_program_free (pc.prog);
	}
#endif
    }

  /* Cleanup.  */
  if (transform_done)
    cleanup_tree_cfg ();

  free_scops (scops);
  cloog_finalize ();
  free_original_copy_tables ();
}

#else /* If Cloog is not available: #ifndef HAVE_cloog.  */

void
graphite_transform_loops (void)
{
  sorry ("Graphite loop optimizations cannot be used");
}

#endif
