// Test trylock annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
 private:
  Mutex mu_;
  Mutex mu2_;
  int a_ GUARDED_BY(mu_);
  int b_ GUARDED_BY(mu2_);
 public:
  bool func(int y)
  {
    int x;
    x = y - 2;
    if (mu_.TryLock()) {
      a_ = x * 3;
      if (mu2_.TryLock()) {
        b_ += y;
        mu2_.Unlock();
      }
      mu_.Unlock();
      return true;
    }
    mu_.Unlock(); // { dg-warning "Try to unlock 'mu_' that was not acquired" }
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
  if (d)
    {
      return 1;
    }
  foo->func(2); // { dg-warning "Reading variable 'foo' requires lock 'mu1'" }
  mu1.Unlock(); // { dg-warning "Try to unlock 'mu1' that was not acquired" }
  return 0;
}

// { dg-warning "Lock 'mu1' \\(acquired at line 39\\) is not released at the end of its scope in function 'main'" "" { target *-*-* } 39 }
