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

// PCP Dynamic Int Array 
#ifndef _PCP_SET_
#define _PCP_SET_

#include "pcp_error.h"
#include "pcp_alloc.h"
#include "pcp_dynamic_array.h"

template <typename T> class PcpSet
{
protected:
  PcpDynamicArray<T>* array;
  
  void setArray(PcpDynamicArray<T>* array)
  {
    this->array = array;
  }
  
  PcpDynamicArray<T>* getArray()
  {
    return this->array;
  }
  
public:
  PcpIterator<T>* getIterator()
  {
    return getArray()->getIterator();
  }
  
  int getSize()
  {
    return getArray()->getSize();
  }
  
  bool contains(T element)
  {
    PcpIterator<T>* iter = this->getIterator();
    for(;iter->hasNext(); iter->next())
      {
	if(iter->get() == element)
	  {
	    delete iter;
	    return true;
	  }
      }
    delete iter;
    return false;
  }
  
  void insert(T element)
  {
    if(!this->contains(element))
      this->getArray()->add(element);
  }
  
  PcpSet()
  {
    this->setArray(new PcpDynamicArray<T>(4));
  }
};

#endif // _PCP_SET_ 
