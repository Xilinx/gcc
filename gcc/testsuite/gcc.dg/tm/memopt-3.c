/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O -fdump-tree-tmmark" } */

struct large { int x[100]; };

int f()
{
  int i = readint();
  struct large lala = { 0 };
  __transaction {
    lala.x[i] = 666;
    if (test())
      __transaction_cancel;
  }
  return lala.x[0];
}

/* { dg-final { scan-tree-dump-times "Address: lala.x\\\[i_1\\\]" 1 "tmmark" } } */
