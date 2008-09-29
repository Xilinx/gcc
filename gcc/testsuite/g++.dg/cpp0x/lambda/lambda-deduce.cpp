// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>

int main() {
  [] {};
  [] {} ();
  [] () {};
  [] () {} ();

  int i = 1, j = 2;
  [&i, j] () { return i = j; } ();
  assert(i == 2);
  assert(j == 2);

  return 0;
}

