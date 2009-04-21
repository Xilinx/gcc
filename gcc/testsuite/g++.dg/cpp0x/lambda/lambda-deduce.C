// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>

int main() {
  [] {};
  [] {} ();
  [] () {};
  [] () {} ();
  [] () { return "lambda"; };

  int i = 1, j = 2;
  [&i, j] () { i = j; } ();
  assert(i == 2);
  assert(j == 2);

  i = [] () { return 3; } ();
  assert(i == 3);

  []{ return; };

  return 0;
}

