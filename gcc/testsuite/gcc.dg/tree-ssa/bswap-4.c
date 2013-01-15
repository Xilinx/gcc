/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

int h(int b)
{
  int c = 0x12345678;
  return __builtin_bswap32 (c);
}

/* We should be able to optimize away the byteswap expressions into a constant expression. */
/* { dg-final { scan-tree-dump-times "byteswap_expr" 0 "optimized"} } */
/* { dg-final { scan-tree-dump-times "2018915346" 1 "optimized"} } */

/* { dg-final { cleanup-tree-dump "optimized" } } */

