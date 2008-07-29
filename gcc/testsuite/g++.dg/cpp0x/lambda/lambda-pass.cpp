// { dg-do "compile" }
// { dg-options "-std=c++0x"}
#include <cassert>
#include <vector>
#include <algorithm>

template<typename F>
void call(F f) { f(0); }

int main() {
  call([] (int x) -> void {});
  //call([] () -> void mutable {}); // { dg-error "`f' does not have const `operator()'" }

  int i = -1;
  call([&i] (int x) -> void { i = x; });
  assert(i == 0);
  //call([i] () -> void { i = 0; }); // { dg-error: "assignment to non-reference capture in const lambda" }

  std::vector<int*> pointers;
  std::for_each(pointers.begin(), pointers.end(), [] (int* pointer) -> void { delete pointer; });

  return 0;
}

