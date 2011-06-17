/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpbroadcastb\[ \\t\]+\[^\n\]*%xmm\[0-9\]" } } */

#include <immintrin.h>

__m128i x;

void static
avx2_test (void)
{
  x = _mm_broadcastb_epi8 (x);
}
