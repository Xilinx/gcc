/* file testsuite/melt/tmallbuf.c */
/*** 
      run in buildir/gcc 
  ./cc1 -fbasilys=smallana \
   -fbasilys-dynlibdir=. \
   -fbasilys-compile-script=./built-melt-cc-script \
   -fbasilys-gensrcdir=. -fbasilys-tempdir=/tmp \
   -fbasilys-init=@warmelt2:ana-base -fbasilys-debug \
   -Wall .../tmallbuf.c

  and likewise, with -fbasilys=simpana 
 ***/
#include <stdlib.h>

int *
alloctab (int n)
{
  int i = 0;
  int *p = NULL;
  if (n <= 0)
    return NULL;
  p = calloc ((size_t) n, sizeof (int));
  for (i = 0; i < n; i++)
    p[i] = i * 2;
  return p;
}

int
main (int argc, char **argv)
{
  int k = 10;
  int *t = NULL;
  if (argc > 1)
    k = atoi (argv[1]);
  if (k < 0)
    k = 0;
  t = alloctab (k);
  /* following instruction access t out of bounds */
  t[k + 1] = 2 * k;
  return 0;
}
