// pph asm xwant 28345
// The test will inherently have a different assembly order
// because the PPH file is read before any parsing.

#include "x3hardorder2.h"

int test () {
  return a1 + a2 - b1 + b2 * N::n1 + N::n2 - N::m1 + N::m2;
}
