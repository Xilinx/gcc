/* { dg-options "-O2 -fclone-hot-version-paths -fripa" } */
/* { dg-additional-sources "mversn15.C" } */

#include <stdio.h>

inline int
 __attribute__ ((version_selector))
featureTest()
{
  return 1;
}

int foo ()
{
  return 1;
}

int bar ()
{
  return 1;
}

int dispatch ()
{
  return __builtin_dispatch (featureTest, (void *)foo, (void *)bar);
}
