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

#include "pcp_domain.h"
#include "pcp_emitter.h"
#include "pcp_expr_canonicalizer.h"

void PcpDomainMap::Visitor::setDomainMap(PcpDomainMap* domainMap)
{
  this->domainMap = domainMap;
}

PcpDomainMap*
PcpDomainMap::Visitor::getDomainMap()
{
  return this->domainMap;
}

void
PcpDomainMap::Visitor::setScalarContextStack(PcpScalarContextStack* scalarContextStack)
{
  this->scalarContextStack = scalarContextStack;
}


PcpScalarContextStack*
PcpDomainMap::Visitor::getScalarContextStack()
{
  return this->scalarContextStack;
}


void
PcpDomainMap::Visitor::setDomainExprStack(PcpStack<PcpBoolExpr*>* domainExprStack)
{
  this->domainExprStack = domainExprStack;
}

PcpStack<PcpBoolExpr*>*
PcpDomainMap::Visitor::getDomainExprStack()
{
  return this->domainExprStack;
}

void
PcpDomainMap::Visitor::debugDomainExprStack()
{
  PcpStack<PcpBoolExpr*>* domainExprStack = this->getDomainExprStack();
  int numExprs = domainExprStack->getSize();
  int i;
  //  printf("DomainStack:\n");
  //  for(i = 0; i < numExprs; i++)
  //    {
  //      PcpBoolExpr* expr = domainExprStack->peek(i);
  //      printf("%i: %s\n", i, PcpEmitter::pcpBoolExprToString(expr));
  //    }
}

PcpExprCanonicalizer*
PcpDomainMap::Visitor::getExprCanonicalizer()
{
  return getDomainMap()->getExprCanonicalizer();
}

PcpBoolArith*
PcpDomainMap::Visitor::createCanonicalizedDomainExpr()
{
  PcpStack<PcpBoolExpr*>* domainExprStack = this->getDomainExprStack();
  int numExprs = domainExprStack->getSize();
  int i;
  PcpBoolArithBuilder* builder = new PcpBoolArithBuilder();
  builder->setOperator(PcpBoolArithOperator::boolAnd());

  for(i = 0; i < numExprs; i++)
    {
      PcpBoolExpr* expr = domainExprStack->peek(i);
      builder->addOperand(expr);
    }
  PcpBoolArith* domainExpr = builder->createBoolArith();
  //printf("Original domain expr: %s\n", PcpEmitter::pcpBoolExprToString(domainExpr));
  PcpBoolArith* result = getExprCanonicalizer()->canonicalizeBoolExpr(domainExpr);
  delete builder;
  return result;
}

PcpPolyLinearExpr*
PcpDomainMap::Visitor::translateLinearExpr(PcpArith* linearExpr)
{
  pcpAssert(linearExpr->getOperator().isAdd());
  int numArgs = linearExpr->getNumOperands();
  PcpPolyLinearExpr* result = new PcpPolyLinearExpr(numArgs-1); //Constant part is inhomogeneous
  for(int i = 0; i < numArgs; i++)
    {
      PcpExpr* operand = linearExpr->getOperand(i);
      PcpConstant* coefficient = NULL;
      if(operand->isArith())
	{
	  PcpArith* arithOperand = operand->toArith();
	  pcpAssert(arithOperand->getOperator().isMultiply());
	  pcpAssert(arithOperand->getNumOperands() == 2);
	  PcpExpr* lhs = arithOperand->getOperand(0);
	  pcpAssert(lhs->isConstant());
	  coefficient = lhs->toConstant();
	  pcpAssert(coefficient != NULL);
	  result->setCoefficient(i, coefficient->getValue());
	}
      else if(operand->isConstant())
	{
	  coefficient = operand->toConstant();
	  result->addToInhomogenous(coefficient->getValue());
	}
      
    }
  return result;
}

PcpPolyConstraint*
PcpDomainMap::Visitor::translateCompare(PcpCompare* compare)
{
  PcpExpr* lhs = compare->getLhs();
  pcpAssert(lhs->isArith());
  PcpPolyLinearExpr* linearExpr = this->translateLinearExpr(lhs->toArith());
  //  printf("OriginalExpr: %s\n", PcpEmitter::pcpExprToString(lhs));
  //  printf("Created linear expr: %s\n", linearExpr->toString());
  
  PcpPolyConstraint* constraint = new PcpPolyConstraint(compare->getOperator().isEqual() 
							? PcpPolyConstraintOperator::equalZero()
							: PcpPolyConstraintOperator::greaterEqualZero(),
							linearExpr);
  //printf("Original compare: %s\n", PcpEmitter::pcpBoolExprToString(compare));
  printf("%s\n", constraint->toString());
  return NULL;
}

