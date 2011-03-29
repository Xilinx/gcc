// Test lock canonicalization when populating the initial lock sets of a
// function. It locks are properly canonicalized, the analysis should not
// complained about a_ in function FooBar::GetA() is not protected.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  Mutex *mu_;
};

class FooBar {
 public:
  Foo *foo_;
  int GetA() EXCLUSIVE_LOCKS_REQUIRED(foo_->mu_) { return a_; }
  int a_ GUARDED_BY(foo_->mu_);
};

FooBar *fb;

main()
{
  int x;
  fb->foo_->mu_->Lock();
  x = fb->GetA();
  fb->foo_->mu_->Unlock();
}
