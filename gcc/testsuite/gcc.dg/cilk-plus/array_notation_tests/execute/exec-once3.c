#if HAVE_IO
#include <stdio.h>
#endif

#define N 150

int func (int *x)
{
  *x = *x + 1;
  return *x;
}

int main (int argc, char **argv)
{
  int array[N][N];
  int ii, jj;
  int return_val = 0;
  array[:][:] = func (&argc);
  
  for (ii = 0; ii < N; ii++)
    for (jj = 0; jj < N; jj++)  {
#if HAVE_IO
	printf("array[%4d][%4d] = %4d\n", ii, jj, array[ii][jj]);
#endif
      if (array[ii][jj] != 2)
	return_val = 1;
    }
  return return_val;
}
