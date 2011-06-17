/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpmaskmovq\[ \\t\]+\[^\n\]" } } */

#include <immintrin.h>

__m128i x;
long long int *y;

void static
avx2_test (void)
{
  _mm_maskstore_epi64 (y, x, x);
}
