/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */
int g(int a)
{
  unsigned int b = __builtin_bswap32(__builtin_bswap32 (a));
  return b;
}

/* We should be able to optimize away the byteswap expressions. */
/* { dg-final { scan-tree-dump-times "byteswap_expr" 0 "optimized"} } */
/* { dg-final { cleanup-tree-dump "optimized" } } */

