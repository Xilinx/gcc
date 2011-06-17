/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */
/* { dg-final { scan-assembler "vperm2i128\[ \\t\]+\[^\n\]*" } } */

#include <immintrin.h>

__m256i x;

void static
avx2_test (void)
{
  x = _mm256_permute2x128_si256 (x,x,13);
}
