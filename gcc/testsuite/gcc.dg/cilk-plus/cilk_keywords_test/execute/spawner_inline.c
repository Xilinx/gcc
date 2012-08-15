void exit (int);
void abort(void);
#define DEFAULT_VALUE 30
int fib (int n)
{
  if (n<2)
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

int main_parallel (int argc, char *argv[])
{
  int n, result;
  if (argc == 2)
    n = atoi(argv[1]);
  else
    n = DEFAULT_VALUE;
  result = cilk_spawn fib(n);
  cilk_sync; 
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

