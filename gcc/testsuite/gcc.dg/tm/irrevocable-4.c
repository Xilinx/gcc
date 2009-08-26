/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-tmmark" } */

extern void bar(void) __attribute__((tm_callable));
void orig(void);
void xyz(void) __attribute__((tm_wrap (orig)));


foo()
{
	__tm_atomic orig();
}

/* { dg-final { scan-tree-dump-times "GTMA_MAY_ENTER_IRREVOCABLE" 1 "tmmark" } } */
