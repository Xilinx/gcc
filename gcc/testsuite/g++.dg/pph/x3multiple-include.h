#ifndef __X3_MULTIPLE_INCLUDE_H
#define __X3_MULTIPLE_INCLUDE_H

#include "x1multiple-include.h"
#include "x0multiple-include.h"
#include "x2multiple-include.h"

my_float baz (my_int);

my_int foo(int x, int y)
{
  return x - y;
}

my_float bar(int y)
{
  return (my_float) y;
}

#endif
