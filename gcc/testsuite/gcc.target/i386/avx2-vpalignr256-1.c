/* { dg-do compile } */
/* { dg-options "-O0 -mavx2" } */
/* { dg-final { scan-assembler "vpalignr\[ \\t\]+\[^\n\]*%ymm\[0-9\]" } } */

#include <immintrin.h>

__m256i x;

void static
avx2_test (void)
{
  /* imm = 13 is arbitrary here */
  x = _mm256_alignr_epi8 (x, x, 13);
}
