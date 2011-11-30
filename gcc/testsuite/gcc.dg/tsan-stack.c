/* { dg-do run { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-ftsan -O1" } */
#include "tsan.h"

/* Check shadow stack maintance.  */

void barbaz (int volatile *p)
{
  __tsan_expect_mop(2, p, 1, 1, sizeof(*p), __LINE__);
  *p = 11;
}

void
foobar (int volatile *p)
{
  __tsan_expect_mop(1, p, 1, 1, sizeof(*p), __LINE__);
  p[0] = 13;
  barbaz (p);
  __tsan_expect_mop(1, p, 1, 1, sizeof(*p), __LINE__);
  p[0] = 17;
}

int
main (void)
{
  int volatile p;

  __tsan_expect_mop(0, &p, 1, 1, sizeof(p), __LINE__);
  p = 0;
  foobar (&p);
  __tsan_expect_mop(0, &p, 1, 1, sizeof(p), __LINE__);
  p = 1;
  return 0;
}
