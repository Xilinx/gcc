/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-tmmark-asmname" } */

void foo(void) __attribute__((transaction_safe));

void bar(void)
{
  __transaction {
    foo();
  }
}

/* { dg-final { scan-tree-dump-times "_ZGTt3foo" 1 "tmmark" } } */
/* { dg-final { cleanup-tree-dump "tmmark" } } */
