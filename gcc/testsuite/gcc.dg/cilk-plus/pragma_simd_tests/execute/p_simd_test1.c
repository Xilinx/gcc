/* { dg-do run } */
/* { dg-options "-O3 -ftree-vectorize -fcilkplus" } */

#define ARRAY_SIZE  (256)
#if HAVE_IO
#include <stdio.h>
#endif
int a[ARRAY_SIZE];

int main () {
  int i, s = 0, r = 0;
#if 1
  for (i = 0; i < ARRAY_SIZE; i++)
    {
      a[i] = i;
    }
#endif
#pragma simd reduction (+:s) private (i) 
  for (i = 0; i < ARRAY_SIZE; i++)
    {
#if HAVE_IO
      printf("i = %d\n", i); 
#endif
      s += a[i];
    }

  for (i = 0; i < ARRAY_SIZE; i++) 
    r += i;

  if (s == r)
    return 0;
  else
    {
#if HAVE_IO 
      printf("s = %5d\t (expected value = %5d)\n", s, r);
#endif 
      return 1;
    }
}
