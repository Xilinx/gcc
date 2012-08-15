void abort (void);
void exit (int);
int fib        (int);
int fib_serial (int);

int main(int argc, char **argv)
{
  int ii = 0, error = 0;
  int fib_result[41], fib_serial_result[41];

  for (ii = 0; ii <= 40; ii++)
    {
      fib_result[ii]        = fib (ii);
      fib_serial_result[ii] = fib_serial (ii);
    }

  for (ii = 0; ii <= 40; ii++)
    {
      if (fib_result[ii] != fib_serial_result[ii])
	error = 1;
    }
  if (error)
    abort ();
  else
    exit (0);
  return 0;
}

int fib_serial (int n)
{
  int x = 0, y = 0;
  if (n < 2)
    return n;
  else
    {
      x = fib (n-1);
      y = fib (n-2);
      return (x+y);
    }
}

int fib(int n)
{
  int x = 0, y = 0;
  if (n < 2) 
    return n;
  else
  {
    x = _Cilk_spawn fib(n-1);
    y = fib(n-2);
    _Cilk_sync;
    return (x+y);
  }
}
