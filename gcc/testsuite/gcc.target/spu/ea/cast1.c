/* { dg-do run { target spu-*-* } } */
/* { dg-options "-std=gnu99" } */

extern void abort (void);
extern unsigned long long __ea_local_store;

__ea int *ppu;
int x, *spu = &x, *spu2;

int
main (int argc, char **argv)
{
  ppu = (__ea int *) spu;
  spu2 = (int *) ppu;

  if ((int) ppu != (int) __ea_local_store + (int) spu)
    abort ();

  if (spu != spu2)
    abort ();

  return 0;
}
