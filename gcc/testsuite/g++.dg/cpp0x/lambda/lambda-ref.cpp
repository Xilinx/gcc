// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>

template<typename F>
void call(F f) { f(); }

int main() {
  int i = 1, j = 2;
  call([&i, &j] () -> void { i = 3; j = 4; });
  assert(i == 3);
  assert(j == 4);

  return 0;
}

