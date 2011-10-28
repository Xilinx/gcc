/* { dg-do compile } */
/* { dg-options "-O3 -msse2 -mno-sse3 -fdump-tree-vect-details" } */

#include "sse2-cvt-1.c"

/* { dg-final { scan-tree-dump-times "note: vectorized 1 loops in function" 6 "vect" } } */
/* { dg-final { scan-assembler "cvttpd2dq" } } */
/* { dg-final { scan-assembler "cvtdq2ps" } } */
/* { dg-final { scan-assembler "cvtps2pd" } } */
/* { dg-final { scan-assembler "cvttps2dq" } } */
/* { dg-final { scan-assembler "cvtdq2pd" } } */
/* { dg-final { scan-assembler "cvtpd2ps" } } */
/* { dg-final { cleanup-tree-dump "vect" } } */
