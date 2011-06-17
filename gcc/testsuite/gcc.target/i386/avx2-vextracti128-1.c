/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vextracti128\[ \\t\]+\[^\n\]*%ymm\[0-9\]" } } */

#include <immintrin.h>

__m256i x;
__m128i y;

void static
avx2_test (void)
{
  y = _mm256_extracti128_si256 (x, 1);
}
