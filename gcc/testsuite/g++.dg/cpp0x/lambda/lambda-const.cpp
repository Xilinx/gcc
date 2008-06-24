// { dg-do "compile" }
// { dg-options "-std=c++0x"}

template<typename F>
void call(const F& f) { f(); }

int main() {
  call([] () -> void {});
  //call([] () -> void mutable {}); // { dg-error "`f' does not have const `operator()'" }

  int i = -1;
  call([&i] () -> void { i = 0; });
  assert(i == 0);
  //call([i] () -> void { i = 0; }); // { dg-error: "assignment to non-reference capture in const lambda" }

  return 0;
}

