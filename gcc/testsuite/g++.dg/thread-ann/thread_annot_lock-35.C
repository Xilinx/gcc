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

     child->Func(new_foo); // There shouldn't be any warning here as the
                           // acquired lock is not in child.
     child->bar(7); // { dg-warning "Calling function 'bar' requires lock" }
     child->a_ = 5; // { dg-warning "Writing to variable 'child->a_' requires lock" }
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
