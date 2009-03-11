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

// Written by Silvius Rus <silvius.rus@gmail.com>

#ifndef PROFCXX_PROFILER_MAP_TO_UNORDERED_MAP_H__
#define PROFCXX_PROFILER_MAP_TO_UNORDERED_MAP_H__ 1

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace cxxprof_runtime
{

// Cost model. XXX: this must be taken from the machine model instead.
//  Map operations:
//   - insert: 1.5 * log(size)
//   - erase: 1.5 * log(size)
//   - find: log(size)
//   - iterate: 2.3
//  Unordered map operations:
//   - insert: 12
//   - erase: 12
//   - find: 10
//   - iterate: 1.7

const float map_insert_cost_factor = 1.5;
const float map_erase_cost_factor = 1.5;
const float map_find_cost_factor = 1;
const float map_iterate_cost = 2.3;

const float umap_insert_cost = 12.0;
const float umap_erase_cost = 12.0;
const float umap_find_cost = 10.0;
const float umap_iterate_cost = 1.7;

inline int log2(size_t size)
{
  for (int bit_count = sizeof(size_t) - 1; bit_count >= 0; --bit_count) {
    if ((2 << bit_count) & size) {
      return bit_count;
    }
  }
  return 0;
}

inline float map_insert_cost(size_t size)
{
  return map_insert_cost_factor * static_cast<float>(log2(size));
}

inline float map_erase_cost(size_t size)
{
  return map_erase_cost_factor * static_cast<float>(log2(size));
}

inline float map_find_cost(size_t size)
{
  return map_find_cost_factor * static_cast<float>(log2(size));
}

// Class for vector to list 
class map2umap_info: public object_info_base
{
 public:
  map2umap_info()
      : _M_insert(0), _M_erase(0), _M_find(0), _M_iterate(0),
        _M_map_cost(0.0), _M_umap_cost(0.0), _M_valid(true) {}
  map2umap_info(stack_t __stack)
      : object_info_base(__stack), _M_insert(0), _M_erase(0), _M_find(0), 
        _M_iterate(0), _M_map_cost(0.0), _M_umap_cost(0.0), _M_valid(true) {} 
  virtual ~map2umap_info() {}
  map2umap_info(const map2umap_info& o);
  void merge(const map2umap_info& o);
  void write(FILE* f) const;

  void record_insert(size_t size, size_t count);
  void record_erase(size_t size, size_t count);
  void record_find(size_t size);
  void record_iterate(size_t count);
  void record_invalidate();

private:
  size_t _M_insert;
  size_t _M_erase;
  size_t _M_find;
  size_t _M_iterate;
  float _M_umap_cost;
  float _M_map_cost;
  bool  _M_valid;
};

inline map2umap_info::map2umap_info(const map2umap_info& o)
    : object_info_base(o), 
      _M_insert(o._M_insert),
      _M_erase(o._M_erase),
      _M_find(o._M_find),
      _M_iterate(o._M_iterate),
      _M_map_cost(o._M_map_cost),
      _M_umap_cost(o._M_umap_cost),
      _M_valid(o._M_valid)
{}

inline void map2umap_info::merge(const map2umap_info& o)
{
  _M_insert    += o._M_insert;
  _M_erase     += o._M_erase;
  _M_find      += o._M_find;
  _M_map_cost  += o._M_map_cost;
  _M_umap_cost += o._M_umap_cost;
  _M_valid     &= o._M_valid;
}

inline void map2umap_info:: record_insert(size_t size, size_t count)
{
  _M_insert += count;
  _M_map_cost += count * map_insert_cost(size);
  _M_umap_cost += count * umap_insert_cost;
}

inline void map2umap_info:: record_erase(size_t size, size_t count)
{
  _M_erase += count;
  _M_map_cost += count * map_erase_cost(size);
  _M_umap_cost += count * umap_erase_cost;
}

inline void map2umap_info:: record_find(size_t size)
{
  _M_find += 1;
  _M_map_cost += map_find_cost(size);
  _M_umap_cost += umap_find_cost;
}

inline void map2umap_info:: record_iterate(size_t count)
{
  _M_iterate += count;
  _M_map_cost += count * map_iterate_cost;
  _M_umap_cost += count * umap_iterate_cost;
}

inline void map2umap_info:: record_invalidate()
{
  _M_valid = false;
}

class map2umap_stack_info: public map2umap_info {
 public:
  map2umap_stack_info(const map2umap_info& o) : map2umap_info(o) {}
};

class trace_map2umap
    : public trace_base<map2umap_info, map2umap_stack_info> 
{
 public:
  trace_map2umap();
};

inline trace_map2umap::trace_map2umap()
    : trace_base<map2umap_info, map2umap_stack_info>()
{
  id = "map-to-unordered-map";
}

} // namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_MAP_TO_UNORDERED_MAP_H__ */
