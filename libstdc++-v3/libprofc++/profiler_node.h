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

/** @file libprofc++/profiler_node.h
 *  @brief Data structures to represent a single profiling event.
 */

// Written by Lixia Liu

#ifndef PROFCXX_PROFILER_NODE_H__
#define PROFCXX_PROFILER_NODE_H__ 1

#include <assert.h>
#include <stdio.h>
#include <vector>

namespace cxxprof_runtime
{
typedef unsigned long stdlib_size_t;

#ifndef _GLIBCXX_PROFILE
using std::vector;
#else
using std::_GLIBCXX_STD_PR::vector;
#endif

typedef vector<void*> stack_t;

// Base class for all information nodes.
class stdlib_info_base 
{
 public:
  stdlib_info_base()
  : _M_object(NULL), _M_stack(NULL) {}

  stdlib_info_base(void* __obj, stack_t* __stack);
  virtual ~stdlib_info_base() {}

  // Accessors.
  void set_stack(stack_t* stack);
  stack_t* stack() { return _M_stack; }
  int stack_size() { return _M_stack?_M_stack->size():0; }
  void set_cost(float cost);
  float cost() { return _M_cost; }

 protected:
  void* _M_object;
  float  _M_cost;
  stack_t* _M_stack;
};

// Class for container size node. 
class stdlib_info_size:public stdlib_info_base 
{
 public:
  stdlib_info_size() {}
  stdlib_info_size(void* __obj, stack_t* __stack, stdlib_size_t __num); 
  virtual ~stdlib_info_size() {}
  
  // Call if a container is destructed or cleaned.
  void destruct(void* __obj, stdlib_size_t __num, stdlib_size_t __inum);

  // Estimate the cost of resize/rehash. 
  float resize_cost(stdlib_size_t __from, stdlib_size_t __to) { return __from; }

  // Call if container is resized.
  void resize(void* __obj, stdlib_size_t __from, stdlib_size_t __to);

  void print_debug();

  void print(FILE* f);
private:
  stdlib_size_t _M_init;
  stdlib_size_t _M_max;  // range of # buckets
  stdlib_size_t _M_min;
  stdlib_size_t _M_total;
  stdlib_size_t _M_item_min;  // range of # items
  stdlib_size_t _M_item_max;
  stdlib_size_t _M_item_total;
  stdlib_size_t _M_count;
  stdlib_size_t _M_resize;
};

inline stdlib_info_base::stdlib_info_base(void* __obj, stack_t* __stack) 
{
  _M_stack  = __stack;
  _M_object = __obj;
  _M_cost = 0;
}

inline stdlib_info_size::stdlib_info_size(void* __obj, stack_t* __stack, 
                                          stdlib_size_t __num)
  : stdlib_info_base(__obj, __stack)
{
  _M_init = _M_max = __num;
  _M_item_min = _M_item_max = _M_item_total = _M_total = 0;
  _M_min = 0;
  _M_count = 0;
  _M_resize = 0;
}

inline void stdlib_info_size::destruct(void* __obj, stdlib_size_t __num, 
                                       stdlib_size_t __inum) 
{ 
  assert(__obj != NULL);
  _M_max = _M_max < __num ? __num:_M_max;
  _M_item_max = _M_item_max < __inum ? __inum:_M_item_max;
  if (_M_min==0) { 
    _M_min = __num; 
    _M_item_min = __inum;
  } else {
    _M_min = _M_min < __num ? _M_min:__num;
    _M_item_min = _M_item_min < __inum ? _M_item_min:__inum;
  }
  _M_total += __num;
  _M_item_total += __inum;
  _M_count += 1;
}

inline void stdlib_info_size::resize(void* __obj, stdlib_size_t __from,
                                     stdlib_size_t __to) 
{
  assert(__obj == _M_object);
  assert(__from <= __to);
  _M_cost += this->resize_cost(__from, __to);
  _M_resize += 1;
  _M_max = _M_max > __to ? _M_max : __to;
}

} // namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_NODE_H__ */
