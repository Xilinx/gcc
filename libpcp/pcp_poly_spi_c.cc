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

#include "config.h"
#include "pcp_poly_spi.h"
#include "pcp_dynamic_array.h"
#include "pcp_string_buffer.h"
#include "pcp_poly_spi_c.h"

class PcpPolySpiC : public PcpPolySpi
{
private:
  struct pcp_poly_spi* interface;

public:

    // Coefficient primitives
int
coefficientGetValue(Coefficient* coeff)
{
  return this->interface->coefficientGetValue(coeff);
}

PcpPolySpi::Coefficient*
coefficientCreate(int value) 
{
  return (PcpPolySpi::Coefficient*)this->interface->coefficientCreate(value);
}

const char*
coefficientToString(Coefficient* coeff)
{
  return this->interface->coefficientToString(coeff);
}

void coefficientDestroy(Coefficient* coeff)
{
  this->interface->coefficientDestroy(coeff);
}


// Linear Expr Primitives
PcpPolySpi::LinearExpr* linearExprCopy(LinearExpr* linearExpr)
{
  return (PcpPolySpi::LinearExpr*)this->interface->linearExprCopy(linearExpr);
}

void linearExprAddToCoefficient(LinearExpr* linearExpr, int dimension, Coefficient* value)
{
  this->interface->linearExprAddToCoefficient(linearExpr, dimension, value);
}

void linearExprAddToInhomogenous(LinearExpr* linearExpr, Coefficient* value)
{
  this->interface->linearExprAddToInhomogenous(linearExpr, value);
}

void linearExprSetCoefficient(LinearExpr* linearExpr, int dimension, Coefficient* value)
{
  this->interface->linearExprSetCoefficient(linearExpr, dimension, value);
}


int linearExprGetNumDimensions(LinearExpr* linearExpr)
{
  return this->interface->linearExprGetNumDimensions(linearExpr);
}

void linearExprSubtract(LinearExpr* linearExpr, LinearExpr* subtract)
{
  this->interface->linearExprSubtract(linearExpr, subtract);
}

const char*
linearExprToString(LinearExpr* linearExpr)
{
  return this->interface->linearExprToString(linearExpr);
}

PcpPolySpi::LinearExpr* linearExprCreate()
{
  return (PcpPolySpi::LinearExpr*)this->interface->linearExprCreate();
}

PcpPolySpi::LinearExpr* linearExprCreate(int numDimensions)
{
  return (PcpPolySpi::LinearExpr*) this->interface->linearExprCreateDim(numDimensions);
}

void linearExprDestroy(LinearExpr* linearExpr)
{
  this->interface->linearExprDestroy(linearExpr);
}

  // Constraint primitives
PcpPolySpi::Constraint* constraintCreateEqualZero(LinearExpr* linearExpr)
{
  return (PcpPolySpi::Constraint*) this->interface->constraintCreateEqualZero(linearExpr);
}

PcpPolySpi::Constraint* constraintCreateGreaterEqualZero(LinearExpr* linearExpr)
{
  return (PcpPolySpi::Constraint*) this->interface->constraintCreateGreaterEqualZero(linearExpr);
}

void constraintDestroy(Constraint* constraint)
{
  this->interface->constraintDestroy(constraint);
}

const char*
constraintToString(Constraint* constraint)
{
  return this->interface->constraintToString(constraint);
}

// Polyhedron primitives
PcpPolySpi::ConstraintSystem* polyhedronGetConstraintSystem(Polyhedron* polyhedron)
{
  return (PcpPolySpi::ConstraintSystem*) this->interface->polyhedronGetConstraintSystem(polyhedron);
}


void polyhedronAddConstraint(Polyhedron* polyhedron, Constraint* constraint)
{
  this->interface->polyhedronAddConstraint(polyhedron, constraint);
}

void polyhedronAddConstraints(Polyhedron* polyhedron, ConstraintSystem* constraintSystem)
{
  this->interface->polyhedronAddConstraints(polyhedron, constraintSystem);
}

void polyhedronRemap(Polyhedron* polyhedron, PcpArray<int>* map)
{
  // FIXME: implement conversion from array to int*
  int size = map->getSize();
  int* array = (int*)malloc(sizeof(int)*size);
  for(int i = 0; i < size; i++)
    array[i] = map->get(i);
  this->interface->polyhedronRemap(polyhedron, array, size);
}

PcpPolySpi::Polyhedron* polyhedronCopy(Polyhedron* polyhedron)
{
  return (PcpPolySpi::Polyhedron*) this->interface->polyhedronCopy(polyhedron);
}

PcpPolySpi::Polyhedron* polyhedronCreate(int numDimensions)
{
  return (PcpPolySpi::Polyhedron*) this->interface->polyhedronCreate(numDimensions);
}

void polyhedronDestroy(Polyhedron* polyhedron)
{
  this->interface->polyhedronDestroy(polyhedron);
}

void pointsetPowersetDestroy(PointsetPowerset* poinsetPowerset)
{
  this->interface->pointsetPowersetDestroy(poinsetPowerset);
}


PcpPolySpi::PointsetPowerset* pointsetPowersetCreate(Polyhedron* polyhedron)
{
  return (PcpPolySpi::PointsetPowerset*) this->interface->pointsetPowersetCreate(polyhedron);
}

PcpPolySpiC()
{
  this->interface = NULL;
}

PcpPolySpiC(struct pcp_poly_spi* spi)
{
  this->interface = spi;
}

};

