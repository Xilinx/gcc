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

#ifndef _PCP_POLY_
#define _PCP_POLY_

#include "pcp_dynamic_array.h"
#include "pcp_poly_spi.h"


// This class is the gateway to the SPI. Utilities using the primitives as well 
class PcpPoly
{
protected:
  PcpPolySpi* spi;

  virtual void setSpi(PcpPolySpi* spi);
  virtual PcpPolySpi* getSpi();

  static PcpPoly* theDefault;
public:
  static PcpPoly* getDefault();
  PcpPoly(PcpPolySpi* spi);

  // Coefficient primitives
  
  virtual int coefficientGetValue(PcpPolySpi::Coefficient* coeff);
  virtual PcpPolySpi::Coefficient* coefficientCreate(int value);
  virtual const char* coefficientToString(PcpPolySpi::Coefficient* coeff);
  virtual void coefficientDestroy(PcpPolySpi::Coefficient* coeff);

  // Linear Expr Primitives
  virtual PcpPolySpi::LinearExpr* linearExprCopy(PcpPolySpi::LinearExpr* linearExpr); 
  virtual void linearExprAddToCoefficient(PcpPolySpi::LinearExpr* linearExpr, 
					  int dimension, 
					  PcpPolySpi::Coefficient* coefficient);
  virtual void linearExprAddToCoefficient(PcpPolySpi::LinearExpr* linearExpr, int dimension, int value);
  virtual void linearExprAddToInhomogenous(PcpPolySpi::LinearExpr* linearExpr, int value);
  virtual void linearExprAddToInhomogenous(PcpPolySpi::LinearExpr* linearExpr, 
					   PcpPolySpi::Coefficient* value);
  virtual void linearExprSetCoefficient(PcpPolySpi::LinearExpr* linearExp, int dimension, int value);
  virtual void linearExprSetCoefficient(PcpPolySpi::LinearExpr* linearExp,
					int dimension,
					PcpPolySpi::Coefficient* coefficient);
  virtual void linearExprSubtract(PcpPolySpi::LinearExpr* linearExpr, PcpPolySpi::LinearExpr* subtract);
  virtual const char* linearExprToString(PcpPolySpi::LinearExpr* linearExpr);

  virtual PcpPolySpi::LinearExpr* linearExprCreate();
  virtual PcpPolySpi::LinearExpr* linearExprCreate(int numDimensions);
  virtual void linearExprDestroy(PcpPolySpi::LinearExpr* linearExpr);


  // Constraint primitives
  virtual PcpPolySpi::Constraint* constraintCreateEqualZero(PcpPolySpi::LinearExpr* linearExpr);
  virtual PcpPolySpi::Constraint* constraintCreateGreaterEqualZero(PcpPolySpi::LinearExpr* linearExpr);
  virtual void constraintDestroy(PcpPolySpi::Constraint* constraint);
  virtual const char* constraintToString(PcpPolySpi::Constraint* constraint);


  // Polyhedron primitives
  virtual PcpPolySpi::ConstraintSystem* polyhedronGetConstraintSystem(PcpPolySpi::Polyhedron* polyhedron);
  virtual void polyhedronAddConstraint(PcpPolySpi::Polyhedron* polyhedron,
				       PcpPolySpi::Constraint* constraint);
  virtual void polyhedronAddConstraints(PcpPolySpi::Polyhedron* polyhedron,
					PcpPolySpi::ConstraintSystem* constraintSystem);
  virtual void polyhedronRemap(PcpPolySpi::Polyhedron* polyhedron, PcpArray<int>* map);
  virtual PcpPolySpi::Polyhedron* polyhedronCopy(PcpPolySpi::Polyhedron* polyhedron);
  
  virtual PcpPolySpi::Polyhedron* polyhedronCreate(int numDimensions);
  virtual void polyhedronDestroy(PcpPolySpi::Polyhedron* polyhedron);

  // PointsetPowerset primitives
  virtual PcpPolySpi::PointsetPowerset* pointsetPowersetCreate(PcpPolySpi::Polyhedron* polyhedron);
  virtual void pointsetPowersetDestroy(PcpPolySpi::PointsetPowerset* poinsetPowerset);

};

class PcpPolyCoefficient
{
 protected:
  PcpPolySpi::Coefficient* coefficient;

  virtual void setCoefficient(PcpPolySpi::Coefficient* coefficient);
  virtual PcpPolySpi::Coefficient* getCoefficient();

  virtual void setValue(int value);
 public:
  virtual int getValue();

  PcpPolyCoefficient(int value);
  virtual ~PcpPolyCoefficient();
};

