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

#include "pcp_error.h"
#include "pcp_stack.h"
#include "pcp_expr_canonicalizer.h"
#include "pcp_emitter.h"

// Creates an expression:
// +(*(0, iv_0), ... *(0, iv_n), *(0, param_0), ..., *(0, param_m), 0)
PcpExpr*
PcpScalarContextStack::createBaseExpr()
{
  int size = this->getSize();
  PcpArithBuilder* builder = new PcpArithBuilder();
  builder->setOperator(PcpArithOperator::add());
  for(int i = 0; i < size; i++)
    {
      PcpExpr* scalar = this->peek(i);
      PcpArith* mulZeroScalar = PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
							       new PcpConstant(0),
							       scalar);
      builder->addOperand(mulZeroScalar);
    }
  builder->addOperand(new PcpConstant(0));
  PcpArith* zeros = builder->createArith();
  delete builder;
  //  printf("Zero expr: %s\n", PcpEmitter::pcpExprToString(zeros));
  return zeros;
}

PcpScalarContextStack::PcpScalarContextStack(PcpScop* scop) : PcpListStack<PcpExpr*>()
{
  PcpIterator<PcpParameter*>* iter = scop->getParametersIterator();
  for(;iter->hasNext(); iter->next())
    {
      this->push(iter->get());
    }
}


void
PcpExprCanonicalizer::setScalarOrder(PcpScalarOrder* scalarOrder)
{
  this->scalarOrder = scalarOrder;
}

PcpScalarOrder*
PcpExprCanonicalizer::getScalarOrder()
{
  return this->scalarOrder;
}

void
PcpExprCanonicalizer::setScalarContextStack(PcpScalarContextStack* scalarContextStack)
{
  this->scalarContextStack = scalarContextStack;
}

PcpScalarContextStack*
PcpExprCanonicalizer::getScalarContextStack()
{
  return this->scalarContextStack;
}


PcpExpr*
PcpExprCanonicalizer::stripMultiplier(PcpExpr* expr)
{
  pcpAssert(!expr->isArith() || expr->toArith()->getOperator().isMultiply());
  return expr->isArith() ? expr->toArith()->getOperand(1) : expr;
}

bool
PcpExprCanonicalizer::equal(PcpExpr* expr1, PcpExpr* expr2)
{
  return this->getScalarOrder()->equal(this->stripMultiplier(expr1),
				       this->stripMultiplier(expr2));
}

bool
PcpExprCanonicalizer::less(PcpExpr* expr1, PcpExpr* expr2)
{
  return this->getScalarOrder()->less(this->stripMultiplier(expr1),
				      this->stripMultiplier(expr2));
}

PcpConstant*
PcpExprCanonicalizer::negateConstant(PcpConstant* constant)
{
  return new PcpConstant(-(constant->getValue()));
}

PcpConstant*
PcpExprCanonicalizer::addConstants(PcpConstant* constant1,
			      PcpConstant* constant2)
{
  return new PcpConstant(constant1->getValue() +
			 constant2->getValue());
}

PcpConstant*
PcpExprCanonicalizer::addConstants(PcpConstant* constant1,
			      int constant2)
{
  return new PcpConstant(constant1->getValue() +
			 constant2);
}

PcpConstant*
PcpExprCanonicalizer::addConstants(int constant1,
			      int constant2)
{
  return new PcpConstant(constant1 + constant2);
}

