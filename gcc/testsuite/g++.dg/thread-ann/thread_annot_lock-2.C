// Test guarded_by/guarded/pt_guarded_by annotations
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
Mutex mu2 ACQUIRED_AFTER(mu1);
Mutex mu3 ACQUIRED_AFTER(mu2);

int gx  GUARDED_BY(mu1) = 3;
float gy GUARDED_BY(mu2) = 5.0;
int *gp GUARDED_BY(mu3) PT_GUARDED_BY(mu1);
int gw GUARDED_VAR = 2;

class Foo {
 private:
  Mutex mu_ ACQUIRED_AFTER(mu2);
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);

 public:

  Foo() : a_(1) { b_ = 5.0; }

  ~Foo() { a_ = 0; b_ = 0.0; }

  void incrementA(int i) LOCKS_EXCLUDED(mu_)
  {
    mu_.Lock();
    a_ += i;
    mu_.Unlock();
  }

  float decrementB(float f) LOCKS_EXCLUDED(mu_) SHARED_LOCKS_REQUIRED(mu1)
  {
    float res;
    mu_.Lock();
    if (gx > 2)
      b_ -= f;
    else
      b_ -= 2 * f;
    res = b_;
    mu_.Unlock();
    return res;
  }
};

void func1(void) LOCKS_EXCLUDED(mu1, mu2, mu3);

void func1(void)
{
  int la;
  float *p PT_GUARDED_BY(mu2) = &gy;
  Foo foo;

  mu3.Lock();
  gp = &gx;
  mu3.Unlock();

  mu1.ReaderLock();

  if (gx > 3) {
    la = gx + gw;
  }
  else {
    mu2.Lock();
    *p = foo.decrementB(gy);
    mu2.Unlock();
  }

  foo.incrementA(gx);

  mu1.Unlock();

  if (la < 10) {
    MutexLock l(&mu1);
    ReaderMutexLock rl(&mu3);
    *gp = 7;
  }
}
