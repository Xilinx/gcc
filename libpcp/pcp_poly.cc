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

#include "pcp_poly.h"


// PcpPoly
PcpPoly* PcpPoly::theDefault = NULL;

PcpPoly* PcpPoly::getDefault()
{
  if(PcpPoly::theDefault == NULL) 
    {
      theDefault = new PcpPoly(PcpPolySpi::create());
    }
  return theDefault;
}

PcpPoly::PcpPoly(PcpPolySpi* spi)
{
  setSpi(spi);
}

void
PcpPoly::setSpi(PcpPolySpi* spi)
{
  this->spi = spi;}

PcpPolySpi*
PcpPoly::getSpi()
{
  return this->spi;
}

int
PcpPoly::coefficientGetValue(PcpPolySpi::Coefficient* coefficient)
{
  return getSpi()->coefficientGetValue(coefficient);
}

const char* 
PcpPoly::coefficientToString(PcpPolySpi::Coefficient* coefficient)
{
  return getSpi()->coefficientToString(coefficient);
}


PcpPolySpi::Coefficient*
PcpPoly::coefficientCreate(int value)
{
  return getSpi()->coefficientCreate(value);
}

void
PcpPoly::coefficientDestroy(PcpPolySpi::Coefficient* coefficient)
{
  getSpi()->coefficientDestroy(coefficient);
}

PcpPolySpi::LinearExpr*
PcpPoly::linearExprCopy(PcpPolySpi::LinearExpr* linearExpr)
{
  return getSpi()->linearExprCopy(linearExpr);
}

void
PcpPoly::linearExprAddToCoefficient(PcpPolySpi::LinearExpr* linearExpr, 
				    int dimension, 
				    PcpPolySpi::Coefficient* coefficient)
{
  getSpi()->linearExprAddToCoefficient(linearExpr, dimension, coefficient);
}

void
PcpPoly::linearExprAddToCoefficient(PcpPolySpi::LinearExpr* linearExpr, int dimension, int value)
{
  getSpi()->linearExprAddToCoefficient(linearExpr, dimension, getSpi()->coefficientCreate(value));
}

void
PcpPoly::linearExprAddToInhomogenous(PcpPolySpi::LinearExpr* linearExpr, PcpPolySpi::Coefficient* value)
{
  getSpi()->linearExprAddToInhomogenous(linearExpr, value);
}

void
PcpPoly::linearExprAddToInhomogenous(PcpPolySpi::LinearExpr* linearExpr, int value)
{
  getSpi()->linearExprAddToInhomogenous(linearExpr, getSpi()->coefficientCreate(value));
}

void
PcpPoly::linearExprSetCoefficient(PcpPolySpi::LinearExpr* linearExpr, int dimension,
				  PcpPolySpi::Coefficient* coefficient)
{
  return getSpi()->linearExprSetCoefficient(linearExpr, dimension, coefficient);
}

void
PcpPoly::linearExprSetCoefficient(PcpPolySpi::LinearExpr* linearExpr, int dimension, int value)
{
  return getSpi()->linearExprSetCoefficient(linearExpr, dimension, getSpi()->coefficientCreate(value));
}

void
PcpPoly::linearExprSubtract(PcpPolySpi::LinearExpr* linearExpr, PcpPolySpi::LinearExpr* subtract)
{
  getSpi()->linearExprSubtract(linearExpr, subtract);
}

const char*
PcpPoly::linearExprToString(PcpPolySpi::LinearExpr* linearExpr)
{
  return getSpi()->linearExprToString(linearExpr);
}


PcpPolySpi::LinearExpr*
PcpPoly::linearExprCreate()
{
  return getSpi()->linearExprCreate();
}

PcpPolySpi::LinearExpr*
PcpPoly::linearExprCreate(int numDimensions)
{
  return getSpi()->linearExprCreate(numDimensions);
}

void
PcpPoly::linearExprDestroy(PcpPolySpi::LinearExpr* linearExpr)
{
  getSpi()->linearExprDestroy(linearExpr);
}


PcpPolySpi::Constraint* 
PcpPoly::constraintCreateEqualZero(PcpPolySpi::LinearExpr* linearExpr)
{
  return getSpi()->constraintCreateEqualZero(linearExpr);
}

PcpPolySpi::Constraint*
PcpPoly::constraintCreateGreaterEqualZero(PcpPolySpi::LinearExpr* linearExpr)
{
  return getSpi()->constraintCreateGreaterEqualZero(linearExpr);
}

void
PcpPoly::constraintDestroy(PcpPolySpi::Constraint* constraint)
{
  getSpi()->constraintDestroy(constraint);
}

const char*
PcpPoly::constraintToString(PcpPolySpi::Constraint* constraint)
{
  getSpi()->constraintToString(constraint);
}

