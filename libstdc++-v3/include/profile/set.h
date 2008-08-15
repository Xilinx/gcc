// Profiling set implementation -*- C++ -*-

// Copyright (C) 2003, 2004, 2005, 2006, 2007
// Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/** @file profile/set.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_SET_H
#define _GLIBCXX_PROFILE_SET_H 1

#include <utility>

namespace std 
{
namespace __profile
{
  template<typename _Key, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<_Key> >
    class set
    : public _GLIBCXX_STD_PR::set<_Key,_Compare,_Allocator>
    {
      typedef _GLIBCXX_STD_PR::set<_Key, _Compare, _Allocator> _Base;

     public:
      explicit set(const _Compare& __comp = _Compare(),
           const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) { }

      template<typename _InputIterator>
        set(_InputIterator __first, _InputIterator __last,
        const _Compare& __comp = _Compare(),
        const _Allocator& __a = _Allocator())
    : _Base(__first, __last,  __comp, __a) { }

      set(const set& __x)
      : _Base(__x) { }

      set(const _Base& __x)
      : _Base(__x) { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      set(set&& __x)
      : _Base(std::forward<set>(__x))
      { }
#endif
      ~set() { }

      set&
      operator=(const set& __x)
      {
        *static_cast<_Base*>(this) = __x;
        return *this;
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      set&
      operator=(set&& __x)
      {
        _Base::clear();
        _Base::swap(__x);
        return *this;
      }
#endif

      void
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      swap(set&& __x)
#else
      swap(set& __x)
#endif
      {
        _Base::swap(__x);
      }
 
      _Base&
      _M_base()       { return *this; }

      const _Base&
      _M_base() const { return *this; }
    };

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<(const set<_Key, _Compare, _Allocator>& __lhs,
          const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() < __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<=(const set<_Key, _Compare, _Allocator>& __lhs,
           const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() <= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>=(const set<_Key, _Compare, _Allocator>& __lhs,
           const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() >= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>(const set<_Key, _Compare, _Allocator>& __lhs,
          const set<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() > __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(set<_Key, _Compare, _Allocator>& __x,
     set<_Key, _Compare, _Allocator>& __y)
    { return __x.swap(__y); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(set<_Key, _Compare, _Allocator>&& __x,
     set<_Key, _Compare, _Allocator>& __y)
    { return __x.swap(__y); }

  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(set<_Key, _Compare, _Allocator>& __x,
     set<_Key, _Compare, _Allocator>&& __y)
    { return __x.swap(__y); }
#endif

} // namespace __profile
} // namespace std

#endif
