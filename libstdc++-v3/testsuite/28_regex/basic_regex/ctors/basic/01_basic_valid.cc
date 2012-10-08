// { dg-options "-std=c++0x" }

//
// Copyright (C) 2010 Free Software Foundation, Inc.
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

#include <regex>
#include <testsuite_hooks.h>

// Test on a compilation of simple expressions.
void 
simple_string_test()
{
  std::regex re0("Simple string", std::regex::basic);
  std::regex re1("String[a-z] string", std::regex::basic);
  std::regex re2("String[a-zA-Z-]", std::regex::basic);
  std::regex re3("\\[a-z\\]", std::regex::basic);
  std::regex re4("(\\[\\[a-z\\]\\])*", std::regex::basic);
  std::regex re5("a(([a-z]))", std::regex::basic);
  std::regex re6("(),))(,)(((", std::regex::basic);
  std::regex re7("\\[\\[\\[\\[\\]", std::regex::basic);
  std::regex re8("", std::regex::basic);
  std::regex re9("(ss)s{s,s}\\[dd\\]", std::regex::basic);
  std::regex re10("((ss}}\\a\\s", std::regex::basic);
}

// The test for the compilation of expressions with brackets
void 
bracket_expressions_test()
{
  std::regex re0("[]", std::regex::basic);
  std::regex re1("[a-z]", std::regex::basic);
  std::regex re2("[az-]", std::regex::basic);
  std::regex re3("[-za]", std::regex::basic);
  std::regex re4("[a-zA-Z]", std::regex::basic);
  std::regex re5("[a-z-A-Z]", std::regex::basic);
  std::regex re6("[\\]a-z]", std::regex::basic);
  std::regex re7("[a\\-z\\]]", std::regex::basic);
  std::regex re8("[-]", std::regex::basic);
  std::regex re9("[a-]", std::regex::basic);
  std::regex re10("[-a]", std::regex::basic);
  std::regex re11("[abce{asdf}a-h\\]\\[]", std::regex::basic);
}

void
interval_expressions_test() 
{
  std::regex re0("a\\{,4\\}", std::regex::basic);
  std::regex re1("a\\{2,4\\}", std::regex::basic);
  std::regex re2("a\\{2,\\}", std::regex::basic);
  std::regex re3("a*", std::regex::basic);
  std::regex re4("a+", std::regex::basic);
  std::regex re5("a?", std::regex::basic);
  std::regex re6("a?b?c+", std::regex::basic);
  std::regex re7("\\++\\?+\\??\\+?", std::regex::basic);
}

int
main()
{
  simple_string_test();
  bracket_expressions_test();
  interval_expressions_test();
  return 0;
}
