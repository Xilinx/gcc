/* { dg-do compile } */
/* { dg-options "-O2 -march=nocona" } */
/* { dg-final { scan-assembler-not "bswap\[ \t\]" } } */

/* bswap(a) ^ bswap(b) -> bswap (a ^ b) */

int fxor(int a, int b)
{
  int tmp;
  a = __builtin_bswap32(a);
  b = __builtin_bswap32(b);
  tmp = a ^ b;
  return __builtin_bswap32(tmp);
}
/* bswap(a) | bswap(b) -> bswap (a | b) */
int fior(int a, int b)
{
  int tmp;
  a = __builtin_bswap32(a);
  b = __builtin_bswap32(b);
  tmp = a | b;
  return __builtin_bswap32(tmp);
}
/* bswap(a) & bswap(b) -> bswap (a & b) */
int fand(int a, int b)
{
  int tmp;
  a = __builtin_bswap32(a);
  b = __builtin_bswap32(b);
  tmp = a & b;
  return __builtin_bswap32(tmp);
}
