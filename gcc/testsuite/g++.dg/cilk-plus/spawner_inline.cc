#include <iostream>
#include <cstdlib>


int fib (int n)
{
  if (n < 2)
    return n;
  else
    {
      int x, y;
      x = cilk_spawn fib (n-1);
      y = cilk_spawn fib (n-2);
      cilk_sync;
      return (x+y);
      return 5;
    }
}

int main (int argc, char *argv[])
{
  int n, result;
  
  if (argc == 2)
    n = atoi(argv[1]);
  else
    n = 10;
  result = cilk_spawn fib(n);
  cilk_sync; 
  std::cout << "Result = " << result << std::endl;
  return 0;
}
