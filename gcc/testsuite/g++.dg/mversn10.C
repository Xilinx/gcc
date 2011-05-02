/* The purpose of this test is to check if the attributes of the clone
   correctly shadow the parent function.  In this case, function "dispatch"
   is cloned but is a static function.  The other file, "mversn10a.C" also
   has a dispatch function that is cloned.  So, if the attributes of the
   clone are not correct the linker will complain. */

/* { dg-do run } */
/* { dg-additional-sources "mversn10a.C" } */
/* { dg-options "-O2 -fclone-hot-version-paths -fdump-tree-optimized" } */

static int __attribute__ ((version_selector))
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

static int __attribute__ ((noinline))
dispatch ()
{
  __builtin_dispatch (featureTest, (void *)foo, (void *)bar);
  return 0;
}

int
fn1 ()
{
  for (int i = 0; i < 1000; i++)
    dispatch ();
  return 0;
}

extern int fn2 ();

int __attribute__ ((hot))
main ()
{
  fn1 ();
  fn2 ();
  return 0;
}

/* { dg-final { scan-tree-dump "dispatchv_clone_0" "optimized" } } */
/* { dg-final { scan-tree-dump "dispatchv_clone_1" "optimized" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
