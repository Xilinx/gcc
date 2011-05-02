/* Check if everything is fine when the feature test body is in a different
   module that does not have a __builtin-dispatch".  Requires mversn14a.C. */

/* { dg-do run } */
/* { dg-additional-sources "mversn14a.C" } */
/* { dg-options "-O2 -fclone-hot-version-paths" } */

int __attribute__ ((version_selector))
featureTest ();

int
foo ()
{
  return 0;
}

int
bar ()
{
  return 1;
}

int main ()
{
   return __builtin_dispatch (featureTest, (void *)foo, (void *)bar);
}
