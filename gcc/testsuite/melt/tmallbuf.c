/* file testsuite/melt/tmallbuf.c */
/*** 
      run in buildir/gcc 
  ./cc1 -fmelt=smallana \
   -fmelt-dynlibdir=. \
   -fmelt-compile-script=./built-melt-cc-script \
   -fmelt-gensrcdir=. -fmelt-tempdir=/tmp \
   -fmelt-debug \
   -Wall -O .../tmallbuf.c

 ***/
#include <stdlib.h>

int *
alloctab (int n, int y)
{
  int i = 0;
  int *p = NULL;
  if (n <= 0)
    n = 1;
  p = calloc ((size_t) n,
	      sizeof (int));
  for (i = 0; 
       i < n; 
       i++)
    p[i] = i * 2 + y;
  return p;
}

int
main (void)
{
  int j;
  int k = 10;
  int *t = NULL;
  t = alloctab (k, 17);
  for (j = 0;
       j < k; 
       j += 2)
    t[j] *= 2;
  /* following instruction access t out of bounds */
  t[k + 1] = 2 * k;
  return 0;
}
