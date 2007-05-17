/* Loop Distribution
   Copyright (C) 2006, 2007 Free Software Foundation, Inc.
   Contributed by Georges-Andre Silber <Georges-Andre.Silber@ensmp.fr>
   and Sebastian Pop <sebpop@gmail.com>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

/* This pass performs loop distribution using a reduced dependence graph
   (RDG) build using the data dependence analyzer.  This RDG is then used
   to build a "partition" RDG graph without taking into account data
   dependences.  Data dependences are then used to build a graph of
   Strongly Connected Components.  This graph is then topologically sorted
   to obtain a code generation plan for the distributed loops.  

   When using the -fdump-tree-ldist option, it outputs each graph in
   "dotty" (used by graphviz), surrounded by some XML structure.

   TODO: Replace arrays by vectors for edges and vertices of RDG.
   TODO: Generate new distributed loops.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "ggc.h"
#include "tree.h"
#include "target.h"

#include "rtl.h"
#include "basic-block.h"
#include "diagnostic.h"
#include "tree-flow.h"
#include "tree-dump.h"
#include "timevar.h"
#include "cfgloop.h"
#include "expr.h"
#include "optabs.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "lambda.h"
#include "langhooks.h"

#ifdef USE_MAPPED_LOCATION
typedef source_location LOC;
#define UNKNOWN_LOC UNKNOWN_LOCATION
#define EXPR_LOC(e) EXPR_LOCATION(e)
#define LOC_FILE(l) LOCATION_FILE (l)
#define LOC_LINE(l) LOCATION_LINE (l)
#else
typedef source_locus LOC;
#define UNKNOWN_LOC NULL
#define EXPR_LOC(e) EXPR_LOCUS(e)
#define LOC_FILE(l) (l)->file
#define LOC_LINE(l) (l)->line
#endif

/* Initial value for VEC data structures used in RDG.  */
# define RDG_VS 10

/* Values used in DFS algorithm.  */
# define VERTEX_WHITE 0
# define VERTEX_GRAY 1
# define VERTEX_BLACK 2

typedef struct rdg *rdg_p;
typedef struct rdg_vertex *rdg_vertex_p;
typedef struct rdg_edge *rdg_edge_p;
typedef struct prdg *prdg_p;
typedef struct prdg_vertex *prdg_vertex_p;
typedef struct prdg_edge *prdg_edge_p;

DEF_VEC_P(rdg_vertex_p);
DEF_VEC_ALLOC_P(rdg_vertex_p, heap);
DEF_VEC_P(rdg_edge_p);
DEF_VEC_ALLOC_P(rdg_edge_p, heap);
DEF_VEC_P(prdg_vertex_p);
DEF_VEC_ALLOC_P(prdg_vertex_p, heap);
DEF_VEC_P(prdg_edge_p);
DEF_VEC_ALLOC_P(prdg_edge_p, heap);

/* A RDG (Reduced Dependence Graph) represents all data dependence
   constraints between the statements of a loop nest. */
struct rdg 
{
  /* The loop nest represented by this RDG.  */
  struct loop *loop_nest;
  
  /* The SSA_NAME used for loop index.  */
  tree loop_index;
  
  /* The MODIFY_EXPR used to update the loop index.  */
  tree loop_index_update;
  
  /* The COND_EXPR that is the exit condition of the loop.  */
  tree loop_exit_condition;
  
  /* The PHI_NODE of the loop index.  */
  tree loop_index_phi_node;
  
  /* The vertices of the graph.  There is one vertex per
     statement of the basic block of the loop.  */
  unsigned int nb_vertices;
  rdg_vertex_p vertices;

  /* The edges of the graph.  There is one edge per data dependence (between
     memory references) and one edge per scalar dependence.  */
  unsigned int nb_edges;  
  rdg_edge_p edges;
  
  /* Vertices that contain a statement containing an ARRAY_REF.  */
  VEC (rdg_vertex_p, heap) *dd_vertices;
  
  /* Data references and array data dependence relations.  */
  VEC (ddr_p, heap) *dependence_relations;
  VEC (data_reference_p, heap) *datarefs;
};

#define RDG_LOOP(G)	  (G)->loop_nest
#define RDG_IDX(G)        (G)->loop_index
#define RDG_IDX_UPDATE(G) (G)->loop_index_update
#define RDG_EXIT_COND(G)  (G)->loop_exit_condition
#define RDG_IDX_PHI(G)    (G)->loop_index_phi_node
#define RDG_NBV(G)        (G)->nb_vertices
#define RDG_NBE(G)        (G)->nb_edges
#define RDG_V(G)          (G)->vertices
#define RDG_VERTEX(G,i)   &((G)->vertices[i])
#define RDG_E(G)          (G)->edges
#define RDG_EDGE(G,i)     &((G)->edges[i])
#define RDG_DDV(G)        (G)->dd_vertices
#define RDG_DR(G)         (G)->datarefs
#define RDG_DDR(G)        (G)->dependence_relations

/* A RDG vertex representing a statement.  */
struct rdg_vertex 
{
  /* This color is used for graph algorithms.  */
  int color;

  /* The number of the basic block in the loop body.  */
  unsigned int bb_number;

  /* The number of the vertex.  It represents the number of
     the statement in the basic block.  */
  unsigned int number;
  
  /* The statement represented by this vertex.  */
  tree stmt;
  
  /* True when this vertex contains a data reference 
     that is an ARRAY_REF.  */
  bool has_dd_p; 
  
  /* Vertex is the sink of those edges.  */
  VEC (rdg_edge_p, heap) *in_edges;
  
  /* Vertex is the source of those edges. */
  VEC (rdg_edge_p, heap) *out_edges;

  /* Partitions the vertex is in.  
     If 'has_dd_p' is true, the vertex can only be in one partition.
     If not, the vertex can be duplicated in several partitions.  */
  VEC (int, heap) *partition_numbers;
  
  /* Strongly connected components the vertex is in.
     If 'has_dd_p' is true, the vertex can only be in one SCC.
     If not, the vertex can be in several SCCs.  */
  VEC (int, heap) *scc_numbers;
};

#define RDGV_COLOR(V)      (V)->color
#define RDGV_BB(V)         (V)->bb_number
#define RDGV_N(V)          (V)->number
#define RDGV_STMT(V)       (V)->stmt
#define RDGV_DD_P(V)       (V)->has_dd_p
#define RDGV_IN(V)         (V)->in_edges
#define RDGV_OUT(V)        (V)->out_edges
#define RDGV_PARTITIONS(V) (V)->partition_numbers
#define RDGV_SCCS(V)       (V)->scc_numbers

/* Data dependence type.  */
enum rdg_dep_type 
{
  /* Read After Write (RAW) (source is W, sink is R).  */
  flow_dd = 'f',
  
  /* Write After Read (WAR) (source is R, sink is W).  */
  anti_dd = 'a',
  
  /* Write After Write (WAW) (source is W, sink is W).  */
  output_dd = 'o', 
  
  /* Read After Read (RAR) (source is R, sink is R).  */
  input_dd = 'i' 
};

/* An edge of the RDG with dependence information.  */
struct rdg_edge 
{
  /* Color used for graph algorithms.  */  
  int color;
  
  /* The vertex source of the dependence.  */
  rdg_vertex_p source;
  
  /* The vertex sink of the dependence.  */
  rdg_vertex_p sink;
  
  /* The reference source of the dependence.  */
  tree source_ref;
  
