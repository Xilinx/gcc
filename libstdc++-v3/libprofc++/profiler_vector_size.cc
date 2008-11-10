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

/** @file libprofc++/profiler_vector_size.cc
 *  @brief Collection of vector size traces.
 */

// Written by Lixia Liu

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include "profiler_state.h"
#include "profiler_container_size.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace cxxprof_runtime
{

class trace_vector_size : public trace_container_size
{
 public:
  trace_vector_size() : trace_container_size() { id = "vector-size"; }
};

//////////////////////////////////////////////////////////////////////////////
// Initialization and report.
//////////////////////////////////////////////////////////////////////////////

static trace_vector_size* _S_vector_size = NULL;

void trace_vector_size_init() {
  _S_vector_size = new trace_vector_size();
}

void trace_vector_size_report(FILE* f) {
  if (_S_vector_size) {
    _S_vector_size->write(f);
    delete _S_vector_size;
    _S_vector_size = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Implementations of instrumentation hooks.
//////////////////////////////////////////////////////////////////////////////

void trace_vector_size_construct(const void* __obj, size_t __num)
{
  if (!__profcxx_init()) return;

  _S_vector_size->insert(__obj, get_stack(), __num);
}

void trace_vector_size_destruct(const void* __obj, size_t __num,
                                size_t __inum)
{
  if (!__profcxx_init()) return;

  _S_vector_size->destruct(__obj, __num, __inum);
}

void trace_vector_size_resize(const void* __obj, size_t __from,
                              size_t __to)
{
  if (!__profcxx_init()) return;

  _S_vector_size->resize(__obj, __from, __to);
}

} // namespace cxxprof_runtime
