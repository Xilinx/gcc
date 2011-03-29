// Test the ability to distinguish between the same lock field of
// different objects of a class.
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Bar {
 public:
  bool MyTryLock() EXCLUSIVE_TRYLOCK_FUNCTION(true, mu1_);
  void MyUnlock() UNLOCK_FUNCTION(mu1_);
  int a_ GUARDED_BY(mu1_);

 private:
  Mutex mu1_;
};

Bar *b1, *b2;

void func()
{
  if (b1->MyTryLock()) {
    b1->a_ = 5;
    if (b2->MyTryLock()) {
      b2->a_ = 3;
      b2->MyUnlock();
    }
    b1->MyUnlock();
  }
}
