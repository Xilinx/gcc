/* { dg-do compile } */
/* { dg-options "-O3 -ffast-math -mavx" } */

double a[1024];
float b[1024];
int c[1024];

void dependence_distance_4_mixed_0 (void)
{
  int i;
  for (i = 0; i < 1020; ++i)
    a[i + 4] = a[i] + a[i + 4] + c[i];
}

void dependence_distance_4_mixed_1 (void)
{
  int i;
  for (i = 0; i < 1020; ++i)
    b[i + 4] = b[i] + b[i + 4] + c[i];
}

/* Should use 8byte vectors for _1 and 16byte vectors for _0.  */

/* { dg-final { scan-assembler "vcvtdq2pd" } } */
/* { dg-final { scan-assembler "vaddpd\[\\t \]*%ymm" } } */
/* { dg-final { scan-assembler "vcvtdq2ps" } } */
/* { dg-final { scan-assembler "vaddps\[\\t \]*%xmm" } } */
