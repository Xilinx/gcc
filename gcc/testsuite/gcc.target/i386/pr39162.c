/* { dg-do compile } */
/* { dg-options "-O2 -Wno-psabi -msse2 -mno-avx" } */
/* { dg-additional-options "-mabi=sysv" { target x86_64-*-mingw* } } */

typedef long long __m256i __attribute__ ((__vector_size__ (32), __may_alias__));

extern __m256i y;

void
bar (__m256i x) /* { dg-warning "AVX" "" } */
{
  y = x;
}
