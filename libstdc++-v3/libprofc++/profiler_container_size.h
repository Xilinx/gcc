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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"

#ifndef _GLIBCXX_PROFILE
using std::max;
using std::min;
#else
using std::_GLIBCXX_STD_PR::max;
using std::_GLIBCXX_STD_PR::min;
#endif

namespace cxxprof_runtime
{

// Class for container size node. 
class container_size_info: public object_info_base 
{
 public:
  container_size_info() {}
  container_size_info(const container_size_info& o);
  container_size_info(stack_t __stack, size_t __num);
  virtual ~container_size_info() {}
  void merge(const container_size_info& o);
  void write(FILE* f) const;
  // Call if a container is destructed or cleaned.
  void destruct(size_t __num, size_t __inum);
  // Estimate the cost of resize/rehash. 
  float resize_cost(size_t __from, size_t __to) { return __from; }
  // Call if container is resized.
  void resize(size_t __from, size_t __to);

 private:
  size_t _M_init;
  size_t _M_max;  // range of # buckets
  size_t _M_min;
  size_t _M_total;
  size_t _M_item_min;  // range of # items
  size_t _M_item_max;
  size_t _M_item_total;
  size_t _M_count;
  size_t _M_resize;
  size_t _M_cost;
};

inline void container_size_info::destruct(size_t __num, size_t __inum) 
{
  _M_max = max(_M_max, __num);
  _M_item_max = max(_M_item_max, __inum);
  if (_M_min == 0) {
    _M_min = __num; 
    _M_item_min = __inum;
  } else {
    _M_min = min(_M_min, __num);
    _M_item_min = min(_M_item_min, __inum);
  }
  _M_total += __num;
  _M_item_total += __inum;
  _M_count += 1;
}

inline void container_size_info::resize(size_t __from,
                                        size_t __to) 
{
  assert(__from <= __to);
  _M_cost += this->resize_cost(__from, __to);
  _M_resize += 1;
  _M_max = _M_max > __to ? _M_max : __to;
}

inline container_size_info::container_size_info(stack_t __stack, 
                                                size_t __num)
  : object_info_base(__stack)
{
  _M_init = _M_max = __num;
  _M_item_min = _M_item_max = _M_item_total = _M_total = 0;
  _M_min = 0;
  _M_count = 0;
  _M_resize = 0;
}

inline void container_size_info::merge(const container_size_info& o)
{
  _M_init        = max(_M_init, o._M_init);
  _M_max         = max(_M_max, o._M_max);
  _M_item_max    = max(_M_item_max, o._M_item_max);
  _M_min         = min(_M_min, o._M_min);
  _M_item_min    = min(_M_item_min, o._M_item_min);
  _M_total      += o._M_total;
  _M_item_total += o._M_item_total;
  _M_count      += o._M_count;
  _M_cost       += o._M_cost;
  _M_resize     += o._M_resize;
}

inline container_size_info::container_size_info(const container_size_info& o)
    : object_info_base(o)
{
  _M_init        = o._M_init;
  _M_max         = o._M_max;
  _M_item_max    = o._M_item_max;
  _M_min         = o._M_min;
  _M_item_min    = o._M_item_min;
  _M_total       = o._M_total;
  _M_item_total  = o._M_item_total;
  _M_cost        = o._M_cost;
  _M_count       = o._M_count;
  _M_resize      = o._M_resize;
}

class container_size_stack_info: public container_size_info {
 public:
  container_size_stack_info(const container_size_info& o)
      : container_size_info(o) {}
};

class trace_container_size
    : public trace_base<container_size_info, container_size_stack_info> 
{
 public:
  ~trace_container_size() {}
  trace_container_size()
      : trace_base<container_size_info, container_size_stack_info>() {};

  // Insert a new node at construct with object, callstack and initial size. 
  void insert(const object_t __obj, stack_t __stack, size_t __num);
  // Call at destruction/clean to set container final size.
  void destruct(const void* __obj, size_t __num, size_t __inum);
  void construct(const void* __obj, size_t __inum);
  // Call at resize to set resize/cost information.
  void resize(const void* __obj, int __from, int __to);
};

inline void trace_container_size::insert(const object_t __obj,
                                         stack_t __stack,
                                         size_t __num)
{
  add_object(__obj, container_size_info(__stack, __num));
}

} // namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_CONTAINER_SIZE_H__ */
