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

#include <limits>
#include <testsuite_common_types.h>

namespace __gnu_test
{
  struct constexpr_member_data
  {
    template<typename _Ttesttype>
      void
      operator()()
      {
	struct _Concept
	{
	  void __constraint()
	  { 
	    using std::float_denorm_style;
	    using std::float_round_style;
	    constexpr bool v1(_Ttesttype::is_specialized);
	    constexpr int v2(_Ttesttype::digits);
	    constexpr int v3(_Ttesttype::digits10);
	    constexpr int v4(_Ttesttype::max_digits10);
	    constexpr bool v5(_Ttesttype::is_signed);
	    constexpr bool v6(_Ttesttype::is_integer);
	    constexpr bool v7(_Ttesttype::is_exact);
	    constexpr int v8(_Ttesttype::radix);
	    constexpr int v9(_Ttesttype::min_exponent);
	    constexpr int v10(_Ttesttype::min_exponent10);
	    constexpr int v11(_Ttesttype::max_exponent);
	    constexpr int v12(_Ttesttype::max_exponent10);
	    constexpr bool v13(_Ttesttype::has_infinity);
	    constexpr bool v14(_Ttesttype::has_quiet_NaN);
	    constexpr bool v15(_Ttesttype::has_signaling_NaN);
	    constexpr float_denorm_style v16(_Ttesttype::has_denorm);
	    constexpr bool v17(_Ttesttype::has_denorm_loss);
	    constexpr bool v18(_Ttesttype::is_iec559);
	    constexpr bool v19(_Ttesttype::is_bounded);
	    constexpr bool v20(_Ttesttype::is_modulo);
	    constexpr bool v21(_Ttesttype::traps);
	    constexpr bool v22(_Ttesttype::tinyness_before);
	    constexpr float_round_style v23(_Ttesttype::round_style);
	  }
	};

	_Concept c;
	c.__constraint();
      }
  };

  struct constexpr_member_functions
  {
    template<typename _Ttesttype, typename _Tbasetype>
      void
      operator()()
      {
	struct _Concept
	{
	  void __constraint()
	  { 
	    constexpr _Tbasetype v1(_Ttesttype::min());
	    constexpr _Tbasetype v2(_Ttesttype::max());
	    constexpr _Tbasetype v3(_Ttesttype::lowest());
	    constexpr _Tbasetype v4(_Ttesttype::epsilon());
	    constexpr _Tbasetype v5(_Ttesttype::round_error());
	    constexpr _Tbasetype v6(_Ttesttype::infinity());
	    constexpr _Tbasetype v7(_Ttesttype::quiet_NaN());
	    constexpr _Tbasetype v8(_Ttesttype::signaling_NaN());
	    constexpr _Tbasetype v9(_Ttesttype::denorm_min());
	  }
	};

	_Concept c;
	c.__constraint();
      }
  };
}

int main()
{
  __gnu_test::constexpr_member_data test_data;
  __gnu_cxx::typelist::apply_generator(test_data, __gnu_test::limits_tl());

  __gnu_test::constexpr_member_functions test_member_func;
  __gnu_cxx::typelist::apply_generator(test_member_func, 
				       __gnu_test::limits_tl(),
				       __gnu_test::integral_types::type());
  return 0;
}
