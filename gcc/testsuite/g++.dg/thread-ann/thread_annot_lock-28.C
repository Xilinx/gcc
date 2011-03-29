// Test the unlock attribute that takes a lock name not in scope when parsing.
// The late binding mechanism should be able to match the lock name with the
// decl in the analysis. This is a "good" test that should not incur any
// compilation wanrings.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

class FooBar {
 public:
  bool Foo();
 private:
  void ReleaseFooBar() UNLOCK_FUNCTION(my_lock);
};

static Mutex my_lock;

int a = 0;

bool FooBar::Foo() {
  my_lock.Lock();
  if (a) {
    a += 1;
    my_lock.Unlock();
    return true;
  }
  else {
    a += 2;
    ReleaseFooBar();
    return false;
  }
}

void FooBar::ReleaseFooBar() {
  my_lock.Unlock();
}
