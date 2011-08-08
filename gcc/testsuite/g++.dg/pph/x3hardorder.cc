//pph asm xdiff 28345
//Ordering of globals is different

#include "x3hardorder2.h"

int test () {
  return a1 + a2 - b1 + b2 * N::n1 + N::n2 - N::m1 + N::m2;
}
