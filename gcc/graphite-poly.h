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

#ifndef GCC_GRAPHITE_POLY_H
#define GCC_GRAPHITE_POLY_H

typedef struct poly_dr *poly_dr_p;
DEF_VEC_P(poly_dr_p);
DEF_VEC_ALLOC_P (poly_dr_p, heap);

typedef struct poly_bb *poly_bb_p;
DEF_VEC_P(poly_bb_p);
DEF_VEC_ALLOC_P (poly_bb_p, heap);

typedef struct scop *scop_p;
DEF_VEC_P(scop_p);
DEF_VEC_ALLOC_P (scop_p, heap);

typedef ppl_dimension_type graphite_dim_t;

static inline graphite_dim_t pbb_nb_loops (const struct poly_bb*);
static inline graphite_dim_t pbb_nb_scattering (const struct poly_bb*);
static inline graphite_dim_t pbb_nb_params (poly_bb_p);
static inline graphite_dim_t scop_nb_params (scop_p);

/* A data reference can write or read some memory or we
   just know it may write some memory.  */
enum POLY_DR_TYPE
{
  PDR_READ,
  /* PDR_MAY_READs are represented using PDR_READS. This does not limit the
     expressiveness.  */
  PDR_WRITE,
  PDR_MAY_WRITE
};

struct poly_dr
{
  poly_bb_p black_box;

  enum POLY_DR_TYPE type;

  /* The access polyhedron contains the polyhedral space this data
     reference will access.

     The polyhedron contains these dimensions: 

      - The alias set (a):
      Every memory access is classified in at least one alias set.
    
      - The subscripts (s_0, ..., s_n):
      The memory is accessed using zero or more subscript dimensions.

      - The iteration domain (variables and parameters) 

     Do not hardcode the dimensions. Use the accessors pdr_accessp_*_dim.
     
     Example:

     | int A[1335][123];
     | int *p = malloc ();
     |
     | b = ...
     | for i
     |   {
     |     if (unknown_function ())
     |       p = A;
     |       ... = p[?][?];
     | 	   for j
     |       A[i][j+b] = m; 
     |   }

     The data access A[i][j+b] in alias set "5" is described like this:

     | i   j   k   a   s0  s1  1
     | 0   0   0   1   0   0  -5     =  0
     |-1   0   0   0   1   0   0     =  0
     | 0  -1  -1   0   0   1   0     =  0
     | 0   0   0   0   1   0   0     >= 0  # The last four lines describe the
     | 0   0   0   0   0   1   0     >= 0  # array size.
     | 0   0   0   0   1   0  -1335  <= 0
     | 0   0   0   0   0   1  -123   <= 0

     The pointer "*p" in alias set "5" and "7" is described like this:

     | i   k   a   s0  1
     | 0   0   1   0  -5   =  0
     | 0   0   1   0  -7   =  0
     | 0   0   0   1   0   >= 0

     "*p" accesses all of the object allocated with 'malloc'.

     The scalar data access "m" is represented as an array with zero subscript
     dimensions.

     | i   j   k   a   1
     | 0   0   0  -1   15  = 0 */
  ppl_Polyhedron_t accesses;
};

#define PDR_BB(PDR) (PDR->black_box)
#define PDR_TYPE(PDR) (PDR->type)
#define PDR_BASE(PDR) (PDR->base)
#define PDR_ACCESSES(PDR) (PDR->accesses)

/* The number of subscript dims in PDR.  */

static inline graphite_dim_t
pdr_accessp_nb_subscripts (poly_dr_p pdr)
{
  poly_bb_p pbb = PDR_BB (pdr);
  ppl_dimension_type dim;

  ppl_Polyhedron_space_dimension (PDR_ACCESSES (pdr), &dim);
  return dim - pbb_nb_loops (pbb) - pbb_nb_params (pbb) - 1;
}

/* The dimension in PDR containing iterator ITER.  */

static inline ppl_dimension_type
pdr_accessp_nb_iterators (poly_dr_p pdr ATTRIBUTE_UNUSED)
{
  poly_bb_p pbb = PDR_BB (pdr);
  return pbb_nb_loops (pbb);
}

/* The dimension in PDR containing parameter PARAM.  */

static inline ppl_dimension_type
pdr_accessp_nb_params (poly_dr_p pdr)
{
  poly_bb_p pbb = PDR_BB (pdr);
  return pbb_nb_params (pbb);
}

/* The dimension of the alias set in PDR.  */

static inline ppl_dimension_type
pdr_accessp_alias_set_dim (poly_dr_p pdr)
{
  poly_bb_p pbb = PDR_BB (pdr);

  return pbb_nb_loops (pbb) + pbb_nb_params (pbb);
} 

/* The dimension in PDR containing subscript S.  */

static inline ppl_dimension_type
pdr_accessp_subscript_dim (poly_dr_p pdr, graphite_dim_t s)
{
  poly_bb_p pbb = PDR_BB (pdr);

  return pbb_nb_loops (pbb) + pbb_nb_params (pbb) + 1 + s;
}

/* The dimension in PDR containing iterator ITER.  */

static inline ppl_dimension_type
pdr_accessp_iterator_dim (poly_dr_p pdr ATTRIBUTE_UNUSED, graphite_dim_t iter)
{
  return iter;
}

/* The dimension in PDR containing parameter PARAM.  */

