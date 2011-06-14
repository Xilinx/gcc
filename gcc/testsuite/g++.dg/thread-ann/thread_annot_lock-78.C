// Test the support to handle cloned methods.
// { dg-do compile }
// { dg-options "-O2 -Wthread-safety -fipa-sra" }

#include "thread_annot_common.h"

struct A {
  int *data_ PT_GUARDED_BY(mu_);
  mutable Mutex mu_;
  void Reset(void) EXCLUSIVE_LOCKS_REQUIRED(mu_) {
    delete data_;
  }
};

struct B {
  A a_;
  void TestFunc1();
  void TestFunc2();
};

void B::TestFunc1() {
  MutexLock l(&a_.mu_);
  a_.Reset();  // This call is an IPA-SRA clone candidate.
}

void B::TestFunc2() {
  a_.Reset();  // { dg-warning "Calling function 'Reset' requires lock" }
}
