// Test lock annotations
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;

class Foo {
 public:
  Mutex mu_ ACQUIRED_AFTER(mu1);
  static Mutex mu2_[7] ACQUIRED_AFTER(mu_);
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);
  void foo() const SHARED_LOCKS_REQUIRED(mu_);
  void bar() EXCLUSIVE_LOCKS_REQUIRED((mu2_[0]));
  inline Mutex *get_lock() LOCK_RETURNED(mu_) { return &mu_; }
  inline Mutex *get_lock2() { return &mu_; }
  static bool compare(Foo& x, Foo& y)
  {
    x.mu_.Lock();
    y.get_lock()->Lock();
    if (x.a_ > y.a_ && x.b_ > y.b_) {
      y.mu_.Unlock();
      x.get_lock()->Unlock();
      return true;
    }
    else {
      y.get_lock()->Unlock();
      x.mu_.Unlock();
      return false;
    }
  }
};

class Bar {
 public:
  Foo foo[3];
  Foo *get_foo() __attribute__((pure)) { return &foo[2]; }
};

class Cat {
 public:
  Bar *bar;
};

Foo *q  GUARDED_BY(mu1);
Foo y;
Cat w[3];
Bar *p GUARDED_BY((y.mu_));
int gx GUARDED_BY((w[1].bar->foo[2].mu_));


main()
{
  Foo x;
  Foo::compare(x, y);
  mu1.Lock();
  q->mu_.Lock();
  q->mu2_[0].Lock();
  q->foo();
  q->a_ = 5;
  q->bar();
  q->mu2_[0].Unlock();
  q->mu_.Unlock();
  mu1.Unlock();
  w[2].bar->foo[1].mu_.Lock();
  w[2].bar->foo[1].a_ = 3;
  Foo::mu2_[0].Lock();
  w[2].bar->foo[1].bar();
  Foo::mu2_[0].Unlock();
  w[2].bar->foo[1].mu_.Unlock();
  w[1].bar->foo[2].get_lock()->Lock();
  gx = 7;
  w[1].bar->foo[2].get_lock()->Unlock();
  y.mu_.Lock();
  p->foo[2].get_lock()->Lock();
  p->foo[2].b_ += 1;
  p->foo[2].mu_.Unlock();
  y.a_ = 2;
  y.mu_.Unlock();
}
