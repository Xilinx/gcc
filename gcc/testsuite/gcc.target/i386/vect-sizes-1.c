/* { dg-do compile } */
/* { dg-options "-O3 -ffast-math -mavx" } */

double a[1024];

void dependence_distance_2 (void)
{
  int i;
  for (i = 0; i < 1022; ++i)
    a[i + 2] = a[i] + a[i + 2];
}

void dependence_distance_4 (void)
{
  int i;
  for (i = 0; i < 1020; ++i)
    a[i + 4] = a[i] + a[i + 4];
}

/* We should use 8 byte vectors for the first and 16 byte vectors for
   the second loop.  */

/* { dg-final { scan-assembler "vmovapd\[\\t \]*%ymm" } } */
/* { dg-final { scan-assembler "vmovapd\[\\t \]*%xmm" } } */
