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

#include "pcp.h"
#include "pcp_visitor.h"

void
PcpVisitor::visitChildren(PcpArrayType* arrayType)
{
  PcpIterator<PcpExpr*>* iter = arrayType->getDimensionsIterator();
  for(;iter->hasNext(); iter->next())
    {
      iter->get()->accept(this);
    }
  delete iter;
}

void
PcpVisitor::visitChildren(PcpArith* arith)
{
  PcpIterator<PcpExpr*>* iter = arith->getOperandsIterator();
  for(;iter->hasNext(); iter->next())
    {
      iter->get()->accept(this);
    }
  delete iter;
}

void
PcpVisitor::visitChildren(PcpArrayAccess* arrayAccess)
{
  PcpIterator<PcpExpr*>* iter = arrayAccess->getSubscriptsIterator();
  for(;iter->hasNext(); iter->next())
    {
      iter->get()->accept(this);
    }
  delete iter;
}

void
PcpVisitor::visitChildren(PcpCopy* copy)
{
  copy->getDest()->accept(this);
  copy->getSrc()->accept(this);
}

void
PcpVisitor::visitChildren(PcpUserStmt* userStmt)
{
  PcpIterator<PcpArrayAccess*>* iter = userStmt->getArrayAccessesIterator();
  for(;iter->hasNext(); iter->next())
    {
      iter->get()->accept(this);
    }
  delete iter;
}

void
PcpVisitor::visitChildren(PcpSequence* sequence)
{
  PcpIterator<PcpStmt*>* iter = sequence->getStmtsIterator();
  for(;iter->hasNext(); iter->next())
    {
      iter->get()->accept(this);
    }
  delete iter;
}

void
PcpVisitor::visitChildren(PcpGuard* guard)
{
  guard->getCondition()->accept(this);
  guard->getBody()->accept(this);
}

void
PcpVisitor::visitChildren(PcpLoop* loop)
{
  loop->getIv()->accept(this);
  loop->getStart()->accept(this);
  loop->getCondition()->accept(this);
  loop->getStride()->accept(this);
  loop->getBody()->accept(this);
}

void
PcpVisitor::visitChildren(PcpScop* scop)
{
  PcpIterator<PcpVariable*>* varIter = scop->getVariablesIterator();
  for(;varIter->hasNext(); varIter->next())
    {
      varIter->get()->accept(this);
    }
  delete varIter;

  PcpIterator<PcpParameter*>* paramIter = scop->getParametersIterator();
  for(;paramIter->hasNext(); paramIter->next())
    {
      paramIter->get()->accept(this);
    }
  delete paramIter;

  scop->getBody()->accept(this);
}

void
PcpVisitor::visitChildren(PcpCompare* compare)
{
  this->visit(compare->getLhs());
  this->visit(compare->getRhs());
}


void
PcpVisitor::visit(PcpObject* object)
{
  // Do nothing since this is the top level.
}

void
PcpVisitor::visit(PcpArrayType* arrayType)
{
  this->visit((PcpObject*)arrayType);
}

void
PcpVisitor::visit(PcpArith* arith)
{
  this->visit((PcpExpr*) arith);
}

void
PcpVisitor::visit(PcpConstant* constant)
{
  this->visit((PcpExpr*) constant);
}

void
PcpVisitor::visit(PcpIv* iv)
{
  this->visit((PcpExpr*) iv);
}

void
PcpVisitor::visit(PcpParameter* parameter)
{
  this->visit((PcpExpr*) parameter);
}

void
PcpVisitor::visit(PcpCompare* compare)
{
  this->visit((PcpBoolExpr*) compare);
}

void
PcpVisitor::visit(PcpBoolArith* boolArith)
{
  this->visit((PcpBoolExpr*) boolArith);
}

void
PcpVisitor::visit(PcpVariable* variable)
{  
  this->visit((PcpObject*) variable);
}

void
PcpVisitor::visit(PcpArrayAccess* arrayAccess)
{
  this->visit((PcpObject*) arrayAccess);
}

void
PcpVisitor::visit(PcpCopy* copy)
{
  this->visit((PcpStmt*) copy);
}

void
PcpVisitor::visit(PcpUserStmt* userStmt)
{
  this->visit((PcpStmt*) userStmt);
}

void
PcpVisitor::visit(PcpSequence* sequence)
{
  this->visit((PcpStmt*) sequence);
}

void
PcpVisitor::visit(PcpGuard* guard)
{
  this->visit((PcpStmt*) guard);
}

void
PcpVisitor::visit(PcpLoop* loop)
{
  this->visit((PcpStmt*) loop);
}

void
PcpVisitor::visit(PcpScop* scop)
{
  this->visit((PcpObject*) scop);
}

void
PcpVisitor::visit(PcpExpr* expr)
{
  this->visit((PcpObject*) expr);
}

void
PcpVisitor::visit(PcpBoolExpr* boolExpr)
{
  this->visit((PcpObject*) boolExpr);
}

void
PcpVisitor::visit(PcpStmt* stmt)
{
  this->visit((PcpObject*) stmt);
}

void
PcpVisitor::visit(PcpAnnot* annot)
{
  // Do nothing. Top level.
}

void
PcpVisitor::visit(PcpAnnotSet* annot)
{
  // Do nothing. Top level.
}

void
PcpVisitor::visit(PcpAnnotInt* annotInt)
{
  this->visit((PcpAnnot*) annotInt);
}

void
PcpVisitor::visit(PcpAnnotTerm* annotTerm)
{
  this->visit((PcpAnnot*) annotTerm);
}

void
PcpVisitor::visit(PcpAnnotString* annotString)
{
  this->visit((PcpAnnot*) annotString);
}

void
PcpVisitor::visit(PcpAnnotObject* annotObject)
{
  this->visit((PcpAnnot*) annotObject);
}

PcpVisitor::PcpVisitor()
{
}

