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

#ifndef _PCP_SCATTERING_
#define _PCP_SCATTERING_

#include "pcp.h"
#include "pcp_map.h"
#include "pcp_poly.h"
#include "pcp_stack.h"
#include "pcp_visitor.h"
#include "pcp_expr_canonicalizer.h"

class PcpScattering
{
 protected:
  PcpPolyPointsetPowerset* scattering;

  void setScattering(PcpPolyPointsetPowerset* scattering);
  PcpPolyPointsetPowerset* getScattering();

 public:
  PcpScattering();
  PcpScattering(PcpPolyPointsetPowerset* scattering);
};

class PcpScatteringMap : public PcpTreeMap<PcpStmt*, PcpScattering*> 
{
 protected:
    class ScheduleStack
    {
    protected:
      PcpScalarContextStack* scalarContextStack;
      PcpStack<int>* sequenceStack;
      bool onSequence;

      void setScalarContextStack(PcpScalarContextStack* scalarContextStack);
      PcpScalarContextStack* getScalarContextStack();

      void setSequenceStack(PcpStack<int>* sequenceStack);
      PcpStack<int>* getSequenceStack();

      void setOnSequence(bool onSequence);
      bool getOnSequence();
    public:
      void pushSequence(int sequenceNumber);
      int popSequence();
      int peekSequenceStack(int index);
      int getSequenceStackSize();
      void incrementSequenceTop();

      void pushIv(PcpIv* iv);
      PcpExpr* popIv();
      PcpExpr* peekScalarStack(int index);
      int getScalarStackSize();

      void createIdentityScattering();
      const char* toString();
      ScheduleStack(PcpScop* scop);
      
    };

  class Visitor : public PcpVisitor
  {
  protected:
    ScheduleStack* scheduleStack;
    PcpScatteringMap* scatteringMap;

    void setScatteringMap(PcpScatteringMap* scatteringMap);
    PcpScatteringMap* getScatteringMap();

    void setScheduleStack(ScheduleStack* scheduleStack);
    ScheduleStack* getScheduleStack();

  public:
    virtual void visit(PcpScop* scop);
    virtual void visit(PcpLoop* loop);
    virtual void visit(PcpSequence* sequence);
    virtual void visit(PcpGuard* guard);
    virtual void visit(PcpCopy* copy);
    virtual void visit(PcpUserStmt* userStmt);

    Visitor(PcpScatteringMap* scatteringMap);
  };
  
  void buildScatteringMap(PcpScop* scop);
 public:
  PcpScatteringMap(PcpScop* scop);
};

#endif // _PCP_SCATTERING_ 
