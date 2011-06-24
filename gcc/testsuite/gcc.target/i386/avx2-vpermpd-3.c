/* { dg-do compile } */
/* { dg-options "-mavx2 -O0" } */

#include <immintrin.h>

__m256d x;
int i;

void static
avx2_test (void)
{
  x = _mm256_permute4x64_pd (x, 257); /* { dg-error "the last argument must be an 8-bit immediate" } */
  x = _mm256_permute4x64_pd (x, -1);  /* { dg-error "the last argument must be an 8-bit immediate" } */
  x = _mm256_permute4x64_pd (x, i);   /* { dg-error "the last argument must be an 8-bit immediate" } */
}
