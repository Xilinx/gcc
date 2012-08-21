/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */



#include<cstdlib>
#define DEFAULT_VALUE 30
int fib (int n)
{
  if (n<2)
    return n;
  else
    {
      int x, y;
      x = _Cilk_spawn fib (n-1);
      y = _Cilk_spawn fib (n-2);
      _Cilk_sync;
      return (x+y);
      return 5;
    }
}

int main_parallel (int argc, char *argv[])
{
  int n, result;
  if (argc == 2)
    n = atoi(argv[1]);
  else
    n = DEFAULT_VALUE;
  result = _Cilk_spawn fib(n);
  _Cilk_sync; 
  return result;
}

int fib_serial (int n)
{
  int x, y;
  if (n < 2)
    return n;
  else
    {
      x = fib (n-1);
      y = fib (n-2);
      return (x+y);
    }
}
  
int main_serial (int argc, char *argv[])
{
  int n, result;

  if (argc == 2)
    n = atoi (argv[1]);
  else
    n = DEFAULT_VALUE;
  result = fib_serial (n);

  return result;
}

int main (int argc, char *argv[])
{
  if (main_serial (argc, argv) != main_parallel (argc, argv))
    abort ();
  else
    exit (0);
  return 0;
}

