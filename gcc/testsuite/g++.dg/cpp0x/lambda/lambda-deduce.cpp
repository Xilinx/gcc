// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>

int main() {
  int i = 1, j = 2;
  [i, j] () (i = 3, j = 4) ();
  assert(i == 1);
  assert(j == 2);
  [&i, &j] () (i = 5, j = 6) ();
  assert(i == 5);
  assert(j == 6);

  return 0;
}

