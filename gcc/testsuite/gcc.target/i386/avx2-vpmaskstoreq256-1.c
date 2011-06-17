/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpmaskmovq\[ \\t\]+\[^\n\]" } } */

#include <immintrin.h>

__m256i x;
long long int *y;

void static
avx2_test (void)
{
  _mm256_maskstore_epi64 (y, x, x);
}
