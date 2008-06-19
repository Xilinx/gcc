#include "call.h"

int main() {
  int i = 1, j = 2;
  call([i, j] () -> void { i = 0; j = 0; });
  assert(i == 1);
  assert(j == 2);

  //call([] () -> void { i; });
  //call([1] () -> void {});

  return 0;
}

