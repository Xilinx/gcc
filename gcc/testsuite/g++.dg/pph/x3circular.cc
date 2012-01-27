// { dg-do run }
#include "x2circular.h"

extern "C" { void abort(void); }

int bar(int x, int y)
{
  return x - y;
}

int foo(int x, int y)
{
  return bar (x, y) + x + y;
}

int main(void)
{
  if (foo (0, 0) != 0)
    abort ();
  return 0;
}
