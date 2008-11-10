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

/** @file libprofc++/profiler_hash_func.cc
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include "profiler_state.h"
#include "profiler_hash_func.h"

namespace cxxprof_runtime
{

void hashfunc_info::write(FILE* f) const
{
  fprintf(f, "%Zu %Zu %Zu\n", _M_hops, _M_accesses, _M_longest_chain);
}

void trace_hash_func::destruct(const void* __obj, size_t __chain,
                               size_t __accesses, size_t __hops)
{
  if (!is_on()) return;

  // First find the item from the live objects and update the informations.
  hashfunc_info* objs = get_object_info(__obj);
  if (!objs)
    return;

  objs->destruct(__chain, __accesses, __hops);
  printf("Retiring %p.\n", __obj);
  retire_object(__obj);
}

//////////////////////////////////////////////////////////////////////////////
// Initialization and report.
//////////////////////////////////////////////////////////////////////////////

static trace_hash_func* _S_hash_func = NULL;

void trace_hash_func_init() {
  _S_hash_func = new trace_hash_func();
}

void trace_hash_func_report(FILE* f) {
  if (_S_hash_func) {
    _S_hash_func->write(f);
    delete _S_hash_func;
    _S_hash_func = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Implementations of instrumentation hooks.
//////////////////////////////////////////////////////////////////////////////

void trace_hash_func_construct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _S_hash_func->insert(__obj, get_stack());
}

void trace_hash_func_destruct(const void* __obj, size_t __chain,
                               size_t __accesses, size_t __hops)
{
  if (!__profcxx_init()) return;

  _S_hash_func->destruct(__obj, __chain, __accesses, __hops);
}

} // namespace cxxprof_runtime
