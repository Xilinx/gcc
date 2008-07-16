// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>

int main() {
  int i = 1, j = 2;
  [&i, &j] () -> void { i = 3; j = 4; } ();
  assert(i == 3);
  assert(j == 4);

  return 0;
}

