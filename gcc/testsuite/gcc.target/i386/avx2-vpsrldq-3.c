/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */

#include <immintrin.h>

extern __m256i x;
int i;

void extern
avx2_test (void)
{
  x = _mm256_srli_si256 (x, 257); /* { dg-error "the last argument must be an 8-bit immediate" } */
  x = _mm256_srli_si256 (x, -1);  /* { dg-error "the last argument must be an 8-bit immediate" } */
  x = _mm256_srli_si256 (x, i);   /* { dg-error "the last argument must be an 8-bit immediate" } */
}
