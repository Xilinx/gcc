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

/** @file profile/impl/profiler_node.h
 *  @brief Data structures to represent a single profiling event.
 */

// Written by Lixia Liu

#ifndef PROFCXX_PROFILER_NODE_H__
#define PROFCXX_PROFILER_NODE_H__ 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cstdio>
#include <cstdint>
#include <cstring>
#else
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif
#include <vector>
#if defined HAVE_EXECINFO_H
#include <execinfo.h>
#endif

namespace __cxxprof_impl
{
typedef const void* object_t;
typedef void* instruction_address_t;
typedef std::_GLIBCXX_STD_PR::vector<instruction_address_t> stack_npt;
typedef stack_npt* stack_t;

size_t stack_max_depth();

inline stack_t get_stack()
{
#if defined HAVE_EXECINFO_H
  stack_npt buffer(stack_max_depth());
  int __depth = backtrace(&buffer[0], stack_max_depth());
  stack_t __stack = new stack_npt(__depth);
  memcpy(&(*__stack)[0], &buffer[0], __depth * sizeof(object_t));
  return __stack;
#else
  return NULL;
#endif
}

inline size(const stack_t& stack)
{
  if (!stack) {
    return 0;
  } else {
    return stack->size();
  }
}

inline void write(FILE* f, const stack_t stack)
{
  if (!stack) {
    return;
  }

  stack_npt::const_iterator it;
  for (it = stack->begin(); it != stack->end(); ++it) {
    fprintf(f, "%p ", *it);
  }
}

// Hash function for summary trace using stack as index.
class stack_hash 
{
 public:
  size_t operator()(const stack_t __s) const
  {
    if (!__s) {
      return 0;
    }

    uintptr_t index = 0;
    stack_npt::const_iterator it;
    for (it = __s->begin(); it != __s->end(); ++it) {
      index += reinterpret_cast<uintptr_t>(*it);
    } 
    return index;
  }

  bool operator() (const stack_t stack1, const stack_t stack2) const
  {
    if (!stack1 && !stack2) return true;
    if (!stack1 || !stack2) return false;
    if (stack1->size() != stack2->size()) return false;

    size_t byte_size = stack1->size() * sizeof(stack_npt::value_type);
    return memcmp(&(*stack1)[0], &(*stack2)[0], byte_size) == 0;
  }
};

class object_info_base 
{
 public:
  object_info_base() {}
  object_info_base(stack_t stack) { _M_stack = stack; _M_valid = true; }
  object_info_base(const object_info_base& o);
  virtual ~object_info_base() {}
  bool is_valid() const { return _M_valid; }
  stack_t stack() const { return _M_stack; }
  virtual void write(FILE* f) const = 0;

 protected:
  stack_t _M_stack;
  bool _M_valid;
};

inline object_info_base::object_info_base(const object_info_base& o)
{
  _M_stack = o._M_stack;
  _M_valid = o._M_valid;
}

template<typename object_info>
class stack_info_base
{
 public:
  stack_info_base() {}
  stack_info_base(const object_info& info) = 0;
  virtual ~stack_info_base() {}
  void merge(const object_info& info) = 0;
};

} // namespace __cxxprof_impl
#endif /* PROFCXX_PROFILER_NODE_H__ */
