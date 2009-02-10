/* Dependence analysis for Graphite.
   Copyright (C) 2007, 2008 Free Software Foundation, Inc.
   Contributed by Konrad Trifunovic <konrad.trifunovic@inria.fr>.

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
#include "pointer-set.h"
#include "gimple.h"

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "graphite.h"


/* Copy M, add a row of zeros at the end of the copy, and return the
   new matrix.  */

static CloogMatrix *
graphite_add_a_null_row (CloogMatrix *m) 
{
  int i,j;
  CloogMatrix *res = cloog_matrix_alloc (m->NbRows+1, m->NbColumns);

  for (i = 0; i < m->NbRows; i++)
    for (j = 0; j < m->NbColumns; j++)
      value_assign (res->p[i][j], m->p[i][j]);

  for (j = 0; j < m->NbColumns; j++)
    value_set_si (res->p[i][j],0);  

  return res;
} 

/* Returns a matrix representing the data dependence between memory
   accesses A and B in the context of SCOP.  */

static CloogMatrix *
graphite_initialize_dependence_polyhedron (scop_p scop,
					   graphite_bb_p gb1,
					   graphite_bb_p gb2,   
					   struct data_reference *a, 
					   struct data_reference *b)
{
  int nb_cols, nb_rows, nb_params, nb_iter1, nb_iter2;
  int row, col, offset;
  CloogMatrix *domain1, *domain2;
  CloogMatrix *dep_constraints;
  lambda_vector access_row_vector;
  Value tmp;

  domain1 = new_Cloog_Matrix_from_ppl_Constraint_System (GBB_DOMAIN (gb1));
  domain2 = new_Cloog_Matrix_from_ppl_Constraint_System (GBB_DOMAIN (gb2));

  /* Adding 2 columns: one for the eq/neq column, one for constant
     term.  */
  nb_params = scop_nb_params (scop);
  nb_iter1 = domain1->NbColumns - 2 - nb_params;
  nb_iter2 = domain2->NbColumns - 2 - nb_params;

  nb_cols = nb_iter1 + nb_iter2 + scop_nb_params (scop) + 2;
  nb_rows = domain1->NbRows + domain2->NbRows + DR_NUM_DIMENSIONS (a) 
            + 2 * MIN (nb_iter1, nb_iter2);
  dep_constraints = cloog_matrix_alloc (nb_rows, nb_cols);

  /* Initialize dependence polyhedron.  */
  for (row = 0; row < dep_constraints->NbRows ; row++)
    for (col = 0; col < dep_constraints->NbColumns; col++)
      value_init (dep_constraints->p[row][col]);

  /* Copy the iterator part of Ds (domain of S statement), with eq/neq
     column.  */
  for (row = 0; row < domain1->NbRows; row++)
    for (col = 0; col <= nb_iter1; col++)
      value_assign (dep_constraints->p[row][col], domain1->p[row][col]);

  /* Copy the parametric and constant part of Ds.  */
  for (row = 0; row < domain1->NbRows; row++)
    {
      value_assign (dep_constraints->p[row][nb_cols-1],
		    domain1->p[row][domain1->NbColumns - 1]);
      for (col = 1; col <= nb_params; col++)
	value_assign (dep_constraints->p[row][col + nb_iter1 + nb_iter2],
		      domain1->p[row][col + nb_iter1]);
    }

  /* Copy the iterator part of Dt (domain of T statement), without eq/neq column.  */
  for (row = 0; row < domain2->NbRows; row++)
    for (col = 1; col <= nb_iter2; col++)
      value_assign (dep_constraints->p[row + domain1->NbRows][col + nb_iter2],
		    domain2->p[row][col]);
  
  /* Copy the eq/neq column of Dt to dependence polyhedron.  */
  for (row = 0; row < domain2->NbRows; row++)
    value_assign (dep_constraints->p[row + domain1->NbRows][0], domain2->p[row][0]);

  /* Copy the parametric and constant part of Dt.  */
  for (row = 0; row < domain2->NbRows; row++)
    {
      value_assign (dep_constraints->p[row + domain1->NbRows][nb_cols-1],
		    domain1->p[row][domain2->NbColumns - 1]);
      for (col = 1; col <= nb_params; col++)
        value_assign (dep_constraints->p[row + domain1->NbRows][col + nb_iter1 + nb_iter2],
                      domain2->p[row][col + nb_iter2]);
    }

