// Test lock annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;

class Foo {
 public:
  Mutex mu_ ACQUIRED_AFTER(mu1);
  static Mutex mu2_[2];
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);
  void foo() const SHARED_LOCKS_REQUIRED(mu_);
  void bar() EXCLUSIVE_LOCKS_REQUIRED((mu2_[0]));
  inline Mutex *get_lock() LOCK_RETURNED(mu_) { return &mu_; }
  inline Mutex *get_lock2() { return &mu_; }
  static bool compare(Foo& x, Foo& y)
  {
    if (x.a_ > y.a_ && x.b_ > y.b_) { // { dg-warning "Reading variable 'x->a_' requires lock 'x->mu_'" }
      return true;
    }
    else {
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
  w[2].bar->foo[1].a_ = 3; // { dg-warning "Writing to variable 'w\\\[2\\\].bar->foo\\\[1\\\].a_' requires lock 'w\\\[2\\\].bar->foo\\\[1\\\].mu_'" }
  w[2].bar->foo[1].bar(); // { dg-warning " Calling function 'bar' requires lock 'mu2_\\\[0\\\]'" }
  w[1].bar->foo[2].get_lock()->Lock();
  gx = 7;
  y.mu_.Lock();
  p->foo[2].get_lock()->Lock();
  p->foo[2].b_ += 1;
  y.a_ = 2;
  y.mu_.Unlock();
}

// { dg-warning "Reading variable 'y->a_' requires lock 'y->mu_'" "" { target *-*-* } 21 }
// { dg-warning "Reading variable 'x->b_' requires lock 'x->mu_'" "" { target *-*-* } 21 }
// { dg-warning "Reading variable 'y->b_' requires lock 'y->mu_'" "" { target *-*-* } 21 }
// { dg-warning "Lock 'w\\\[1\\\].bar->foo\\\[2\\\].mu_' \\(acquired at line 63\\) is not released at the end of function 'main'" "" { target *-*-* } 63 }
// { dg-warning "Lock 'p->foo\\\[2\\\].mu_' \\(acquired at line 66\\) is not released at the end of function 'main'" "" { target *-*-* } 66 }
