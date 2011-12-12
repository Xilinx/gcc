/* { dg-do run { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-ftsan -O1" } */
#include "tsan.h"

/* Sanity check for memory accesses instrumentation. */

typedef struct C C;
struct C
{
  int r, g, b;
};

typedef struct P P;
struct P
{
  int x, y;
  C c;
  C *cp;
  C ca[2];
};

void teststructp (P *p)
{
  int volatile tmp;

  __tsan_expect_mop(2, &p->x, 1, 1, sizeof(p->x), __LINE__);
  p->x = 42;

  __tsan_expect_mop(2, &p->x, 0, 1, sizeof(p->x), __LINE__);
  tmp = p->x;
  (void)tmp;
}

void testfor (P *p)
{
  long long volatile tmp = 0;
  long long *a = (long long *)p->ca;
  int i;
  for (i = 0; i < 2; i++)
    {
      __tsan_expect_mop(2, &a [i], 0, 1, sizeof(a [0]), __LINE__);
      tmp += a [i];
    }
}

void
teststruct (void)
{
  P p;
  int volatile tmp;

  __tsan_expect_mop(1, &p.x, 1, 1, sizeof(p.x), __LINE__);
  p.x = 42;

  __tsan_expect_mop(1, &p.x, 0, 1, sizeof(p.x), __LINE__);
  tmp = p.x;
  (void)tmp;

  __tsan_expect_mop(1, &p.cp, 1, 1, sizeof(p.cp), __LINE__);
  p.cp = &p.c;

  __tsan_expect_mop(1, &p.c.r, 1, 1, sizeof(p.c.r), __LINE__);
  p.c.r = 11;

  teststructp (&p);
  testfor (&p);
}

void
foobar (int *p)
{
  __tsan_expect_mop(1, p, 1, 1, sizeof(*p), __LINE__);
  p[0] = 1;

  __tsan_expect_mop(1, p, 1, 1, sizeof(*p), __LINE__);
  *p = 2;

  __tsan_expect_mop(1, (char*)p+3, 1, 1, 1, __LINE__);
  *((char*)p+3) = 3;
}

int
main (void)
{
  int p;

  __tsan_expect_mop(0, &p, 1, 1, sizeof(p), __LINE__);
  p = 0;
  foobar (&p);

  teststruct ();

  return 0;
}
