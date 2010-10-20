// { dg-do compile }
// { dg-options "-std=gnu++0x" }

// Copyright (C) 2010 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <complex>
#include <testsuite_common_types.h>

int main()
{
  __gnu_test::constexpr_default_constructible test1;
#if 0
  test1.operator()<std::complex<float>>();
  test1.operator()<std::complex<double>>();
  test1.operator()<std::complex<long double>>();
#endif

  __gnu_test::constexpr_single_value_constructible test2;
#if 0
  test2.operator()<std::complex<float>, float>();
  test2.operator()<std::complex<double>, double>();
  test2.operator()<std::complex<long double>, long double>();
#endif

  return 0;
}
