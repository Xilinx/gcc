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
#ifndef _PCP_STRING_BUFFER_
#define _PCP_STRING_BUFFER_

#include <stdio.h>
#include <string.h>

class PcpStringBuffer 
{
 protected:
  int size;
  int capacity;
  char* buffer;

  virtual void setSize(int size);
  virtual void setCapacity(int capacity);
  virtual int getCapacity();
  virtual void setBuffer(char* rawBuffer);
  virtual char* getBuffer();
  virtual void ensureCapacity(int capacity);

 public:
  // Get length of string in BUFFER. 
  virtual int getLength();
  
  // Get size of BUFFER.(one more than length because of '\0') 
  virtual int getSize();
  
  // Convert BUFFER to string.  
  virtual const char* toString();
  
  // Create a new string buffer.  
  PcpStringBuffer();
  
  // Append STRING to BUFFER.  
  virtual void append(const char* string);
  
  // Append newline to BUFFER.  
  virtual void newline();
  
  // Append CHARACTER to BUFFER.  
  virtual void appendChar(char character);
  
  // Append integer VALUE to BUFFER.  
  virtual void appendInt(int value);
  
  // Append integer PTR to BUFFER.  
  virtual void appendPointer(void* ptr);
  
  // Append BUFFER2 to BUFFER.  
  virtual void appendBuffer(PcpStringBuffer* buffer2);

};

#endif // _PCP_STRING_BUFFER_ 
