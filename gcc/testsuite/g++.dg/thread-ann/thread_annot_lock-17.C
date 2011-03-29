// Test trylock annotations
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu3;

class Foo {
 private:
  Mutex mu_;
  Mutex mu2_;
  int a_ GUARDED_BY(mu_);
  int b_ GUARDED_BY(mu2_);
  bool MyTryLock() EXCLUSIVE_TRYLOCK_FUNCTION(false, mu2_) {
    return !mu2_.TryLock();
  }
  int MyTryLock2(int a, float c, Mutex *lock1, Mutex *lock2)
      EXCLUSIVE_TRYLOCK_FUNCTION(2, lock2, lock1)
  {
    if (lock2->TryLock() && lock1->TryLock())
      return 2;
    else
      return 0;
  }
 public:
  bool func(int y)
  {
    int x;
    x = y - 2;
    if (mu_.TryLock()) {
      a_ = x * 3;
      if (!MyTryLock()) {
        b_ += y;
        mu2_.Unlock();
      }
      mu_.Unlock();
      return true;
    }
    int ret = MyTryLock2(17, 0.5, &mu2_, &mu3);
    bool cond = ret == 2;
    bool t = !cond;
    bool s = !t;
    if (s) {
      b_ += y;
      mu2_.Unlock();
      mu3.Unlock();
    }
    return false;
  }
};

Mutex mu1 ;

Foo *foo GUARDED_BY(mu1);

int main()
{
  bool a, b, c, d;
  a = mu1.TryLock();
  b = !a;
  c = b;
  d = !c;
  if (!d)
    {
      return 1;
    }
  foo->func(2);
  mu1.Unlock();
  return 0;
}