class PcpPolyLinearExpr
{
  friend class PcpPolyConstraint;

protected:
  PcpPolySpi::LinearExpr* linearExpr;

  virtual void setLinearExpr(PcpPolySpi::LinearExpr* linearExpr);
  virtual PcpPolySpi::LinearExpr* getLinearExpr();

 public:
  virtual PcpPolyLinearExpr* copy();
  virtual void addToCoefficient(int dimension, int value);
  virtual void addToInhomogenous(int value);
  virtual void setCoefficient(int dimension, int value);
  //  virtual int getNumDimensions(); // Needs to be stored probably.
  virtual void subtract(PcpPolyLinearExpr* expr);
  virtual const char* toString();

  PcpPolyLinearExpr();
  PcpPolyLinearExpr(int numDimensions);
  virtual ~PcpPolyLinearExpr();
};

class PcpPolyConstraintOperator 
{
protected:
  enum Opcode
  {
    CONSTRAINT_UNKNOWN,
    EQUAL_ZERO,
    GREATER_EQUAL_ZERO,
  };

  Opcode op;

  inline void setOpcode(Opcode op)
  {
    this->op = op;
  }

  inline Opcode getOpcode()
  {
    return this->op;
  }

  inline PcpPolyConstraintOperator(Opcode op)
  {
    setOpcode(op);
  }
 public:

  inline PcpPolyConstraintOperator()
  {
    setOpcode(CONSTRAINT_UNKNOWN);
  }

  static inline PcpPolyConstraintOperator unknown()
  {
    return PcpPolyConstraintOperator(CONSTRAINT_UNKNOWN);
  }

  static inline PcpPolyConstraintOperator equalZero()
  {
    return PcpPolyConstraintOperator(EQUAL_ZERO);
  }

  static inline PcpPolyConstraintOperator greaterEqualZero()
  {
    return PcpPolyConstraintOperator(GREATER_EQUAL_ZERO);
  }

  inline bool isUnknown()
  {
    return this->getOpcode() == CONSTRAINT_UNKNOWN;
  }

  inline bool isEqualZero()
  {
    return this->getOpcode() == EQUAL_ZERO;
  }

  inline bool isGreaterEqualZero()
  {
    return this->getOpcode() == GREATER_EQUAL_ZERO;
  }
};

class PcpPolyConstraint
{
  friend class PcpPolyPolyhedron;

 private:
  PcpPolyConstraintOperator oper;
  PcpPolySpi::Constraint* constraint;
  
  void setOperator(PcpPolyConstraintOperator oper);
  PcpPolyConstraintOperator getOperator();
  void setConstraint(PcpPolySpi::Constraint* constraint);
  PcpPolySpi::Constraint* getConstraint();

 public:
  const char* toString();
  PcpPolyConstraint(PcpPolyConstraintOperator oper,
		    PcpPolyLinearExpr* expr);
  virtual ~PcpPolyConstraint();
};

class PcpPolyConstraintSystem
{
  friend class PcpPolyPolyhedron;

protected:
  PcpPolySpi::ConstraintSystem* constraintSystem;
  
  void setConstraintSystem(PcpPolySpi::ConstraintSystem* constraintSystem);
  PcpPolySpi::ConstraintSystem* getConstraintSystem();
  PcpPolyConstraintSystem(PcpPolySpi::ConstraintSystem* constraintSystem);
};

class PcpPolyPolyhedron
{
  friend class PcpPolyPointsetPowerset;

private:
  PcpPolySpi::Polyhedron* polyhedron;

  void setPolyhedron(PcpPolySpi::Polyhedron* polyhedron);
  PcpPolySpi::Polyhedron* getPolyhedron();
  
public:
  virtual PcpPolyConstraintSystem* getConstraintSystem();
  virtual void addConstraint(PcpPolyConstraint* constraint);
  virtual void addConstraints(PcpPolyConstraintSystem* constraintSystem);
  virtual void remap(PcpArray<int>* map);

  PcpPolyPolyhedron(PcpPolyPolyhedron* polyhedron);
  PcpPolyPolyhedron(int numDimensions);
  virtual ~PcpPolyPolyhedron();
};

class PcpPolyPointsetPowerset
{
protected:
  PcpPolySpi::PointsetPowerset* pointsetPowerset;

  void setPointsetPowerset(PcpPolySpi::PointsetPowerset* pointsetPowerset);
  PcpPolySpi::PointsetPowerset* getPointsetPowerset();

public:
  PcpPolyPointsetPowerset(PcpPolyPolyhedron* polyhedron);

  ~PcpPolyPointsetPowerset();
};

#endif // _PCP_POLY_
