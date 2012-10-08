// { dg-options "-std=c++0x" }

//
// 2012-06-14 Sergey Lega
//
// Copyright (C) 2012 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// 28.11.2 regex_match
// Tests BRE against a std::string target.

#include <regex>
#include <testsuite_hooks.h>

void
test01 () {
	std::regex re("[\\[]", std::regex::basic);
	std::string target_accept("[");
	std::string target_reject("f");
        std::smatch m;

	VERIFY( !std::regex_match(target_reject, m, re) );
	VERIFY( std::regex_match(target_accept, m, re) );

	VERIFY( m.size()  == re.mark_count()+1 );
	VERIFY( m.empty() == false );
	VERIFY( m.prefix().first == target_accept.begin() );
	VERIFY( m.prefix().second == target_accept.begin() );
	VERIFY( m.prefix().matched == false );
	VERIFY( m.suffix().first == target_accept.end() );
	VERIFY( m.suffix().second == target_accept.end() );
	VERIFY( m.suffix().matched == false );
	VERIFY( m[0].first == target_accept.begin() );
	VERIFY( m[0].second == target_accept.end() );
	VERIFY( m[0].matched == true );
	VERIFY( m[1].first == target_accept.begin() );
	VERIFY( m[1].second == target_accept.begin()+1 );
	VERIFY( m[1].matched == true );
}

int
main()
{ 
  test01();
  return 0;
}

