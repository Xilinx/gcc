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

#include "pcp_scattering.h"
#include "pcp_emitter.h"
#include "pcp_expr_canonicalizer.h"

void
PcpScatteringMap::ScheduleStack::setScalarContextStack(PcpScalarContextStack* scalarContextStack)
{
  this->scalarContextStack = scalarContextStack;
}

PcpScalarContextStack*
PcpScatteringMap::ScheduleStack::getScalarContextStack()
{
  return this->scalarContextStack;
}

void
PcpScatteringMap::ScheduleStack::setSequenceStack(PcpStack<int>* sequenceStack)
{
  this->sequenceStack = sequenceStack;
}

PcpStack<int>*
PcpScatteringMap::ScheduleStack::getSequenceStack()
{
  return this->sequenceStack;
}

void
PcpScatteringMap::ScheduleStack::setOnSequence(bool onSequence)
{
  this->onSequence = onSequence;
}

bool
PcpScatteringMap::ScheduleStack::getOnSequence()
{
  return this->onSequence;
}

void
PcpScatteringMap::ScheduleStack::pushSequence(int sequenceNumber)
{
  pcpAssert(this->getOnSequence());
  this->getSequenceStack()->push(sequenceNumber);
  this->setOnSequence(false);
}

int
PcpScatteringMap::ScheduleStack::popSequence()
{
  int result;
  pcpAssert(!this->getOnSequence());
  result = this->getSequenceStack()->pop();
  this->setOnSequence(true);
  return result;
}

int
PcpScatteringMap::ScheduleStack::peekSequenceStack(int index)
{
  return this->getSequenceStack()->peek(index);
}

int
PcpScatteringMap::ScheduleStack::getSequenceStackSize()
{
  return this->getSequenceStack()->getSize();
}

void
PcpScatteringMap::ScheduleStack::incrementSequenceTop()
{
  int value = this->popSequence();
  this->pushSequence(value + 1);
}

void
PcpScatteringMap::ScheduleStack::pushIv(PcpIv* iv)
{
  pcpAssert(!this->getOnSequence());
  this->getScalarContextStack()->push(iv);
  this->setOnSequence(true);
}

PcpExpr*
PcpScatteringMap::ScheduleStack::popIv()
{
  PcpExpr* result = NULL;
  pcpAssert(this->getOnSequence());
  result = this->getScalarContextStack()->pop();
  this->setOnSequence(false);
  return result;
}

PcpExpr*
PcpScatteringMap::ScheduleStack::peekScalarStack(int index)
{
  return this->getScalarContextStack()->peek(index);
}

int
PcpScatteringMap::ScheduleStack::getScalarStackSize()
{
  return this->getScalarContextStack()->getSize();
}

const char*
PcpScatteringMap::ScheduleStack::toString()
{
  int i;
  int numSequence = this->getSequenceStackSize();
  PcpStringBuffer buffer;
  buffer.appendInt(this->peekSequenceStack(numSequence-1));
  for(i = numSequence-2; i >= 0; i--)
    {
      buffer.append(" ");
      PcpExpr* expr = this->peekScalarStack(i);
      buffer.append(PcpEmitter::pcpExprToString(expr));
      buffer.append(" ");
      int sequenceNumber = this->peekSequenceStack(i);
      buffer.appendInt(sequenceNumber);
    }
  return buffer.toString();
}

void
PcpScatteringMap::ScheduleStack::createIdentityScattering()
{
  int i;
  int numSequence = this->getSequenceStackSize();
  PcpStringBuffer buffer;
  buffer.appendInt(this->peekSequenceStack(numSequence-1));
  for(i = numSequence-2; i >= 0; i--)
    {
      buffer.append(" ");
      PcpExpr* expr = this->peekScalarStack(i);
      buffer.append(PcpEmitter::pcpExprToString(expr));
      buffer.append(" ");
      int sequenceNumber = this->peekSequenceStack(i);
      buffer.appendInt(sequenceNumber);
    }
  printf("Scattering function: %s\n",  buffer.toString());
}

PcpScatteringMap::ScheduleStack::ScheduleStack(PcpScop* scop)
{
  this->setScalarContextStack(new PcpScalarContextStack(scop));
  this->setSequenceStack(new PcpListStack<int>());
  this->setOnSequence(true);
}


void PcpScatteringMap::Visitor::setScatteringMap(PcpScatteringMap* scatteringMap)
{
  this->scatteringMap = scatteringMap;
}

PcpScatteringMap*
PcpScatteringMap::Visitor::getScatteringMap()
{
  return this->scatteringMap;
}

void
PcpScatteringMap::Visitor::setScheduleStack(ScheduleStack* scheduleStack)
{
  this->scheduleStack = scheduleStack;
}

PcpScatteringMap::ScheduleStack*
PcpScatteringMap::Visitor::getScheduleStack()
{
  return this->scheduleStack;
}

void
PcpScatteringMap::Visitor::visit(PcpScop* scop)
{
  this->setScheduleStack(new ScheduleStack(scop));
  this->getScheduleStack()->pushSequence(0);
  scop->getBody()->accept(this);
}

void
PcpScatteringMap::Visitor::visit(PcpLoop* loop)
{
  this->getScheduleStack()->pushIv(loop->getIv());
  this->getScheduleStack()->pushSequence(0);
  loop->getBody()->accept(this);
  this->getScheduleStack()->popSequence();
  this->getScheduleStack()->popIv();
  this->getScheduleStack()->incrementSequenceTop();
}

void
PcpScatteringMap::Visitor::visit(PcpSequence* sequence)
{
  visitChildren(sequence);
}

void
PcpScatteringMap::Visitor::visit(PcpGuard* guard)
{
  visitChildren(guard);
}

void
PcpScatteringMap::Visitor::visit(PcpCopy* copy)
{
  // Create schedule here
  this->getScheduleStack()->createIdentityScattering();
  this->getScheduleStack()->incrementSequenceTop();
}

void
PcpScatteringMap::Visitor::visit(PcpUserStmt* userStmt)
{
  // Create schedule here
  this->getScheduleStack()->createIdentityScattering();
  this->getScheduleStack()->incrementSequenceTop();
}

PcpScatteringMap::Visitor::Visitor(PcpScatteringMap* scatteringMap)
{
  setScatteringMap(scatteringMap);
}

void
PcpScatteringMap::buildScatteringMap(PcpScop* scop) 
{
  Visitor visitor(this);
  visitor.visit(scop);
}

PcpScatteringMap::PcpScatteringMap(PcpScop* scop) : PcpTreeMap<PcpStmt*, PcpScattering*>() 
{
  buildScatteringMap(scop);
}
