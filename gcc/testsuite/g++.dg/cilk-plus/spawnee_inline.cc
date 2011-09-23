#include <iostream>
#include <cstdio>
#include <cstdlib>

int fib (char *n_char)
{
  int n;
  char n_char_minus_one[20], n_char_minus_two[20];
  if (n_char)
    n = atoi (n_char);
  else
    n = 10;
  
  if (n < 2)
    return n;
  else
    {	   
      int x, y;
      sprintf(n_char_minus_one,"%d", n-1); 
      sprintf(n_char_minus_two,"%d", n-2); 
      x = cilk_spawn fib (n_char_minus_one);
      y = cilk_spawn fib (n_char_minus_two);
      cilk_sync;
      return (x+y);
    }
}
int main (int argc, char *argv[])
{
  int n, result;
  if (argc == 2)
    {
      result = cilk_spawn fib (argv[1]);
      cilk_sync; 
      std::cout << "Result = " << result << std::endl;
    }
  else
    {
      result = cilk_spawn fib(NULL);
      cilk_sync; 
      std::cout << "Result = " << result << std::endl;
    }
  return 0;
}
