/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-tmmark" } */

void foo(void) __attribute__((tm_callable));

void bar(void)
{
  __tm_atomic {
    foo();
  }
}

/* { dg-final { scan-tree-dump-times "_ZGTt3foo" 1 "tmmark" } } */
/* { dg-final { cleanup-tree-dump "tmmark" } } */
