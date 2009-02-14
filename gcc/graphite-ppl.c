/* Gimple Represented as Polyhedra.
   Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Sebastian Pop <sebastian.pop@amd.com>
   and Tobias Grosser <grosser@fim.uni-passau.de>

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

#include "graphite-ppl.h"
#include <tsystem.h>

/* Translates row ROW of the CloogMatrix MATRIX to a PPL Constraint.  */

static ppl_Constraint_t
cloog_matrix_to_ppl_constraint (CloogMatrix *matrix, int row)
{
  int j;
  ppl_Constraint_t cstr;
  ppl_Coefficient_t coef;
  ppl_Linear_Expression_t expr;
  ppl_dimension_type dim = matrix->NbColumns - 2;

  ppl_new_Coefficient (&coef);
  ppl_new_Linear_Expression_with_dimension (&expr, dim);

  for (j = 1; j < matrix->NbColumns - 1; j++)
    {
      ppl_assign_Coefficient_from_mpz_t (coef, matrix->p[row][j]);
      ppl_Linear_Expression_add_to_coefficient (expr, j - 1, coef);
    }

  ppl_assign_Coefficient_from_mpz_t (coef,
				     matrix->p[row][matrix->NbColumns - 1]);
  ppl_Linear_Expression_add_to_inhomogeneous (expr, coef);
  ppl_delete_Coefficient (coef);

  if (matrix->p[row][0] == 0)
    ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_EQUAL);
  else
    ppl_new_Constraint (&cstr, expr, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);

  ppl_delete_Linear_Expression (expr);
  return cstr;
}

/* Creates a PPL constraint system from MATRIX.  */

static void
new_Constraint_System_from_Cloog_Matrix (ppl_Constraint_System_t *pcs,
					 CloogMatrix *matrix)
{
  int i;

  ppl_new_Constraint_System (pcs);

  for (i = 0; i < matrix->NbRows; i++)
    {
      ppl_Constraint_t c = cloog_matrix_to_ppl_constraint (matrix, i);
      ppl_Constraint_System_insert_Constraint (*pcs, c);
      ppl_delete_Constraint (c);
    }
}

/* Creates a PPL Polyhedron from MATRIX.  */

void
new_NNC_Polyhedron_from_Cloog_Matrix (ppl_Polyhedron_t *ph,
				      CloogMatrix *matrix)
{
  ppl_Constraint_System_t cs;
  new_Constraint_System_from_Cloog_Matrix (&cs, matrix);
  ppl_new_NNC_Polyhedron_recycle_Constraint_System (ph, cs);
}

/* Counts the number of constraints in PCS.  */

static int
ppl_Constrain_System_number_of_constraints (ppl_const_Constraint_System_t pcs)
{
  ppl_Constraint_System_const_iterator_t cit, end;
  int num = 0;

  ppl_new_Constraint_System_const_iterator (&cit);
  ppl_new_Constraint_System_const_iterator (&end);

  for (ppl_Constraint_System_begin (pcs, cit),
        ppl_Constraint_System_end (pcs, end);
       !ppl_Constraint_System_const_iterator_equal_test (cit, end);
       ppl_Constraint_System_const_iterator_increment (cit))
    num++;

  ppl_delete_Constraint_System_const_iterator (cit);
  ppl_delete_Constraint_System_const_iterator (end);
  return num;
}

static void
oppose_constraint (CloogMatrix *m, int row)
{
  int k;

  /* Do not oppose the first column: it is the eq/ineq one.  */
  for (k = 1; k < m->NbColumns; k++)
    value_oppose (m->p[row][k], m->p[row][k]);
}

/* Inserts constraint CSTR at row ROW of matrix M.  */

void
insert_constraint_into_matrix (CloogMatrix *m, int row,
			       ppl_const_Constraint_t cstr)
{
  ppl_Coefficient_t c;
  ppl_dimension_type i, dim, nb_cols = m->NbColumns;

  ppl_Constraint_space_dimension (cstr, &dim);
  ppl_new_Coefficient (&c);

  for (i = 0; i < dim; i++)
    {
      ppl_Constraint_coefficient (cstr, i, c);
      ppl_Coefficient_to_mpz_t (c, m->p[row][i + 1]);
    }

  for (i = dim; i < nb_cols - 1; i++)
    value_set_si (m->p[row][i + 1], 0);

  ppl_Constraint_inhomogeneous_term  (cstr, c);
  ppl_Coefficient_to_mpz_t (c, m->p[row][nb_cols - 1]);
  value_set_si (m->p[row][0], 1);

  switch (ppl_Constraint_type (cstr))
    {
    case PPL_CONSTRAINT_TYPE_LESS_THAN:
      oppose_constraint (m, row);
    case PPL_CONSTRAINT_TYPE_GREATER_THAN:
      value_sub_int (m->p[row][nb_cols - 1],
		     m->p[row][nb_cols - 1], 1);
      break;

    case PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL:
      oppose_constraint (m, row);
    case PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL:
      break;

    case PPL_CONSTRAINT_TYPE_EQUAL:
      value_set_si (m->p[row][0], 0);
      break;

    default:
      /* Not yet implemented.  */
      gcc_unreachable();
    }

  ppl_delete_Coefficient (c);
}

/* Creates a CloogMatrix from constraint system PCS.  */

static CloogMatrix *
new_Cloog_Matrix_from_ppl_Constraint_System (ppl_const_Constraint_System_t pcs)
{
  CloogMatrix *matrix;
  ppl_Constraint_System_const_iterator_t cit, end;
  ppl_dimension_type dim;
  int rows;
  int row = 0;

  rows  = ppl_Constrain_System_number_of_constraints (pcs);
  ppl_Constraint_System_space_dimension (pcs, &dim);
  matrix = cloog_matrix_alloc (rows, dim + 2);
  ppl_new_Constraint_System_const_iterator (&cit);
  ppl_new_Constraint_System_const_iterator (&end);

  for (ppl_Constraint_System_begin (pcs, cit),
        ppl_Constraint_System_end (pcs, end);
       !ppl_Constraint_System_const_iterator_equal_test (cit, end);
       ppl_Constraint_System_const_iterator_increment (cit))
    {
      ppl_const_Constraint_t c;
      ppl_Constraint_System_const_iterator_dereference (cit, &c);
      insert_constraint_into_matrix (matrix, row, c);
      row++;
    }

  ppl_delete_Constraint_System_const_iterator (cit);
  ppl_delete_Constraint_System_const_iterator (end);

  return matrix;
}

/* Creates a CloogMatrix from constraint system PCS.  */

CloogMatrix *
new_Cloog_Matrix_from_ppl_Polyhedron (ppl_const_Polyhedron_t ph)
{
  ppl_const_Constraint_System_t pcs;
  CloogMatrix *res;

  ppl_Polyhedron_get_constraints (ph, &pcs);
  res = new_Cloog_Matrix_from_ppl_Constraint_System (pcs);

  return res;
}
