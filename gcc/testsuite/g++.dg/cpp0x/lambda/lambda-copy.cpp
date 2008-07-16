// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>

int main() {
  int i = 1, j = 2;
  [i, j] () -> void { i = 3; j = 4; } ();
  assert(i == 1);
  assert(j == 2);

  //[] () -> void { i; } (); // { dg-error: "`i' is not in scope" }
  //[1] () -> void {} (); // { dg-error: "expected identifier" }

  return 0;
}

