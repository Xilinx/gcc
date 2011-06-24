/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */

#include <immintrin.h>

__m256i x;
__m128i y;
int i;

void static
avx2_test (void)
{
  y = _mm256_extracti128_si256 (x, 2);  /* { dg-error "the last argument must be an 1-bit immediate" } */
  y = _mm256_extracti128_si256 (x, -1); /* { dg-error "the last argument must be an 1-bit immediate" } */
  y = _mm256_extracti128_si256 (x, i);  /* { dg-error "the last argument must be an 1-bit immediate" } */
}