PcpExpr*
PcpExprCanonicalizer::addTerms(PcpExpr* expr1, PcpExpr* expr2)
{
  PcpExpr* result;
  if(expr1->isConstant())
    {
      result = this->addConstants(expr1->toConstant(), expr2->toConstant());
    }
  else if(expr1->isArith() && !expr2->isArith())
    result = addTerms(expr2, expr1);
//   else if(!expr1->isArith())
//     {
//       pcpAssert(false); // This case is no longer used since we always create *(1,x)
//       PcpExpr* multiplier = NULL;
//       if(!expr2->isArith())
// 	{
// 	  multiplier = this->addConstants(1,1);
// 	}
//       else 
// 	multiplier = this->addConstants(expr2->toArith()->getOperand(0)->toConstant(), 1);
//       result = PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
// 					      multiplier,
// 					      expr1);
//     }
  else // Iv is multiply
    {
      PcpExpr* multiplier = 
	this->addConstants(expr1->toArith()->getOperand(0)->toConstant(),
			       expr2->toArith()->getOperand(0)->toConstant());
      result = PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
				    multiplier,
				    expr1->toArith()->getOperand(1));
    }

  return result;
}

// Merges normalized arithmetic expressions, where scalars are also
// wrapped with a + operator, e.g. +(42). 
PcpArith*
PcpExprCanonicalizer::addSums(PcpArith* sum1, PcpArith* sum2)
{
  pcpAssert(sum1 != NULL && sum2 != NULL);
  pcpAssert(sum1->getOperator().isAdd());
  pcpAssert(sum2->getOperator().isAdd());

  //  printf("      Sum1: %s\n", PcpEmitter::pcpArithToString(sum1));
  //  printf("      Sum2: %s\n", PcpEmitter::pcpArithToString(sum2));

  PcpArithBuilder* builder = new PcpArithBuilder();
  builder->setOperator(PcpArithOperator::add());

  PcpIterator<PcpExpr*>* iter1 = sum1->getOperandsIterator();
  PcpIterator<PcpExpr*>* iter2 = sum2->getOperandsIterator();
  
  while(iter1->hasNext() || iter2->hasNext())
    {
      PcpExpr* newOperand = NULL;
      if(!iter1->hasNext())
	{
	  newOperand = iter2->get();
	  iter2->next();
	}
      else if(!iter2->hasNext())
	{
	  newOperand = iter1->get();
	  iter1->next();
	}
      else 
	{
	  PcpExpr* expr1 = iter1->get();
	  PcpExpr* expr2 = iter2->get();
	  //	  printf("Expr1: %s\n", PcpEmitter::pcpExprToString(expr1));
	  // printf("Expr2: %s\n", PcpEmitter::pcpExprToString(expr2));
	  if(this->equal(expr1, expr2))
	    {
	      newOperand = this->addTerms(expr1, expr2);
	      iter1->next();
	      iter2->next();
	      //	      printf("Equal: add them\n");
	    }
	  else if(this->less(expr1, expr2))
	    {
	      newOperand = iter1->get();
	      iter1->next();
	      //	      printf("Less: Push expr 1\n");
	    }
	  else
	    {
	      newOperand = iter2->get();
	      iter2->next();
	      //	      printf("Greater: Push expr 2\n");
	    }
	}

      // Add 0 to make expr complete (commented out expression)
      if(newOperand != NULL) // && 
	//(!newOperand->isConstant() ||
	//newOperand->toConstant()->getValue() != 0))
	builder->addOperand(newOperand);
    }
  
  PcpArith* result = builder->createArith();
  delete builder;
  delete iter1;
  delete iter2;

  //  printf("Sum Result: %s\n", PcpEmitter::pcpArithToString(result));
  return result;
}

PcpArith*
PcpExprCanonicalizer::createUnaryArith(PcpArithOperator oper, PcpExpr* expr)
{
  PcpArithBuilder* builder = new PcpArithBuilder();

  builder->setOperator(oper);
  builder->addOperand(expr);

  PcpArith* result = builder->createArith();
  delete builder;
  return result;
}

PcpArith*
PcpExprCanonicalizer::createUnaryPlus(PcpExpr* expr)
{
  return createUnaryArith(PcpArithOperator::add(), expr);
}

PcpArith*
PcpExprCanonicalizer::createUnaryMinus(PcpExpr* expr)
{
  return createUnaryArith(PcpArithOperator::subtract(), expr);
}

