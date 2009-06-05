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
#include "pcp.h"
#include "pcp_emitter.h"
#include "pcp_scalar_order.h"

void
PcpScalarOrder::Visitor::setIvCount(int ivCount)
{
  this->ivCount = ivCount;
}

int 
PcpScalarOrder::Visitor::getIvCount()
{
  return this->ivCount;
}

void
PcpScalarOrder::Visitor::incrementIvCount()
{
  this->setIvCount(this->getIvCount() + 1);
}

void
PcpScalarOrder::Visitor::decrementIvCount()
{
  this->setIvCount(this->getIvCount() - 1);
}


void
PcpScalarOrder::Visitor::setParameterCount(int parameterCount)
{
  this->parameterCount = parameterCount;
}

int
PcpScalarOrder::Visitor::getParameterCount()
{
  return this->parameterCount;
}

void
PcpScalarOrder::Visitor::incrementParameterCount()
{
  this->setParameterCount(this->getParameterCount() + 1);
}

void
PcpScalarOrder::Visitor::decrementParameterCount()
{
  this->setParameterCount(this->getParameterCount() - 1);
}

void
PcpScalarOrder::Visitor::setOrderMap(PcpMap<PcpExpr*, int>* orderMap)
{
  this->orderMap = orderMap;
}

PcpMap<PcpExpr*, int>*
PcpScalarOrder::Visitor::getOrderMap()
{
  return this->orderMap;
}

void
PcpScalarOrder::Visitor::visit(PcpScop* scop)
{
  PcpIterator<PcpParameter*>* paramIter = scop->getParametersIterator();
  for(;paramIter->hasNext(); paramIter->next())
    {
      paramIter->get()->accept(this);
    }
  delete paramIter;
  scop->getBody()->accept(this);
}

void
PcpScalarOrder::Visitor::visit(PcpLoop* loop)
{
  loop->getIv()->accept(this);
  this->incrementIvCount();
  loop->getBody()->accept(this);
  this->decrementIvCount();
}

void
PcpScalarOrder::Visitor::visit(PcpSequence* sequence)
{
  this->visitChildren(sequence);
}

void
PcpScalarOrder::Visitor::visit(PcpGuard* guard)
{
  guard->getBody()->accept(this);
}

void
PcpScalarOrder::Visitor::visit(PcpIv* iv)
{
  //printf("IV: (%s, %d)\n", iv->getName(), this->getIvCount());
  this->getOrderMap()->insert(iv, this->getIvCount());

}

void
PcpScalarOrder::Visitor::visit(PcpParameter* parameter)
{
  //printf("Param: (%s, %d)\n", parameter->getName(), this->getParameterCount());
  this->getOrderMap()->insert(parameter, this->getParameterCount());
  incrementParameterCount();
}
    
PcpScalarOrder::Visitor::Visitor(PcpMap<PcpExpr*, int>* orderMap)
{
  this->setIvCount(1);
  this->setParameterCount(1);
  this->setOrderMap(orderMap);
}


void
PcpScalarOrder::setOrderMap(PcpMap<PcpExpr*, int>* orderMap)
{
  this->orderMap = orderMap;
}

PcpMap<PcpExpr*, int>*
PcpScalarOrder::getOrderMap()
{
  return this->orderMap;
}

bool
PcpScalarOrder::equal(PcpExpr* expr1, PcpExpr* expr2)
{
  return expr1 == expr2 || expr1->isConstant() && expr2->isConstant();
}

bool
PcpScalarOrder::less(PcpExpr* expr1, PcpExpr* expr2)
{
  if(this->equal(expr1, expr2))
    return false;

  if(expr2->isConstant())
    return true;

  if(expr2->isParameter())
    {
      if(expr1->isParameter())
	{
	  int value1 = this->getOrderMap()->lookup(expr1);
	  int value2 = this->getOrderMap()->lookup(expr2);
	  pcpAssert(value1 != value2);
	  return value1 < value2;
	}
      else if(expr1->isConstant())
	return false;
      else
	return true;
    }
  
  if(expr2->isIv() && expr1->isIv())
    {
      pcpAssert(this->getOrderMap()->contains(expr1));
      pcpAssert(this->getOrderMap()->contains(expr1));

      if(!this->getOrderMap()->contains(expr1))
	printf("Non existing 1 iv: %s\n", PcpEmitter::pcpExprToString(expr1));
      if(!this->getOrderMap()->contains(expr2))
	printf("Non existing 2 iv: %s\n", PcpEmitter::pcpExprToString(expr2));
   
      int value1 = this->getOrderMap()->lookup(expr1);
      int value2 = this->getOrderMap()->lookup(expr2);
      pcpAssert(value1 != value2);
      return value1 < value2;
    }
  else
    return false;
}

PcpScalarOrder::PcpScalarOrder(PcpScop* scop)
{
  this->setOrderMap(new PcpTreeMap<PcpExpr*, int>());
  PcpScalarOrder::Visitor visitor(this->getOrderMap());
  visitor.visit(scop);
}
