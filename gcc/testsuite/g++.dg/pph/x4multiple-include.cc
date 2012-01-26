#include "x3multiple-include.h"

extern "C" { void abort (void); }

my_float baz(my_int x)
{
  return bar (foo (x, x));
}
