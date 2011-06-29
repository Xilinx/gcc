// { dg-do run }
#include "x1ten-hellos.h"

int main(void)
{
  A a;
  for (int i = 0; i < 10; i++)
    a.hello();
  return 0;
}
