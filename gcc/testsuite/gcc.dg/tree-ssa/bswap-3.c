/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

int fxor(int a, int b)
{
  int tmp;
  a = __builtin_bswap32(a);
  b = __builtin_bswap32(b);
  tmp = a == b;
  return tmp;
}
int fior(int a, int b)
{
  int tmp;
  a = __builtin_bswap32(a);
  b = __builtin_bswap32(b);
  tmp = a != b;
  return tmp;
}

/* We should be able to optimize away the byteswap expressions. */
/* { dg-final { scan-tree-dump-times "byteswap_expr" 0 "optimized"} } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
