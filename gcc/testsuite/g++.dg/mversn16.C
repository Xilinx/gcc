/* dispatch is cloned. Make sure the double is returned to main correctly. */

/* { dg-do run } */
/* { dg-options "-O2 -fclone-hot-version-paths -fdump-tree-optimized" } */

#include "assert.h"

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}

static int foo ()
{
  return 0;
}

static int bar ()
{
  return 1;
}

double
dispatch ()
{
  __builtin_dispatch (featureTest, (void *)foo, (void *)bar);
  return 2.536;
}

int main ()
{
  double d = dispatch ();
  assert (d == 2.536);
  return 0; 
}

/* { dg-final { scan-tree-dump "dispatchv_clone" "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
