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

/** @file libprofc++/profiler_state.cc
 *  @brief Global profiler state.
 */

// Written by Lixia Liu

#ifndef PROFCXX_PROFILER_STATE_H__
#define PROFCXX_PROFILER_STATE_H__ 1

#include <cassert>
#include <cstdio>

namespace cxxprof_runtime
{

class state
{
 public:
  state() : _M_state(INVALID) {}
  ~state() {}

  bool is_on() { return _M_state == ON; }
  bool is_off() { return _M_state == OFF; }
  bool is_invalid() { return _M_state == INVALID; }
  void turn_on() { _M_state = ON; }
  void turn_off() { _M_state = OFF; }

 private:
  enum state_type { ON, OFF, INVALID }; 
  state_type _M_state;
};

extern state* _S_diag_state;

inline bool is_on()
{
  return _S_diag_state && _S_diag_state->is_on();
}

inline bool is_off()
{
  return _S_diag_state && _S_diag_state->is_off();
}

inline bool is_invalid()
{
  return !_S_diag_state || _S_diag_state->is_invalid();
}

inline void turn_on()
{
  if (!_S_diag_state) { _S_diag_state = new state; }
  _S_diag_state->turn_on();
}

inline void turn_off()
{
  assert(_S_diag_state);
  _S_diag_state->turn_off();
}


} // end namespace cxxprof_runtime
#endif /* PROFCXX_PROFILER_STATE_H__ */
