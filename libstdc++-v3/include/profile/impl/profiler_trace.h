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
#define _GLIBCXX_IMPL_UNORDERED_MAP std::_GLIBCXX_STD_PR::unordered_map
#include <unordered_map>
#else
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tr1/unordered_map>
#define _GLIBCXX_IMPL_UNORDERED_MAP std::tr1::unordered_map
#endif
#if defined _GLIBCXX_PROFILE_THREADS && defined HAVE_TLS
#include <pthread.h>
#endif
#include "profile/impl/profiler_state.h"
#include "profile/impl/profiler_node.h"

namespace __cxxprof_impl
{

#if defined _GLIBCXX_PROFILE_THREADS && defined HAVE_TLS
#define _GLIBCXX_IMPL_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
typedef pthread_mutex_t __mutex_t;
template <int __Unused=0>
class __mutex {
 public:
  static __mutex_t __init_lock;
  static void __lock(__mutex_t& __m) { pthread_mutex_lock(&__m); }
  static void __unlock(__mutex_t& __m) { pthread_mutex_unlock(&__m); }
};
#else
#define _GLIBCXX_IMPL_MUTEX_INITIALIZER 0
typedef int __mutex_t;
template <int __Unused=0>
class __mutex {
 public:
  static __mutex_t __init_lock;
  static void __lock(__mutex_t& __m) {}
  static void __unlock(__mutex_t& __m) {}
};
#endif

template <int __Unused>
__mutex_t __mutex<__Unused>::__init_lock = _GLIBCXX_IMPL_MUTEX_INITIALIZER;

// Defined in profiler_<diagnostic name>.h.
class __trace_hash_func;
class __trace_hashtable_size;
class __trace_map2umap;
class __trace_vector_size;
class __trace_vector_to_list;
void __trace_vector_size_init();
void __trace_hashtable_size_init();
void __trace_hash_func_init();
void __trace_vector_to_list_init();
void __trace_map_to_unordered_map_init();
void __trace_vector_size_report(FILE* f);
void __trace_hashtable_size_report(FILE* f);
void __trace_hash_func_report(FILE* f);
void __trace_vector_to_list_report(FILE* f);
void __trace_map_to_unordered_map_report(FILE* f);

// Utility functions.
inline size_t __max(size_t __a, size_t __b) {
  return __a >= __b ? __a : __b;
}

inline size_t __min(size_t __a, size_t __b) {
  return __a <= __b ? __a : __b;
}

// Diagnostic tables.
template <int __Unused=0>
class __tables {
 public:
  static __trace_hash_func* _S_hash_func;
  static __trace_hashtable_size* _S_hashtable_size;
  static __trace_map2umap* _S_map2umap;
  static __trace_vector_size* _S_vector_size;
  static __trace_vector_to_list* _S_vector_to_list;
};

template <int __Unused>
__trace_hash_func* __tables<__Unused>::_S_hash_func = NULL;
template <int __Unused>
__trace_hashtable_size* __tables<__Unused>::_S_hashtable_size = NULL;
template <int __Unused>
__trace_map2umap* __tables<__Unused>::_S_map2umap = NULL;
template <int __Unused>
__trace_vector_size* __tables<__Unused>::_S_vector_size = NULL;
template <int __Unused>
__trace_vector_to_list* __tables<__Unused>::_S_vector_to_list = NULL;

// Settings.
template <int __Unused=0>
class __settings {
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

template <int __Unused>
char __settings<__Unused>::_S_trace_default_file_name[] = 
    "./profile-stdlib.txt";
template <int __Unused>
char __settings<__Unused>::_S_trace_env_var[] = 
    "GLIBCXX_PROFILE_TRACE";
template <int __Unused>
char* __settings<__Unused>::_S_trace_file_name = 
    "./profile-stdlib.txt";
template <int __Unused>
char __settings<__Unused>::_S_off_env_var[] = 
    "GLIBCXX_PROFILE_OFF";
template <int __Unused>
char __settings<__Unused>::_S_max_stack_depth_env_var[] =
    "GLIBCXX_PROFILE_MAX_STACK_DEPTH";
template <int __Unused>
char __settings<__Unused>::_S_max_mem_env_var[] = 
    "GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC";
template <int __Unused>
size_t __settings<__Unused>::_S_max_stack_depth = 32;
template <int __Unused>
size_t __settings<__Unused>::_S_max_mem = 2 << 27;  // 128 MB.

inline size_t __stack_max_depth()
{
  return __settings<0>::_S_max_stack_depth;
}

inline size_t __max_mem()
{
  return __settings<0>::_S_max_mem;
}

typedef const char* __trace_id_t;
inline void __print_trace_id(FILE* __f, __trace_id_t __id)
{
  if (__id) {
    fprintf(__f, "%s", __id);
  } else {
    fprintf(stderr, "Undefined trace id.");
    abort();
  }
}

template <typename __object_info, typename __stack_info>
class __trace_base
{
 public:
  __trace_base();
  virtual ~__trace_base() {}
  
