/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>
#include <cilk/cilk.h>

int fib(int );
int sfib(int );

int main(int argc, char **argv)
{
  for (int ii = 0; ii < 30; ii++) 
    if (fib(ii) != sfib(ii))
      abort ();
  return 0;
}


int fib(int n)
{
  int x = 0, y = 0;
  if (n < 2)
  {
    return n;
  }
  else
  {
    x = cilk_spawn fib(n-1);
    y = fib(n-2);
    cilk_sync; 
    return (x+y);
  }
}

int sfib(int n)
{
  int x = 0, y = 0;
  if (n < 2)
  {
    return n;
  }
  else
  {
    x = fib(n-1);
    y = fib(n-2);
    return (x+y);
  }
}
