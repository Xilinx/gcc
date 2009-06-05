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
#ifndef _PCP_DYNAMIC_ARRAY_
#define _PCP_DYNAMIC_ARRAY_
#include "pcp_alloc.h"
#include "pcp_error.h"

template <typename T> class PcpIterator
{
 public:
  virtual bool hasNext() = 0;
  virtual T get() = 0;
  virtual void next() = 0;
};

template <typename T> class PcpArrayIterator;

template <typename T> class PcpArray
{
 protected:
  int size;
  T* array;

  virtual void setSize(int size)
  {
    this->size = size;
  }

  virtual void setArray(T* array)
  {
    this->array = array;
  }

  virtual T* getArray()
  {
    return this->array;
  }

 public:
  
 virtual int getSize()
 {
   return this->size;
   
 }
  
 void set(int index, T value)
 {
   pcpAssert(this->getSize() > index);
   this->getArray()[index] = value;
 }
  
 virtual T get(int index)
 {
   pcpAssert(this->getSize() > index);
   return this->getArray()[index];
 }

 PcpArray(int size)
 {
   size = size > 0 ? size : 1;
   T* array =(T*) PCP_NEWVEC(T*, size);
   setArray(array);
   setSize(size);
 }

 PcpIterator<T>* getIterator();

};

template <typename T> class PcpArrayIterator : public PcpIterator<T>
{
 protected:
  int index;
  PcpArray<T>* array;

  void setIndex(int index)
  {
    this->index = index;
  }

  int getIndex()
  {
    return this->index;
  }

  void setArray(PcpArray<T>* array)
  {
    this->array = array;
  }

  PcpArray<T>* getArray()
  {
    return this->array;
  }

  int getSize()
  {
    return this->getArray()->getSize();
  }

 public:
  virtual bool hasNext() 
  {
    return this->getIndex() < this->getSize();
  }

  virtual T get()
  {
    return this->getArray()->get(this->getIndex());
  }

  virtual void next()
  {
    this->setIndex(this->getIndex() + 1);
  }

  PcpArrayIterator(PcpArray<T>* array)
    {
      this->setArray(array);
      this->setIndex(0);
    }

};

template <typename T> PcpIterator<T>* PcpArray<T>::getIterator()
   {
     return new PcpArrayIterator<T>(this);
   }


template <typename T> class PcpDynamicArray : public PcpArray<T>
{
 protected:
  int capacity;

  virtual void setCapacity(int capacity)
  {
    this->capacity = capacity;
  }

  virtual int getCapacity()
  {
    return this->capacity;
  }

  virtual void ensureCapacity(int capacity)
  {
    int oldCapacity = this->getCapacity();
    if(oldCapacity < capacity)
    {
       int newCapacity = capacity > oldCapacity * 2 ? capacity 
                                                    : oldCapacity * 2;
       T* newArray =(T*) PCP_NEWVEC(T, newCapacity);
       T* oldArray = this->getArray();

       int size = this->getSize();
       int i;

       for(i = 0; i < size; i++)
	 {
	   newArray[i] = oldArray[i];
	 }

      this->setArray(newArray);
      this->setCapacity(newCapacity);
      delete oldArray;
    }
  }

  virtual void initialize()
  {
    this->setCapacity(this->getSize());
    this->setSize(0);
  }

 public:
 PcpDynamicArray() : PcpArray<T>(4)
  {
    this->initialize();
  }

 PcpDynamicArray(int initialCapacity) : PcpArray<T>(initialCapacity)
  {
    this->initialize();
  }
  
  virtual void set(int index, T value)
  {
    if(this->getSize() <= index)
      {
	this->ensureCapacity(index + 1);
	this->setSize(index + 1);
      }
   this->getArray()[index] = value;
  }

  virtual void add(T element)
  {
    this->set(this->getSize(), element);
  }
  
};

#endif // _PCP_DYNAMIC_ARRAY_ 