  /* The reference sink of the dependence.  */
  tree sink_ref;
  
  /* Type of the dependence.  */
  enum rdg_dep_type type;
  
  /* Level of the dependence: the depth of the loop that
    carries the dependence.  */
  int level;
  
  /* true if the dependence is between two scalars.  Usually,
    it is known of a dependence between two memory elements
    of dimension 0.  */
  bool scalar_p;  
};

#define RDGE_COLOR(E)       (E)->color
#define RDGE_SOURCE(E)      (E)->source
#define RDGE_SINK(E)        (E)->sink
#define RDGE_SOURCE_REF(E)  (E)->source_ref
#define RDGE_SINK_REF(E)    (E)->sink_ref
#define RDGE_TYPE(E)        (E)->type
#define RDGE_LEVEL(E)       (E)->level
#define RDGE_SCALAR_P(E)    (E)->scalar_p

/* This graph represents a partition: each vertex is a group of
   existing RDG vertices, each edge is a dependence between two
   partitions.  */
struct prdg 
{
  /* The RDG used for partitionning.  */
  rdg_p rdg;
  
  /* The vertices of the graph.  */
  VEC (prdg_vertex_p, heap) *vertices;
  
  /* The edges of the graph.  */
  VEC (prdg_edge_p, heap) *edges;
};

#define PRDG_RDG(G)       (G)->rdg
#define PRDG_NBV(G)       VEC_length (prdg_vertex_p,(G)->vertices)
#define PRDG_V(G)         (G)->vertices
#define PRDG_VERTEX(G,i)  VEC_index (prdg_vertex_p,(G)->vertices,i) 
#define PRDG_NBE(G)       VEC_length (prdg_edge_p,(G)->edges)
#define PRDG_E(G)         (G)->edges
#define PRDG_EDGE(G,i)    VEC_index (prdg_edge_p,(G)->edges,i)

/* A vertex representing a group of RDG vertices.  */
struct prdg_vertex 
{
  /* The partition number.  */
  int num;
  
  /* Used for graph algorithms.  */
  int color; 
  
  /* Discovery time.  Used by DFS.  */
  int d;
  
  /* Finishing time.  Used by DFS and SCC.  */
  int f;
  
  /* SCC number.  */
  int scc; 
  
  /* Predecessor after DFS computation.  */
  prdg_vertex_p pred; 
   
  /* Vertices of the RDG that are in this partition.  */
  VEC (rdg_vertex_p, heap) *pvertices;
};

#define PRDGV_N(V)           (V)->num
#define PRDGV_COLOR(V)       (V)->color
#define PRDGV_D(V)           (V)->d
#define PRDGV_F(V)           (V)->f
#define PRDGV_SCC(V)         (V)->scc
#define PRDGV_PRED(V)        (V)->pred
#define PRDGV_NPV(V)         VEC_length (rdg_vertex_p,(V)->pvertices)
#define PRDGV_PV(V)          (V)->pvertices
#define PRDGV_PVERTEX(V,i)   VEC_index (rdg_vertex_p,(V)->pvertices,i)

/* Dependence egde of the partition graph.  */
struct prdg_edge 
{
  /* Vertex source of the dependence.  */
  prdg_vertex_p source;
  
  /* Vertex sink of the dependence.  */
  prdg_vertex_p sink;
  
  /* Original edge of the RDG.  */
  rdg_edge_p rdg_edge;
};

#define PRDGE_SOURCE(V)    (V)->source
#define PRDGE_SINK(V)      (V)->sink
#define PRDGE_RDG_EDGE(V)  (V)->rdg_edge

/* Helper function for Depth First Search.  */

static void
dfs_rdgp_1 (prdg_p g, prdg_vertex_p v, unsigned int *t, unsigned int scc)
{
  unsigned int i;
  prdg_edge_p e;
  rdg_vertex_p rdg_v;
  
  PRDGV_COLOR (v) = VERTEX_GRAY;
  (*t)++;
  PRDGV_D (v) = *t;
  PRDGV_SCC (v) = scc;

  /* If scc!=0, add this SCC to each vertex of the partition. */
  if (scc)
    for (i = 0; VEC_iterate (rdg_vertex_p, PRDGV_PV (v), i, rdg_v); i++)
      VEC_safe_push (int, heap, RDGV_SCCS (rdg_v), scc);
  
  for (i = 0; VEC_iterate (prdg_edge_p, PRDG_E (g), i, e); i++)
    if (PRDGE_SOURCE (e) == v)
      {
	prdg_vertex_p u = PRDGE_SINK (e);
      
	if (PRDGV_COLOR (u) == VERTEX_WHITE)
	  {
	    PRDGV_PRED (u) = v;
	    dfs_rdgp_1 (g, u, t, scc);
	  }
      }

  PRDGV_COLOR (v) = VERTEX_BLACK;
  (*t)++;
  PRDGV_F (v) = *t;
}

/* Depth First Search.  This is an adaptation of the depth first search
   described in Cormen et al., "Introduction to Algorithms", MIT Press.
   Returns the max of "finishing times" for the partition graph G.  */

static int
dfs_rdgp (prdg_p g)
{
  unsigned int i;
  /* t represents the max of finishing times.  */
  unsigned int t = 0;
  prdg_vertex_p v;

  for (i = 0; VEC_iterate (prdg_vertex_p, PRDG_V (g), i, v); i++)
    {
      PRDGV_COLOR (v) = VERTEX_WHITE;
      PRDGV_PRED (v) = NULL;
    }

  for (i = 0; VEC_iterate (prdg_vertex_p, PRDG_V (g), i, v); i++)
    if (PRDGV_COLOR (v) == VERTEX_WHITE)
      dfs_rdgp_1 (g, v, &t, 0);
  
  return t;
}

/* Comparison function to compare "finishing times" of
   two vertices.  */

static bool
rdgp_vertex_less_than_p (const prdg_vertex_p a,
                         const prdg_vertex_p b)
{
  return (PRDGV_F (a) < PRDGV_F (b));
}

/* Helper function for the computation of strongly connected components.  */

static unsigned int
scc_rdgp_1 (prdg_p g, int max_f)
{
  unsigned int i;
  unsigned int t = 0;
  unsigned int scc = 0;
  prdg_vertex_p v;
  VEC (prdg_vertex_p, heap) *sorted_vertices;

  for (i = 0; VEC_iterate (prdg_vertex_p, PRDG_V (g), i, v); i++)
    {
      PRDGV_COLOR (v) = VERTEX_WHITE;
      PRDGV_PRED (v) = NULL;
    }
  
  /* Here we build a vector containing the vertices sorted by increasing
     finishing times F (computed by DFS).   This is a contradiction with
     the complexity of the SCC algorithm that is in linear time
     O(V+E).   We could have used an array containing pointers to vertices,
     the index of this array representing F for the corresponding vertex.
     This array has a size equal to 'max_f' with holes.  */
  
  sorted_vertices = VEC_alloc (prdg_vertex_p, heap, max_f);
  
  for (i = 0; VEC_iterate (prdg_vertex_p, PRDG_V (g), i, v); i++)
    {
      unsigned int idx = VEC_lower_bound (prdg_vertex_p, sorted_vertices,
					  v, rdgp_vertex_less_than_p);

      VEC_safe_insert (prdg_vertex_p, heap, sorted_vertices, idx, v);
    }

  gcc_assert (VEC_length (prdg_vertex_p, sorted_vertices));
  
  while (VEC_length (prdg_vertex_p, sorted_vertices))
    {
      v = VEC_pop (prdg_vertex_p, sorted_vertices);
    
      if (PRDGV_COLOR (v) == VERTEX_WHITE)
	dfs_rdgp_1 (g, v, &t, ++scc);
    }

  VEC_free (prdg_vertex_p, heap, sorted_vertices);
  
  return scc;
}

