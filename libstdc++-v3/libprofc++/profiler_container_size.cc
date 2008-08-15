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

// Written by Lixia Liu

#include "profiler.h"
#include "profiler_node.h"
#include "profiler_trace.h"
#include "profiler_state.h"
#include "profiler_container_size.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace cxxprof_runtime
{

static long _S_reserve_size = -1; // 1M.

long reserve_size() 
{ 
  return _S_reserve_size;
}

void trace_container_size::destruct(void* __obj, stdlib_size_t __num, 
                                    stdlib_size_t __inum)
{
  if (!is_on()) return;

  // First find the item from the live objects and update the informations.
  ObjMap* live = &trace_info.first;
  ObjMap::iterator objs = live->find(__obj);
  StackMap* summary = &trace_info.second;

  if (objs != live->end()) {
    stdlib_info_size *res = objs->second;
    res->destruct(__obj, __num, __inum);

    // Remove the node.        
    live->erase(__obj);
    stack_t* s = res->stack();

    // Find the node in the summary.
    StackMap::iterator it = summary->find(s);
   
    // Merge the result into the first one.
    // Basically now merge data with same stack.
    if (it == summary->end()) {
      // Create new node.
      summary->insert(StackMap::value_type(s, res));
      // Turn off the profiling if having enough information.
      if (_S_reserve_size > 0 && summary->size() >= _S_reserve_size ) {
        turn_off();
      }
    } else {
      // Merge data with previous node.
      res = it->second;
      res->destruct(__obj, __num, __inum);
    }    
  }
  // Skip the case if never find constructor.
}

void trace_container_size::print() 
{
  char* trace_file_name();
  FILE* f = fopen(trace_file_name(), "a");

  StackMap summary = trace_info.second;
  StackMap::iterator it;
  for (it = summary.begin(); it != summary.end(); it++) {
    stdlib_info_size *res = it->second;
    res->print(f);
  }

  fclose(f);
}

void trace_container_size::resize(void* __obj, int __from, int __to)
{
  if (!is_on()) return;

 // First find the item from the live objects and update the informations.
  ObjMap um = trace_info.first;  
  ObjMap::iterator objs = um.find(__obj);

 if (objs != um.end()) {
    stdlib_info_size *res = objs->second;
    res->resize(__obj, __from, __to);
  } 
  // Do nothing if we don't have it registered.
}

// Free all information content of traces.
void trace_container_size::free_all()
{
  ObjMap* live = &trace_info.first;
  StackMap* summary = &trace_info.second; 
  if (live) {
    ObjMap::iterator it;
    for (it = live->begin(); it != live->end(); it++)
    {
      delete it->second;
    }
  }
  if (summary) {
    StackMap::iterator it;
    for (it = summary->begin(); it != summary->end(); it++)
    {
      delete it->second;
      delete it->first;
    }
  }
}

} // namespace cxxprof_runtime