#include "cloog/cloog.h"
#include "ppl_c.h"
#include "pcp_poly_spi_c.h"
#include <string.h>
#include "pcp_string_buffer.h"

static void ppl_coefficient_print(ppl_Coefficient_t* coeff)
{
  printf("Coefficient: ");
  ppl_io_print_Coefficient(*coeff);
  printf("\n");
}

static int ppl_coefficient_to_int(void* coeff)
{
  Value v;
  value_init(v);
  ppl_Coefficient_to_mpz_t(*((ppl_Coefficient_t*) coeff), v);
  return value_get_si(v);
}

static const char* ppl_coefficient_to_string(void* coeff)
{
  PcpStringBuffer stringBuffer;
  stringBuffer.appendInt(ppl_coefficient_to_int(coeff));
  return stringBuffer.toString();
}

static void* ppl_coefficient_create(int value)
{
  Value v;
  value_init(v);
  value_set_si(v, value);
  
  ppl_Coefficient_t* coeff = (ppl_Coefficient_t*) malloc(sizeof(ppl_Coefficient_t));
  ppl_new_Coefficient(coeff);
  ppl_assign_Coefficient_from_mpz_t(*coeff, v);
  return coeff;
}

static void ppl_coefficient_destroy(void* coeff)
{
  ppl_delete_Coefficient(*((ppl_Coefficient_t*)coeff));
  free((ppl_Coefficient_t*)coeff);
}


static void ppl_linear_expr_print(ppl_Linear_Expression_t* linearExpr)
{
  printf("Linear Expression: ");
  ppl_io_print_Linear_Expression(*linearExpr);
  printf("\n");
}

static void ppl_linear_expr_add_to_coefficient(void* linearExpr, int dimension, void* value)
{
  ppl_Linear_Expression_add_to_coefficient(*((ppl_Linear_Expression_t*)linearExpr), 
					   dimension, 
					   *((ppl_Coefficient_t*)value));
}

static void ppl_linear_expr_add_to_inhomogenous(void* linearExpr, void* value)
{
  ppl_Linear_Expression_add_to_inhomogeneous(*((ppl_Linear_Expression_t*)linearExpr),
					     *((ppl_Coefficient_t*)value));
}

static void
set_coef (ppl_Linear_Expression_t e, ppl_dimension_type i, int x)
{
  Value v0, v1;
  ppl_Coefficient_t c;

  value_init (v0);
  value_init (v1);
  ppl_new_Coefficient (&c);

  ppl_Linear_Expression_coefficient (e, i, c);
  ppl_Coefficient_to_mpz_t (c, v1);
  value_oppose (v1, v1);
  value_set_si (v0, x);
  value_addto (v0, v0, v1);
  ppl_assign_Coefficient_from_mpz_t (c, v0);
  ppl_Linear_Expression_add_to_coefficient (e, i, c);

  value_clear (v0);
  value_clear (v1);
  ppl_delete_Coefficient (c);
}

static void ppl_linear_expr_set_coefficient(void* linearExpr, int dimension, void* value)
{
  ppl_Coefficient_t coeff;
  ppl_new_Coefficient(&coeff);
  ppl_Linear_Expression_coefficient(*((ppl_Linear_Expression_t*)linearExpr), dimension, coeff);
  set_coef(*((ppl_Linear_Expression_t*)linearExpr), 
	   dimension, 
	   ppl_coefficient_to_int(value));
  
  ppl_Linear_Expression_coefficient(*((ppl_Linear_Expression_t*)linearExpr), dimension, coeff);
}

static int ppl_linear_expr_get_num_dimensions(void* linearExpr)
{
  ppl_dimension_type numDimensions;
  ppl_Linear_Expression_space_dimension (*((ppl_Linear_Expression_t*)linearExpr), &numDimensions);
  return (int)numDimensions;
}

