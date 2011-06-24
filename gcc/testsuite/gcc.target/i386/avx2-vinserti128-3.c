/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */

#include <immintrin.h>

__m256i x;
__m128i y;
int i;

void static
avx2_test (void)
{
  x = _mm256_inserti128_si256 (x, y, 2);  /* { dg-error "the last argument must be an 1-bit immediate" } */
  x = _mm256_inserti128_si256 (x, y, -1); /* { dg-error "the last argument must be an 1-bit immediate" } */
  x = _mm256_inserti128_si256 (x, y, i);  /* { dg-error "the last argument must be an 1-bit immediate" } */
}
