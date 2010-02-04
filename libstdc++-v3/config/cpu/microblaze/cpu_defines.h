// Specific definitions for Xilinx MicroBlaze platforms  -*- C++ -*-

// Copyright (C) 2009, 2010 Free Software Foundation, Inc.
//
// Contributed by Michael Eager <eager@eagercon.com>.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GCC; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.  */

// As a special exception, if you link this library with other files,
// some of which are compiled with GCC, to produce an executable,
// this library does not by itself cause the resulting executable
// to be covered by the GNU General Public License.
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License. 

#ifndef _GLIBCXX_CPU_DEFINES
#define _GLIBCXX_CPU_DEFINES 1

// Optimize memory size for embedded systems
//   -- Use abort for eh terminate
#define _GLIBCXX_NO_VERBOSE_TERMINATE 1

#endif