static void ppl_linear_expr_subtract(void* linearExpr0, void* linearExpr1)
{
  ppl_subtract_Linear_Expression_from_Linear_Expression(*((ppl_Linear_Expression_t*)linearExpr0),
							*((ppl_Linear_Expression_t*)linearExpr1));
}

static const char* ppl_linear_expr_to_string(void* linearExpr)
{
  int numDimensions = ppl_linear_expr_get_num_dimensions(linearExpr);
  int i;
  PcpStringBuffer buffer;
  ppl_Coefficient_t coeff;

  ppl_new_Coefficient(&coeff);
  buffer.append("(");
  for(i = 0; i < numDimensions; i++)
    {

      ppl_Linear_Expression_coefficient(*((ppl_Linear_Expression_t*)linearExpr), 
					i,
					coeff);
      buffer.appendInt(ppl_coefficient_to_int(coeff));
      buffer.append(" ");
    }

  ppl_Linear_Expression_inhomogeneous_term(*((ppl_Linear_Expression_t*)linearExpr),
					  coeff);
  buffer.appendInt(ppl_coefficient_to_int(&coeff));
  ppl_delete_Coefficient(coeff);
  return buffer.toString();
}

static void* ppl_linear_expr_create_dim(int numDimensions)
{
  ppl_Linear_Expression_t* linearExpr = (ppl_Linear_Expression_t*) malloc(sizeof(ppl_Linear_Expression_t));
  ppl_new_Linear_Expression_with_dimension(linearExpr, numDimensions);
  return linearExpr;
}

static void ppl_linear_expr_destroy(void* linearExpr)
{
  ppl_delete_Linear_Expression(*((ppl_Linear_Expression_t*)linearExpr));
  free(linearExpr);
}

static void* ppl_constraint_create_equal_zero(void* linearExpr)
{
  ppl_Constraint_t* constraint = (ppl_Constraint_t*) malloc(sizeof(ppl_Constraint_t));
  ppl_new_Constraint(constraint, *((ppl_Linear_Expression_t*)linearExpr), PPL_CONSTRAINT_TYPE_EQUAL);
  return constraint;
}

