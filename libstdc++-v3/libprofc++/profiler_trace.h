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

#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include "profiler_state.h"
#include "profiler_node.h"

#ifndef _GLIBCXX_PROFILE
using std::unordered_map;
using std::vector;
#else
using std::_GLIBCXX_STD_PR::unordered_map;
using std::_GLIBCXX_STD_PR::vector;
#endif

namespace cxxprof_runtime 
{

// Hash function for summary trace using stack as index.
class stack_hash 
{
 public:
  size_t operator()(const stack_t __s) const
  {
    size_t index = 0;
    stack_npt::const_iterator it;
    for (it = __s->begin(); it != __s->end(); ++it) {
      index += reinterpret_cast<size_t>(*it);
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

inline size_t max_mem()
{
  extern size_t _S_max_mem;
  return _S_max_mem;
}

typedef const char* trace_id_t;
inline void print_trace_id(FILE* f, trace_id_t id)
{
  if (id) {
    fprintf(f, "%s", id);
  } else {
    fprintf(stderr, "Undefined trace id.");
    abort();
  }
}

template <typename object_info, typename stack_info>
class trace_base
{
 public:
  trace_base();
  ~trace_base() {}
  void add_object(object_t object, object_info info);
  object_info* get_object_info(object_t object);
  void retire_object(object_t object);
  void write(FILE* f);

 private:
  typedef unordered_map<object_t, object_info> object_table_t;
  typedef unordered_map<stack_t, stack_info, stack_hash,
                        stack_hash> stack_table_t;
  object_table_t object_table;
  stack_table_t stack_table;
  size_t stack_table_byte_size;

 protected:
  trace_id_t id;
};

template <typename object_info, typename stack_info>
trace_base<object_info, stack_info>::trace_base()
{
  // Do not pick the initial size too large, as we don't know which diagnostics
  // are more active.
  object_table.rehash(10000);
  stack_table.rehash(10000);
  stack_table_byte_size = 0;
  id = NULL;
}

template <typename object_info, typename stack_info>
void trace_base<object_info, stack_info>::add_object(object_t object,
                                                     object_info info)
{
  if (max_mem() == 0 || object_table.size() * sizeof(object_info) <= max_mem())
    object_table.insert(typename object_table_t::value_type(object, info));
  else
    printf("Out of profiler memory: %s.\n", id);
}

template <typename object_info, typename stack_info>
object_info* trace_base<object_info, stack_info>::get_object_info(
    object_t object)
{
  typename object_table_t::iterator object_it = object_table.find(object);
  if (object_it == object_table.end()){
    return NULL;
  } else {
    return &object_it->second;
  }
}

template <typename object_info, typename stack_info>
void trace_base<object_info, stack_info>::retire_object(object_t object)
{
  typename object_table_t::iterator object_it = object_table.find(object);
  if (object_it != object_table.end()){
    const object_info& info = object_it->second;
    const stack_t& stack = info.stack();
    typename stack_table_t::iterator stack_it = stack_table.find(stack);
    if (stack_it == stack_table.end()) {
      // First occurence of this call context.
      if (max_mem() == 0 || stack_table_byte_size < max_mem()) {
        stack_table_byte_size += (sizeof(instruction_address_t) * stack->size()
                                  + sizeof(stack) + sizeof(stack_info));
        cxxprof_runtime::write(stdout, stack);
        printf(" Adding to stack table.\n");
        stack_table.insert(make_pair(stack, stack_info(info)));
      } else {
        printf("Out of profiler memory: %s.\n", id);
      }
    } else {
      // Merge object info into info summary for this call context.
      stack_it->second.merge(info);
      delete stack;
    }
    object_table.erase(object);
  } else {
    printf("Bad object to retire %p.", object);
  }
}

template <typename object_info, typename stack_info>
void trace_base<object_info, stack_info>::write(FILE* f)
{
  typename stack_table_t::iterator it;

  for (it = stack_table.begin(); it != stack_table.end(); it++) {
    if (it->second.is_valid()) {
      print_trace_id(f, id);
      fprintf(f, "|");
      cxxprof_runtime::write(f, it->first);
      fprintf(f, "|");
      it->second.write(f);
    }
  }
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
