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

// PCP String Buffer 
#include "pcp_error.h"
#include "pcp_alloc.h"
#include "pcp_string_buffer.h"
#include <string.h>

// Set size of BUFFER to SIZE.  
void
PcpStringBuffer::setSize(int size)
{
  this->size = size;
}

// Get size of BUFFER.  
int
PcpStringBuffer::getSize()
{
  return this->size;
}

// Get length of string in BUFFER.  
int
PcpStringBuffer::getLength()
{
  return this->size - 1;
}

// Set capacity of BUFFER to CAPACITY.  
void
PcpStringBuffer::setCapacity(int capacity)
{
  this->capacity = capacity;
}

// Get capacity of BUFFER.  
int
PcpStringBuffer::getCapacity()
{
  return this->capacity;
}

// Set internal buffer of BUFFER to RAW_BUFFER.  
void
PcpStringBuffer::setBuffer(char* rawBuffer)
{
  this->buffer = rawBuffer;
}

// Get internal buffer of BUFFER.  
char* 
PcpStringBuffer::getBuffer()
{
  return this->buffer;
}

// Convert BUFFER to string.  
const char* 
PcpStringBuffer::toString()
{
  return this->getBuffer();
}

// Ensure that the internal buffer of BUFFER capacity is CAPACIY.  If
// the current capacity is not enough the internal buffer is replaced
// with a bigger buffer and the contents are copied to the new
// buffer.  
void
PcpStringBuffer::ensureCapacity(int capacity)
{
  int oldCapacity = this->getCapacity();
  if(oldCapacity < capacity)
    {
      int i;
      int size = this->getSize();
      int newCapacity = oldCapacity * 2 > capacity ? oldCapacity * 2
	: capacity;
      char* newBuffer =(char*) PCP_NEWVEC(int, newCapacity);
      char* oldBuffer = this->getBuffer();

      for(i = 0; i < size; i++)
	{
	  newBuffer[i] = oldBuffer[i];
	}

      // FREE: oldBuffer.  
      this->setBuffer(newBuffer);
      this->setCapacity(newCapacity);
    }
}

// Create a new string buffer.  
PcpStringBuffer::PcpStringBuffer()
{
  this->setCapacity(0);
  this->setSize(0);
  this->ensureCapacity(16);
  this->setSize(1);
  this->getBuffer()[0] = '\0';
}

// Append STRING to BUFFER.  
void
PcpStringBuffer::append(const char* string)
{
  int oldSize = this->getSize();
  int length = strlen(string);
  int newSize = oldSize + length;
  int startIndex = this->getLength();
  int i;

  this->ensureCapacity(newSize);
  char* rawBuffer = this->getBuffer();
  for(i = 0; i < length; i++)
    {
      rawBuffer[startIndex + i] = string[i];
    }
  rawBuffer[newSize - 1] = '\0';
  this->setSize(newSize);
}

// Append newline to BUFFER.  
void
PcpStringBuffer::newline()
{
  this->append("\n");
}

// Append CHARACTER to BUFFER.  
void
PcpStringBuffer::appendChar(char character)
{
  char tempbuf[2];
  sprintf(tempbuf, "%c", character);
  this->append(tempbuf);
}

// Append integer VALUE to BUFFER.  
void
PcpStringBuffer::appendInt(int value)
{
  char tempbuf[64];
  sprintf(tempbuf, "%d", value);
  this->append(tempbuf);
}

// Append integer PTR to BUFFER.  
void
PcpStringBuffer::appendPointer(void* ptr)
{
  char tempbuf[64];
  sprintf(tempbuf, "%p", ptr);
  this->append(tempbuf);
}

// Append BUFFER2 to BUFFER.  
void
PcpStringBuffer::appendBuffer(PcpStringBuffer* buffer2)
{
  this->append(buffer2->toString());
}
