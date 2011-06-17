/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpslldq\[ \\t\]+\[^\n\]*%ymm\[0-9\]" } } */

#include <immintrin.h>

extern __m256i x;

void extern
avx2_test (void)
{
  x = _mm256_slli_si256 (x, 13);
}
