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

#ifndef _PCP_FULL_VISITOR_H_
#define _PCP_FULL_VISITOR_H_

#include "pcp.h"
#include "pcp_visitor.h"

class PcpFullVisitor : public PcpVisitor
{
 protected:

 public:
  virtual void visit(PcpObject* object);
  virtual void visit(PcpArrayType* arrayType);
  virtual void visit(PcpArith* arith);
  virtual void visit(PcpConstant* constant);
  virtual void visit(PcpIv* iv);
  virtual void visit(PcpParameter* parameter);
  virtual void visit(PcpVariable* variable);
  virtual void visit(PcpArrayAccess* arrayAccess);
  virtual void visit(PcpStmt* stmt);
  virtual void visit(PcpCopy* copy);
  virtual void visit(PcpUserStmt* userStmt);
  virtual void visit(PcpSequence* sequence);
  virtual void visit(PcpGuard* guard);
  virtual void visit(PcpLoop* loop);
  virtual void visit(PcpScop* scop);
  virtual void visit(PcpExpr* expr);
  virtual void visit(PcpBoolExpr* boolExpr);
  virtual void visit(PcpCompare* compare);
  virtual void visit(PcpBoolArith* boolArith);


  virtual void visit(PcpAnnotSet* annot);

  virtual void visit(PcpAnnot* annot);
  virtual void visit(PcpAnnotInt* annotInt);
  virtual void visit(PcpAnnotTerm* annotTerm);
  virtual void visit(PcpAnnotString* annotString);
  virtual void visit(PcpAnnotObject* annotObject);

  PcpFullVisitor();
  // Extended components
};

#endif // _PCP_VISITOR_H_