static inline ppl_dimension_type
pdr_accessp_param_dim (poly_dr_p pdr, graphite_dim_t param)
{
  poly_bb_p pbb = PDR_BB (pdr);

  return pbb_nb_loops (pbb) + param;
}

/* POLY_BB represents a blackbox in the polyhedral model.  */

struct poly_bb 
{
  void *black_box;

  scop_p scop;

  /* The iteration domain of this bb.
     Example:

     for (i = a - 7*b + 8; i <= 3*a + 13*b + 20; i++)
       for (j = 2; j <= 2*i + 5; j++)
         for (k = 0; k <= 5; k++)
           S (i,j,k)

     Loop iterators: i, j, k 
     Parameters: a, b
      
     | i >=  a -  7b +  8
     | i <= 3a + 13b + 20
     | j >= 2
     | j <= 2i + 5
     | k >= 0 
     | k <= 5

     The number of variables in the DOMAIN may change and is not
     related to the number of loops in the original code.  */
  ppl_Polyhedron_t domain;

  /* The data references we access.  */
  VEC (poly_dr_p, heap) *drs;

  /* The scattering function containing the transformations.  */
  ppl_Polyhedron_t transformed_scattering;

  /* The original scattering function.  */
  ppl_Polyhedron_t original_scattering;
};

#define PBB_BLACK_BOX(PBB) ((gimple_bb_p) PBB->black_box)
#define PBB_SCOP(PBB) (PBB->scop)
#define PBB_DOMAIN(PBB) (PBB->domain)
#define PBB_DRS(PBB) (PBB->drs)
#define PBB_TRANSFORMED_SCATTERING(PBB) (PBB->transformed_scattering)
#define PBB_ORIGINAL_SCATTERING(PBB) (PBB->original_scattering)

extern void new_poly_bb (scop_p, void *);
extern void free_poly_bb (poly_bb_p);
extern void debug_loop_vec (poly_bb_p);
extern void schedule_to_scattering (poly_bb_p, int);
extern void print_pbb_domain (FILE *, poly_bb_p);
extern void print_pbb (FILE *, poly_bb_p);
extern void print_scop (FILE *, scop_p);
extern void debug_pbb_domain (poly_bb_p);
extern void debug_pbb (poly_bb_p);
extern void debug_scop (scop_p);

/* Set black box of PBB to BLACKBOX.  */

static inline void
pbb_set_black_box (poly_bb_p pbb, void *black_box)
{
  pbb->black_box = black_box;
}

/* The number of loops around PBB.  */

static inline graphite_dim_t
pbb_nb_loops (const struct poly_bb *pbb)
{
  scop_p scop = PBB_SCOP (pbb);
  ppl_dimension_type dim;

  ppl_Polyhedron_space_dimension (PBB_DOMAIN (pbb), &dim);
  return dim - scop_nb_params (scop);
}

/* The number of scattering dimensions in PBB.  */

static inline graphite_dim_t 
pbb_nb_scattering (const struct poly_bb *pbb)
{
  scop_p scop = PBB_SCOP (pbb);
  ppl_dimension_type dim;

  ppl_Polyhedron_space_dimension (PBB_TRANSFORMED_SCATTERING (pbb), &dim);
  return dim - pbb_nb_loops (pbb) - scop_nb_params (scop);
}

/* The number of params defined in PBB.  */

static inline graphite_dim_t
pbb_nb_params (poly_bb_p pbb)
{
  scop_p scop = PBB_SCOP (pbb); 

  return scop_nb_params (scop);
}

/* A SCOP is a Static Control Part of the program, simple enough to be
   represented in polyhedral form.  */
struct scop
{
  /* A SCOP is defined as a SESE region.  */
  void *region;

  /* Number of parameters in SCoP.  */
  graphite_dim_t nb_params;

  /* All the basic blocks in this scop that contain memory references
     and that will be represented as statements in the polyhedral
     representation.  */
  VEC (poly_bb_p, heap) *bbs;

  /* Data dependence graph for this SCoP.  */
  struct graph *dep_graph;
};

#define SCOP_BBS(S) (S->bbs)
#define SCOP_REGION(S) ((sese) S->region)
#define SCOP_DEP_GRAPH(S) (S->dep_graph)

extern scop_p new_scop (void *);
extern void free_scop (scop_p);
extern void free_scops (VEC (scop_p, heap) *);
extern void print_generated_program (FILE *, scop_p);
extern void debug_generated_program (scop_p);
extern void print_scattering_function (FILE *, poly_bb_p);
extern void print_scattering_functions (FILE *, scop_p);
extern void debug_scattering_function (poly_bb_p);
extern void debug_scattering_functions (scop_p);
extern void print_iteration_domain (FILE *, poly_bb_p);
extern int scop_max_loop_depth (scop_p);
extern int unify_scattering_dimensions (scop_p);
extern bool apply_poly_transforms (scop_p);

/* Set the region of SCOP to REGION.  */

static inline void 
scop_set_region (scop_p scop, void *region)
{
  scop->region = region;
}

/* Returns the number of parameters for SCOP.  */

static inline graphite_dim_t
scop_nb_params (scop_p scop)
{
  return scop->nb_params;
}

/* Set the number of params of SCOP to NB_PARAMS.  */

static inline void
scop_set_nb_params (scop_p scop, graphite_dim_t nb_params)
{
  scop->nb_params = nb_params;
}

#endif
