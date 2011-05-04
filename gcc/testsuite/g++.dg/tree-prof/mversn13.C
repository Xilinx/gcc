/* Make sure -fprofile-generate and -fprofile-use work fine. */

/* { dg-options "-O2 -fclone-hot-version-paths" } */

static int glob = 0;
int __attribute__ ((version_selector))
featureTest ()
{
  return glob;
}

int bar (int i)
{
  if (i > 500)
    return 2 * i;
  return 3 * i;
}

int foo (int i)
{
  bar (i);
}

int
dispatch ()
{
  int ret = 0;
  for (int i = 0; i < 1000; i++)
    ret += __builtin_dispatch (featureTest, (void *)foo, (void *)bar,  i);
  return ret;
}

int main ()
{
  int val = dispatch ();
  return val > 10000000;
}
