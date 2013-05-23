/* { dg-options "-O3 -mcpu=v8.30.b" } */

unsigned short swapu16_1 (unsigned short x)
{
  return (x << 8) | (x >> 8);
}

unsigned short swapu16_2 (unsigned short x)
{
  return (x >> 8) | (x << 8);
}

/* { dg-final { scan-assembler "swapb\t" } } */
/* { dg-final { scan-assembler "swaph\t" } } */