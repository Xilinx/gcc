/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-tmmark" } */

extern void bar(void) __attribute__((tm_callable));

foo()
{
	__tm_atomic bar();
}

/* { dg-final { scan-tree-dump-times "GTMA_MAY_ENTER_IRREVOCABLE" 1 "tmmark" } } */