/* Change the directions of all edges.  */

static void
transpose_rdgp (prdg_p g)
{
  unsigned int i;
  prdg_edge_p e;
  
  for (i = 0; VEC_iterate (prdg_edge_p, PRDG_E (g), i, e); i++)
    {
      prdg_vertex_p tmp = PRDGE_SINK (e);

      PRDGE_SINK (e) = PRDGE_SOURCE (e);
      PRDGE_SOURCE (e) = tmp;
    }
}

/* Computes the strongly connected components of G.  */

static unsigned int
scc_rdgp (prdg_p g)
{
  unsigned int nb_sccs;
  int max_f;
  
  max_f = dfs_rdgp (g);
  transpose_rdgp (g);
  nb_sccs = scc_rdgp_1 (g, max_f);
  transpose_rdgp (g);

  return nb_sccs;
}

/* Returns true when vertex V is in partition P.  */

static bool
vertex_in_partition_p (rdg_vertex_p v, int p)
{
  int i;
  int vp;
  
  for (i = 0; VEC_iterate (int, RDGV_PARTITIONS (v), i, vp); i++)
    if (vp == p)
      return true;
  
  return false;
}

/* Returns true when vertex V is in SCC S.  */

static bool
vertex_in_scc_p (rdg_vertex_p v, int s)
{
  int i;
  int vs;
  
  for (i = 0; VEC_iterate (int, RDGV_SCCS (v), i, vs); i++)
    if (vs == s)
      return true;
  
  return false;
}

/* Allocates a new partition vertex.  */

static prdg_vertex_p
new_prdg_vertex (unsigned int p)
{
  prdg_vertex_p v;
  
  v = XNEW (struct prdg_vertex);
  PRDGV_N (v) = p;
  PRDGV_COLOR (v) = 0;
  PRDGV_D (v) = 0;
  PRDGV_F (v) = 0;
  PRDGV_PRED (v) = NULL;
  PRDGV_SCC (v) = 0;
  PRDGV_PV (v) = VEC_alloc (rdg_vertex_p, heap, RDG_VS);
  
  return v;
}

/* Free a partition vertex.  */

static void
free_prdg_vertex (prdg_vertex_p v)
{
  VEC_free (rdg_vertex_p, heap, PRDGV_PV (v));
  free (v);
}

/* Allocates a new partition edge.  */
static prdg_edge_p
new_prdg_edge (rdg_edge_p re, 
	       prdg_vertex_p sink,
               prdg_vertex_p source)
{
  prdg_edge_p e;
  
  e = XNEW (struct prdg_edge);
  PRDGE_RDG_EDGE (e) = re;
  PRDGE_SINK (e) = sink;
  PRDGE_SOURCE (e) = source;
  
  return e;
}

/* Free a partition edge.  */

static void
free_prdg_edge (prdg_edge_p e)
{
  free (e);
}

/* Allocates a new partition graph.  */

static prdg_p
new_prdg (rdg_p rdg)
{
  prdg_p rdgp = XNEW (struct prdg);

  PRDG_RDG (rdgp) = rdg;
  PRDG_V (rdgp) = VEC_alloc (prdg_vertex_p, heap, RDG_VS);
  PRDG_E (rdgp) = VEC_alloc (prdg_edge_p, heap, RDG_VS);

  return rdgp;
}

/* Free a partition graph.  */

static void
free_prdg (prdg_p g)
{
  unsigned int i;
  prdg_vertex_p v;
  prdg_edge_p e;
  
  for (i = 0; VEC_iterate (prdg_vertex_p, PRDG_V (g), i, v); i++)
    free_prdg_vertex (v);
  
  for (i = 0; VEC_iterate (prdg_edge_p, PRDG_E (g), i, e); i++)
    free_prdg_edge (e);
  
  VEC_free (prdg_vertex_p, heap, PRDG_V (g));
  VEC_free (prdg_edge_p, heap, PRDG_E (g));
}

/* Builds a strongly connected components partition graph of G.  */

static prdg_p
build_scc_graph (prdg_p g)
{
  prdg_p sccg;
  unsigned int nb_sccs;
  unsigned int i, j;
  
  /* Computes the SCC of g.  */
  nb_sccs = scc_rdgp (g);

  /* Builds a new partition graph of the SCC of g.  */
  sccg = new_prdg (PRDG_RDG (g));
  
  /* Create SCC vertices.  */
  for (i = 0; i < nb_sccs; i++)
    {
      unsigned int current_scc = i + 1;
      unsigned int nbv = RDG_NBV (PRDG_RDG (sccg));
      prdg_vertex_p v = new_prdg_vertex (current_scc);
      
      for (j = 0; j < nbv; j++)
	{
	  rdg_vertex_p rdg_v = RDG_VERTEX (PRDG_RDG (sccg), j);
        
	  if (vertex_in_scc_p (rdg_v, current_scc))
	    VEC_safe_push (rdg_vertex_p, heap, PRDGV_PV (v), rdg_v);
	}
      
      PRDGV_SCC (v) = current_scc;
      VEC_safe_push (prdg_vertex_p, heap, PRDG_V (sccg), v);
    }
  
  /* Create SCC edges.  */
  for (i = 0; i < RDG_NBE (PRDG_RDG (g)); i++)
    {
      rdg_edge_p e = RDG_EDGE (PRDG_RDG (g), i);
    
      /* Here we take only into account data dependences.  */
      if (!RDGE_SCALAR_P (e))
	{
	  prdg_edge_p pe;
	  int source_idx = VEC_index (int, RDGV_SCCS (RDGE_SOURCE (e)), 0);
	  int sink_idx = VEC_index (int, RDGV_SCCS (RDGE_SINK (e)), 0);
          
	  gcc_assert (source_idx && sink_idx);   
          
	  pe = new_prdg_edge (e, PRDG_VERTEX (sccg, source_idx - 1),
			      PRDG_VERTEX (sccg, sink_idx - 1));
	 
	  VEC_safe_push (prdg_edge_p, heap, sccg->edges, pe);
	}	
    }
    
  return sccg;
}

/* Returns true if the vertex can be recomputed, meaning
   that the vertex and all the nodes on the path that goes up are only
   scalars.  */

static bool
can_recompute_vertex_p (rdg_vertex_p v)
{
  rdg_edge_p in_edge;
  unsigned int i;
  
  if (RDGV_DD_P (v))
    return false;
  
  for (i = 0; VEC_iterate (rdg_edge_p, RDGV_IN (v), i, in_edge); i++)
    if (RDGE_SCALAR_P (in_edge))
      if (!can_recompute_vertex_p (RDGE_SOURCE (in_edge)))
        return false;

  return true;
}

/* Create one partition in RDG starting from vertex V with a number p.  */ 

