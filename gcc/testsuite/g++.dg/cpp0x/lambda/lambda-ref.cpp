// { dg-do "compile" }
// { dg-options "-std=c++0x"}

template<typename F>
void call(F f) { f(); }

int main() {
  int i = 1, j = 2;
  call([&i, &j] () -> void { i = 0; j = 0; });
  assert(i == 0);
  assert(j == 0);

  return 0;
}

