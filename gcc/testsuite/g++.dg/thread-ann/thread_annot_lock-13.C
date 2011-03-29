// Test lock annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
int g GUARDED_BY(mu1);
int w GUARDED_BY(mu2);

class Foo {
 public:
  void bar() LOCKS_EXCLUDED(mu_, mu1)
  {
    int x;

    mu_.Lock();
    x = foo(); // { dg-warning "Calling function 'foo' requires lock 'mu2'" }
    a_ = x + 1;
    mu_.Unlock();
    if (x > 5) {
      mu1.Lock();
      g = 2.3;
      mu1.Unlock();
    }
  }
  int foo() SHARED_LOCKS_REQUIRED(mu_) EXCLUSIVE_LOCKS_REQUIRED(mu2);

 private:
  int a_ GUARDED_BY(mu_);
 public:
  Mutex mu_ ACQUIRED_AFTER(mu1);
};

Mutex mu2;

int Foo::foo()
{
  int res;
  w = 5.2;
  res = a_ + 5;
  return res;
}

main()
{
  Foo f1, *f2;
  f1.mu_.Lock();
  f1.bar();
  mu2.Lock();
  f1.foo();
  mu2.Unlock();
  f1.mu_.Unlock();
  f2->mu_.Lock();
  f2->bar();
  f2->mu_.Unlock();
  mu2.Lock();
  w = 2.5;
  mu2.Unlock();
}

// { dg-warning "Cannot call function 'bar' with lock 'f1.mu_' held \\(previously acquired at line 48\\)" "" { target *-*-* } 49 }
// { dg-warning "Cannot call function 'bar' with lock 'f2->mu_' held \\(previously acquired at line 54\\)" "" { target *-*-* } 55 }
