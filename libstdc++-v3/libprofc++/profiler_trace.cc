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

/** @file libprofc++/profiler_trace.cc
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_state.h"
#include "profiler_trace.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cerrno>

namespace cxxprof_runtime
{

// Trace file name.  Default to "profile-stdlib.txt".
static char _S_trace_default_file_name[] = "./profile-stdlib.txt";
static char _S_trace_env_var[] = "GLIBCXX_PROFILE_TRACE";
static char* _S_trace_file_name = _S_trace_default_file_name;

// Environment variable to turn everything off.
static char _S_off_env_var[] = "GLIBCXX_PROFILE_OFF";

// Environment variable to set maximum stack depth.
static char _S_max_stack_depth_env_var[] = "GLIBCXX_PROFILE_MAX_STACK_DEPTH";
size_t _S_max_stack_depth = 32;

// Space budget for each object table.
static char _S_max_mem_env_var[] = "GLIBCXX_PROFILE_MEM_PER_DIAGNOSTIC";
size_t _S_max_mem = 2 << 27;  // 128 MB.

// Individual diagnostic init and report/destroy methods.
void trace_vector_size_init();
void trace_hashtable_size_init();
void trace_hash_func_init();
void trace_vector_to_list_init();
void trace_vector_size_report(FILE* f);
void trace_hashtable_size_report(FILE* f);
void trace_hash_func_report(FILE* f);
void trace_vector_to_list_report(FILE* f);

static size_t env_to_size_t(const char* env_var, size_t default_value)
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

static void set_max_stack_trace_depth()
{
  _S_max_stack_depth = env_to_size_t(_S_max_stack_depth_env_var, 
                                     _S_max_stack_depth);
}

static void set_max_mem()
{
  _S_max_mem = env_to_size_t(_S_max_mem_env_var, _S_max_mem);
}

// Final report, meant to be registered by "atexit".
static void __profcxx_report(void)
{
  turn_off();

  FILE* f = fopen(_S_trace_file_name, "a");

  if (!f) {
    fprintf(stderr, "Could not open trace file '%s'.", _S_trace_file_name);
    abort();
  }

  trace_vector_size_report(f);
  trace_hashtable_size_report(f);
  trace_hash_func_report(f);
  trace_vector_to_list_report(f);

  fclose(f);
}

static void set_trace_path() {
  char* env_trace_file_name = getenv(_S_trace_env_var);

  if (env_trace_file_name) { 
    _S_trace_file_name = env_trace_file_name; 
  }

  // Make sure early that we can create the trace file.
  FILE* f = fopen(_S_trace_file_name, "w");
  if (f) {
    fclose(f);
  } else {
    fprintf(stderr, "Cannot create trace file at given path '%s'.",
            _S_trace_file_name);
    exit(1);
  }
}

void __profcxx_init_unconditional()
{
  stdlib_lock(&init_lock);

  if (is_invalid()) {
    if (getenv(_S_off_env_var)) {
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

      // Go live.
      turn_on();
    }
  }

  stdlib_unlock(&init_lock);
}

} // namespace cxxprof_runtime