static void
one_prdg (rdg_p rdg, rdg_vertex_p v, int p)
{
  rdg_edge_p o_edge, i_edge;
  unsigned int i;
  
  if (vertex_in_partition_p (v, p))
    return;

  VEC_safe_push (int, heap, RDGV_PARTITIONS (v), p);

  for (i = 0; VEC_iterate (rdg_edge_p, RDGV_IN (v), i, i_edge); i++)
    if (RDGE_SCALAR_P (i_edge))
      one_prdg (rdg, RDGE_SOURCE (i_edge), p);

  if (!can_recompute_vertex_p (v))
    for (i = 0; VEC_iterate (rdg_edge_p, RDGV_OUT (v), i, o_edge); i++)
      if (RDGE_SCALAR_P (o_edge))
        one_prdg (rdg, RDGE_SINK (o_edge), p);
}

/* Returns true if partitions are correct.  */

static bool
correct_partitions_p (rdg_p rdg, int p)
{
  unsigned int i;
  
  if (!p)
    return false;
  
  /* All vertices must have color != 0.  */
  for (i = 0; i < RDG_NBV (rdg); i++)
    {
      if (RDGV_DD_P (RDG_VERTEX (rdg, i))
	  && !VEC_length (int, RDGV_PARTITIONS (RDG_VERTEX (rdg, i))) == 1)
	return false;
    
      if (!VEC_length (int, RDGV_PARTITIONS (RDG_VERTEX (rdg, i))))
        return false;
    }

  return true;
}

/* Marks each vertex that contains an ARRAY_REF with the number of the
   partition it belongs. Returns the number of partitions. 
   This number is at least 1.  */

static unsigned int
mark_partitions (rdg_p rdg)
{
  rdg_vertex_p rdg_v;
  unsigned int i;
  int k, p = 0;

  /* Clear all existing partitions.  */
  for (i = 0; i < RDG_NBV (rdg); i++)
    VEC_truncate (int, RDGV_PARTITIONS (RDG_VERTEX (rdg,i)), 0);
  
  /* If there are no dd_vertices, put all in one single partition.  */
  if (VEC_length (rdg_vertex_p, RDG_DDV (rdg)) == 0)
    {
      /* Mark all vertices with p=1.  */
      for (i = 0; i < RDG_NBV (rdg); i++)
        VEC_safe_push (int, heap, RDGV_PARTITIONS (RDG_VERTEX (rdg, i)), 1);

      return 1;
    }
    
  /* Mark each vertex with its own color and propagate.  */
  for (i = 0; VEC_iterate (rdg_vertex_p, RDG_DDV (rdg), i, rdg_v); i++)
    if (VEC_length (int, RDGV_PARTITIONS (rdg_v)) == 0)
      one_prdg (rdg, rdg_v, ++p);
  
  /* Add the vertices that are not in a partition in all partitions.
     Those vertices does not contain any ARRAY_REF (otherwise, they would
     have been added by the previous loop on dd_vertices).  */
  for (i = 0; i < RDG_NBV (rdg); i++)
    if (VEC_length (int, RDGV_PARTITIONS (RDG_VERTEX (rdg, i))) == 0)
      for (k = 1; k <= p; k++)
        VEC_safe_push (int, heap, RDGV_PARTITIONS (RDG_VERTEX (rdg, i)), k);
    
  gcc_assert (correct_partitions_p (rdg, p));
  
  return p;
}

/* Builds a partition graph of an RDG.  This partition represents the
   maximal distribution of the loops if we break all dependences of level l
   greater than 0 that are of dimension l.  Note that this graph can have
   new cycles that were not visible in the RDG.
 
   The principle of this partition is twofold:
    - we allow the recomputation of scalar values;
    - we do not allow the recomputation of array references, because this
      is what we try to distribute to parallelize the iterations of the loop.
 
   Vertices that contain an array reference (has_dd_p == true) are in one 
   and only one partition.  Vertices that do not contain any array reference
   are in one or more partitions.
 
   This function returns NULL if there are no ARRAY_REF statement 
   in the rdg.  */ 

static prdg_p
build_prdg (rdg_p rdg)
{
  unsigned int i, j;
  rdg_vertex_p rdg_v;  
  prdg_p rdgp = new_prdg (rdg);
  unsigned int nbp = mark_partitions (rdg);
  
  /* Create partition vertices.  */
  for (i = 0; i < nbp; i++)
    {
      unsigned int current_partition = i+1;
      prdg_vertex_p v = new_prdg_vertex (current_partition);
      
      for (j = 0; j < rdg->nb_vertices; j++)
	{
	  rdg_v = RDG_VERTEX (rdg, j);
        
	  if (vertex_in_partition_p (rdg_v, current_partition))
	    VEC_safe_push (rdg_vertex_p, heap, PRDGV_PV (v), rdg_v);
	}

      VEC_safe_push (prdg_vertex_p, heap, PRDG_V (rdgp), v);
    }

  /* Create partition edges.  */
  for (i = 0; i < rdg->nb_edges; i++)
    {
      rdg_edge_p e = RDG_EDGE (rdg, i);
    
      /* Here we take only into account data dependences.  */
      if (!RDGE_SCALAR_P (e))
	{
	  int so_idx = VEC_index (int, RDGV_PARTITIONS (RDGE_SOURCE (e)), 0);
          int si_idx = VEC_index (int, RDGV_PARTITIONS (RDGE_SINK (e)), 0);
	  prdg_edge_p pe = new_prdg_edge (e, PRDG_VERTEX (rdgp, so_idx-1),
					  PRDG_VERTEX (rdgp, si_idx-1));

	  VEC_safe_push (prdg_edge_p, heap, PRDG_E (rdgp), pe);
	}
    }
  
  return rdgp;
}

/* Print out a partition graph in DOT format and other informations.  */

static void
dump_prdg (FILE *outf, prdg_p rdgp)
{
  unsigned int p, i;
  prdg_vertex_p pv;
  prdg_edge_p pe;
  rdg_vertex_p v;

  fprintf (outf, "<graphviz><![CDATA[\n");
  fprintf (outf, "digraph ");
  print_generic_expr (outf, RDG_IDX (PRDG_RDG (rdgp)), 0);
  fprintf (outf, " {\n");
    
  /* Print out vertices. Each vertex represents a partition, then it
    can contain several statements.  */
  for (p = 0; VEC_iterate (prdg_vertex_p, PRDG_V (rdgp), p, pv); p++)
    {
      fprintf (outf, " P%d [ shape=rect,label = \" P%d(%d): ", 
	       PRDGV_N (pv), PRDGV_N (pv), PRDGV_SCC (pv));
    
      for (i = 0; VEC_iterate (rdg_vertex_p, PRDGV_PV (pv), i, v); i++)
	fprintf (outf, "S%d;", RDGV_N (v));
      
      fprintf (outf, "\"];\n");
    
      fprintf (outf, " v%d [ label = \" P%d(%d)",  PRDGV_N (pv), 
               PRDGV_N (pv), PRDGV_SCC (pv));   
      
      fprintf (outf, "\"];\n");
      
      fprintf (outf, "{rank=same; P%d; v%d; }\n",  PRDGV_N (pv), PRDGV_N (pv)); 
    } 
  
  for (i = 0; VEC_iterate (prdg_edge_p, PRDG_E (rdgp), i, pe); i++)
    fprintf (outf, "v%d -> v%d [label=\"%c:%d\" style=dotted];\n",
	     PRDGV_N (PRDGE_SOURCE (pe)),
	     PRDGV_N (PRDGE_SINK (pe)),
	     RDGE_TYPE (PRDGE_RDG_EDGE (pe)), 
	     RDGE_LEVEL (PRDGE_RDG_EDGE (pe)));
  
  fprintf (outf, "}\n");
  fprintf (outf, "]]></graphviz>\n");
}