PcpConstant*
PcpExprCanonicalizer::getCanonicalConstant(PcpArith* expr)
{
  return (expr->getOperator().isAdd() && expr->getNumOperands() == 1 &&
	  expr->getOperand(0)->isConstant())
    ? expr->getOperand(0)->toConstant()
    : NULL;
}

PcpConstant*
PcpExprCanonicalizer::multiplyConstants(PcpConstant* constant1, PcpConstant* constant2)
{
  return new PcpConstant(constant1->getValue() * constant2->getValue());
}

PcpArith*
PcpExprCanonicalizer::multiplyConstMul(PcpConstant* multiplier, PcpArith* multiply)
{
  pcpAssert(multiply->getOperator().isMultiply());
  PcpConstant* constant = multiply->getOperand(0)->toConstant();
  PcpConstant* newConstant = this->multiplyConstants(multiplier, constant);
  return PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
					  newConstant,
					  multiply->getOperand(1));
}

PcpArith*
PcpExprCanonicalizer::multiplyScalar(PcpConstant* constant, PcpExpr* scalar)
{
  return PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
					constant,
					scalar);
}

PcpArith*
PcpExprCanonicalizer::multiplyFactors(PcpConstant* multiplier, PcpArith* sum)
{

  // Don't simplify anything since we want the full expressions
  /*
  if(multiplier->getValue() == 0)
    return this->createUnaryPlus(multiplier);
  if(multiplier->getValue() == 1)
    return sum;
  */

  PcpIterator<PcpExpr*>* iter = sum->getOperandsIterator();
  PcpArithBuilder* builder = new PcpArithBuilder();

  builder->setOperator(PcpArithOperator::add());

  for(; iter->hasNext(); iter->next())
    {
      PcpExpr* term = iter->get();
      if(term->isArith())
	{
	  PcpArith* newTerm = this->multiplyConstMul(multiplier, term->toArith());
	  builder->addOperand(newTerm);
	}
      else if(term->isConstant())
	{
	  PcpConstant* newConstant = this->multiplyConstants(multiplier, term->toConstant());
	  builder->addOperand(newConstant);
	}
      else
	{
	  PcpArith* newTerm = this->multiplyScalar(multiplier, term);
	  builder->addOperand(newTerm);
	}
    }

  PcpArith* result = builder->createArith();
  delete builder;
  return result;
}

bool
PcpExprCanonicalizer::sameScalarBase(PcpArith* factor1, PcpArith* factor2)
{
  return (factor1->getNumOperands() == 2 &&
	  factor2->getNumOperands() == 2 &&
	  factor1->getOperator().isMultiply() &&
	  factor2->getOperator().isMultiply() &&
	  factor1->getOperand(0)->isConstant() &&
	  factor2->getOperand(0)->isConstant() &&
	  factor1->getOperand(1) == factor2->getOperand(1));
}

PcpArith*
PcpExprCanonicalizer::multiplyFactors(PcpArith* factor1, PcpArith* factor2)
{
  PcpConstant* constant1 = this->getCanonicalConstant(factor1);
  PcpConstant* constant2 = this->getCanonicalConstant(factor2);

  pcpAssert(constant1 != NULL || constant2 != NULL 
	    || sameScalarBase(factor1, factor2));

  bool nonConstants = constant1 == NULL && constant2 == NULL;
  bool factor1isConst = constant1 != NULL;
  bool factor2isConst = constant2 != NULL;

  PcpConstant* multiplier = 
        nonConstants ? factor1->getOperand(0)->toConstant() 
    : factor1isConst ? constant1
    : constant2;
  
  PcpArith* sum = !factor2isConst ? factor2 : factor1;

  return this->multiplyFactors(multiplier, sum);
}

