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

class PcpPolySpiDummy : public PcpPolySpi
{
public:
  class DummyCoefficient : public Coefficient
  {
  public:
    int value;
  };

  class DummyLinearExpr : public LinearExpr
  {
  public:
    PcpArray<int>* array;
  };

  class DummyConstraint : public Constraint
  {
  public:
    int kind;
    LinearExpr* linearExpr;
  };

  class DummyConstraintSystem : public ConstraintSystem
  {

  };

  class DummyPolyhedron : public Polyhedron
  {
    
  };

  class DummyPointsetPowerset : public PointsetPowerset
  {
  };

    // Coefficient primitives
int coefficientGetValue(Coefficient* coeff)
{
  return ((DummyCoefficient*)coeff)->value;
}

PcpPolySpi::Coefficient* coefficientCreate(int value) 
{
  DummyCoefficient* coeff = new DummyCoefficient();
  coeff->value = value;
  return coeff;
}

const char*
coefficientToString(Coefficient* coeff)
{
  PcpStringBuffer buffer;
  buffer.appendInt(((DummyCoefficient*)coeff)->value);
  return buffer.toString();
}

void coefficientDestroy(Coefficient* coeff)
{
  delete (DummyCoefficient*)coeff;
}

  // Linear Expr Primitives
PcpPolySpi::LinearExpr* linearExprCopy(LinearExpr* linearExpr)
{
  return NULL;
}

void linearExprAddToCoefficient(LinearExpr* linearExpr, int dimension, Coefficient* value)
{
  PcpArray<int>* array = ((DummyLinearExpr*)linearExpr)->array;
  array->set(dimension, array->get(dimension) + ((DummyCoefficient*)value)->value);
}

void linearExprAddToInhomogenous(LinearExpr* linearExpr, Coefficient* value)
{
  PcpArray<int>* array = ((DummyLinearExpr*)linearExpr)->array;
  int index = array->getSize()-1;
  linearExprAddToCoefficient(linearExpr, index, value);
}

void linearExprSetCoefficient(LinearExpr* linearExpr, int dimension, Coefficient* value)
{
  PcpArray<int>* array = ((DummyLinearExpr*)linearExpr)->array;
  array->set(dimension, ((DummyCoefficient*)value)->value);
}

int linearExprGetNumDimensions(LinearExpr* linearExpr)
{
  return ((DummyLinearExpr*)linearExpr)->array->getSize();
}

void linearExprSubtract(LinearExpr* linearExpr, LinearExpr* subtract)
{
  
}

const char*
linearExprToString(LinearExpr* linearExpr)
{
  PcpArray<int>* array = ((DummyLinearExpr*) linearExpr)->array;
  PcpStringBuffer buffer;
  for(int i = 0; i < array->getSize(); i++)
    {
      if(i > 0)
	buffer.append(" ");
      buffer.appendInt(array->get(i));
    }
  return buffer.toString();
}

PcpPolySpi::LinearExpr* linearExprCreate()
{
  return NULL;
}

PcpPolySpi::LinearExpr* linearExprCreate(int numDimensions)
{
  DummyLinearExpr* linearExpr = new DummyLinearExpr();
  linearExpr->array = new PcpArray<int>(numDimensions);
  return linearExpr;
}

void linearExprDestroy(LinearExpr* linearExpr)
{
  delete ((DummyLinearExpr*)linearExpr)->array;
}

  // Constraint primitives
PcpPolySpi::Constraint* constraintCreateEqualZero(LinearExpr* linearExpr)
{
  DummyConstraint* constraint = new DummyConstraint();
  constraint->kind = 0;
  constraint->linearExpr = linearExpr;
  return constraint;
}

PcpPolySpi::Constraint* constraintCreateGreaterEqualZero(LinearExpr* linearExpr)
{
  DummyConstraint* constraint = new DummyConstraint();
  constraint->kind = 1;
  constraint->linearExpr = linearExpr;
  return constraint;
}

void constraintDestroy(Constraint* constraint)
{
  delete ((DummyConstraint*)constraint)->linearExpr;
}

const char*
constraintToString(Constraint* constraint)
{
  DummyConstraint* dconstraint = (DummyConstraint*) constraint;
  PcpStringBuffer buffer;
  const char* linearExprString = linearExprToString(dconstraint->linearExpr);
  buffer.append(linearExprString);
  buffer.append(dconstraint->kind == 0 ? " == 0" : " >= 0");
  return buffer.toString();
}


  // Polyhedron primitives
PcpPolySpi::ConstraintSystem* polyhedronGetConstraintSystem(Polyhedron* polyhedron)
{
  return NULL;
}

void polyhedronAddConstraint(Polyhedron* polyhedron, Constraint* constraint)
{
}

void polyhedronAddConstraints(Polyhedron* polyhedron, ConstraintSystem* constraintSystem)
{
}

void polyhedronRemap(Polyhedron* polyhedron, PcpArray<int>* map)
{
}

PcpPolySpi::Polyhedron* polyhedronCopy(Polyhedron* polyhedron)
{
  return NULL;
}

PcpPolySpi::Polyhedron* polyhedronCreate(int numDimensions)
{
  return NULL;
}

void polyhedronDestroy(Polyhedron* polyhedron)
{
}

void pointsetPowersetDestroy(PointsetPowerset* poinsetPowerset)
{
}


PcpPolySpi::PointsetPowerset* pointsetPowersetCreate(Polyhedron* polyhedron)
{
    return NULL;
}


};

#include "pcp_poly_spi_c.h"
PcpPolySpi* PcpPolySpi::create()
{
  //return new PcpPolySpiDummy();
  return createPPLPolySpi();
}