/* Function prototype from tree-vectorizer.  */
LOC find_loop_location (struct loop*);

/* Print out loop informations.  */

static void
dump_loop_infos (FILE *outf, struct loop *loop)
{
  LOC dist_loop_location = find_loop_location (loop);

  fprintf (outf, " <location>\n");
  
  if (dist_loop_location == UNKNOWN_LOC)
    fprintf (outf, "  <filename>%s</filename>\n  <line>%d</line>\n",
             DECL_SOURCE_FILE (current_function_decl),
             DECL_SOURCE_LINE (current_function_decl));
  else
    fprintf (outf, "  <filename>%s</filename>\n  <line>%d</line>\n",
	     LOC_FILE (dist_loop_location), LOC_LINE (dist_loop_location));
  
  fprintf (outf, " </location>\n");
  fprintf (outf, " <depth>%d</depth>\n", loop->depth);
  fprintf (outf, " <nodes>%d</nodes>\n", loop->num_nodes);
}

/* Dump a RDG in DOT format plus other informations.  */

static void
dump_rdg (FILE *outf, rdg_p rdg)
{
  unsigned int i;
  rdg_vertex_p vertex;
  
  fprintf (outf, "<graphviz><![CDATA[\n");
  fprintf (outf, "digraph ");
  print_generic_expr (outf, RDG_IDX (rdg), 0);
  fprintf (outf, " {\n");
  
  for (i = 0; i < RDG_NBV (rdg); i++)
    { 
      rdg_vertex_p v = RDG_VERTEX (rdg, i);
    
      fprintf (outf, " v%d [ label = \"", RDGV_N (v));
      fprintf (outf, "S%d : ", RDGV_N (v));
      print_generic_expr (outf, RDGV_STMT (v), 0);
      fprintf (outf, "\"");
      
      if (RDGV_DD_P (v))
	fprintf (outf, " shape=rect style=filled color=\".7 .3 1.0\"]");
      else
	fprintf (outf, " shape=rect]");	
      
      fprintf (outf, ";\n");
    }
  
  for (i = 0; i < RDG_NBE (rdg); i++)
    {
      struct rdg_edge *e = RDG_EDGE (rdg, i);
      struct rdg_vertex *sink = RDGE_SINK (e);
      struct rdg_vertex *source = RDGE_SOURCE (e);
      
      fprintf (outf, " v%d -> v%d", RDGV_N (source), RDGV_N (sink));      
      fprintf (outf, " [ label=\"%c:%d", RDGE_TYPE (e), RDGE_LEVEL (e));
      
      if (RDGE_SCALAR_P (e))
        fprintf (outf, " d=0");
      else
        fprintf (outf, " d=x");
      
      fprintf(outf, "\" ");
      
      /* TODO: Here, it is not the level that matters...
         In fact, it is the dimension of the dependence, a dependence
         of level=0 with a dimension=1 can be stored and
         then can be broken.  */
      if (RDGE_LEVEL (e) > 0)
        fprintf (outf, " style=dotted");
      
      fprintf(outf, "]\n");
    }
  
  fprintf (outf, "}\n");
  fprintf (outf, "]]></graphviz>\n");
  fprintf (outf, "<dd_vertices>\n");
  
  for (i = 0; VEC_iterate (rdg_vertex_p, RDG_DDV (rdg), i, vertex); i++)
    {
      fprintf (outf, "<dd_vertex s=\"s%d\">", RDGV_N (vertex));
      print_generic_expr (outf, RDGV_STMT (vertex), 0);
      fprintf (outf, "</dd_vertex>\n");
    }

  fprintf (outf, "</dd_vertices>\n");
}

/* Find the vertex containing a given statement in a RDG or return NULL
   if the statement is not in any vertex.  */

static rdg_vertex_p
find_vertex_with_stmt (rdg_p rdg, tree stmt)
{
  rdg_vertex_p vertex = NULL;
  unsigned int i;
  
  for (i = 0; i < RDG_NBV (rdg) && vertex == NULL; i++)
    if (RDGV_STMT (RDG_VERTEX (rdg,i)) == stmt)
      vertex = RDG_VERTEX (rdg, i);
  
  return vertex;
}

/* Returns true if the statement is a control statement of the loop.  */

static bool
loop_nest_control_p (rdg_p rdg, tree stmt)
{
  if (TREE_CODE (stmt) == LABEL_EXPR)
    return true;
  
  if (stmt == RDG_EXIT_COND (rdg))
    return true;
  
  return false;
}

/* Computes the number of vertices of a given loop nest.  */

static int
number_of_vertices (rdg_p rdg)
{
  basic_block bb;
  unsigned int i;
  unsigned int nb_stmts = 0;
  block_stmt_iterator bsi;
  struct loop *loop = RDG_LOOP (rdg);
  basic_block *bbs = get_loop_body (loop);
  
  for (i = 0; i < loop->num_nodes; i++)
    {
      bb = bbs[i];
    
      /* Test whether the basic block is a direct son of the loop,
         the bbs array contains all basic blocks in DFS order.  */
      if (bb->loop_father == loop)
        /* Iterate of each statement of the basic block.  */
        for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
          if (!loop_nest_control_p (rdg, bsi_stmt (bsi)))
            nb_stmts++;
    }
  
  free (bbs);

  return nb_stmts;
}

/* Returns true if a statement has a data reference in the given
   data reference vector.  */

static bool
contains_dr_p (tree stmt, VEC (data_reference_p, heap) *datarefs)
{
  data_reference_p dr;
  unsigned int i;
  
  for (i = 0; VEC_iterate (data_reference_p, datarefs, i, dr); i++)
    if (DR_STMT (dr) == stmt)
      return true;
  
  return false;
}

/* Create vertices of a RDG.  */

static void
create_vertices (rdg_p rdg)
{
  basic_block *bbs;
  basic_block bb;
  unsigned int i;
  unsigned int vertex_index;
  block_stmt_iterator bsi;
  struct loop *loop = RDG_LOOP (rdg);
  
  RDG_NBV (rdg) = number_of_vertices (rdg);
  RDG_V (rdg) = XCNEWVEC (struct rdg_vertex, RDG_NBV (rdg));
  
  vertex_index = 0;
  bbs = get_loop_body (loop);
  
  for (i = 0; i < loop->num_nodes; i++)
    {
      bb = bbs[i];
    
      for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
        {
          tree stmt = bsi_stmt (bsi);
	
          if (!loop_nest_control_p (rdg, stmt))
            {
              rdg_vertex_p v = RDG_VERTEX (rdg, vertex_index);
              RDGV_STMT (v) = stmt;
              RDGV_N (v) = vertex_index;
              RDGV_BB (v) = i;
              RDGV_COLOR (v) = 0;
              RDGV_DD_P (v) = contains_dr_p (stmt, RDG_DR (rdg));
              RDGV_IN (v) = VEC_alloc (rdg_edge_p, heap, RDG_VS) ;
              RDGV_OUT (v) = VEC_alloc (rdg_edge_p, heap, RDG_VS) ;
              RDGV_PARTITIONS (v) = VEC_alloc (int, heap, RDG_VS) ;
              RDGV_SCCS (v) = VEC_alloc (int, heap, RDG_VS) ;
              vertex_index++;
            }
        }
    }
  free (bbs);
}

