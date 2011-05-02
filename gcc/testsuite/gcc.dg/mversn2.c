/* This checks if cloning works correctly.  Since dispatch and fn1 are hot, they
   should be cloned.  main should not be cloned.*/

/* { dg-do run } */
/* { dg-options "-O2 -fclone-hot-version-paths -fdump-tree-optimized" } */

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}

int __attribute__ ((cold))
foo ()
{
  return 0;
}

int __attribute__ ((cold))
bar ()
{
  return 1;
}

int __attribute__ ((hot))
dispatch ()
{
  return __builtin_dispatch (featureTest, (void *)foo, (void *) bar);
}
int __attribute__ ((hot))
fn1 ()
{
  return dispatch ();
}

int __attribute__ ((cold))
main ()
{
  return fn1 ();
}

/* { dg-final { scan-tree-dump "fn1_clone_1" "optimized" } } */
/* { dg-final { scan-tree-dump "dispatch_clone_0" "optimized" } } */
/* { dg-final { scan-tree-dump "dispatch_clone_1" "optimized" } } */
/* { dg-final { scan-tree-dump-not "main_clone_0" "optimized" } } */
/* { dg-final { scan-tree-dump-not "main_clone_1" "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
