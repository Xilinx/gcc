// -*- C++ -*-
//
// Copyright (C) 2008 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License as published by the Free Software
// Foundation; either version 2, or (at your option) any later
// version.

// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this library; see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.

// As a special exception, you may use this file as part of a free
// software library without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to
// produce an executable, this file does not by itself cause the
// resulting executable to be covered by the GNU General Public
// License.  This exception does not however invalidate any other
// reasons why the executable file might be covered by the GNU General
// Public License.

/** @file libprofc++/profiler_trace.h
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu

#ifndef PROFCXX_PROFILER_CONTAINER_SIZE_H__
#define PROFCXX_PROFILER_CONTAINER_SIZE_H__ 1

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace cxxprof_runtime
{
long reserve_size();

class trace_container_size
    : public trace_base<stdlib_info_size*, stdlib_info_size*> 
{
 public:
  trace_container_size() {}
  ~trace_container_size() {free_all();}
  trace_container_size(unsigned long size)  
      : trace_base<stdlib_info_size*, stdlib_info_size*>(size) {};

  // Insert a new node at construct with object, callstack and initial size. 
  void insert(void* __obj, stack_t* __stack, stdlib_size_t __num);
  // Call at destruction/clean to set container final size.
  void destruct(void* __obj, stdlib_size_t __num, stdlib_size_t __inum);
  void construct(void* __obj, stdlib_size_t __inum);
  // Call at resize to set resize/cost information.
  void resize(void* __obj, int __from, int __to);
  void print();
  void free_all();
  // Get stack but return a vector.
  stack_t* get_stack();
};

inline void trace_container_size::insert(void* __obj, stack_t* __stack,
                                  stdlib_size_t __num)
{
  ObjMap* live = &trace_info.first;

  // Control the size to be less than reserved size. 
  if (reserve_size() > 0 && live->size() > reserve_size()) return;

  stdlib_info_size *node = new stdlib_info_size(__obj, __stack, __num);
  live->insert(ObjMap::value_type(__obj, node));
}

} // namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_CONTAINER_SIZE_H__ */
