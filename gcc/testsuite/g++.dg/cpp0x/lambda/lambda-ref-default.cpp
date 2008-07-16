// { dg-do "compile" }
// { dg-options "-std=c++0x"}


int main() {
  int i = 1, j = 2;
  [&] () -> void { i = 0; j = 0; } ();
  assert(i == 0);
  assert(j == 0);

  return 0;
}

