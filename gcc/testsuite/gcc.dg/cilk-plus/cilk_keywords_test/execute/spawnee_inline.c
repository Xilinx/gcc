#include<stdlib.h>
#include<stdio.h>
#define DEFAULT_VALUE "30"

void exit (int);
void abort (void);


int fib (char *n_char)
{
  int n;
  char n_char_minus_one[20], n_char_minus_two[20];
  if (n_char)
    n = atoi (n_char);
  else
    n = atoi(DEFAULT_VALUE);
  
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

int fib_serial (int n)
{
  int x, y;
  if (n < 2)
    return n;
  else
    {
      x = fib_serial (n-1);
      y = fib_serial (n-2);
      return (x+y);
    }
  return 0;
}

int main2_parallel (int argc, char *argv[])
{
  int n, result_parallel = 0;

  if (argc == 2)
    {
      result_parallel = cilk_spawn fib (argv[1]);
      cilk_sync; 
    }
  else
    {
      result_parallel = cilk_spawn fib(0);
      cilk_sync; 
    }
  return result_parallel;
}

int main2_serial (int argc, char *argv[])
{
  int n, result_serial = 0;
  if (argc == 2) 
    result_serial = fib_serial (atoi (argv[1]));
  else
    result_serial = fib_serial (atoi (DEFAULT_VALUE));

  return result_serial;
}

int main (int argc, char *argv[])
{
  if (main2_serial (argc, argv) != main2_parallel (argc, argv))
    abort ();
  else
    exit (0);

  return 0;
}

