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

#ifndef PROFCXX_PROFILER_VECTOR_TO_LIST_H__
#define PROFCXX_PROFILER_VECTOR_TO_LIST_H__ 1

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace cxxprof_runtime
{

// Class for vector to list 
class vector2list_info: public object_info_base
{
 public:
  vector2list_info()
      :_M_shift_count(0), _M_iterate(0), _M_resize(0), _M_pred_cost(0),
       _M_valid(true) {}
  vector2list_info(stack_t __stack)
      : object_info_base(__stack), _M_shift_count(0), _M_iterate(0),
        _M_resize(0), _M_pred_cost(0), _M_valid(true) {} 
  virtual ~vector2list_info() {}
  vector2list_info(const vector2list_info& o);
  void merge(const vector2list_info& o);
  void write(FILE* f) const;

  size_t shift_count() { return _M_shift_count; }
  size_t iterate()   { return _M_iterate; }
  float pred_cost() { return _M_pred_cost; }
  size_t resize() { return _M_resize; }
  void set_pred_cost(float __lc) { _M_pred_cost = __lc; }
  void set_cost(float __c) { _M_cost = __c; }
  bool is_valid() { return _M_valid; }
  void set_invalid() { _M_valid = false; }

  void opr_insert(size_t __pos, size_t __num);
  void opr_iterate(size_t __num) { _M_iterate += __num; }
  void resize(size_t __from, size_t __to);

private:
  size_t _M_shift_count;
  size_t _M_iterate;
  size_t _M_resize;
  float _M_pred_cost;
  float _M_cost;
  bool  _M_valid;
};

inline vector2list_info::vector2list_info(const vector2list_info& o)
    : object_info_base(o)
{
  _M_shift_count  = o._M_shift_count;
  _M_iterate      = o._M_iterate;
  _M_cost         = o._M_cost;
  _M_pred_cost    = o._M_pred_cost;
  _M_valid        = o._M_valid;
  _M_resize       = o._M_resize;
}

inline void vector2list_info::merge(const vector2list_info& o)
{
  _M_shift_count  += o._M_shift_count;
  _M_iterate      += o._M_iterate;
  _M_cost         += o._M_cost;
  _M_pred_cost    += o._M_pred_cost;
  _M_valid        &= o._M_valid;
  _M_resize       += o._M_resize;
}

inline void vector2list_info::opr_insert(size_t __pos, 
                                                size_t __num)
{
  _M_shift_count += __num - __pos;
}

inline void vector2list_info::resize(size_t __from, 
                                            size_t __to)
{
  _M_resize += __from;
}

class vector2list_stack_info: public vector2list_info {
 public:
  vector2list_stack_info(const vector2list_info& o) : vector2list_info(o) {}
};

class trace_vector_to_list
    : public trace_base<vector2list_info, vector2list_stack_info> 
{
 public:
  trace_vector_to_list();
  ~trace_vector_to_list() {}

  // Insert a new node at construct with object, callstack and initial size. 
  void insert(object_t __obj, stack_t __stack);
  // Call at destruction/clean to set container final size.
  void destruct(const void* __obj);

  // Find the node in the live map.
  vector2list_info* find(const void* __obj);

  // Collect cost of operations.
  void opr_insert(const void* __obj, size_t __pos, size_t __num);
  void opr_iterate(const void* __obj, size_t __num);
  void invalid_operator(const void* __obj);
  void resize(const void* __obj, size_t __from, size_t __to);
  float vector_cost(size_t __shift, size_t __iterate, size_t __resize);
  float list_cost(size_t __shift, size_t __iterate, size_t __resize);
};

inline trace_vector_to_list::trace_vector_to_list()
    : trace_base<vector2list_info, vector2list_stack_info>()
{
  id = "vector-to-list";
}

inline void trace_vector_to_list::insert(object_t __obj, stack_t __stack)
{
  add_object(__obj, vector2list_info(__stack));
}

} // namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_VECTOR_TO_LIST_H__ */
