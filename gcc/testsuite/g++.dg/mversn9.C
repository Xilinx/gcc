/* Two __builtin_dispatch calls in different basic blocks. */

/* { dg-do run } */
/* { dg-options "-O2 -fclone-hot-version-paths" } */

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}

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

int main (int argc, char **argv)
{
  if (argc)
   return __builtin_dispatch (featureTest, (void *)foo, (void *)bar);
  else
   return (__builtin_dispatch (featureTest, (void *)bar, (void *)foo) - 1);
}