/* Checks whether the modify expression correspond to something we
   can deal with.  */

static bool
correct_modify_expr_p (tree stmt)
{
  tree lhs;
  
  if (TREE_CODE (stmt) != MODIFY_EXPR)
    return false;
  
  lhs = TREE_OPERAND (stmt, 0);
    
  switch (TREE_CODE (lhs))
    {
      case SSA_NAME:
      case ARRAY_REF:
      case INDIRECT_REF:
        return true;
      default:
        return false;
    }
}

/* Checks the statements of a loop body.  */

static bool
check_statements (struct loop *loop)
{
  basic_block *bbs;
  basic_block bb;
  unsigned int i;
  block_stmt_iterator bsi;
  
  bbs = get_loop_body (loop);  
  
  for (i = 0; i < loop->num_nodes; i++)
    {
      bb = bbs[i];
    
      for (bsi = bsi_start (bb); !bsi_end_p (bsi); bsi_next (&bsi))
        {
          tree stmt = bsi_stmt (bsi);
        
          if (TREE_CODE (stmt) == MODIFY_EXPR
	      && !correct_modify_expr_p (stmt))
	    return false;
        }
    }

  free (bbs);
  return true;
}

/* Computes the number of uses of a lvalue.  */

static int
number_of_lvalue_uses (rdg_p rdg, tree stmt)
{
  tree lhs;
  
  gcc_assert (TREE_CODE (stmt) == MODIFY_EXPR);
  
  lhs = TREE_OPERAND (stmt, 0);
  
  if (TREE_CODE (lhs) == SSA_NAME)
    {
      use_operand_p imm_use_p;
      imm_use_iterator iterator;
      int n = 0;

      FOR_EACH_IMM_USE_FAST (imm_use_p, iterator, lhs)
        if (find_vertex_with_stmt (rdg, USE_STMT (imm_use_p)))
          n++;

      return n;
    }  

  return 0;
}

/* Computes the number of scalar dependences to add to the RDG.  */

static int
number_of_scalar_dependences (rdg_p rdg)
{
  unsigned int i;
  unsigned int nb_deps = 0;

  for (i = 0; i < RDG_NBV (rdg); i++)
    {
      tree stmt = RDGV_STMT (RDG_VERTEX (rdg, i));
    
      if (TREE_CODE (stmt) == MODIFY_EXPR)
        nb_deps += number_of_lvalue_uses (rdg, stmt);
    }

  return nb_deps;
}

/* Computes the number of data dependences to add to the RDG.  */

static int
number_of_data_dependences (rdg_p rdg)
{
  unsigned int nb_deps = 0;
  ddr_p ddr;
  unsigned int i;

  for (i = 0; VEC_iterate (ddr_p, RDG_DDR (rdg), i, ddr); i++)
    if (DDR_ARE_DEPENDENT (ddr) == NULL_TREE)
      nb_deps += DDR_NUM_DIST_VECTS (ddr);
  
  return nb_deps;
}

/* Gets the dependence level with a distance vector.  */

static unsigned int
get_dependence_level (lambda_vector dist_vect, unsigned int length)
{
  unsigned int level;
  unsigned int i;
  
  level = 0; /* 0 means a lexicographic dependence */
  
  for (i = 0; i < length && level == 0; i++)
    if (dist_vect[i] > 0)
      level = i + 1;
  
  return level;
}

/* Creates an edge with a data dependence vector.  */

static void
update_edge_with_ddv (ddr_p ddr, unsigned int index_of_vector, rdg_p rdg,
                      unsigned int index_of_edge)
{
  data_reference_p dra;
  data_reference_p drb;
  rdg_edge_p edge = RDG_EDGE (rdg, index_of_edge);
  rdg_vertex_p va;
  rdg_vertex_p vb;
  
  /* Invert data references according to the direction of the 
     dependence.  */
  if (DDR_REVERSED_P (ddr))
    {
      dra = DDR_B (ddr);
      drb = DDR_A (ddr);
    }
  else
    {
      dra = DDR_A (ddr);
      drb = DDR_B (ddr);
    }
  
  /* Locate the vertices containing the statements that contain
     the data references.  */
  va = find_vertex_with_stmt (rdg, DR_STMT (dra));
  vb = find_vertex_with_stmt (rdg, DR_STMT (drb));
  gcc_assert (va && vb);

  /* Update source and sink of the dependence.  */
  RDGE_SOURCE (edge) = va;
  RDGE_SINK (edge) = vb;
  RDGE_SOURCE_REF (edge) = DR_REF (dra);
  RDGE_SINK_REF (edge) = DR_REF (drb);
  
  /* Determines the type of the data dependence.  */
  if (DR_IS_READ (dra) && DR_IS_READ (drb))
    RDGE_TYPE (edge) = input_dd;
  else if (!DR_IS_READ (dra) && !DR_IS_READ (drb))
    RDGE_TYPE (edge) = output_dd;
  else if (!DR_IS_READ (dra) && DR_IS_READ (drb))
    RDGE_TYPE (edge) = flow_dd;
  else if (DR_IS_READ (dra) && !DR_IS_READ (drb))
    RDGE_TYPE (edge) = anti_dd;

  RDGE_LEVEL (edge) = get_dependence_level (DDR_DIST_VECT (ddr, 
                                                           index_of_vector), 
					    DDR_NB_LOOPS (ddr));
  RDGE_COLOR (edge) = 0;
  RDGE_SCALAR_P (edge) = false;
  
  VEC_safe_push (rdg_edge_p, heap, RDGV_OUT (va), edge);
  VEC_safe_push (rdg_edge_p, heap, RDGV_IN (vb), edge);
}

/* Creates all the edges of a RDG.  */

static void
create_edges (rdg_p rdg)
{
  unsigned int i;
  unsigned int j;
  unsigned int edge_index;
  unsigned int data_edges;
  unsigned int scalar_edges;
  struct data_dependence_relation *ddr;

  scalar_edges = number_of_scalar_dependences (rdg);  
  data_edges = number_of_data_dependences (rdg);
  
  if (scalar_edges == 0 && data_edges == 0)
    {
      RDG_NBE (rdg) = 0;
      RDG_E (rdg) = NULL;
      return;
    }  
  
  /* Allocate an array for scalar edges and data edges.  */
  RDG_NBE (rdg) = scalar_edges + data_edges;
  RDG_E (rdg) = XCNEWVEC (struct rdg_edge, RDG_NBE (rdg));

  /* Create data edges.  */
  edge_index = 0;
  
  for (i = 0; VEC_iterate (ddr_p, RDG_DDR (rdg), i, ddr); i++)
    if (DDR_ARE_DEPENDENT (ddr) == NULL_TREE) 
      for (j = 0; j < DDR_NUM_DIST_VECTS (ddr); j++)
        update_edge_with_ddv (ddr, j, rdg, edge_index++);
          
  /* Create scalar edges. The principle is as follows: for each vertex, 
     if the vertex represents a MODIFY_EXPR (an assignment), we create one
     edge for each use of the SSA_NAME on the LHS. This edge
     represents a flow scalar dependence of level 0.  */
  
  for (i = 0; i < RDG_NBV (rdg); i++)
    {
      rdg_vertex_p def_vertex = RDG_VERTEX (rdg, i);
      tree stmt = RDGV_STMT (def_vertex);

      if (TREE_CODE (stmt) == MODIFY_EXPR)
        {
          tree lhs = TREE_OPERAND (stmt, 0);
	
          if (TREE_CODE (lhs) == SSA_NAME)
            {
              use_operand_p imm_use_p;
              imm_use_iterator iterator;
           
              FOR_EACH_IMM_USE_FAST (imm_use_p, iterator, lhs)
                {
                  rdg_vertex_p use_vertex;
		  
		  use_vertex = find_vertex_with_stmt (rdg, 
						      USE_STMT (imm_use_p));
		  
		  /* If use_vertex != NULL, it means that there is a vertex
		     in the RDG that uses the value defined in 
		     def_vertex.  */
                  if (use_vertex) 
		    {
		      rdg_edge_p edge = RDG_EDGE (rdg, edge_index);
		    
		      RDGE_LEVEL (edge) = 0;
		      RDGE_SINK (edge) = use_vertex;
                      RDGE_SOURCE (edge) = def_vertex;
                      RDGE_SINK_REF (edge) = *(imm_use_p->use);
                      RDGE_SOURCE_REF (edge) = lhs;
                      RDGE_COLOR (edge) = 0;
                      RDGE_TYPE (edge) = flow_dd;
		      RDGE_SCALAR_P (edge) = true;
		      VEC_safe_push (rdg_edge_p, heap, 
                                     RDGV_IN (use_vertex), edge);
		      VEC_safe_push (rdg_edge_p, heap, 
                                     RDGV_OUT (def_vertex), edge);
                      edge_index++;
                    }
                }
            }  
        }
    }

  gcc_assert (edge_index == RDG_NBE (rdg));
}

