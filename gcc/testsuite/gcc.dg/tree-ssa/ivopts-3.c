/* { dg-do compile } */
/* { dg-options "-O2 -fivopts -fdump-tree-ivopts-details" } */

void main (void)
{
  int i;
  for (i = 0; i < 10; i++)
    f2 ();
}

/* { dg-final { scan-tree-dump-times "!= 0" 5 "ivopts" } }  */
/* { dg-final { cleanup-tree-dump "ivopts" } }  */