PcpPolyPolyhedron*
PcpDomainMap::Visitor::translateConjunction(PcpBoolArith* conjunction)
{
  printf("Polyhedron begin:\n");
  for(int i = 0; i < conjunction->getNumOperands(); i++)
    {
      PcpBoolExpr* operand = conjunction->getOperand(i);
      pcpAssert(operand->isCompare());
      translateCompare(operand->toCompare());
    }
  return NULL;
}

PcpPolyPointsetPowerset*
PcpDomainMap::Visitor::translateCondition(PcpBoolExpr* condition)
{
  if(condition->isBoolArith())
    {
      PcpBoolArith* disjunction = condition->toBoolArith();
      if(disjunction->getOperator().isBoolOr())
	{
	  for(int i = 0; i < disjunction->getNumOperands(); i++)
	    {
	      PcpBoolExpr* operand = disjunction->getOperand(i);
	      pcpAssert(operand->isBoolArith());
	      translateConjunction(operand->toBoolArith());
	    }
	}
      else
	translateConjunction(disjunction);
    }
  else if(condition->isCompare())
    translateCompare(condition->toCompare());
  else
    PcpError::reportErrorNewline("Cannot make polyhedron for expr");
  return NULL;
}

void
PcpDomainMap::Visitor::visit(PcpScop* scop)
{
  this->setScalarContextStack(new PcpScalarContextStack(scop));
  scop->getBody()->accept(this);
}

void
PcpDomainMap::Visitor::visit(PcpLoop* loop)
{
  this->getDomainExprStack()->push(new PcpCompare(PcpCompareOperator::greaterEqual(), 
						  loop->getIv(), 
						  loop->getStart()));
  this->getDomainExprStack()->push(loop->getCondition());
  this->getScalarContextStack()->push(loop->getIv());
  PcpExpr* oldBase = this->getExprCanonicalizer()->getCanonicalizationBase();
  PcpExpr* newBase = this->getScalarContextStack()->createBaseExpr();
  this->getExprCanonicalizer()->setCanonicalizationBase(newBase);
  this->debugDomainExprStack();
  loop->getBody()->accept(this);
  this->getDomainExprStack()->pop();
  this->getDomainExprStack()->pop();
  this->getScalarContextStack()->pop();
  this->getExprCanonicalizer()->setCanonicalizationBase(oldBase);
}

void
PcpDomainMap::Visitor::visit(PcpSequence* sequence)
{
  visitChildren(sequence);
}

void
PcpDomainMap::Visitor::visit(PcpGuard* guard)
{
  // Push condition on stack
  //  this->getDomainExprStack()->push(NULL);
  visitChildren(guard);
  //this->getDomainExprStack()->pop(NULL);
}

void
PcpDomainMap::Visitor::visit(PcpCopy* copy)
{
  PcpBoolArith* domainExpr = this->createCanonicalizedDomainExpr();
  printf("Stmt: %s\n", PcpEmitter::pcpStmtToString(copy));
  this->translateCondition(domainExpr);
  
  // Create domain here
}

void
PcpDomainMap::Visitor::visit(PcpUserStmt* userStmt)
{
  PcpBoolArith* domainExpr = this->createCanonicalizedDomainExpr();
  //  printf("DomainExpr: %s\n", PcpEmitter::pcpBoolExprToString(domainExpr));
  printf("Stmt: %s\n", PcpEmitter::pcpStmtToString(userStmt));
  this->translateCondition(domainExpr);
}

PcpDomainMap::Visitor::Visitor(PcpDomainMap* domainMap)
{
  setDomainMap(domainMap);
  setDomainExprStack(new PcpListStack<PcpBoolExpr*>());
}


void
PcpDomainMap::setExprCanonicalizer(PcpExprCanonicalizer* exprCanonicalizer)
{
  this->exprCanonicalizer = exprCanonicalizer;
}

PcpExprCanonicalizer*
PcpDomainMap::getExprCanonicalizer()
{
  return this->exprCanonicalizer;
}

void
PcpDomainMap::buildDomainMap(PcpScop* scop) 
{
  Visitor visitor(this);
  visitor.visit(scop);
}

PcpDomainMap::PcpDomainMap(PcpScop* scop,
			   PcpExprCanonicalizer* canonicalizer) : PcpTreeMap<PcpStmt*, PcpDomain*>() 
{
  setExprCanonicalizer(canonicalizer);
  buildDomainMap(scop);
}
