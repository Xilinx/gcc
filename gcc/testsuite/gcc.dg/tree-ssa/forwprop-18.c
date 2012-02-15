/* { dg-do compile } */
/* { dg-options "-O -fdump-tree-forwprop1" } */

int f(int a, int b)
{
  int c = a ^ b;
  int d = c & b;
  int e = c & a;
  int d1 = b & c;
  int e1 = a & c;
  return e|d | d1|e1;
}

/* { dg-final { scan-tree-dump-times "~" 4 "forwprop1" } } */
/* { dg-final { cleanup-tree-dump "forwprop1" } } */
