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

/** @file profile/impl/profiler.h
 *  @brief Interface of the profiling runtime library.
 */

// Written by Lixia Liu

#ifndef PROFCXX_PROFILER_H__
#define PROFCXX_PROFILER_H__ 1

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <cstddef>
#else
#include <stddef.h>
#endif

// Forward declarations of instrumentation hooks.
namespace __cxxprof_impl
{

// Reentrance guard.

template <int Unused=0>
class Guard {
 public:
  static __thread bool inside_cxxprof_impl;
};

template <int Unused>
__thread bool Guard<Unused>::inside_cxxprof_impl = false;

inline bool get_in() {
  if (Guard<0>::inside_cxxprof_impl)
    return false;
  Guard<0>::inside_cxxprof_impl = true;
  return true;
}

inline void get_out() {
  Guard<0>::inside_cxxprof_impl = false;
}

// XXX: All hook calls must be protected by a reentrance guard.
#define GUARD(x...)                             \
  {                                             \
    if (__cxxprof_impl::get_in()) {             \
      x;                                        \
      __cxxprof_impl::get_out();                \
    }                                           \
}

// State management.
void turn_on();
void turn_off();
bool is_invalid();
bool is_on();
bool is_off();

// Instrumentation hooks.  Do not use them directly in instrumented headers.
// Instead use the corresponding __profcxx_ wrapper declared below.
void trace_hashtable_size_resize(const void*, size_t, size_t);
void trace_hashtable_size_destruct(const void*, size_t, size_t);
void trace_hashtable_size_construct(const void*, size_t);
void trace_vector_size_resize(const void*, size_t, size_t);
void trace_vector_size_destruct(const void*, size_t, size_t);
void trace_vector_size_construct(const void*, size_t);
void trace_hash_func_destruct(const void*, size_t, size_t, size_t);
void trace_hash_func_construct(const void*);
void trace_vector_to_list_destruct(const void*);
void trace_vector_to_list_construct(const void*);
void trace_vector_to_list_insert(const void*, size_t, size_t);
void trace_vector_to_list_iterate(const void*, size_t);
void trace_vector_to_list_invalid_operator(const void*);
void trace_vector_to_list_resize(const void*, size_t, size_t);
void trace_map_to_unordered_map_construct(const void*);
void trace_map_to_unordered_map_invalidate(const void*);
void trace_map_to_unordered_map_insert(const void*, size_t, size_t);
void trace_map_to_unordered_map_erase(const void*, size_t, size_t);
void trace_map_to_unordered_map_iterate(const void*, size_t);
void trace_map_to_unordered_map_find(const void*, size_t);
void trace_map_to_unordered_map_destruct(const void*);
} // namespace __cxxprof_impl

// Master switch turns on all diagnostics.
#ifdef _GLIBCXX_PROFILE
#define _GLIBCXX_PROFILE_HASHTABLE_TOO_SMALL
#define _GLIBCXX_PROFILE_HASHTABLE_TOO_LARGE
#define _GLIBCXX_PROFILE_VECTOR_TOO_SMALL
#define _GLIBCXX_PROFILE_VECTOR_TOO_LARGE
#define _GLIBCXX_PROFILE_INEFFICIENT_HASH
#define _GLIBCXX_PROFILE_VECTOR_TO_LIST
#define _GLIBCXX_PROFILE_MAP_TO_UNORDERED_MAP
#endif

// Turn on/off instrumentation for HASHTABLE_TOO_SMALL and HASHTABLE_TOO_LARGE.
#if ((defined(_GLIBCXX_PROFILE_HASHTABLE_TOO_SMALL) \
      && !defined(_NO_GLIBCXX_PROFILE_HASHTABLE_TOO_SMALL)) \
     || (defined(_GLIBCXX_PROFILE_HASHTABLE_TOO_LARGE) \
         && !defined(_NO_GLIBCXX_PROFILE_HASHTABLE_TOO_LARGE)))
#define __profcxx_hashtable_resize(x...) \
  GUARD(__cxxprof_impl::trace_hashtable_size_resize(x))
#define __profcxx_hashtable_destruct(x...) \
  GUARD(__cxxprof_impl::trace_hashtable_size_destruct(x))
#define __profcxx_hashtable_construct(x...) \
  GUARD(__cxxprof_impl::trace_hashtable_size_construct(x))
#else
#define __profcxx_hashtable_resize(x...)  
#define __profcxx_hashtable_destruct(x...) 
#define __profcxx_hashtable_construct(x...)  
#endif

// Turn on/off instrumentation for VECTOR_TOO_SMALL and VECTOR_TOO_LARGE.
#if ((defined(_GLIBCXX_PROFILE_VECTOR_TOO_SMALL) \
      && !defined(_NO_GLIBCXX_PROFILE_VECTOR_TOO_SMALL)) \
     || (defined(_GLIBCXX_PROFILE_VECTOR_TOO_LARGE) \
         && !defined(_NO_GLIBCXX_PROFILE_VECTOR_TOO_LARGE)))
#define __profcxx_vector_resize(x...) \
  GUARD(__cxxprof_impl::trace_vector_size_resize(x))
#define __profcxx_vector_destruct(x...) \
  GUARD(__cxxprof_impl::trace_vector_size_destruct(x))
#define __profcxx_vector_construct(x...) \
  GUARD(__cxxprof_impl::trace_vector_size_construct(x))
#else
#define __profcxx_vector_resize(x...)  
#define __profcxx_vector_destruct(x...) 
#define __profcxx_vector_construct(x...)  
#endif 

// Turn on/off instrumentation for INEFFICIENT_HASH.
#if (defined(_GLIBCXX_PROFILE_INEFFICIENT_HASH) \
     && !defined(_NO_GLIBCXX_PROFILE_INEFFICIENT_HASH))
#define __profcxx_hashtable_construct2(x...) \
  GUARD(__cxxprof_impl::trace_hash_func_construct(x))
#define __profcxx_hashtable_destruct2(x...) \
  GUARD(__cxxprof_impl::trace_hash_func_destruct(x))
#else
#define __profcxx_hashtable_destruct2(x...) 
#define __profcxx_hashtable_construct2(x...)  
#endif

// Turn on/off instrumentation for VECTOR_TO_LIST.
#if (defined(_GLIBCXX_PROFILE_VECTOR_TO_LIST) \
     && !defined(_NO_GLIBCXX_PROFILE_VECTOR_TO_LIST))
#define __profcxx_vector_construct2(x...) \
  GUARD(__cxxprof_impl::trace_vector_to_list_construct(x))
#define __profcxx_vector_destruct2(x...) \
  GUARD(__cxxprof_impl::trace_vector_to_list_destruct(x))
#define __profcxx_vector_insert(x...) \
  GUARD(__cxxprof_impl::trace_vector_to_list_insert(x))
#define __profcxx_vector_iterate(x...) \
  GUARD(__cxxprof_impl::trace_vector_to_list_iterate(x))
#define __profcxx_vector_invalid_operator(x...) \
  GUARD(__cxxprof_impl::trace_vector_to_list_invalid_operator(x))
#define __profcxx_vector_resize2(x...) \
  GUARD(__cxxprof_impl::trace_vector_to_list_resize(x))
#else
#define __profcxx_vector_destruct2(x...)
#define __profcxx_vector_construct2(x...)
#define __profcxx_vector_insert(x...)
#define __profcxx_vector_iterate(x...)
#define __profcxx_vector_invalid_operator(x...)
#define __profcxx_vector_resize2(x...)
#endif

// Turn on/off instrumentation for MAP_TO_UNORDERED_MAP.
#if (defined(_GLIBCXX_PROFILE_MAP_TO_UNORDERED_MAP) \
     && !defined(_NO_GLIBCXX_PROFILE_MAP_TO_UNORDERED_MAP))
#define __profcxx_map_to_unordered_map_construct(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_construct(x))
#define __profcxx_map_to_unordered_map_destruct(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_destruct(x))
#define __profcxx_map_to_unordered_map_insert(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_insert(x))
#define __profcxx_map_to_unordered_map_erase(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_erase(x))
#define __profcxx_map_to_unordered_map_iterate(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_iterate(x))
#define __profcxx_map_to_unordered_map_invalidate(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_invalidate(x))
#define __profcxx_map_to_unordered_map_find(x...) \
  GUARD(__cxxprof_impl::trace_map_to_unordered_map_find(x))
#else
#define __profcxx_map_to_unordered_map_construct(x...) \
  
#define __profcxx_map_to_unordered_map_destruct(x...)
#define __profcxx_map_to_unordered_map_insert(x...)
#define __profcxx_map_to_unordered_map_erase(x...)
#define __profcxx_map_to_unordered_map_iterate(x...)
#define __profcxx_map_to_unordered_map_invalidate(x...)
#define __profcxx_map_to_unordered_map_find(x...)
#endif

// Run multithreaded unless instructed not to do so.
#ifndef _GLIBCXX_PROFILE_NOTHREADS
#define _GLIBCXX_PROFILE_THREADS
#endif

// Instrumentation hook implementations.
// XXX: Don't move to top of file.
#include "profile/impl/profiler_hash_func.h"
#include "profile/impl/profiler_hashtable_size.h"
#include "profile/impl/profiler_map_to_unordered_map.h"
#include "profile/impl/profiler_vector_size.h"
#include "profile/impl/profiler_vector_to_list.h"

#endif // PROFCXX_PROFILER_H__