PcpArith*
PcpExprCanonicalizer::negateExpr(PcpArith* canonicalizedExpr)
{
  PcpIterator<PcpExpr*>* iter = canonicalizedExpr->getOperandsIterator();
  PcpArithBuilder* builder = new PcpArithBuilder();
  builder->setOperator(PcpArithOperator::add());
  for(;iter->hasNext(); iter->next())
    {
      PcpExpr* operand = iter->get();
      if(operand->isArith())
	{
	  PcpArith* arith = operand->toArith();
	  PcpArithOperator oper = arith->getOperator();
	  pcpAssert(oper.isMultiply());
	  builder->addOperand(PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
							    this->negateConstant(arith->getOperand(0)->toConstant()),
							    arith->getOperand(1)));
	}
      else if(operand->isConstant())
	builder->addOperand(this->negateConstant(operand->toConstant()));
      else 
	builder->addOperand(PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(), 
							   new PcpConstant(-1), 
							   operand));
      
    }
  PcpArith* result = builder->createArith();
  delete builder;
  delete iter;
  return result;
}

PcpArith* 
PcpExprCanonicalizer::canonicalizeExpr(PcpExpr* expr)
{
  if(!expr->isArith())
    return createUnaryPlus(expr->isConstant() 
			   ? expr 
			   : PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
							    new PcpConstant(1),
							    expr));
  PcpArith* arith = expr->toArith();
  PcpArithOperator oper = arith->getOperator();
  
  if(arith->getNumOperands() == 1)
    return oper.isSubtract() ? negateExpr(canonicalizeExpr(arith->getOperand(0)))
                             : arith;
  if(oper.isSubtract() && arith->getNumOperands() == 2)
    {
      PcpArith* lhs = this->negateExpr(this->canonicalizeExpr(arith->getOperand(1)));
      return addSums(this->canonicalizeExpr(arith->getOperand(0)), lhs);
    }
  else
    {
      PcpIterator<PcpExpr*>* iter = arith->getOperandsIterator();
      PcpArith* result = this->canonicalizeExpr(iter->get());

      iter->next();
      for(;iter->hasNext();iter->next())
	{
	  PcpExpr* operand = iter->get();
	  PcpArith* canonicalizedOperand = this->canonicalizeExpr(operand);
	  if(oper.isAdd())
	    result = this->addSums(canonicalizedOperand, result);
	  else if(oper.isMultiply())
	    result = this->multiplyFactors(canonicalizedOperand, result);
	  else
	    pcpAssert(false);
	}
      return result;
    }
}

PcpArith*
PcpExprCanonicalizer::canonicalizeExprWithBase(PcpExpr* expr)
{
  PcpExpr* base = this->getCanonicalizationBase();
  if(base) 
    expr = PcpArith::pcpArithBinaryCreate(PcpArithOperator::add(), base, expr);
  return canonicalizeExpr(expr);
}

PcpCompare*
PcpExprCanonicalizer::canonicalizeCompare(PcpCompare* compare)
{
  PcpCompareOperator oper = compare->getOperator();
  PcpExpr* lhs = compare->getLhs();
  PcpExpr* rhs = compare->getRhs();

  //  printf("Lhs: %s\n", PcpEmitter::pcpExprToString(lhs));
  // printf("Rhs: %s\n", PcpEmitter::pcpExprToString(rhs));

  PcpArith* canonLhs = canonicalizeExprWithBase(lhs);
  PcpArith* canonRhs = canonicalizeExprWithBase(rhs);

  //  printf("canonLhs: %s\n", PcpEmitter::pcpExprToString(canonLhs));
  // printf("canonRhs: %s\n", PcpEmitter::pcpExprToString(canonRhs));

  if(canonRhs->isConstant() && canonRhs->toConstant()->getValue() == 0)
    return compare;
  else
    {
      PcpArith* negRhs = negateExpr(canonRhs);
      PcpArith* newLhs = addSums(canonLhs, negRhs);
      return new PcpCompare(compare->getOperator(), 
			    newLhs,
			    createUnaryPlus(new PcpConstant(0)));
    }
}

