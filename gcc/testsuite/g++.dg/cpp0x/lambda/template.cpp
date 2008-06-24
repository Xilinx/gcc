// { dg-do "compile" }
// { dg-options "-std=c++0x"}

template<typename F>
void call(F f) { f(); }

int main() {

  return 0;
}

