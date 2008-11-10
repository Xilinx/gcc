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

/** @file libprofc++/profiler_vector_to_list.cc
 *  @brief Data structures to represent profiling traces.
 */

// Written by Lixia Liu

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include "profiler_state.h"
#include "profiler_vector_to_list.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace cxxprof_runtime
{

void vector2list_info::write(FILE* f) const
{
  fprintf(f, "%Zu %Zu %Zu %.0f %.0f\n",
          _M_shift_count, _M_resize, _M_iterate, _M_cost, _M_pred_cost);
}

inline float trace_vector_to_list::vector_cost(size_t shift, 
                                               size_t iterate,
                                               size_t resize)
{
  // Cost model
  //  We assume operation cost of vector as follows.
  //   - Cost per shift: 1
  //   - Cost per access: 1
  //   - Cost per resize: 1 //Already consider the # of elements
  //  However, operation cost in list is assumed as follows:
  //   - Cost per shift: 0
  //   - Cost per accesse: 10
  //   - Cost per resize: 0
  return shift * 1 + iterate * 1 + resize * 1; 
}

inline float trace_vector_to_list::list_cost(size_t shift, 
                                             size_t iterate,
                                             size_t resize)
{
  // Cost model
  //  We assume operation cost of vector as follows.
  //   - Cost per shift: 1
  //   - Cost per access: 1
  //   - Cost per resize: 1 //Already consider the # of elements
  //  However, operation cost in list is assumed as follows:
  //   - Cost per shift: 0
  //   - Cost per accesse: 10
  //   - Cost per resize: 0
  return shift * 0 + iterate * 10 + resize * 0; 
}

void trace_vector_to_list::destruct(const void* __obj)
{
  if (!is_on())
    return;

 vector2list_info* res = get_object_info(__obj);
  if (!res)
    return;

  float vc = vector_cost(res->shift_count(), res->iterate(), res->resize());
  float lc = list_cost(res->shift_count(), res->iterate(), res->resize());
  res->set_cost(vc);
  res->set_pred_cost(lc);

  retire_object(__obj);
}

void trace_vector_to_list::opr_insert(const void* __obj, size_t __pos, 
                                      size_t __num)
{
  vector2list_info* res = get_object_info(__obj);
  if (res)
    res->opr_insert(__pos, __num);
}

void trace_vector_to_list::opr_iterate(const void* __obj, size_t __num)
{
  vector2list_info* res = get_object_info(__obj);
  if (res)
    res->opr_iterate(__num);
}

void trace_vector_to_list::invalid_operator(const void* __obj)
{
  vector2list_info* res = get_object_info(__obj);
  if (res)
    res->set_invalid();
}

void trace_vector_to_list::resize(const void* __obj, size_t __from, 
                                  size_t __to)
{
  vector2list_info* res = get_object_info(__obj);
  if (res)
    res->resize(__from, __to);
}

//////////////////////////////////////////////////////////////////////////////
// Initialization and report.
//////////////////////////////////////////////////////////////////////////////

static trace_vector_to_list* _S_vector_to_list = NULL;

void trace_vector_to_list_init() {
  _S_vector_to_list = new trace_vector_to_list();
}

void trace_vector_to_list_report(FILE* f) {
  if (_S_vector_to_list) {
    _S_vector_to_list->write(f);
    delete _S_vector_to_list;
    _S_vector_to_list = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Implementations of instrumentation hooks.
//////////////////////////////////////////////////////////////////////////////

void trace_vector_to_list_construct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _S_vector_to_list->insert(__obj, get_stack());
}

void trace_vector_to_list_destruct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _S_vector_to_list->destruct(__obj);
}


void trace_vector_to_list_insert(const void* __obj, 
                                 size_t __pos,
                                 size_t __num)
{
  if (!__profcxx_init()) return;

  _S_vector_to_list->opr_insert(__obj, __pos, __num);
}


void trace_vector_to_list_iterate(const void* __obj, size_t __num)
{
  if (!__profcxx_init()) return;

  _S_vector_to_list->opr_iterate(__obj, __num);
}

void trace_vector_to_list_invalid_operator(const void* __obj)
{
  if (!__profcxx_init()) return;

  _S_vector_to_list->invalid_operator(__obj);
}

void trace_vector_to_list_resize(const void* __obj, 
                                 size_t __from, 
                                 size_t __to)
{
  if (!__profcxx_init()) return;

  _S_vector_to_list->resize(__obj, __from, __to);
}

} // namespace cxxprof_runtime
