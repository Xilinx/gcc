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

// Written by Silvius Rus <silvius.rus@gmail.com>

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include "profiler_state.h"
#include "profiler_map_to_unordered_map.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace cxxprof_runtime
{

void map2umap_info::write(FILE* f) const
{
  fprintf(f, "%Zu %Zu %Zu %Zu %.0f %.0f %s\n",
          _M_insert, _M_erase, _M_find, _M_iterate, _M_map_cost, _M_umap_cost,
          _M_valid ? "valid" : "invalid");
}


//////////////////////////////////////////////////////////////////////////////
// Initialization and report.
//////////////////////////////////////////////////////////////////////////////

static trace_map2umap* _S_map2umap = NULL;

void trace_map_to_unordered_map_init() {
  _S_map2umap = new trace_map2umap();
}

void trace_map_to_unordered_map_report(FILE* f) {
  if (_S_map2umap) {
    _S_map2umap->write(f);
    delete _S_map2umap;
    _S_map2umap = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Implementations of instrumentation hooks.
//////////////////////////////////////////////////////////////////////////////

void trace_map_to_unordered_map_construct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _S_map2umap->add_object(__obj, map2umap_info(get_stack()));
}

void trace_map_to_unordered_map_destruct(const void* __obj)
{
  if (!__profcxx_init()) return;

  _S_map2umap->retire_object(__obj);
}

void trace_map_to_unordered_map_insert(const void* obj, size_t size,
                                       size_t count)
{
  if (!__profcxx_init()) return;

  map2umap_info* info = _S_map2umap->get_object_info(obj);

  if (info) info->record_insert(size, count);
}

void trace_map_to_unordered_map_erase(const void* obj, size_t size,
                                       size_t count)
{
  if (!__profcxx_init()) return;

  map2umap_info* info = _S_map2umap->get_object_info(obj);

  if (info) info->record_erase(size, count);
}

void trace_map_to_unordered_map_find(const void* obj, size_t size)
{
  if (!__profcxx_init()) return;

  map2umap_info* info = _S_map2umap->get_object_info(obj);

  if (info) info->record_find(size);
}

void trace_map_to_unordered_map_iterate(const void* obj, size_t count)
{
  if (!__profcxx_init()) return;

  map2umap_info* info = _S_map2umap->get_object_info(obj);

  if (info) info->record_iterate(count);
}

void trace_map_to_unordered_map_invalidate(const void* obj)
{
  if (!__profcxx_init()) return;

  map2umap_info* info = _S_map2umap->get_object_info(obj);

  if (info) info->record_invalidate();
}

} // namespace cxxprof_runtime
