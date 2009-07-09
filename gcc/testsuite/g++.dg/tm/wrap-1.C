/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-optimized" } */

void orig(void);
void xyzzy(void) __attribute__((tm_wrap (orig)));

void foo() { __tm_atomic { orig (); } }

/* { dg-final { scan-tree-dump-times "xyzzy" 1 "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