/* Get the loop index of a loop nest.  */

static tree
get_loop_index (struct loop *loop)
{
  tree expr = get_loop_exit_condition (loop);
  tree ivarop;
  tree test;

  if (expr == NULL_TREE)
    return NULL_TREE;

  if (TREE_CODE (expr) != COND_EXPR)
    return NULL_TREE;

  test = TREE_OPERAND (expr, 0);

  if (!COMPARISON_CLASS_P (test))
    return NULL_TREE;
  
  if (expr_invariant_in_loop_p (loop, TREE_OPERAND (test, 0)))
    ivarop = TREE_OPERAND (test, 1);
  else if (expr_invariant_in_loop_p (loop, TREE_OPERAND (test, 1)))
    ivarop = TREE_OPERAND (test, 0);
  else
    return NULL_TREE;
  
  if (TREE_CODE (ivarop) != SSA_NAME)
    return NULL_TREE;
  
  return ivarop;
}

/* Returns true if the dependences are all computable.  */

static bool
known_dependences_p (VEC (ddr_p, heap) *dependence_relations)
{
  ddr_p ddr;
  unsigned int i;

  for (i = 0; VEC_iterate (ddr_p, dependence_relations, i, ddr); i++)
    if (DDR_ARE_DEPENDENT (ddr) == chrec_dont_know)
      return false;
 
  return true;
}

/* Returns the number of phi-nodes of a basic block.  */

static int
number_of_phi_nodes (basic_block bb)
{
  tree phi = phi_nodes (bb);
  int n;
  
  for (n = 0; phi; phi = PHI_CHAIN (phi))
    if (is_gimple_reg (PHI_RESULT (phi)))
      n++;
  
  return n;
}

/* Returns the phi-node containing the loop index.
   Right know it just returns the first valid phi-node it finds.  */

static tree
get_index_phi_node (struct loop *loop)
{
  tree phi = phi_nodes (loop->header);
  
  for (; phi; phi = PHI_CHAIN (phi))
    if (is_gimple_reg (PHI_RESULT (phi)))
      return phi;
  
  return NULL_TREE;
}

static void
dump_check_info (const char *msg)
{
  if (dump_file)
    fprintf (dump_file, "<loop_check>%s</loop_check>\n", msg);
}

/* Returns true when LOOP is simple enough to be analyzed and
   distributed.  */

static bool
can_be_distributed (struct loop *loop)
{
  /* Right now, we only deal with loop nests that are at depth 1.  */
  if (loop->depth != 1)
    {
      dump_check_info ("depth != 1");
      return false;
    }

  /* Right now, only consider single loop nests.  */
  else if (loop->inner)
    {
      dump_check_info ("Loop has inner loop(s)");      
      return false;
    }

  /* Only consider loops with a single exit.  */
  else if (!single_exit (loop))
    {
      dump_check_info ("More than one exit");
      return false;
    }

  /* The exit condition is too difficult to analyze.  */
  else if (!get_loop_exit_condition (loop))
    {
      dump_check_info ("Cannot determine loop exit condition");
      return false;
    }

  /* If we have two basic blocks, it means that we have the loop body
     plus the basic block containing the exit label.  If we have more
     that two basic blocks, it means that we have some complicated
     control flow.  */
  else if (loop->num_nodes != 2)
    {
      dump_check_info ("Complicated control flow");
      return false;
    }

  /* Too many incoming edges.  */
  else if (EDGE_COUNT (loop->header->preds) != 2)
    {
      dump_check_info ("Too many incoming edges");
      return false;
    }

  /* The loop exit condition must be at the end of the loop, the loop
     header has to contain all the executable statements and the latch
     has to be empty.  */
  else if (!empty_block_p (loop->latch))
    {
      dump_check_info ("Bad loop form");
      return false;
    }

  /* The loop must contain some statements.  */
  else if (empty_block_p (loop->header))
    {
      dump_check_info ("Empty loop body");
      return false;
    }  

  /* We consider that we should have no more than 1 PHI node
     representing the loop index.  */
  else if (number_of_phi_nodes (loop->header) > 1)
    {
      dump_check_info ("More than one PHI node");
      return false; 
    }

  /* Some lvalues are not correct and then cannot be handled right
     now.  */
  else if (!check_statements (loop))
    {
      dump_check_info ("Bad statement(s) in loop body");
      return false;
    }

  /* We are not able to find out the loop index.  */
  else if (!get_loop_index (loop))
    {
      dump_check_info ("Cannot find loop index");
      return false;
    }

  /* Cannot find the PHI node of the loop index.  */
  else if (!get_index_phi_node (loop))
    {
      dump_check_info ("Cannot find loop index PHI node");
      return false;
    }
  
  /* Note that we do not have to check whether the statements inside
     the loop body have side effects or not because this check is
     is going to be done by the data dependence analyzer. */
  dump_check_info ("OK");

  return true;
}

/* Build a Reduced Dependence Graph with one vertex per statement of the
   loop nest and one edge per data dependence or scalar dependence.  */

