/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vpbroadcastw\[ \\t\]+\[^\n\]*%xmm\[0-9\]" } } */

#include <immintrin.h>

__m128i x;

void static
avx2_test (void)
{
  x = _mm_broadcastw_epi16 (x);
}
