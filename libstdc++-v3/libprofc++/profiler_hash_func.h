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

#ifndef PROFCXX_PROFILER_HASH_FUNC_H__
#define PROFCXX_PROFILER_HASH_FUNC_H__ 1

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

#ifndef _GLIBCXX_PROFILE
using std::max;
using std::min;
#else
using std::_GLIBCXX_STD_PR::max;
using std::_GLIBCXX_STD_PR::min;
#endif

namespace cxxprof_runtime
{

// Class for inefficient hash function. 
class hashfunc_info: public object_info_base
{
 public:

  hashfunc_info()
      :_M_longest_chain(0), _M_accesses(0), _M_hops(0) {}

  hashfunc_info(const hashfunc_info& o);

  hashfunc_info(stack_t __stack)
      : object_info_base(__stack),
        _M_longest_chain(0), _M_accesses(0), _M_hops(0){} 

  virtual ~hashfunc_info() {}

  void merge(const hashfunc_info& o);

  void destruct(size_t __chain, size_t __accesses, size_t __hops);

  void write(FILE* f) const;

private:
  size_t _M_longest_chain;
  size_t _M_accesses;
  size_t _M_hops;
};

inline hashfunc_info::hashfunc_info(const hashfunc_info& o)
    : object_info_base(o)
{
  _M_longest_chain = o._M_longest_chain;
  _M_accesses      = o._M_accesses;
  _M_hops          = o._M_hops;
}

inline void hashfunc_info::merge(const hashfunc_info& o)
{
  _M_longest_chain  = max(_M_longest_chain, o._M_longest_chain);
  _M_accesses      += o._M_accesses;
  _M_hops          += o._M_hops;
}

inline void hashfunc_info::destruct(size_t __chain, size_t __accesses, 
                                    size_t __hops)
{ 
  _M_longest_chain  = max(_M_longest_chain, __chain);
  _M_accesses      += __accesses;
  _M_hops          += __hops;
}

class hashfunc_stack_info: public hashfunc_info {
 public:
  hashfunc_stack_info(const hashfunc_info& o) : hashfunc_info(o) {}
};

class trace_hash_func
    : public trace_base<hashfunc_info, hashfunc_stack_info> 
{
 public:
  trace_hash_func()
      : trace_base<hashfunc_info, hashfunc_stack_info>() { id = "hash-distr"; }
  ~trace_hash_func() {}

  // Insert a new node at construct with object, callstack and initial size. 
  void insert(object_t __obj, stack_t __stack);
  // Call at destruction/clean to set container final size.
  void destruct(const void* __obj, size_t __chain,
                size_t __accesses, size_t __hops);
};

inline void trace_hash_func::insert(object_t __obj, stack_t __stack)
{
  printf("Adding %p.\n", __obj);
  add_object(__obj, hashfunc_info(__stack));
}

} // namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_HASH_FUNC_H__ */
