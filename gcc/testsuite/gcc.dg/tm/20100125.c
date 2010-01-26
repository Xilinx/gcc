/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O -fdump-tree-tmmark" } */

int trxn;

void set_remove(int * val)
{
  __transaction {
      trxn = 5;
  }
  george();
}

/* { dg-final { scan-tree-dump-times "getTMCloneOrIrrevocable" 0 "tmmark" } } */
/* { dg-final { cleanup-tree-dump "tmmark" } } */