  void __add_object(__object_t object, __object_info __info);
  __object_info* __get_object_info(__object_t __object);
  void __retire_object(__object_t __object);
  void __write(FILE* f);

  void __lock_object_table();
  void __lock_stack_table();
  void __unlock_object_table();
  void __unlock_stack_table();

 private:
  __mutex_t __object_table_lock;
  __mutex_t __stack_table_lock;
  typedef _GLIBCXX_IMPL_UNORDERED_MAP<__object_t, 
                                      __object_info> __object_table_t;
  typedef _GLIBCXX_IMPL_UNORDERED_MAP<__stack_t, __stack_info, __stack_hash, 
                                      __stack_hash> __stack_table_t;
  __object_table_t __object_table;
  __stack_table_t __stack_table;
  size_t __stack_table_byte_size;

 protected:
  __trace_id_t __id;
};

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__lock_object_table()
{
  __mutex<0>::__lock(this->__object_table_lock);
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__lock_stack_table()
{
  __mutex<0>::__lock(this->__stack_table_lock);
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__unlock_object_table()
{
  __mutex<0>::__unlock(this->__object_table_lock);
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__unlock_stack_table()
{
  __mutex<0>::__unlock(this->__stack_table_lock);
}

template <typename __object_info, typename __stack_info>
__trace_base<__object_info, __stack_info>::__trace_base()
{
  // Do not pick the initial size too large, as we don't know which diagnostics
  // are more active.
  __object_table.rehash(10000);
  __stack_table.rehash(10000);
  __stack_table_byte_size = 0;
  __id = NULL;
  __object_table_lock = __stack_table_lock = _GLIBCXX_IMPL_MUTEX_INITIALIZER;
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__add_object(
    __object_t __object, __object_info __info)
{
  if (__max_mem() == 0 
      || __object_table.size() * sizeof(__object_info) <= __max_mem()) {
    __lock_object_table();
    __object_table.insert(
        typename __object_table_t::value_type(__object, __info));
    __unlock_object_table();
  }
}

template <typename __object_info, typename __stack_info>
__object_info* __trace_base<__object_info, __stack_info>::__get_object_info(
    __object_t __object)
{
  // XXX: Revisit this to see if we can decrease mutex spans.
  // Without this mutex, the object table could be rehashed during an
  // insertion on another thread, which could result in a segfault.
  __lock_object_table();
  typename __object_table_t::iterator __object_it = 
      __object_table.find(__object);
  if (__object_it == __object_table.end()){
    __unlock_object_table();
    return NULL;
  } else {
    __unlock_object_table();
    return &__object_it->second;
  }
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__retire_object(
    __object_t __object)
{
  __lock_object_table();
  __lock_stack_table();
  typename __object_table_t::iterator __object_it =
      __object_table.find(__object);
  if (__object_it != __object_table.end()){
    const __object_info& __info = __object_it->second;
    const __stack_t& __stack = __info.__stack();
    typename __stack_table_t::iterator __stack_it = 
        __stack_table.find(__stack);
    if (__stack_it == __stack_table.end()) {
      // First occurence of this call context.
      if (__max_mem() == 0 || __stack_table_byte_size < __max_mem()) {
        __stack_table_byte_size += 
            (sizeof(__instruction_address_t) * __size(__stack)
             + sizeof(__stack) + sizeof(__stack_info));
        __stack_table.insert(make_pair(__stack, __stack_info(__info)));
      }
    } else {
      // Merge object info into info summary for this call context.
      __stack_it->second.__merge(__info);
      delete __stack;
    }
    __object_table.erase(__object);
  }
  __unlock_stack_table();
  __unlock_object_table();
}

template <typename __object_info, typename __stack_info>
void __trace_base<__object_info, __stack_info>::__write(FILE* __f)
{
  typename __stack_table_t::iterator __it;

  for (__it = __stack_table.begin(); __it != __stack_table.end(); __it++) {
    if (__it->second.__is_valid()) {
      __print_trace_id(__f, __id);
      fprintf(__f, "|");
      __cxxprof_impl::__write(__f, __it->first);
      fprintf(__f, "|");
      __it->second.__write(__f);
    }
  }
}

inline size_t __env_to_size_t(const char* __env_var, size_t __default_value)
{
  char* __env_value = getenv(__env_var);
  if (__env_value) {
    long int __converted_value = strtol(__env_value, NULL, 10);
    if (errno || __converted_value < 0) {
      fprintf(stderr, "Bad value for environment variable '%s'.", __env_var);
      abort();
    } else {
      return static_cast<size_t>(__converted_value);
    }
  } else {
    return __default_value;
  }
}

inline void __set_max_stack_trace_depth()
{
  __settings<0>::_S_max_stack_depth = __env_to_size_t(
      __settings<0>::_S_max_stack_depth_env_var, 
      __settings<0>::_S_max_stack_depth);
}

inline void __set_max_mem()
{
  __settings<0>::_S_max_mem = __env_to_size_t(
      __settings<0>::_S_max_mem_env_var, 
      __settings<0>::_S_max_mem);
}

// Final report, meant to be registered by "atexit".
inline void __profcxx_report(void)
{
  __turn_off();

  FILE* __f = fopen(__settings<0>::_S_trace_file_name, "a");

  if (!__f) {
    fprintf(stderr, "Could not open trace file '%s'.", 
            __settings<0>::_S_trace_file_name);
    abort();
  }

  __trace_vector_size_report(__f);
  __trace_hashtable_size_report(__f);
  __trace_hash_func_report(__f);
  __trace_vector_to_list_report(__f);
  __trace_map_to_unordered_map_report(__f);

  fclose(__f);
}

inline void __set_trace_path()
{
  char* __env_trace_file_name = getenv(__settings<0>::_S_trace_env_var);

  if (__env_trace_file_name) { 
    __settings<0>::_S_trace_file_name = __env_trace_file_name; 
  }

  // Make sure early that we can create the trace file.
  FILE* __f = fopen(__settings<0>::_S_trace_file_name, "w");
  if (__f) {
    fclose(__f);
  } else {
    fprintf(stderr, "Cannot create trace file at given path '%s'.",
            __settings<0>::_S_trace_file_name);
    exit(1);
  }
}

inline void __profcxx_init_unconditional()
{
  __mutex<0>::__lock(__mutex<0>::__init_lock);

  if (__is_invalid()) {
    if (getenv(__settings<0>::_S_off_env_var)) {
      __turn_off();
    } else {
      __set_trace_path();
      atexit(__profcxx_report);

      __set_max_stack_trace_depth();
      __set_max_mem();

      // Initialize data structures for each trace class.
      __trace_vector_size_init();
      __trace_hashtable_size_init();
      __trace_hash_func_init();
      __trace_vector_to_list_init();
      __trace_map_to_unordered_map_init();

      // Go live.
      __turn_on();
    }
  }

  __mutex<0>::__unlock(__mutex<0>::__init_lock);
}

// This function must be called by each instrumentation point.
// The common path is inlined fully.
inline bool __profcxx_init(void)
{
  if (__is_invalid()) {
    __profcxx_init_unconditional();
  }
  return __is_on();
}

} // namespace __cxxprof_impl

#endif /* PROFCXX_PROFILER_TRACE_H__ */