static void* ppl_constraint_create_greater_equal_zero(void* linearExpr)
{
  ppl_Constraint_t* constraint = (ppl_Constraint_t*) malloc(sizeof(ppl_Constraint_t));
  ppl_new_Constraint(constraint, *((ppl_Linear_Expression_t*)linearExpr), PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
  return constraint;
}

static void ppl_constraint_destory(void* constraint)
{
  ppl_delete_Constraint(*((ppl_Constraint_t*)constraint));
  free(constraint);
}

static const char* ppl_constraint_to_string(void* constr)
{
  ppl_Constraint_t* constraint = (ppl_Constraint_t*) constr;
  bool isEqual = ppl_Constraint_type(*constraint) == PPL_CONSTRAINT_TYPE_EQUAL;
  ppl_dimension_type numDimensions;
  PcpStringBuffer buffer;
  ppl_Coefficient_t coeff;
  int i;

  ppl_new_Coefficient(&coeff);
  ppl_Constraint_space_dimension(*constraint, &numDimensions);
  for(i = 0; i < numDimensions; i++)
    {
      ppl_Constraint_coefficient(*constraint, i, coeff);
      buffer.append(ppl_coefficient_to_string(&coeff));
      buffer.append(" ");
    }
  ppl_Constraint_inhomogeneous_term(*constraint, coeff);
  buffer.appendInt(ppl_coefficient_to_int(&coeff));

  buffer.append( isEqual ? " == 0" : " >= 0" );
  ppl_delete_Coefficient(coeff);
  return buffer.toString();
}

static void* ppl_polyhedron_get_constraint_system(void* polyhedron)
{
  ppl_const_Constraint_System_t* constraintSystem = 
    (ppl_const_Constraint_System_t*) malloc(sizeof(ppl_const_Constraint_System_t));
  ppl_Polyhedron_get_constraints(*((ppl_Polyhedron_t*)polyhedron), constraintSystem);
  return constraintSystem;
}

static void ppl_polyhedron_add_constraint(void* polyhedron, void* constraint)
{
  ppl_Polyhedron_add_constraint(*((ppl_Polyhedron_t*)polyhedron), *((ppl_Constraint_t*)constraint));
}

static void ppl_polyhedron_add_constraints(void* polyhedron, void* constraintsystem)
{
  ppl_Polyhedron_add_constraints(*((ppl_Polyhedron_t*)polyhedron), *((ppl_Constraint_System_t*)constraintsystem));
}

static void ppl_polyhedron_remap(void* polyhedron, int* map, int numElements)
{
  ppl_Polyhedron_map_space_dimensions(*((ppl_Polyhedron_t*)polyhedron), (ppl_dimension_type*)map, numElements);
}

static void* ppl_polyhedron_copy(void* polyhedron)
{
  ppl_Polyhedron_t* copy = (ppl_Polyhedron_t*)malloc(sizeof(ppl_Polyhedron_t*));
  ppl_new_NNC_Polyhedron_from_NNC_Polyhedron(copy, *((ppl_Polyhedron_t*)polyhedron));
  return copy;
}

static void* ppl_polyhedron_create(int numDimensions)
{
  ppl_Polyhedron_t* polyhedron = (ppl_Polyhedron_t*)malloc(sizeof(ppl_Polyhedron_t*));
  ppl_new_NNC_Polyhedron_from_space_dimension(polyhedron, numDimensions, 0);
  return polyhedron;
}

static void ppl_polyhedron_destroy(void* polyhedron)
{
  ppl_delete_Polyhedron(*((ppl_Polyhedron_t*)polyhedron));
}

void* ppl_pointset_powerset_create(void* polyhedron)
{
  ppl_Pointset_Powerset_NNC_Polyhedron_t* pointsetPowerset = (ppl_Pointset_Powerset_NNC_Polyhedron_t*)
    malloc(sizeof(ppl_Pointset_Powerset_NNC_Polyhedron_t));
  ppl_new_Pointset_Powerset_NNC_Polyhedron_from_NNC_Polyhedron(pointsetPowerset, *((ppl_Polyhedron_t*)polyhedron));
  return pointsetPowerset;
}

void ppl_pointset_powerset_destroy(void* pointsetPowerset)
{
  ppl_delete_Pointset_Powerset_NNC_Polyhedron(*((ppl_Pointset_Powerset_NNC_Polyhedron_t*)pointsetPowerset));
}

static pcp_poly_spi* create_ppl_c_interface()
{
  struct pcp_poly_spi* pcp_poly_spi_impl = (struct pcp_poly_spi*)malloc(sizeof(pcp_poly_spi));

  pcp_poly_spi_impl->coefficientGetValue = &ppl_coefficient_to_int;
  pcp_poly_spi_impl->coefficientToString = &ppl_coefficient_to_string;
  pcp_poly_spi_impl->coefficientCreate = &ppl_coefficient_create;
  pcp_poly_spi_impl->coefficientDestroy = &ppl_coefficient_destroy;
  pcp_poly_spi_impl->linearExprCopy = NULL; // Probably not needed 
  pcp_poly_spi_impl->linearExprAddToCoefficient = ppl_linear_expr_add_to_coefficient;
  pcp_poly_spi_impl->linearExprAddToInhomogenous = ppl_linear_expr_add_to_inhomogenous;
  pcp_poly_spi_impl->linearExprSetCoefficient = ppl_linear_expr_set_coefficient;
  pcp_poly_spi_impl->linearExprGetNumDimensions = ppl_linear_expr_get_num_dimensions;
  pcp_poly_spi_impl->linearExprSubtract = ppl_linear_expr_subtract;
  pcp_poly_spi_impl->linearExprToString = ppl_linear_expr_to_string;
  pcp_poly_spi_impl->linearExprCreate = NULL; // Not needed
  pcp_poly_spi_impl->linearExprCreateDim = ppl_linear_expr_create_dim;
  pcp_poly_spi_impl->linearExprDestroy = ppl_linear_expr_destroy;
  pcp_poly_spi_impl->constraintCreateGreaterEqualZero = ppl_constraint_create_greater_equal_zero;
  pcp_poly_spi_impl->constraintCreateEqualZero = ppl_constraint_create_equal_zero;
  pcp_poly_spi_impl->constraintDestroy = ppl_constraint_destory;
  pcp_poly_spi_impl->constraintToString = ppl_constraint_to_string;
  pcp_poly_spi_impl->polyhedronGetConstraintSystem = ppl_polyhedron_get_constraint_system;
  pcp_poly_spi_impl->polyhedronAddConstraint = ppl_polyhedron_add_constraint;
  pcp_poly_spi_impl->polyhedronAddConstraints = ppl_polyhedron_add_constraints;
  pcp_poly_spi_impl->polyhedronRemap = ppl_polyhedron_remap;
  pcp_poly_spi_impl->polyhedronCopy = ppl_polyhedron_copy;
  pcp_poly_spi_impl->polyhedronCreate = ppl_polyhedron_create;
  pcp_poly_spi_impl->polyhedronDestroy = ppl_polyhedron_destroy;
  pcp_poly_spi_impl->pointsetPowersetCreate = ppl_pointset_powerset_create;
  pcp_poly_spi_impl->pointsetPowersetDestroy = ppl_pointset_powerset_destroy;
}

PcpPolySpi* createPPLPolySpi()
{
  return new PcpPolySpiC(create_ppl_c_interface());
}




