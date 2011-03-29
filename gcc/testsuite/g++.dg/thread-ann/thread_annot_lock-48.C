// Test the support for use of lock expression in the annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  Mutex foo_mu_;
};

class Bar {
 private:
  Foo *foo;
  Mutex bar_mu_ ACQUIRED_AFTER(foo->foo_mu_);

 public:
  void Test1() {
    bar_mu_.Lock();
    foo->foo_mu_.Lock(); // { dg-warning "is acquired after" }
    bar_mu_.Unlock();
    foo->foo_mu_.Unlock();
  }
};

main() {
  Bar bar;
  bar.Test1();
}
