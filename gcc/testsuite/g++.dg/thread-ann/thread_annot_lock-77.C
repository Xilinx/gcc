// Test that when -Wthread-attr-bind-param is enabled (which is the default),
// the compiler will try to bind the names (used in lock attributes) to
// function parameters.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-attr-bind-param -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  Mutex *mu1_;
};

class Bar {
 public:
  int func1(int a, Foo *foo) EXCLUSIVE_LOCKS_REQUIRED(foo->mu1_) {
    if (a > 1) {
      foo->mu1_->Unlock();
      a = 0;
      foo->mu1_->Lock();
    }
    else {
      a += 1;
    }
    return a;
  }
};

Mutex *mu;
Foo *fu;

void Test1() {
  Bar bar;
  mu->Lock();
  bar.func1(20, fu);  // { dg-warning "Calling function 'func1' requires lock" }
  mu->Unlock();
}
