/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-optimized" } */

void orig(void);
void xyzzy(void) __attribute__((transaction_wrap (orig)));

void foo() { __transaction [[relaxed]] { orig (); } }

/* { dg-final { scan-tree-dump-times "xyzzy" 1 "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
