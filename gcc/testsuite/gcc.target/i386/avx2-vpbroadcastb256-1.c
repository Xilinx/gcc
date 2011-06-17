/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpbroadcastb\[ \\t\]+\[^\n\]*%ymm\[0-9\]" } } */

#include <immintrin.h>

__m128i x;
__m256i y;

void static
avx2_test (void)
{
  y = _mm256_broadcastb_epi8 (x);
}
