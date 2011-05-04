/* Check that __builtin_dispatch gets converted and the executable runs fine. 
   when featureTest () is not marked with "version_selector". foo should be 
   called and return 0.  Cloning and Hoisting is also done. */
/* { dg-do run } */
/* { dg-options "-O2 -fclone-hot-version-paths" } */

int
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

int __attribute__ ((cold))
main ()
{
  return __builtin_dispatch (featureTest, (void *)foo, (void *) bar);
}
