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

#ifndef _PCP_EXPR_CANONICALIZER_
#define _PCP_EXPR_CANONICALIZER_
#include "pcp.h"
#include "pcp_visitor.h"
#include "pcp_scalar_order.h"
#include "pcp_stack.h"

class PcpScalarContextStack : public PcpListStack<PcpExpr*>
{
 public:
  virtual PcpExpr* createBaseExpr();
  PcpScalarContextStack(PcpScop* scop);
};

// Normali
class PcpExprCanonicalizer : public PcpVisitor
{
  protected:
  PcpScalarOrder* scalarOrder;
  PcpScalarContextStack* scalarContextStack;
  PcpExpr* canonicalizationBase;
  
  virtual void setScalarOrder(PcpScalarOrder* scalarOrder);
  virtual PcpScalarOrder* getScalarOrder();

  virtual void setScalarContextStack(PcpScalarContextStack* scalarContextStack);
  virtual PcpScalarContextStack* getScalarContextStack();

  virtual PcpExpr* stripMultiplier(PcpExpr* expr);
  virtual bool equal(PcpExpr* expr1, PcpExpr* expr2);
  virtual bool less(PcpExpr* expr1, PcpExpr* expr2);

  virtual PcpConstant* negateConstant(PcpConstant* constant);
  virtual PcpConstant* addConstants(PcpConstant* constant1, PcpConstant* constant2);
  virtual PcpConstant* addConstants(PcpConstant* constant1, int constant2);
  virtual PcpConstant* addConstants(int constant1,int constant2);
  virtual PcpExpr* addTerms(PcpExpr* expr1, PcpExpr* expr2);
  virtual PcpArith* addSums(PcpArith* sum1, PcpArith* sum2);

  virtual PcpArith* createUnaryArith(PcpArithOperator oper, PcpExpr* expr);
  virtual PcpArith* createUnaryPlus(PcpExpr* expr);
  virtual PcpArith* createUnaryMinus(PcpExpr* expr);
  virtual bool sameScalarBase(PcpArith* factor1, PcpArith* factor2);

  virtual PcpConstant* getCanonicalConstant(PcpArith* expr);
  virtual PcpConstant* multiplyConstants(PcpConstant* constant1, PcpConstant* constant2);
  virtual PcpArith* multiplyConstMul(PcpConstant* multiplier, PcpArith* multiply);
  virtual PcpArith* multiplyScalar(PcpConstant* constant1, PcpExpr* scalar);
  virtual PcpArith* multiplyFactors(PcpConstant* multiplier, PcpArith* sum);
  virtual PcpArith* multiplyFactors(PcpArith* factor1, PcpArith* factor2);
  virtual PcpArith* negateExpr(PcpArith* expr);

  virtual PcpCompare* canonicalizeCompare(PcpCompare* compare);
  virtual PcpBoolArith* createUnaryOr(PcpBoolExpr* boolExpr);
  virtual void appendOperandsToBuilder(PcpBoolArithBuilder* builder, 
				       PcpBoolArith* boolArith);
  virtual PcpBoolArith* mergeOr(PcpBoolArith* or1, PcpBoolArith* or2);
  virtual PcpBoolArith* createFlattenedAnd(PcpBoolExpr* boolExpr1, PcpBoolExpr* boolExpr2);
  virtual PcpBoolArith* distributeAnd(PcpBoolArith* or1, PcpBoolArith* or2);

  public:
  virtual void visit(PcpScop* scop);
  virtual void visit(PcpLoop* loop);
  virtual void visit(PcpSequence* sequence);
  virtual void visit(PcpGuard* guard);
  virtual void visit(PcpCopy* copy);
  virtual void visit(PcpUserStmt* userStmt);
  virtual void visit(PcpArrayAccess* arrayAccess);
  
  // Normalizes expressions to have the following form:
  // +(e0, .. en), where each expression e is either constant ord or a
  // multiplication of a constant with either a parameter or induction variable
  // iv: *(c, iv). 
  virtual void setCanonicalizationBase(PcpExpr* expr);
  virtual PcpExpr* getCanonicalizationBase();

  virtual PcpArith* canonicalizeExpr(PcpExpr* expr);
  virtual PcpArith* canonicalizeExprWithBase(PcpExpr* expr);
  virtual PcpBoolArith* canonicalizeBoolExpr(PcpBoolExpr* boolExpr);
  virtual void canonicalize(PcpScop* scop);

  PcpExprCanonicalizer(PcpScalarOrder* scalarOrder);
};

#endif // _PCP_EXPR_CANONICALIZER_ 
