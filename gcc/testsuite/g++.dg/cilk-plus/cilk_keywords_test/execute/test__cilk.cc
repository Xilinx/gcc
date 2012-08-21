/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>

int main (void)
{
  if (__cilk != 200)
    abort ();
  return 0;
}
