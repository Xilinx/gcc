#include "call.h"

int main() {
  int i = 1, j = 2;
  call([=] () -> void { i = 0; j = 0; });
  assert(i == 1);
  assert(j == 2);

  return 0;
}

