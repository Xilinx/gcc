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

#ifndef _PCP_SCALAR_ORDER_
#define _PCP_SCALAR_ORDER_
#include "pcp.h"
#include "pcp_map.h"
#include "pcp_visitor.h"

class PcpScalarOrder
{
 protected:
  PcpMap<PcpExpr*, int>* orderMap;

  void setOrderMap(PcpMap<PcpExpr*, int>* orderMap);
  PcpMap<PcpExpr*, int>* getOrderMap();

  
  class Visitor : public PcpVisitor
  {
  protected:
    int ivCount;
    int parameterCount;
    PcpMap<PcpExpr*, int>* orderMap;

    void setIvCount(int ivCount);
    int getIvCount();
    void incrementIvCount();
    void decrementIvCount();

    void setParameterCount(int parameterCount);
    int getParameterCount();
    void incrementParameterCount();
    void decrementParameterCount();

    void setOrderMap(PcpMap<PcpExpr*, int>* orderMap);
    PcpMap<PcpExpr*, int>* getOrderMap();

  public:
    virtual void visit(PcpScop* scop);
    virtual void visit(PcpLoop* loop);
    virtual void visit(PcpSequence* sequence);
    virtual void visit(PcpGuard* guard);
    virtual void visit(PcpIv* iv);
    virtual void visit(PcpParameter* parameter);

    Visitor(PcpMap<PcpExpr*, int>* orderMap);
  };

 public:
  bool equal(PcpExpr* expr1, PcpExpr* expr2);
  bool less(PcpExpr* expr1, PcpExpr* expr2);

  PcpScalarOrder(PcpScop* scop);
};

#endif // _PCP_SCALAR_ORDER_ 
