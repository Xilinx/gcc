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

#ifndef _PCP_STACK_
#define _PCP_STACK_
#include "pcp_alloc.h"

template <typename Value> class PcpStack
{
 public:
  virtual void push(Value v) = 0;
  virtual Value pop() = 0;
  virtual Value peek(int index) = 0;
  virtual int getSize() = 0;
};


// Simple implementation of a stack using a list.
// Assumptions: operators '=' available

template <typename Value> class PcpListStack 
: public PcpStack<Value>
{
 protected:
  class Entry
  {
    Value first;
    Entry* rest;

  public:

    void setFirst(Value value)
    {
      this->first = value;
    }

    Value getFirst()
    {
      return this->first;
    }

    void setRest(Entry* left)
    {
      this->rest = left;
    }

    Entry* getRest()
    {
      return this->rest;
    }


    Entry(Value value, Entry* rest)
    {
      setFirst(value);
      setRest(rest);
    }
  };

  Entry* stack;

  void setStack(Entry* stack)
  {
    this->stack = stack;
  }

  Entry* getStack()
  {
    return this->stack;
  }


 public:
  virtual void push(Value value)
  {
    this->setStack(new Entry(value, this->getStack()));
  }

  virtual Value pop()
  {
    Entry* oldEntry = this->getStack();
    Value result = oldEntry->getFirst();
    setStack(oldEntry->getRest());
    delete oldEntry;
    return result;
  }

  virtual Value peek(int depth)
  {
    Entry* current = this->getStack();
    while(current != NULL)
      {
	if(depth <= 0)
	  return current->getFirst();
	else
	  {
	    depth = depth - 1;
	    current = current->getRest();
	  }
      }
    return 0;
  }

  virtual int getSize()
  {
    int size = 0;
    Entry* current = this->getStack();
    while(current != NULL)
      {
	size = size + 1;
	current = current->getRest();
      }
    return size;
  }

  PcpListStack()
  {
    setStack(NULL);
  }
};

#endif // _PCP_STACK_ 
