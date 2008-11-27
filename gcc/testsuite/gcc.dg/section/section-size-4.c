/* { dg-do run } */

#include <stdarg.h>

extern void abort (void);
#define N 16
#define M 20
int in[N] = { 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38 };
int arr[N] =
  { 13, 17, 21, 25, 29, 33, 37, 47, 51, 55, 59, 63, 67, 71, 75, 79 };
int out[N];

typedef int (*func) (int);

static int
foo (int a)
{
  return (2 * a + 3);
}

static int
foo1 (int a)
{
  return (2 * a - 3);
}

static void __attribute__((noinline)) 
compute_output (int *out, func f, int elem)
{
  *out = (*f) (elem);
}

int
main (void)
{
  int i;

  for (i = 0; i < N; i++)
    {
      if (in[i] > M)
	compute_output (&out[i], foo, in[i]);
      else
	compute_output (&out[i], foo1, in[i]);
    }

  for (i = 0; i < N; i++)
    if (out[i] != arr[i])
      abort ();

  return 0;
}
