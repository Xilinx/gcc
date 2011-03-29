// Test the case where locks are members of the parent classes. We used to
// have problems handling such cases as the canonical forms of the lock
// expressions are not consistent.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
  Mutex *mu_;
 public:
  Mutex* GetMu() LOCK_RETURNED(mu_);
};

class FooChild : public Foo {
 public:
  int a_ GUARDED_BY(mu_);
  void Func2() EXCLUSIVE_LOCKS_REQUIRED(mu_);
};

class FooChildChild : public FooChild {
 public:
  int x_ GUARDED_BY(mu_);
};

class Bar {
  FooChild *cfoo_;
  FooChildChild *ccfoo_;
  int b_;

 public:
  void Func1() LOCKS_EXCLUDED(cfoo_->GetMu()) {
    MutexLock l(cfoo_->GetMu());
    b_ = cfoo_->a_;
    cfoo_->Func2();
    ccfoo_->GetMu()->Lock();
    ccfoo_->x_ = b_;
    ccfoo_->GetMu()->Unlock();
  }
};

int main() {
  Bar bar;
  bar.Func1();
}