  /* Copy Ds access matrix.  */
  for (row = 0; VEC_iterate (lambda_vector, AM_MATRIX (DR_ACCESS_MATRIX (a)),
			     row, access_row_vector); row++)
    {
      for (col = 1; col <= nb_iter1; col++)
	value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][col],
		      access_row_vector[col]);              

      for (col = 1; col <= nb_params; col++)
        value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows]
                                        [col + nb_iter1 + nb_iter2], 
                      access_row_vector[col + nb_iter1]);
      
      value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_cols-1], 
                    access_row_vector[ref_nb_loops (a) - 1]);
      /* TODO: do not forget about parametric part.  */
    }

  value_init (tmp);
  offset = domain1->NbRows + domain2->NbRows;

  /* Copy -Dt access matrix.  */
  for (row = 0; VEC_iterate (lambda_vector, AM_MATRIX (DR_ACCESS_MATRIX (b)),
			     row, access_row_vector); row++)
    {
      for (col = 1; col <= nb_iter2; col++)
	value_set_si (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_iter1 + col], 
		      -access_row_vector[col]);              
      
      for (col = 1; col <= nb_params; col++)
        {
          value_set_si (tmp, access_row_vector[col + nb_iter2]);

          value_subtract (dep_constraints->p[row + offset][col + nb_iter1 + nb_iter2],
                          dep_constraints->p[row + offset][col + nb_iter1 + nb_iter2],
                          tmp);
        }
      
      value_set_si (tmp, access_row_vector[ref_nb_loops (b) - 1]);
      value_subtract (dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_cols-1],
                      dep_constraints->p[row + domain1->NbRows + domain2->NbRows][nb_cols-1],
                      tmp);
    }

  value_clear (tmp);

  cloog_matrix_free (domain1);
  cloog_matrix_free (domain2);
  return dep_constraints;
}

/* Returns true when the last row of DOMAIN polyhedron is zero.  */

static bool 
is_empty_polyhedron (CloogDomain *domain)
{
  return cloog_domain_isempty (domain);
}

/* Returns a new dependence polyhedron for data references A and B.  */

static struct data_dependence_polyhedron *
initialize_data_dependence_polyhedron (bool loop_carried,
                                       CloogDomain *domain,
                                       unsigned level,
                                       struct data_reference *a,
                                       struct data_reference *b)
{
  struct data_dependence_polyhedron *res;

  res = XNEW (struct data_dependence_polyhedron);
  res->a = a;
  res->b = b;
  res->loop_carried = loop_carried;
  res->level = level;

  if (loop_carried)
    res->polyhedron = domain; 
  else
    res->polyhedron = NULL;

  return res;
}

/* Returns true if statement A, contained in basic block GB_A,
   precedes statement B, contained in basic block GB_B.  The decision
   is based on static schedule of basic blocks and relative position
   of statements.  */

static bool 
statement_precedes_p (graphite_bb_p gb_a,
                      gimple a,
                      graphite_bb_p gb_b,
                      gimple b,
                      unsigned p)
{
  gimple_stmt_iterator gsi;
  bool statm_a_found;

  if (GBB_STATIC_SCHEDULE (gb_a)[p] < GBB_STATIC_SCHEDULE (gb_b)[p])
    return true;

  else if (GBB_STATIC_SCHEDULE (gb_a)[p] == GBB_STATIC_SCHEDULE (gb_b)[p])
    {
      statm_a_found = false;

      for (gsi = gsi_start_bb (GBB_BB (gb_a)); !gsi_end_p (gsi); gsi_next (&gsi))
        {
          if (gsi_stmt (gsi) == a)
            {
              statm_a_found = true;
              continue;
            }

          if (statm_a_found && gsi_stmt (gsi) == b)
            return true;
        }
    }

  return false;
}

/* Returns the dependence polyhedron for the dependence between data
   references A and B. */

struct data_dependence_polyhedron *
graphite_test_dependence (scop_p scop, graphite_bb_p gb1, graphite_bb_p gb2,
                          struct data_reference *a, struct data_reference *b)
{
  unsigned i, j, row;
  signed p;
  CloogMatrix *dep_constraints = NULL, *temp_matrix = NULL;
  CloogDomain *simplified;
  unsigned iter_vector_dim = MIN (nb_loops_around_gb (gb1),
				  nb_loops_around_gb (gb2));

