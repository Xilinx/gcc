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

/** @file libprofc++/profiler_node.h
 *  @brief Data structures to represent a single profiling event.
 */

// Written by Lixia Liu

#include "profiler_node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace cxxprof_runtime
{

void stdlib_info_size::print_debug()
{
  for(stack_t::iterator it = _M_stack->begin(); it != _M_stack->end(); it++) {
    fprintf(stderr, "[%p]", *it);
  }
  fprintf(stderr, "|%lu %lu %d ", _M_init, _M_count, int(_M_cost)); 
  fprintf(stderr, "%lu %lu %lu %lu %lu %lu %lu %lu\n",
          _M_resize, _M_min, _M_max, _M_total, _M_item_min, _M_item_max, 
          _M_item_total, (stdlib_size_t)_M_object);
}

void stdlib_info_size::print(FILE* __f)
{
  // XXX: This will be replaced entirely by a binary trace IO module.
  // For now, 1024 is provably sufficient in all possible cases because
  // max trace depth is 32.
  char __str[1024];
  __str[0] = '\0';

  for (stack_t::iterator it = _M_stack->begin(); it != _M_stack->end(); it++) {
    snprintf(__str + strlen(__str), sizeof(__str), "[%p]", *it);            
  }
  assert(strlen(__str) < sizeof(__str));
  snprintf(__str + strlen(__str), sizeof(__str), "|%lu %lu %lu ", 
           _M_init, _M_count, static_cast<unsigned long>(_M_cost));
  assert(strlen(__str) < sizeof(__str));
  snprintf(__str + strlen(__str), sizeof(__str), "%lu %lu %lu %lu ", 
           _M_resize, _M_min, _M_max, _M_total);
  assert(strlen(__str) < sizeof(__str));
  snprintf(__str + strlen(__str), sizeof(__str), "%lu %lu %lu %lu\n", 
           _M_item_min, _M_item_max, _M_item_total, 
	   reinterpret_cast<stdlib_size_t>(_M_object));

  fputs(__str, __f);
}

} // namespace cxxprof_runtime
