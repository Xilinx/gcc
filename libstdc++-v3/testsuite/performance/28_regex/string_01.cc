// { dg-options "-std=c++0x" }

//
// 2012-07-08  Sergey S. Lega <legabox@gmail.com>
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

// It's a performance test. Tests std::regex_match and time of
// compilation of basic_regex.

#include <time.h>
#include <stdio.h>

#include <regex>
#include <string>

int main () {
  std::string regex("a{1,3}[m-n][^b]*uasbf[i-l]bc{4,7}a{1,3}[m-n][^b]*uasbf[i-l]bc{4,7}m");
  std::regex re_for_match(regex, std::regex::basic);
  std::string target ("nidnvnbuindndihbniudnidnvnbuindndihbniudnidnvnbuindndihbniud");
  std::smatch m;

  time_t t = -time(NULL);

  for (int i = 0; i < 1000000; i++)
  {
    std::regex re(regex, std::regex::basic); 
    regex = i % 10000 ? regex: regex + "a";
  }

  t += time(NULL);
  printf ("time of compilation = %d\n", t);

  t = -time(NULL);
  // Checks match performance.
  for (int i = 0; i < 1000000; i++)
  {
    std::regex_match(target, m, re_for_match);
  }

  t += time(NULL);

  // Added for not eliminate std::regex_match call as a dead code.
  if (m.prefix().first == target.begin())
    printf ("if\n");
  else
    printf ("else");

  printf ("time of execution of regex_match= %d\n", t);

  return 0;
}
