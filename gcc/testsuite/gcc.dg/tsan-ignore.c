/* { dg-do run { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-ftsan -O1 -ftsan-ignore=tsan-ignore.ignore" } */
#include "tsan.h"
#include "tsan-ignore.h"

/* Check ignore file handling. */

void
foo (int *p)
{
  p [0] = 1;
}

void
bar (int *p)
{
  p [0] = 1;
}

void
baz (int *p)
{
  p [0] = 1;
}

void
bla (int *p)
{
  p [0] = 1;
}

void
xxx (int *p)
{
  p [0] = 1;
}

int
main (void)
{
  int p, x;

  __tsan_expect_mop(0, &p, 1, 1, sizeof(p), __LINE__);
  /* All these functions must be ignored. */
  foo (&x);
  bar (&x);
  baz (&x);
  bla (&x);
  xxx (&x);
  in_tsan_ignore_header (&x);
  p = 0;
  return 0;
}
