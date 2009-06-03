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

/** @file profile/impl/profiler_trace.h
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu

#ifndef PROFCXX_PROFILER_TRACE_H__
#define PROFCXX_PROFILER_TRACE_H__ 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#else
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tr1/unordered_map>
#endif
#if defined _GLIBCXX_PROFILE_THREADS && defined HAVE_TLS
#include <pthread.h>
#endif
#include "profile/impl/profiler_state.h"
#include "profile/impl/profiler_node.h"

namespace __cxxprof_impl
{

#if defined _GLIBCXX_PROFILE_THREADS && defined HAVE_TLS
static pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t init_lock = PTHREAD_MUTEX_INITIALIZER;
#define stdlib_lock(A) pthread_mutex_lock(A)
#define stdlib_unlock(A) pthread_mutex_unlock(A)
#else
#define stdlib_lock(A) 
#define stdlib_unlock(A) 
#endif

// Defined in profiler_<diagnostic name>.h.
class trace_hash_func;
class trace_hashtable_size;
class trace_map2umap;
class trace_vector_size;
class trace_vector_to_list;
void trace_vector_size_init();
void trace_hashtable_size_init();
void trace_hash_func_init();
void trace_vector_to_list_init();
void trace_map_to_unordered_map_init();
void trace_vector_size_report(FILE* f);
void trace_hashtable_size_report(FILE* f);
void trace_hash_func_report(FILE* f);
void trace_vector_to_list_report(FILE* f);
void trace_map_to_unordered_map_report(FILE* f);

// Utility functions.
inline size_t max(size_t a, size_t b) {
  return a >= b ? a : b;
}

inline size_t min(size_t a, size_t b) {
  return a <= b ? a : b;
}

// Diagnostic tables.
template <int Unused=0>
class tables {
 public:
  static trace_hash_func* _S_hash_func;
  static trace_hashtable_size* _S_hashtable_size;
  static trace_map2umap* _S_map2umap;
  static trace_vector_size* _S_vector_size;
  static trace_vector_to_list* _S_vector_to_list;
};

template <>
trace_hash_func* tables<0>::_S_hash_func = NULL;
template <>
trace_hashtable_size* tables<0>::_S_hashtable_size = NULL;
template <>
trace_map2umap* tables<0>::_S_map2umap = NULL;
template <>
trace_vector_size* tables<0>::_S_vector_size = NULL;
template <>
trace_vector_to_list* tables<0>::_S_vector_to_list = NULL;

// Settings.
template <int Unused=0>
class settings {
 public:
  static char _S_trace_default_file_name[];
  static char _S_trace_env_var[];
  static char* _S_trace_file_name;

  // Environment variable to turn everything off.
  static char _S_off_env_var[];

  // Environment variable to set maximum stack depth.
  static char _S_max_stack_depth_env_var[];
  static size_t _S_max_stack_depth;

  // Space budget for each object table.
  static char _S_max_mem_env_var[];
  static size_t _S_max_mem;
};

template <>
char settings<0>::_S_trace_default_file_name[] = 
    "./profile-stdlib.txt";
template <>
char settings<0>::_S_trace_env_var[] = 
    "GLIBCXX_PROFILE_TRACE";
template <>
char* settings<0>::_S_trace_file_name = 
    "./profile-stdlib.txt";
template <>
char settings<0>::_S_off_env_var[] = 
    "GLIBCXX_PROFILE_OFF";
template <>
char settings<0>::_S_max_stack_depth_env_var[] =
    "GLIBCXX_PROFILE_MAX_STACK_DEPTH";
template <>
char settings<0>::_S_max_mem_env_var[] = 
    "GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC";
template <>
size_t settings<0>::_S_max_stack_depth = 32;
template <>
size_t settings<0>::_S_max_mem = 2 << 27;  // 128 MB.

inline size_t stack_max_depth()
{
  return settings<0>::_S_max_stack_depth;
}

inline size_t max_mem()
{
  return settings<0>::_S_max_mem;
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
#ifdef __GXX_EXPERIMENTAL_CXX0X__
  typedef std::_GLIBCXX_STD_PR::unordered_map<object_t, object_info> 
      object_table_t;
  typedef std::_GLIBCXX_STD_PR::unordered_map<stack_t, stack_info, 
                                              stack_hash, stack_hash>
      stack_table_t;
#else
  typedef std::tr1::unordered_map<object_t, object_info> object_table_t;
  typedef std::tr1::unordered_map<stack_t, stack_info, 
                                  stack_hash, stack_hash> stack_table_t;
#endif
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
        stack_table_byte_size += (sizeof(instruction_address_t) * size(stack)
                                  + sizeof(stack) + sizeof(stack_info));
        stack_table.insert(make_pair(stack, stack_info(info)));
      }
    } else {
      // Merge object info into info summary for this call context.
      stack_it->second.merge(info);
      delete stack;
    }
    object_table.erase(object);
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
      __cxxprof_impl::write(f, it->first);
      fprintf(f, "|");
      it->second.write(f);
    }
  }
}

inline size_t env_to_size_t(const char* env_var, size_t default_value)
{
  char* env_value = getenv(env_var);
  if (env_value) {
    long int converted_value = strtol(env_value, NULL, 10);
    if (errno || converted_value < 0) {
      fprintf(stderr, "Bad value for environment variable '%s'.", env_var);
      abort();
    } else {
      return static_cast<size_t>(converted_value);
    }
  } else {
    return default_value;
  }
}

inline void set_max_stack_trace_depth()
{
  settings<0>::_S_max_stack_depth = env_to_size_t(
      settings<0>::_S_max_stack_depth_env_var, 
      settings<0>::_S_max_stack_depth);
}

inline void set_max_mem()
{
  settings<0>::_S_max_mem = env_to_size_t(settings<0>::_S_max_mem_env_var, 
                                          settings<0>::_S_max_mem);
}

// Final report, meant to be registered by "atexit".
inline void __profcxx_report(void)
{
  turn_off();

  FILE* f = fopen(settings<0>::_S_trace_file_name, "a");

  if (!f) {
    fprintf(stderr, "Could not open trace file '%s'.", 
            settings<0>::_S_trace_file_name);
    abort();
  }

  trace_vector_size_report(f);
  trace_hashtable_size_report(f);
  trace_hash_func_report(f);
  trace_vector_to_list_report(f);
  trace_map_to_unordered_map_report(f);

  fclose(f);
}

inline void set_trace_path()
{
  char* env_trace_file_name = getenv(settings<0>::_S_trace_env_var);

  if (env_trace_file_name) { 
    settings<0>::_S_trace_file_name = env_trace_file_name; 
  }

  // Make sure early that we can create the trace file.
  FILE* f = fopen(settings<0>::_S_trace_file_name, "w");
  if (f) {
    fclose(f);
  } else {
    fprintf(stderr, "Cannot create trace file at given path '%s'.",
            settings<0>::_S_trace_file_name);
    exit(1);
  }
}

inline void __profcxx_init_unconditional()
{
  stdlib_lock(&init_lock);

  if (is_invalid()) {
    if (getenv(settings<0>::_S_off_env_var)) {
      turn_off();
    } else {
      set_trace_path();
      atexit(__profcxx_report);

      set_max_stack_trace_depth();
      set_max_mem();

      // Initialize data structures for each trace class.
      trace_vector_size_init();
      trace_hashtable_size_init();
      trace_hash_func_init();
      trace_vector_to_list_init();
      trace_map_to_unordered_map_init();

      // Go live.
      turn_on();
    }
  }

  stdlib_unlock(&init_lock);
}

// This function must be called by each instrumentation point.
// The common path is inlined fully.
inline bool __profcxx_init(void)
{
  if (is_invalid()) {
    __profcxx_init_unconditional();
  }
  return is_on();
}


} // namespace __cxxprof_impl

#endif /* PROFCXX_PROFILER_TRACE_H__ */
