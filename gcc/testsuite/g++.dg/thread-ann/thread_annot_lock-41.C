// Test the case where locks from a base class are used in derived classes.
// This is a "good" test that should not incur any compiler warnings.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

class A {
 protected:
  Mutex lock_;
  void BaseMethod() EXCLUSIVE_LOCKS_REQUIRED(lock_) { };
};

class B;
B *foo;

class B : public A {
 public:
  void ChildMethodOne() {
    foo->lock_.Lock();
    foo->BaseMethod();
    lock_.Lock();
    ChildMethodTwo();
    lock_.Unlock();
    foo->lock_.Unlock();
  }

  void ChildMethodTwo() EXCLUSIVE_LOCKS_REQUIRED(lock_) {
    BaseMethod();
  }
};

int main() {
  B b;
  b.ChildMethodOne();
}