static rdg_p
build_rdg (struct loop *loop)
{
  rdg_p rdg;
  VEC (ddr_p, heap) *dependence_relations;
  VEC (data_reference_p, heap) *datarefs;
  unsigned int i;
  rdg_vertex_p vertex;
  
  /* Compute array data dependence relations */
  dependence_relations = VEC_alloc (ddr_p, heap, RDG_VS * RDG_VS) ;
  datarefs = VEC_alloc (data_reference_p, heap, RDG_VS);
  compute_data_dependences_for_loop (loop, 
                                     false,
                                     &datarefs,
                                     &dependence_relations);
  
  /* Check if all the array dependences are known (computable) */
  if (!known_dependences_p (dependence_relations))
    {
      dump_check_info ("Dependences: not computable");
      free_dependence_relations (dependence_relations);
      free_data_refs (datarefs);
      return NULL;
    }
  else
    dump_check_info ("Dependences: OK");
  
  /* OK, now we know that we can build our Reduced Dependence Graph
     where each vertex is a statement and where each edge is a data
     dependence between two references in statements. */
  rdg = XNEW (struct rdg);
  RDG_LOOP (rdg) = loop;
  RDG_EXIT_COND (rdg) = get_loop_exit_condition (loop);
  RDG_IDX (rdg) = get_loop_index (loop);
  RDG_IDX_UPDATE (rdg) = SSA_NAME_DEF_STMT (RDG_IDX (rdg));
  RDG_IDX_PHI (rdg) = get_index_phi_node (loop);

  RDG_DDR (rdg) = dependence_relations;
  RDG_DR (rdg) = datarefs;
  
  create_vertices (rdg);
  create_edges (rdg);

  RDG_DDV (rdg) = VEC_alloc (rdg_vertex_p, heap, RDG_VS);
  
  for (i = 0; i < RDG_NBV (rdg); i++)
    {
      vertex = RDG_VERTEX (rdg, i);

      if (RDGV_DD_P (vertex))
	VEC_safe_push (rdg_vertex_p, heap, RDG_DDV (rdg), vertex);
    }
    
  return rdg;
}

/* Free RDG.  */

static void
free_rdg (rdg_p rdg)
{
  unsigned int i;
  free_dependence_relations (RDG_DDR (rdg));
  free_data_refs (RDG_DR (rdg));
  
  if (RDG_NBV (rdg))
    {
      for (i = 0; i < RDG_NBV (rdg); i++)
        {
          rdg_vertex_p v = RDG_VERTEX (rdg, i);
        
          VEC_free (rdg_edge_p, heap, RDGV_IN (v));
          VEC_free (rdg_edge_p, heap, RDGV_OUT (v));
          VEC_free (int, heap, RDGV_PARTITIONS (v));
          VEC_free (int, heap, RDGV_SCCS (v));
        }
      free (RDG_V (rdg));
    }
  
  if (RDG_NBE (rdg))
    free (RDG_E (rdg));

  VEC_free (rdg_vertex_p, heap, RDG_DDV (rdg));
  free (rdg);
}

/* Sort topologically PRDG vertices.  */

static VEC (prdg_vertex_p, heap) *
topological_sort (prdg_p g)
{
  unsigned int max_f, i;
  prdg_vertex_p *vertices;
  prdg_vertex_p v;
  VEC (prdg_vertex_p, heap) *sorted_vertices;
  
  /* Depth First Search.  */
  max_f = dfs_rdgp (g);
  
  /* Allocate array of vertices.  */
  vertices = XCNEWVEC (prdg_vertex_p, max_f+1);
  
  /* Allocate a vector for sorted vertices.  */ 
  sorted_vertices = VEC_alloc (prdg_vertex_p, heap, RDG_VS);
  
  /* All vertices are set to NULL.  */
  for (i = 0; i <= max_f; i++)
    vertices[i] = NULL;
  
  /* Iterate on each vertex of the PRDG and put each vertex at
     the right place.  */
  for (i = 0; VEC_iterate (prdg_vertex_p, PRDG_V (g), i, v); i++)
    vertices[PRDGV_F (v)] = v;

  /* Push all non-NULL vertices to vector of vertices.  */
  for (i = max_f; i > 0; i--)
    if (vertices[i])
      VEC_safe_push (prdg_vertex_p, heap, sorted_vertices, vertices[i]);
  
  free (vertices);
  
  return sorted_vertices;
}

/* Distributes the code from LOOP in such a way that producer
   statements are placed before consumer statements.  */

void
distribute_loop (struct loop *loop)
{
  rdg_p rdg; /* Reduced dependence graph.  */
  prdg_p rdgp; /* Graph of RDG partitions.  */
  prdg_p sccg; /* Graph of Strongly Connected Components.  */
  VEC (prdg_vertex_p, heap) *dloops; /* Distributed loops.  */

  if (dump_file)
    {
      fprintf (dump_file , "<LOOP num=\"%d\">\n", loop->num);
      dump_loop_infos (dump_file, loop);
    }  

  if (!can_be_distributed (loop))
    {
      if (dump_file)
	fprintf (dump_file, "</LOOP>\n");
      return;
    }

  rdg = build_rdg (loop);
  
  if (!rdg)
    {
      if (dump_file)
	fprintf (dump_file, "</LOOP>\n");
      return;
    }
  
  if (dump_file)
    {
      fprintf (dump_file, "<rdg>\n");
      dump_rdg (dump_file, rdg);
      fprintf (dump_file, "</rdg>\n");
    }

  rdgp = build_prdg (rdg);      
  
  if (dump_file)
    {
      fprintf (dump_file, "<prdg>\n");
      dump_prdg (dump_file, rdgp);
      fprintf (dump_file, "</prdg>\n");
    }
  
  sccg = build_scc_graph (rdgp);

  if (dump_file)
    {
      fprintf (dump_file, "<sccp>\n");
      dump_prdg (dump_file, sccg);
      fprintf (dump_file, "</sccp>\n");
    }
  
  dloops = topological_sort (sccg);

  if (dump_file)
    {
      prdg_vertex_p v;
      int i;
      
      fprintf (dump_file, "<topological_sort>\n");
      
      for (i = 0; VEC_iterate (prdg_vertex_p, dloops, i, v); i++)
        fprintf (dump_file, "  <dloop n=\"%d\">P%d</dloop>\n", 
                 i, PRDGV_N (v));
      
      fprintf (dump_file, "</topological_sort>\n");
    }
  
  free_rdg (rdg);
  free_prdg (rdgp);
  free_prdg (sccg);
  VEC_free (prdg_vertex_p, heap, dloops);

  if (dump_file)
    fprintf (dump_file, "</LOOP>\n");
}

/* Distribute all loops in the current function.  */

static unsigned int
tree_loop_distribution (void)
{
  struct loop *loop;
  loop_iterator li;

  if (dump_file)
    {
      fprintf (dump_file, "<distribute_loops>\n");
    
      if (current_function_decl)
        {
          fprintf (dump_file, "<function name=\"%s\"><![CDATA[\n",
                   lang_hooks.decl_printable_name (current_function_decl, 
                                                   2));
          dump_function_to_file (current_function_decl,
                                 dump_file, dump_flags);
          fprintf (dump_file, "]]></function>\n");
        }
    }

  FOR_EACH_LOOP (li, loop, 0)
    distribute_loop (loop);

  if (dump_file)
    {
      fprintf (dump_file, "</distribute_loops>\n");
    }
  
  return 0;
}

static bool
gate_tree_loop_distribution (void)
{
  return flag_tree_loop_distribution != 0;
}

struct tree_opt_pass pass_loop_distribution =
{
  "ldist",			/* name */
  gate_tree_loop_distribution,  /* gate */
  tree_loop_distribution,       /* execute */
  NULL,				/* sub */
  NULL,				/* next */
  0,				/* static_pass_number */
  TV_TREE_LOOP_DISTRIBUTION,    /* tv_id */
  PROP_cfg | PROP_ssa,		/* properties_required */
  0,				/* properties_provided */
  0,				/* properties_destroyed */
  0,				/* todo_flags_start */
  TODO_verify_loops,            /* todo_flags_finish */
  0                             /* letter */  
};
