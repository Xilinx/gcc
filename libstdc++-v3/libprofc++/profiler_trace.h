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

#ifndef PROFCXX_PROFILER_TRACE_H__
#define PROFCXX_PROFILER_TRACE_H__ 1

#include <execinfo.h>
#include <string.h>
#include <unordered_map>
#include <vector>
#include "profiler_state.h"

namespace cxxprof_runtime 
{
#ifndef _GLIBCXX_PROFILE
using std::unordered_map;
using std::vector;
#else
using std::_GLIBCXX_STD_PR::unordered_map;
using std::_GLIBCXX_STD_PR::vector;
#endif

typedef vector<void*> stack_t;

inline stack_t* get_stack()
{
  const int _S_max_stack_depth = 32;
  void* __stack_buffer[_S_max_stack_depth];

  int __depth = backtrace(__stack_buffer, _S_max_stack_depth);
  stack_t* __stack = new vector<void*>(__depth);
  memcpy(&(*__stack)[0], __stack_buffer, __depth * sizeof(void*));

  return __stack;
}

// Hash function for summary trace using stack as index.
class stack_hash 
{
 public:
  size_t operator()(stack_t* const  __s) const
  {
    if (__s == NULL) return 0;

    size_t index = 0;
    for (stack_t::iterator it = __s->begin(); it != __s->end(); ++it) {
      index += (unsigned long)*it;
    } 
    return index;
  }

  bool operator() (stack_t* const stack1, stack_t* const stack2) const
  {
    if (stack1 == NULL && stack2 == NULL) return true;

    if (stack1 == NULL || stack2 == NULL) return false;

    if (stack1->size() != stack2->size()) return false;

    size_t byte_size = stack1->size() * sizeof(stack_t::value_type);
    return memcmp(&(*stack1)[0], &(*stack2)[0], byte_size) == 0;
  }
};

template <typename _Obj, typename _Summary>
class trace_base
{
 public:
  trace_base() {}
  trace_base(unsigned long size);
  ~trace_base() {}  

protected:
  typedef unordered_map<void*, _Obj> ObjMap;
  typedef unordered_map<stack_t*, _Summary, stack_hash, stack_hash> StackMap;
  std::pair <ObjMap, StackMap> trace_info;
};

template <typename _Obj, typename _Summary>
inline trace_base<_Obj, _Summary>::trace_base(unsigned long size)
{
  trace_info.first.rehash(size);
  trace_info.second.rehash(size);
}

// This function must be called by each instrumentation point.
// The common path is inlined fully.
inline bool __profcxx_init(void)
{
  if (is_invalid()) {
    void __profcxx_init_unconditional();
    __profcxx_init_unconditional();
  }
  return is_on();
}

// Mutex for multiple threads ifdef THEREAD
#ifdef THREADS
static pthread_mutex_t  list_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t  init_lock = PTHREAD_MUTEX_INITIALIZER;

// Disable mutext when runing with single thread.
#define stdlib_lock(A) pthread_mutex_lock(A)
#define stdlib_unlock(A) pthread_mutex_unlock(A)

#else
// Disable mutext when runing with single thread.
#define stdlib_lock(A) 
#define stdlib_unlock(A) 
#endif
} // namespace cxxprof_runtime

#endif /* PROFCXX_PROFILER_TRACE_H__ */
