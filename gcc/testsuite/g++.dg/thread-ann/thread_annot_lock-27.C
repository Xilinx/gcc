// Test lock annotations applied to function definitions. This is a "good"
// test that should not incur any compilation wanrings.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
Mutex mu2 ACQUIRED_AFTER(mu1);

class Foo {
 public:
  int method1(int i) SHARED_LOCKS_REQUIRED(mu2);
};

int Foo::method1(int i) EXCLUSIVE_LOCKS_REQUIRED(mu1)
{
  return i;
}


int foo(int i) EXCLUSIVE_LOCKS_REQUIRED(mu2);
int foo(int i) SHARED_LOCKS_REQUIRED(mu1)
{
  return i;
}

static int bar(int i) EXCLUSIVE_LOCKS_REQUIRED(mu1)
{
  return i;
}

main()
{
  Foo a;

  mu1.Lock();
  mu2.Lock();
  a.method1(1);
  foo(2);
  mu2.Unlock();
  bar(3);
  mu1.Unlock();
}
