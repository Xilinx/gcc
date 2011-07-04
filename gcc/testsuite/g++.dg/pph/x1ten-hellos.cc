// { dg-do run }

#include "x1ten-hellos.h"

int main(void)
{
  A a;
  int i;

  for (i = 0; i < 10; i++)
    a.hello();

  if (i != 10)
    abort ();

  exit (0);
}
