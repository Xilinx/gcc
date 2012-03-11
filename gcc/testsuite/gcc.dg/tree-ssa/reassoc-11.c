/* { dg-do compile } */ 
/* { dg-options "-O2 -fdump-tree-forwprop1" } */
int main(int a, int b, int c, int d)
{
  /* All the xor's cancel each other out, leaving 0  */
  int e = (a ^ b) ^ (c ^ d);
  int f = (c ^ a) ^ (b ^ d);
  return e ^ f;
}
/* { dg-final { scan-tree-dump-times "= 0" 1 "forwprop1"} } */
/* { dg-final { cleanup-tree-dump "forwprop1" } } */
