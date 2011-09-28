// { dg-do compile { target x86*-*-* } }
#include "x0asm1.h"

int X;

int foo () {
  I<int> x;
  x.f();
  if (cas(&X, 0, 1))
    return 0;
}
