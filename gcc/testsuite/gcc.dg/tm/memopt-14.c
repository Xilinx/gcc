/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O -fdump-tree-tmmark-asmname" } */

struct large { int x[100]; };
extern struct large funky (void) __attribute__((transaction_safe));

void f()
{
  __transaction {
      struct large S1 = funky();
  }
}

/* { dg-final { scan-tree-dump-times "ITM_LB \\\(&S1," 1 "tmmark" } } */
/* { dg-final { scan-tree-dump-times "ZGTt5funky.*return slot optim" 1 "tmmark" } } */
