// Test the analyzer's ability to distinguish the lock field of different
// objects.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

class Foo {
 private:
  Mutex lock_;
  int a_ GUARDED_BY(lock_);

 public:
  void Func(Foo* child) LOCKS_EXCLUDED(lock_) {
     Foo *new_foo = new Foo;

     MutexLock l(&lock_);

     child->lock_.Lock();
     child->Func(new_foo); // { dg-warning "Cannot call function 'Func' with lock 'child->lock_' held" }
     child->bar(7);
     child->a_ = 5;
     child->lock_.Unlock();
  }

  void bar(int y) EXCLUSIVE_LOCKS_REQUIRED(lock_) {
    a_ = y;
  }
};

Foo *x;

main() {
  Foo *child = new Foo;
  x->Func(child);
}
