/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vinserti128\[ \\t\]+\[^\n\]" } } */

#include <immintrin.h>

__m256i x;
__m128i y;

void static
avx2_test (void)
{
  x = _mm256_inserti128_si256 (x, y, 1);
}