PcpPolySpi::ConstraintSystem*
PcpPoly::polyhedronGetConstraintSystem(PcpPolySpi::Polyhedron* polyhedron)
{
  return getSpi()->polyhedronGetConstraintSystem(polyhedron);
}

void
PcpPoly::polyhedronAddConstraint(PcpPolySpi::Polyhedron* polyhedron,
				 PcpPolySpi::Constraint* constraint)
{
  getSpi()->polyhedronAddConstraint(polyhedron, constraint);
}

void
PcpPoly::polyhedronAddConstraints(PcpPolySpi::Polyhedron* polyhedron,
				  PcpPolySpi::ConstraintSystem* constraintSystem)
{
  getSpi()->polyhedronAddConstraints(polyhedron, constraintSystem);
}

void
PcpPoly::polyhedronRemap(PcpPolySpi::Polyhedron* polyhedron, PcpArray<int>* map)
{
  getSpi()->polyhedronRemap(polyhedron, map);
}

PcpPolySpi::Polyhedron*
PcpPoly::polyhedronCopy(PcpPolySpi::Polyhedron* polyhedron)
{
  getSpi()->polyhedronCopy(polyhedron);
}

PcpPolySpi::Polyhedron*
PcpPoly::polyhedronCreate(int numDimensions)
{
  return getSpi()->polyhedronCreate(numDimensions);
}

void
PcpPoly::polyhedronDestroy(PcpPolySpi::Polyhedron* polyhedron)
{
  getSpi()->polyhedronDestroy(polyhedron);
}

PcpPolySpi::PointsetPowerset*
PcpPoly::pointsetPowersetCreate(PcpPolySpi::Polyhedron* polyhedron)
{
  return getSpi()->pointsetPowersetCreate(polyhedron);
}

void
PcpPoly::pointsetPowersetDestroy(PcpPolySpi::PointsetPowerset* poinsetPowerset)
{
  return pointsetPowersetDestroy(poinsetPowerset);
}

///////////////////////////////////////////////////////////////////////////////
// PcpPolyCoefficient

void
PcpPolyCoefficient::setCoefficient(PcpPolySpi::Coefficient* coefficient)
{
  this->coefficient = coefficient;
}

PcpPolySpi::Coefficient*
PcpPolyCoefficient::getCoefficient()
{
  return this->coefficient;
}

void
PcpPolyCoefficient::setValue(int value)
{
  setCoefficient(PcpPoly::getDefault()->coefficientCreate(value));
}

int
PcpPolyCoefficient::getValue()
{
  return PcpPoly::getDefault()->coefficientGetValue(this->getCoefficient());
}

PcpPolyCoefficient::PcpPolyCoefficient(int value)
{
  setValue(value);
}

PcpPolyCoefficient::~PcpPolyCoefficient()
{
  PcpPoly::getDefault()->coefficientDestroy(this->getCoefficient());
}

// PcpPolyLinearExpr
void
PcpPolyLinearExpr::setLinearExpr(PcpPolySpi::LinearExpr* linearExpr)
{
  this->linearExpr = linearExpr;
}

PcpPolySpi::LinearExpr* 
PcpPolyLinearExpr::getLinearExpr()
{
  return this->linearExpr;
}


PcpPolyLinearExpr*
PcpPolyLinearExpr::copy()
{
  PcpPoly::getDefault()->linearExprCopy(this->getLinearExpr());
}

void
PcpPolyLinearExpr::addToCoefficient(int dimension, int value)
{
  PcpPoly::getDefault()->linearExprAddToCoefficient(this->getLinearExpr(), dimension, value);
}

void
PcpPolyLinearExpr::addToInhomogenous(int value)
{
  PcpPoly::getDefault()->linearExprAddToInhomogenous(this->getLinearExpr(), value);
}

void
PcpPolyLinearExpr::setCoefficient(int dimension, int value)
{
  PcpPoly::getDefault()->linearExprSetCoefficient(this->getLinearExpr(), dimension, value);
}

void
PcpPolyLinearExpr::subtract(PcpPolyLinearExpr* expr)
{
  PcpPoly::getDefault()->linearExprSubtract(this->getLinearExpr(), expr->getLinearExpr());
}

const char* PcpPolyLinearExpr::toString()
{
  return PcpPoly::getDefault()->linearExprToString(this->getLinearExpr());
}


PcpPolyLinearExpr::PcpPolyLinearExpr()
{
  this->setLinearExpr(PcpPoly::getDefault()->linearExprCreate());
}

PcpPolyLinearExpr::PcpPolyLinearExpr(int numDimensions)
{
  this->setLinearExpr(PcpPoly::getDefault()->linearExprCreate(numDimensions));
}

PcpPolyLinearExpr::~PcpPolyLinearExpr()
{
  PcpPoly::getDefault()->linearExprDestroy(this->getLinearExpr());
}

