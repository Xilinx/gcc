// Test that the compiler tolerates the use of incomplete type in the lock
// annotations (i.e. doesn't emit a error on invalid use of incomplete type).
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo;

class Bar {
 public:
  void func2(Foo& old_foo) EXCLUSIVE_LOCKS_REQUIRED(old_foo.mu1_);
  Mutex *mu2_;
};

class Foo {
 public:
  void func1(Bar& old_bar) EXCLUSIVE_LOCKS_REQUIRED(old_bar.mu2_);
  Mutex *mu1_;
};

void Test() {
  Foo foo1, foo2;
  Bar bar1, bar2;
  foo2.func1(bar1); // { dg-warning "requires a lock" }
  bar2.func2(foo1); // { dg-warning "requires a lock" }
}
