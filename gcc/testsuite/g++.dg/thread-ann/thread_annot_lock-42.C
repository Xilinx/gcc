// Test support of multiple lock attributes of the same kind on a decl.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
 private:
  Mutex mu1, mu2, mu3;
  int x GUARDED_BY(mu1) GUARDED_BY(mu3); // { dg-warning "ignored" }
  int y GUARDED_BY(mu2);

  void f2() LOCKS_EXCLUDED(mu1) LOCKS_EXCLUDED(mu2) LOCKS_EXCLUDED(mu3) {
    mu2.Lock();
    y = 2;
    mu2.Unlock();
  }

 public:
  void f1() EXCLUSIVE_LOCKS_REQUIRED(mu2) EXCLUSIVE_LOCKS_REQUIRED(mu1) {
    x = 5;
    f2(); // { dg-warning "Cannot call function 'f2' with lock 'mu1' held" }
  }
};

Foo *foo;

void func()
{
  foo->f1(); // { dg-warning "Calling function 'f1' requires lock 'foo->mu2'" }
}

// { dg-warning "Cannot call function 'f2' with lock 'mu2' held" "" { target *-*-* } 22 }
// { dg-warning "Calling function 'f1' requires lock 'foo->mu1'" "" { target *-*-* } 30 }
