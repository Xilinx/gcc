/* Check that global inline feature test functions are correctly handled. This
   test case also needs mversn5a.c.  extern_func calls foo and returns 0.*/

/* { dg-do run } */
/* { dg-additional-sources "mversn5a.C" } */

#include "mversn5.h"

extern int extern_func ();

int foo ()
{
  return 0;
}

int bar ()
{
  return 1;
}

int __attribute__ ((cold))
main ()
{
  int a = 1, b = 1;
  a  = extern_func ();
  b  = __builtin_dispatch (featureTest, (void *)bar, (void *) foo);
  return a * b;
}
