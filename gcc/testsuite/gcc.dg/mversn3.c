/* Simple check if parameters are passed correctly. */

/* { dg-do run } */
/* { dg-options "-O2 -fclone-hot-version-paths" } */

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}

int __attribute__ ((cold))
foo (int a)
{
  if (a == 1729)
    return 0;
  return 1;
}

int __attribute__ ((cold))
bar (int a)
{
  return 1;
}

int __attribute__ ((cold))
main ()
{
  return __builtin_dispatch (featureTest, (void *)foo, (void *) bar, 1729);
}
