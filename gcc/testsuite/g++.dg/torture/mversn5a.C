/* { dg-do compile } */

#include "mversn5.h"

extern int foo ();
extern int bar ();


int extern_func ()
{
  return __builtin_dispatch (featureTest, (void *)foo, (void *) bar);
}
