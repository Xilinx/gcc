// { dg-do run }
// { dg-xfail-if "LINK ERROR" { "*-*-*" } { "-fpph-map=pph.map" } }
// pph asm xdiff
#include "x1ten-hellos.h"

int main(void)
{
  A a;
  for (int i = 0; i < 10; i++)
    a.hello();
  return 0;
}