PcpBoolArith*
PcpExprCanonicalizer::createUnaryOr(PcpBoolExpr* boolExpr)
{
  PcpBoolArithBuilder* builder = new PcpBoolArithBuilder();

  builder->setOperator(PcpBoolArithOperator::boolOr());
  builder->addOperand(boolExpr);

  PcpBoolArith* result = builder->createBoolArith();
  delete builder;
  return result;
}

void
PcpExprCanonicalizer::appendOperandsToBuilder(PcpBoolArithBuilder* builder, 
					      PcpBoolArith* boolArith)
{
  PcpIterator<PcpBoolExpr*>* iter = boolArith->getOperandsIterator();
  for(;iter->hasNext(); iter->next())
    {
      builder->addOperand(iter->get());
    }
  delete iter;
}

PcpBoolArith*
PcpExprCanonicalizer::mergeOr(PcpBoolArith* or1, PcpBoolArith* or2)
{
  pcpAssert(or1 != NULL && or2 != NULL);
  pcpAssert(or1->getOperator().isBoolOr());
  pcpAssert(or2->getOperator().isBoolOr());

  PcpBoolArithBuilder* builder = new PcpBoolArithBuilder();
  builder->setOperator(PcpBoolArithOperator::boolOr());

  appendOperandsToBuilder(builder, or1);
  appendOperandsToBuilder(builder, or2);
  
  PcpBoolArith* result = builder->createBoolArith();
  delete builder;
  return result;
}

PcpBoolArith*
PcpExprCanonicalizer::createFlattenedAnd(PcpBoolExpr* boolExpr1, PcpBoolExpr* boolExpr2)
{
  PcpBoolArith* result;
  if(!boolExpr1->isBoolArith() && !boolExpr2->isBoolArith())
    result = PcpBoolArith::pcpBoolArithBinaryCreate(PcpBoolArithOperator::boolAnd(),
						    boolExpr1,
						    boolExpr2);
  else if(!boolExpr1->isBoolArith() && boolExpr2->isBoolArith())
    result = this->createFlattenedAnd(boolExpr2, boolExpr1);
  else
    {
      PcpBoolArithBuilder* builder = new PcpBoolArithBuilder();
      builder->setOperator(PcpBoolArithOperator::boolAnd());

      PcpBoolArith* arith1 = boolExpr1->toBoolArith();
      pcpAssert(arith1->getOperator().isBoolAnd());

      this->appendOperandsToBuilder(builder, boolExpr1->toBoolArith());

      if(boolExpr2->isBoolArith())
	{
	  PcpBoolArith* arith2 = boolExpr2->toBoolArith();
	 
	  pcpAssert(arith2->getOperator().isBoolAnd());

	  this->appendOperandsToBuilder(builder, boolExpr2->toBoolArith());
	}
      else
	builder->addOperand(boolExpr2);
    
      result = builder->createBoolArith();
      delete builder;
    }
  return result;
}

PcpBoolArith*
PcpExprCanonicalizer::distributeAnd(PcpBoolArith* or1, PcpBoolArith* or2)
{
  PcpBoolArithBuilder* builder = new PcpBoolArithBuilder();
  builder->setOperator(PcpBoolArithOperator::boolOr());

  PcpIterator<PcpBoolExpr*>* iter1 = or1->getOperandsIterator();
  for(;iter1->hasNext(); iter1->next())
    {
      PcpBoolExpr* expr1 = iter1->get();
      PcpIterator<PcpBoolExpr*>* iter2 = or2->getOperandsIterator();
      for(;iter2->hasNext(); iter2->next())
	{
	  PcpBoolExpr* expr2 = iter2->get();
	  PcpBoolArith* newAnd = this->createFlattenedAnd(expr1, expr2);
	  builder->addOperand(newAnd);
	}
    }
  PcpBoolArith* result = builder->createBoolArith();
  delete builder;
  return result;
}

