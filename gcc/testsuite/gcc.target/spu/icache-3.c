/* { dg-do compile } */
/* { dg-options "-O2 -msoftware-icache" } */

int
foo (int a)
{
  return (2 * a + 3);
}

/* { dg-final { scan-assembler ".text.ia." } } */
