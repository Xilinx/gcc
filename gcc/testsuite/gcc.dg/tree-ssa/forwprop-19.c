/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-ccp -fno-tree-vrp -fdump-tree-forwprop1 -fno-tree-fre -fno-tree-pre -fdump-tree-optimized" } */

int f(int a)
{
  a&=3;
  a^=3;
  return a|3;
}

/* Forwprop has a simple nonzero bits which just handles straight
   line code so we should find that the nonzero bits on the assignment
   of a&=3 is 3 and a^=3 does not change that. */
/* { dg-final { scan-tree-dump-not "\\| 3" "forwprop1" } } */
/* { dg-final { scan-tree-dump "return 3" "optimized" } } */
/* { dg-final { cleanup-tree-dump "forwprop1" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */

