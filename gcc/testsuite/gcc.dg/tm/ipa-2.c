/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-tmmark" } */

void foo(void);

void bar(void)
{
  __tm_atomic {
    foo();
  }
}

/* { dg-final { scan-tree-dump-times "_ZGTt3foo" 0 "tmmark" } } */
/* { dg-final { cleanup-tree-dump "tmmark" } } */
