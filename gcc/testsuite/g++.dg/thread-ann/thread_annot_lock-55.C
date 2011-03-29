// Test the handling of the annotations with function parameters.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Base {
 private:
  Mutex mu1_;

 public:
  Mutex *mutable_mu() LOCK_RETURNED(mu1_) { return &mu1_; }
};

class Foo {
 public:
  Mutex mu2_;
  void Test1(Mutex* mu) const EXCLUSIVE_LOCKS_REQUIRED(mu, mu2_);
  void Test2(Mutex* mu) const LOCKS_EXCLUDED(mu);
};

class Bar : public Base {
 private:
  Foo foo_;

 public:
  void Test3();
};

void Bar::Test3() {
  MutexLock l(&foo_.mu2_);
  mutable_mu()->Lock();
  foo_.Test1(mutable_mu());
  mutable_mu()->Unlock();
  foo_.Test2(mutable_mu());
}
