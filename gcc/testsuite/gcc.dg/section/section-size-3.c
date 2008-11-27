/* { dg-do run } */

#include <stdarg.h>

#define N 16
int result[N] =
  { 8, 10, 12, 15, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38 };
int result1[N] =
  { 11, 13, 15, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
int out[N];
extern void abort (void);


int
main (void)
{
  void *p = &&a;
  void *q = &&b;
  int arr[N];
  int k = 3;
  int m, i = 0;

  do
    {
      m = k + 5;
      arr[i] = m;
      k = k + 2;
      i++;
    }
  while (i < N);

  /* check results:  */
  for (i = 0; i < N; i++)
    {
      if (arr[i] != result[i])
	goto *p;
    }
b:
  abort ();
a:
  for (i = 0; i < N; i++)
    {
      out[i] = result[i] + 3;
    }
  for (i = 0; i < N; i++)
    {
      if (out[i] != result1[i])
        goto *q;
    }

  return 0;
}
