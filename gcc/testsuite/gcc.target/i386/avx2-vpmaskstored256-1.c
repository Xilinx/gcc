/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpmaskmovd\[ \\t\]+\[^\n\]" } } */

#include <immintrin.h>

__m256i x;
int *y;

void static
avx2_test (void)
{
  _mm256_maskstore_epi32 (y, x, x);
}
