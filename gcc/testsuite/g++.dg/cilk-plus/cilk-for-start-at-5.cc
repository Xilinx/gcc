#include <cstdio>
#include <cstdlib>
int j[10];
int main2(void);

int main(void)
{
  int error = 0;
  int j_serial[10];
  for (int ii = 0; ii < 10; ii++)
    {
      j[ii] = 10;
      j_serial[ii] = 10;
    }
  cilk_for (int ii = 5; ii < 10; ii++)
    {
      j[ii]=ii;
    }

  for (int ii = 5; ii < 10; ii++)
    {
      j_serial[ii] = ii;
    }

  for (int ii = 0; ii < 10; ii++)
    {
      if (j[ii] != j_serial[ii]) 
	error = 1;    
    }

  if (error)
    abort ();
  else
    std::puts ("Done.");

  return j[9];
}