///////////////////////////////////////////////////////////////////////////////
// PcpPolyConstraint

void
PcpPolyConstraint::setOperator(PcpPolyConstraintOperator oper)
{
  this->oper = oper;
}

PcpPolyConstraintOperator
PcpPolyConstraint::getOperator()
{
  return this->oper;
}

void
PcpPolyConstraint::setConstraint(PcpPolySpi::Constraint* constraint)
{
  this->constraint = constraint;
}

PcpPolySpi::Constraint*
PcpPolyConstraint::getConstraint()
{
  return this->constraint;
}

const char*
PcpPolyConstraint::toString()
{
  return PcpPoly::getDefault()->constraintToString(this->getConstraint());
}


PcpPolyConstraint::PcpPolyConstraint(PcpPolyConstraintOperator oper,
				     PcpPolyLinearExpr* expr)
{
  this->setOperator(oper);
  PcpPolySpi::LinearExpr* spiExpr = expr->getLinearExpr();
  this->setConstraint(oper.isEqualZero() 
                      ? PcpPoly::getDefault()->constraintCreateEqualZero(spiExpr)
                      : PcpPoly::getDefault()->constraintCreateGreaterEqualZero(spiExpr));
}
 
PcpPolyConstraint::~PcpPolyConstraint()
{
  PcpPoly::getDefault()->constraintDestroy(this->getConstraint());
}

///////////////////////////////////////////////////////////////////////////////
// PcpPolyConstraintSystem

void
PcpPolyConstraintSystem::setConstraintSystem(PcpPolySpi::ConstraintSystem* constraintSystem)
{
  this->constraintSystem = constraintSystem;
}

PcpPolySpi::ConstraintSystem*
PcpPolyConstraintSystem::getConstraintSystem()
{
  return this->constraintSystem;
}

PcpPolyConstraintSystem::PcpPolyConstraintSystem(PcpPolySpi::ConstraintSystem* constraintSystem)
{
  this->setConstraintSystem(constraintSystem);
}

///////////////////////////////////////////////////////////////////////////////
// PcpPolyPolyhedron

void
PcpPolyPolyhedron::setPolyhedron(PcpPolySpi::Polyhedron* polyhedron)
{
  this->polyhedron = polyhedron;
}

PcpPolySpi::Polyhedron*
PcpPolyPolyhedron::getPolyhedron()
{
  return this->polyhedron;
}

PcpPolyConstraintSystem*
PcpPolyPolyhedron::getConstraintSystem()
{
  PcpPolySpi::Polyhedron* polyhedron = this->getPolyhedron();
  return new PcpPolyConstraintSystem(PcpPoly::getDefault()->polyhedronGetConstraintSystem(polyhedron));
}

void
PcpPolyPolyhedron::addConstraint(PcpPolyConstraint* constraint)
{
  PcpPoly::getDefault()->polyhedronAddConstraint(this->getPolyhedron(), constraint->getConstraint());
}

void
PcpPolyPolyhedron::addConstraints(PcpPolyConstraintSystem* constraintSystem)
{
  PcpPoly::getDefault()->polyhedronAddConstraints(this->getPolyhedron(),
                                                  constraintSystem->getConstraintSystem());
}

void
PcpPolyPolyhedron::remap(PcpArray<int>* map)
{
  PcpPoly::getDefault()->polyhedronRemap(this->getPolyhedron(), map);
}

PcpPolyPolyhedron::PcpPolyPolyhedron(PcpPolyPolyhedron* polyhedron)
{
  this->setPolyhedron(PcpPoly::getDefault()->polyhedronCopy(polyhedron->getPolyhedron()));
}

PcpPolyPolyhedron::PcpPolyPolyhedron(int numDimensions)
{
  this->setPolyhedron(PcpPoly::getDefault()->polyhedronCreate(numDimensions));
}

PcpPolyPolyhedron::~PcpPolyPolyhedron()
{
  PcpPoly::getDefault()->polyhedronDestroy(this->getPolyhedron());
}

///////////////////////////////////////////////////////////////////////////////
void PcpPolyPointsetPowerset::setPointsetPowerset(PcpPolySpi::PointsetPowerset* pointsetPowerset)
{
  this->pointsetPowerset = pointsetPowerset;
}

PcpPolySpi::PointsetPowerset*
PcpPolyPointsetPowerset::getPointsetPowerset()
{
  return this->pointsetPowerset;
}

PcpPolyPointsetPowerset::PcpPolyPointsetPowerset(PcpPolyPolyhedron* polyhedron)
{
  this->setPointsetPowerset(PcpPoly::getDefault()->pointsetPowersetCreate(polyhedron->getPolyhedron()));
}

PcpPolyPointsetPowerset::~PcpPolyPointsetPowerset()
{
  PcpPoly::getDefault()->pointsetPowersetDestroy(this->getPointsetPowerset());
}

