// Profiling multiset implementation -*- C++ -*-

// Copyright (C) 2008
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

/** @file profile/multiset.h
 *  This file is a GNU profile extension to the Standard C++ Library.
 */

#ifndef _GLIBCXX_PROFILE_MULTISET_H
#define _GLIBCXX_PROFILE_MULTISET_H 1

#include <utility>

namespace std
{
namespace __profile
{
  template<typename _Key, typename _Compare = std::less<_Key>,
	   typename _Allocator = std::allocator<_Key> >
    class multiset
    : public _GLIBCXX_STD_PR::multiset<_Key, _Compare, _Allocator>
    {
      typedef _GLIBCXX_STD_PR::multiset<_Key, _Compare, _Allocator> _Base;
    
    public:
      explicit multiset(const _Compare& __comp = _Compare(),
            const _Allocator& __a = _Allocator())
      : _Base(__comp, __a) { }

      template<typename _InputIterator>
        multiset(_InputIterator __first, _InputIterator __last,
         const _Compare& __comp = _Compare(),
         const _Allocator& __a = _Allocator())
     : _Base(__first, __last, __comp, __a) { }

      multiset(const multiset& __x)
      : _Base(__x) { }

      multiset(const _Base& __x)
      : _Base(__x) { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      multiset(multiset&& __x)
      : _Base(std::forward<multiset>(__x))
      {}
#endif
      ~multiset() { }

      multiset&
      operator=(const multiset& __x)
      {
        *static_cast<_Base*>(this) = __x;
        return *this;
      }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
      multiset&
      operator=(multiset&& __x)
      {
        _Base::clear();
        _Base::swap(__x);
        return *this;
      }
#endif

      void
#ifdef __GXX_EXPERIMENTAL_CXX0X__
      swap(multiset&& __x)
#else
      swap(multiset& __x)
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
    operator==(const multiset<_Key, _Compare, _Allocator>& __lhs,
           const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() == __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator!=(const multiset<_Key, _Compare, _Allocator>& __lhs,
           const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() != __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<(const multiset<_Key, _Compare, _Allocator>& __lhs,
          const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() < __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator<=(const multiset<_Key, _Compare, _Allocator>& __lhs,
           const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() <= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>=(const multiset<_Key, _Compare, _Allocator>& __lhs,
           const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() >= __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    inline bool
    operator>(const multiset<_Key, _Compare, _Allocator>& __lhs,
          const multiset<_Key, _Compare, _Allocator>& __rhs)
    { return __lhs._M_base() > __rhs._M_base(); }

  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(multiset<_Key, _Compare, _Allocator>& __x,
     multiset<_Key, _Compare, _Allocator>& __y)
    { return __x.swap(__y); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(multiset<_Key, _Compare, _Allocator>&& __x,
     multiset<_Key, _Compare, _Allocator>& __y)
    { return __x.swap(__y); }

  template<typename _Key, typename _Compare, typename _Allocator>
    void
    swap(multiset<_Key, _Compare, _Allocator>& __x,
     multiset<_Key, _Compare, _Allocator>&& __y)
    { return __x.swap(__y); }
#endif

} // namespace __profile
} // namespace std

#endif