  for (i = 1; i <= 2 * iter_vector_dim + 1; i++)
    {
      /* S - gb1 */
      /* T - gb2 */
      /* S -> T, T - S >=1 */
      /* p is alternating sequence 0,1,-1,2,-2,... */
      p = (i / 2) * (1 - (i % 2) * 2);

      if (p == 0)
	dep_constraints = graphite_initialize_dependence_polyhedron (scop, gb1,
								     gb2, a, b);

      else if (p > 0)
	{
	  /* Assert B0, B1, ..., Bp-1 satisfy the equality.  */
        
	  for (j = 0; j < iter_vector_dim; j++)
	    {
	      temp_matrix = graphite_add_a_null_row (dep_constraints);
       
	      row = j + dep_constraints->NbRows - iter_vector_dim;           
	      value_set_si (temp_matrix->p[row][0], 1);
	      value_oppose (temp_matrix->p[row][p],
			    GBB_DYNAMIC_SCHEDULE (gb1)->p[j][p - 1]);
	      value_assign (temp_matrix->p[row]
			    [nb_loops_around_gb (gb1) + p],
			    GBB_DYNAMIC_SCHEDULE (gb2)->p[j][p - 1]);
	      value_set_si (temp_matrix->p[row][temp_matrix->NbColumns - 1], -1);

	      simplified = cloog_domain_matrix2domain (temp_matrix);

	      if (is_empty_polyhedron (simplified))
		{
		  value_assign (dep_constraints->p[j + dep_constraints->NbRows 
						   - 2*iter_vector_dim][p], 
				GBB_DYNAMIC_SCHEDULE (gb1)->p[j][p - 1]);
          
		  value_oppose (dep_constraints->p[j + dep_constraints->NbRows 
						   - 2 * iter_vector_dim]
				[nb_loops_around_gb (gb1) + p], 
				GBB_DYNAMIC_SCHEDULE (gb2)->p[j][p - 1]);
		}
	      else
		return initialize_data_dependence_polyhedron (true, simplified, 
							      p, a, b);           
	      cloog_matrix_free (temp_matrix);
	    }
	}

      else if (p < 0)
	{
	  temp_matrix = graphite_add_a_null_row (dep_constraints);
	  simplified = cloog_domain_matrix2domain (temp_matrix);

	  if (!is_empty_polyhedron (simplified)
	      && statement_precedes_p (gb1, DR_STMT (a), gb2, DR_STMT (b), -p))
	    return initialize_data_dependence_polyhedron (false, simplified,
							  -p, a, b);

	  cloog_matrix_free (temp_matrix);
	}
    }    

  cloog_matrix_free (dep_constraints);
  return NULL;
}

/* Returns the polyhedral data dependence graph for SCOP.  */

struct graph *
graphite_build_rdg_all_levels (scop_p scop)
{
  unsigned i, j, i1, j1;
  graphite_bb_p gb1, gb2;
  struct graph *rdg = NULL;
  VEC (gimple, heap) *stmts = VEC_alloc (gimple, heap, 10);
  VEC (ddp_p, heap) *dependences = VEC_alloc (ddp_p, heap, 10); 
  ddp_p dependence_polyhedron;    

  for (i = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), i, gb1); i++)
    {
      gimple_stmt_iterator gsi;
      struct data_reference *a, *b;

      for (gsi = gsi_start_bb (GBB_BB (gb1)); !gsi_end_p (gsi); gsi_next (&gsi))
	VEC_safe_push (gimple, heap, stmts, gsi_stmt (gsi));

      for (i1 = 0; 
           VEC_iterate (data_reference_p, GBB_DATA_REFS (gb1), i1, a); 
           i1++)
	for (j = 0; VEC_iterate (graphite_bb_p, SCOP_BBS (scop), j, gb2); j++)
	  for (j1 = 0; 
               VEC_iterate (data_reference_p, GBB_DATA_REFS (gb2), j1, b); 
               j1++)
	    if ((!DR_IS_READ (a) || !DR_IS_READ (b)) && dr_may_alias_p (a,b)
		&& operand_equal_p (DR_BASE_OBJECT (a), DR_BASE_OBJECT (b), 0))
              {
                dependence_polyhedron = graphite_test_dependence (scop, gb1, gb2, a, b);
                if (dependence_polyhedron != NULL)
                  VEC_safe_push (ddp_p, heap, dependences, dependence_polyhedron);
              }
              /* The previous check might be too restrictive.  */ 
    }

  rdg = build_empty_rdg (VEC_length (gimple, stmts));
  create_rdg_vertices (rdg, stmts);

  for (i = 0; VEC_iterate (ddp_p, dependences, i, dependence_polyhedron); i++)
    {
      int va = rdg_vertex_for_stmt (rdg, DR_STMT (dependence_polyhedron->a)); 
      int vb = rdg_vertex_for_stmt (rdg, DR_STMT (dependence_polyhedron->b));
      struct graph_edge *e = add_edge (rdg, va, vb);

      e->data = dependence_polyhedron;
    }

  VEC_free (gimple, heap, stmts);
  return rdg;  
}


/* Dumps the dependence graph G to file F.  */

void
graphite_dump_dependence_graph (FILE *f, struct graph *g)
{
  int i;
  struct graph_edge *e;

  for (i = 0; i < g->n_vertices; i++)
    {
      if (!g->vertices[i].pred
	  && !g->vertices[i].succ)
	continue;

      fprintf (f, "vertex: %d \nStatement: ", i);
      print_gimple_stmt (f, RDGV_STMT (&(g->vertices[i])), 0, 0);
      fprintf (f, "\n-----------------\n");
      
      for (e = g->vertices[i].succ; e; e = e->succ_next)
        {
          struct data_dependence_polyhedron *ddp = RDGE_DDP (e);

          fprintf (f, "edge %d -> %d\n", i, e->dest);

          if (ddp->polyhedron != NULL)
	    cloog_domain_print (f, ddp->polyhedron); 

          else if (!ddp->loop_carried)
            fprintf (f, "loop independent dependence at level %d\n", ddp->level);

          fprintf (f, "--------\n");
        }

      fprintf (f, "\n");
    }
}

#else /* If Cloog is not available: #ifndef HAVE_cloog.  */

#endif
