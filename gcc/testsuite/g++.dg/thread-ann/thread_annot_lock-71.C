// Test the support for users to specify in the annotations a lock that is a
// class/struct member of a function parameter in templates.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  Mutex *mu1_;
  int data_ GUARDED_BY(mu1_);
};

template<typename T>
class Bar {
 public:
  T *foo_;
  void MyLock(T *f) EXCLUSIVE_LOCK_FUNCTION(f->mu1_);
  void MyUnlock(T *f) UNLOCK_FUNCTION(f->mu1_);
  void func2(T *f) SHARED_LOCKS_REQUIRED(f->mu1_);
  void func3();
};

class SCOPED_LOCKABLE MyMutexLock {
 public:
  MyMutexLock(Foo *f) EXCLUSIVE_LOCK_FUNCTION(f->mu1_)
      : mu_(f->mu1_) {
    this->mu_->Lock();
  }

  ~MyMutexLock() UNLOCK_FUNCTION(){ this->mu_->Unlock(); }

 private:
  Mutex *const mu_;
};

Mutex *mu2;

void func1(Bar<Foo> *bar, Mutex *mu)
  EXCLUSIVE_LOCKS_REQUIRED(bar->foo_->mu1_, mu) {
  bar->foo_->data_ = 5;
}

template<typename T>
void Bar<T>::func3() {
  MyMutexLock l(foo_);
  func2(foo_);
}

main() {
  Bar<Foo> *b;

  MutexLock l(mu2);
  b->MyLock(b->foo_);
  func1(b, mu2);
  b->MyUnlock(b->foo_);
  b->func3();
  b->foo_->mu1_->Lock();
  func1(b, mu2);
  b->foo_->mu1_->Unlock();
}
