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

#ifndef _PCP_POLY_SPI_
#define _PCP_POLY_SPI_

#include "pcp_dynamic_array.h"

class PcpPolySpi
{
 public:
  class Coefficient
  {
  };

  class LinearExpr
  {
  };

  class Constraint
  {
  };

  class ConstraintSystem
  {
  };

  class Polyhedron
  {
  };

  class PointsetPowerset
  {
  };

  // Coefficient primitives
  virtual int coefficientGetValue(Coefficient* coeff) = 0;
  virtual const char* coefficientToString(Coefficient* coeff) = 0;

  virtual Coefficient* coefficientCreate(int value) = 0;
  virtual void coefficientDestroy(Coefficient* coeff) = 0;

  // Linear Expr Primitives
  virtual LinearExpr* linearExprCopy(LinearExpr* linearExpr) = 0;
  virtual void linearExprAddToCoefficient(LinearExpr* linearExpr, int dimension, Coefficient* value) = 0;
  virtual void linearExprAddToInhomogenous(LinearExpr* linearExpr, Coefficient* value) = 0;
  virtual void linearExprSetCoefficient(LinearExpr* linearExp, int dimension, Coefficient* value) = 0;
  virtual int linearExprGetNumDimensions(LinearExpr* linearExpr) = 0;
  virtual void linearExprSubtract(LinearExpr* linearExpr, LinearExpr* subtract) = 0;
  virtual const char* linearExprToString(LinearExpr* linearExpr) = 0;

  virtual LinearExpr* linearExprCreate() = 0;
  virtual LinearExpr* linearExprCreate(int numDimensions) = 0;
  virtual void linearExprDestroy(LinearExpr* linearExpr) = 0;

  // Constraint primitives
  virtual Constraint* constraintCreateEqualZero(LinearExpr* linearExpr) = 0;
  virtual Constraint* constraintCreateGreaterEqualZero(LinearExpr* linearExpr) = 0;
  virtual void constraintDestroy(Constraint* constraint) = 0;
  virtual const char* constraintToString(Constraint* constraint) = 0;

  // Polyhedron primitives
  virtual ConstraintSystem* polyhedronGetConstraintSystem(Polyhedron* polyhedron) = 0;
  virtual void polyhedronAddConstraint(Polyhedron* polyhedron, Constraint* constraint) = 0;
  virtual void polyhedronAddConstraints(Polyhedron* polyhedron, ConstraintSystem* constraintSystem) = 0;
  virtual void polyhedronRemap(Polyhedron* polyhedron, PcpArray<int>* map) = 0;
  virtual Polyhedron* polyhedronCopy(Polyhedron* polyhedron) = 0;

  virtual Polyhedron* polyhedronCreate(int numDimensions) = 0;
  virtual void polyhedronDestroy(Polyhedron* polyhedron) = 0;

  // PointsetPowerset primitives
  virtual PointsetPowerset* pointsetPowersetCreate(Polyhedron* polyhedron) = 0;
  virtual void pointsetPowersetDestroy(PointsetPowerset* poinsetPowerset) = 0;

  static PcpPolySpi* create();
};



#endif // _PCP_POLY_SPI_
