/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpgatherdd\[ \\t\]+\[^\n\]*%ymm\[0-9\]" } } */

#include <immintrin.h>

__m256i x;
int *base;
__m256i idx;

void extern
avx2_test (void)
{
  x = _mm256_i32gather_epi32 (base, idx, 1);
}
