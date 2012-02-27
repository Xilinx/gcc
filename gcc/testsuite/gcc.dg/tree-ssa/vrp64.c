/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-forwprop -fno-tree-fre -fno-tree-ccp -fno-tree-pre -fdump-tree-vrp1 -fdump-tree-optimized" } */
void link_error ();
int f(int a, int d)
{
  a |= 1;
  a &= 1;
  if (a != 1)
    d = 0;
  else
    {
      d |= 1;
      d &= 1;
    }
  if (d != 1)
    link_error ();
  return 0;
}

/* { dg-final { scan-tree-dump-times "link_error" 0 "optimized"} } */
/* { dg-final { scan-tree-dump-times "link_error" 0 "vrp1"} } */
/* { dg-final { scan-tree-dump-times "Folding predicate" 2 "vrp1"} } */
/* { dg-final { cleanup-tree-dump "vrp1" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */

