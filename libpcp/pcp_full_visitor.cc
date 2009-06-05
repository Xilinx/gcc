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
#include "pcp_full_visitor.h"


void
PcpFullVisitor::visit(PcpArrayType* arrayType)
{
  this->visitChildren(arrayType);
}

void
PcpFullVisitor::visit(PcpArith* arith)
{
  this->visitChildren(arith);
}

void
PcpFullVisitor::visit(PcpConstant* constant)
{
  // Leaf no need to visit
}

void
PcpFullVisitor::visit(PcpIv* iv)
{
  // Leaf no need to visit
}

void
PcpFullVisitor::visit(PcpParameter* parameter)
{
  // Leaf no need to visit
}

void
PcpFullVisitor::visit(PcpCompare* compare)
{
  this->visitChildren(compare);
}

void
PcpFullVisitor::visit(PcpBoolArith* boolArith)
{
}

void
PcpFullVisitor::visit(PcpVariable* variable)
{  
}

void
PcpFullVisitor::visit(PcpArrayAccess* arrayAccess)
{
}

void
PcpFullVisitor::visit(PcpCopy* copy)
{
}

void
PcpFullVisitor::visit(PcpUserStmt* userStmt)
{
  this->visitChildren(userStmt);
}

void
PcpFullVisitor::visit(PcpSequence* sequence)
{
  this->visitChildren(sequence);
}

void
PcpFullVisitor::visit(PcpGuard* guard)
{
  this->visitChildren(guard);
}

void
PcpFullVisitor::visit(PcpLoop* loop)
{
  this->visitChildren(loop);
}

void
PcpFullVisitor::visit(PcpScop* scop)
{
  this->visitChildren(scop);
}

void
PcpFullVisitor::visit(PcpExpr* expr)
{
  if(expr->isArith())
    this->visit(expr->toArith());
  else if(expr->isConstant())
    this->visit(expr->toConstant());
  else if(expr->isIv())
    this->visit(expr->toIv());
  else if(expr->isParameter())
    this->visit(expr->toParameter());
  else
    PcpError::reportErrorNewline("Unknown expression\n");
}

void
PcpFullVisitor::visit(PcpBoolExpr* boolExpr)
{
}

void
PcpFullVisitor::visit(PcpStmt* stmt)
{
}

void
PcpFullVisitor::visit(PcpAnnot* annot)
{
}

void
PcpFullVisitor::visit(PcpAnnotSet* annot)
{
}

void
PcpFullVisitor::visit(PcpAnnotInt* annotInt)
{
}

void
PcpFullVisitor::visit(PcpAnnotTerm* annotTerm)
{
}

void
PcpFullVisitor::visit(PcpAnnotString* annotString)
{
}

void
PcpFullVisitor::visit(PcpAnnotObject* annotObject)
{
}

PcpFullVisitor::PcpFullVisitor()
{
}

