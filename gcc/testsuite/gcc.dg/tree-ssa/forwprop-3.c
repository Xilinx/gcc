/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-ccp -fdump-tree-forwprop1-details" } */

struct bar {
  int a[2];
};

int foo(struct bar *x)
{
  int *p = &x->a[0];
  int *q = &x->a[1];
  if (p < q)
    return 1;
  return 0;
}

/* { dg-final { scan-tree-dump "Folding statement: if .1 != 0." "forwprop1" } } */
/* { dg-final { scan-tree-dump-times "Folded" 1 "forwprop1" } } */
/* { dg-final { cleanup-tree-dump "forwprop1" } } */
