// Copyright (C) 2009 Free Software Foundation, Inc.
// Contributed by Jan Sjodin <jan.sjodin@amd.com>.

// This file is part of the Polyhedral Compilation Package Library (libpcp).

// Libpcp is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.

// Libpcp is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
// more details.

// You should have received a copy of the GNU Lesser General Public License 
// along with libpcp; see the file COPYING.LIB.  If not, write to the
// Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.  

// As a special exception, if you link this library with other files, some
// of which are compiled with GCC, to produce an executable, this library
// does not by itself cause the resulting executable to be covered by the
// GNU General Public License.  This exception does not however invalidate
// any other reasons why the executable file might be covered by the GNU
// General Public License.  

#include "pcp_cloog_gateway.h"
#include "cloog/cloog.h"
#include "ppl_c.h"

class PcpBlackBox;
#if 0
CloogProgram* 
PcpCloogGateway::buildCloogProgram(PcpPolyScop* polyScop)
{
  int i;
  int max_nb_loops = 0; //scop_max_loop_depth (scop);
  PcpBlackBox* pbb;
  CloogLoop *loop_list = NULL;
  CloogBlockList *block_list = NULL;
  CloogDomainList *scattering = NULL;
  int nbs = 2 * max_nb_loops + 1;
  int *scaldims = (int *) malloc (nbs * (sizeof (int)));

  build_scop_context (polyScop, prog);
  nbs = unify_scattering_dimensions (scop);
  cloog_program_set_nb_scattdims (prog, nbs);
  initialize_cloog_names (scop, prog);

  for (i = 0; VEC_iterate (poly_bb_p, SCOP_BBS (scop), i, pbb); i++)
    {
      /* Build new block.  */
      CloogStatement *stmt = cloog_statement_alloc
	(GBB_BB (PBB_BLACK_BOX (pbb))->index);
      CloogBlock *block = cloog_block_alloc (stmt, 0, NULL,
					     pbb_nb_loops (pbb));

      cloog_statement_set_usr (stmt, pbb);

      /* Build loop list.  */
      {
        CloogLoop *new_loop_list = cloog_loop_malloc ();
        cloog_loop_set_next (new_loop_list, loop_list);
        cloog_loop_set_domain
	  (new_loop_list,
	   new_Cloog_Domain_from_ppl_Pointset_Powerset (PBB_DOMAIN (pbb)));
        cloog_loop_set_block (new_loop_list, block);
        loop_list = new_loop_list;
      }

      /* Build block list.  */
      {
        CloogBlockList *new_block_list = cloog_block_list_malloc ();

        cloog_block_list_set_next (new_block_list, block_list);
        cloog_block_list_set_block (new_block_list, block);
        block_list = new_block_list;
      }

      /* Build scattering list.  */
      {
        /* XXX: Replace with cloog_domain_list_alloc(), when available.  */
        CloogDomainList *new_scattering
	  = (CloogDomainList *) xmalloc (sizeof (CloogDomainList));
        ppl_Polyhedron_t scat;
	CloogDomain *dom;

	scat = PBB_TRANSFORMED_SCATTERING (pbb);
	dom = new_Cloog_Domain_from_ppl_Polyhedron (scat);

        cloog_set_next_domain (new_scattering, scattering);
        cloog_set_domain (new_scattering, dom);
        scattering = new_scattering;
      }
    }

  cloog_program_set_loop (prog, loop_list);
  cloog_program_set_blocklist (prog, block_list);

  for (i = 0; i < nbs; i++)
    scaldims[i] = 0 ;

  cloog_program_set_scaldims (prog, scaldims);

  /* Extract scalar dimensions to simplify the code generation problem.  */
  cloog_program_extract_scalars (prog, scattering);

  /* Apply scattering.  */
  cloog_program_scatter (prog, scattering);
  free_scattering (scattering);

  /* Iterators corresponding to scalar dimensions have to be extracted.  */
  cloog_names_scalarize (cloog_program_names (prog), nbs,
			 cloog_program_scaldims (prog));
  
  /* Free blocklist.  */
  {
    CloogBlockList *next = cloog_program_blocklist (prog);
	
    while (next)
      {
        CloogBlockList *toDelete = next;
        next = cloog_block_list_next (next);
        cloog_block_list_set_next (toDelete, NULL);
        cloog_block_list_set_block (toDelete, NULL);
        cloog_block_list_free (toDelete);
      }
    cloog_program_set_blocklist (prog, NULL);
  }
}

#endif
