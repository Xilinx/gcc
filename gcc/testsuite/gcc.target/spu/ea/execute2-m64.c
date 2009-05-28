/* { dg-do run } */
/* { dg-options "-O2 -std=gnu99 -g -pedantic-errors -mea64" } */

#include <stdlib.h>

char str[] = "abc";

int
main (void)
{
  __ea char *p = (__ea char *)str;

  if (*p++ != 'a')
    abort ();

  if (*p++ != 'b')
    abort ();

  if (*p++ != 'c')
    abort ();

  if (*p++ != '\0')
    abort ();

  return 0;
}
