// { dg-do "compile" }
// { dg-options "-std=c++0x"}


template<typename F>
void call(F f) { f(); }


int main() {
  call([] () -> void {});
  call([] () -> void mutable {});

  int i = -1;
  call([i] () -> void mutable { i = 0; });
  assert(i == 0);

  return 0;
}