PcpBoolArith*
PcpExprCanonicalizer::canonicalizeBoolExpr(PcpBoolExpr* boolExpr)
{
  if(!boolExpr->isBoolArith())
    return this->createUnaryOr(this->canonicalizeCompare(boolExpr->toCompare()));

  PcpBoolArith* boolArith = boolExpr->toBoolArith();
  if(boolArith->getNumOperands() < 2)
    return boolArith;
  else
    {
      PcpIterator<PcpBoolExpr*>* iter = boolArith->getOperandsIterator();
      PcpBoolArithOperator oper = boolArith->getOperator();
      PcpBoolArith* result = this->canonicalizeBoolExpr(iter->get());
      iter->next();
      for(;iter->hasNext(); iter->next())
	{
	  PcpBoolExpr* operand = iter->get();
	  PcpBoolArith* canonicalizedOperand = this->canonicalizeBoolExpr(operand);
	  if(oper.isBoolOr())
	    {
	      result = this->mergeOr(canonicalizedOperand, result);
	    }
	  else if(oper.isBoolAnd())
	    {
	      result = this->distributeAnd(canonicalizedOperand, result);
	    }
	  else
	    pcpAssert(false);
	}
      return result;
    }
}

void
PcpExprCanonicalizer::visit(PcpScop* scop)
{
  scop->getBody()->accept(this);
}

void
PcpExprCanonicalizer::visit(PcpLoop* loop)
{
  this->getScalarContextStack()->push(loop->getIv()); // Iv(loop->getIv());
  PcpExpr* oldBase = this->getCanonicalizationBase();
  PcpExpr* newBase = this->getScalarContextStack()->createBaseExpr();
  this->setCanonicalizationBase(newBase);
  PcpBoolExpr* conditionBefore = loop->getCondition();
  PcpBoolArith* canonicalized = this->canonicalizeBoolExpr(conditionBefore);
  PcpBoolExpr* simplified = canonicalized->getNumOperands() == 1 
    ? canonicalized->getOperand(0)
    : canonicalized;
  loop->setCondition(simplified);
  loop->getBody()->accept(this);
  this->getScalarContextStack()->pop();
  this->setCanonicalizationBase(oldBase);
}

void
PcpExprCanonicalizer::visit(PcpSequence* sequence)
{
  visitChildren(sequence);
}

void
PcpExprCanonicalizer::visit(PcpGuard* guard)
{
  visitChildren(guard);
}

void
PcpExprCanonicalizer::visit(PcpCopy* copy)
{
  visitChildren(copy);
}

void
PcpExprCanonicalizer::visit(PcpUserStmt* userStmt)
{
  visitChildren(userStmt);
}

void
PcpExprCanonicalizer::visit(PcpArrayAccess* arrayAccess)
{
  PcpIterator<PcpExpr*>* iter = arrayAccess->getSubscriptsIterator();
  int i;
  for(i = 0; i < arrayAccess->getNumSubscripts(); i++)
    {
      PcpExpr* expr = arrayAccess->getSubscript(i);
      PcpArith* canonicalized = canonicalizeExprWithBase(expr);
      PcpExpr* simplified = canonicalized->getNumOperands() == 1 
	? canonicalized->getOperand(0)
	: canonicalized;
      
      arrayAccess->setSubscript(i, simplified);
    }
  delete iter;
}

void
PcpExprCanonicalizer::setCanonicalizationBase(PcpExpr* canonicalizationBase)
{
  this->canonicalizationBase = canonicalizationBase;
}

PcpExpr*
PcpExprCanonicalizer::getCanonicalizationBase()
{
  return this->canonicalizationBase;
}


void
PcpExprCanonicalizer::canonicalize(PcpScop* scop)
{
  setScalarContextStack(new PcpScalarContextStack(scop));
  visit(scop);
}

PcpExprCanonicalizer::PcpExprCanonicalizer(PcpScalarOrder* scalarOrder)
{
  setScalarOrder(scalarOrder);
}
