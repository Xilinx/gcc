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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace cxxprof_runtime
{
class trace_hashtable_size; 

//  Point to all stdlib profiling information
static trace_hashtable_size* _S_hashtable_size = NULL;

// Trace file name.  Default to "profile-stdlib.txt".
static char _S_trace_default_file_name[] = "./profile-stdlib.txt";
static char _S_trace_env_var[] = "GLIBCXX_PROFILE_TRACE";
static char* _S_trace_file_name = _S_trace_default_file_name;

// Environment variable to turn everything off.
static char _S_off_env_var[] = "GLIBCXX_PROFILE_OFF";

// Individual diagnostic init and report/destroy methods.
void trace_vector_size_init();
void trace_hashtable_size_init();
void trace_vector_size_report();
void trace_hashtable_size_report();

char* trace_file_name() {return _S_trace_file_name;}

// Final report, meant to be registered by "atexit".
static void __profcxx_report(void)
{  
  turn_off();

  trace_vector_size_report();
  trace_hashtable_size_report();
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

      // Initialize data structures for each trace class.
      trace_vector_size_init();
      trace_hashtable_size_init();

      // Go live.
      turn_on();
    }
  }

  stdlib_unlock(&init_lock);
}

} // namespace cxxprof_runtime
