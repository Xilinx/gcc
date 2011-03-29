// Test that when disabling -Wthread-attr-bind-param, the compiler will not
// try to bind the names (used in lock attributes) to function parameters.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wno-thread-attr-bind-param -O" }

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
      foo->mu1_->Lock();  // { dg-warning "not released" }
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
  bar.func1(20, fu);  // no warning here because of -Wno-thread-attr-bind-param
  mu->Unlock();
}
