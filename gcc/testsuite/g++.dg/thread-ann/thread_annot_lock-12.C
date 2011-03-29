// Test lock annotations
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

extern Mutex mu1;

Mutex mu2 ACQUIRED_AFTER(mu1);
Mutex mu1 ACQUIRED_BEFORE(mu2);
Mutex mu3 ACQUIRED_AFTER(mu2);
Mutex mu4 ACQUIRED_BEFORE(mu2, mu3);

int gx GUARDED_BY(mu1) = 3;
float gy GUARDED_BY(mu2) = 5.0;
int gz GUARDED_BY(mu4) = 2;
int *gp GUARDED_BY(mu3) PT_GUARDED_BY(mu1);

class Foo {
 private:
  Mutex mu_ ACQUIRED_AFTER(mu3);
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);

 public:

  Foo() : a_(1) { b_ = 5.0; }

  ~Foo() { a_ = 0; b_ = 0.0; }

  int incrementA(int i) LOCKS_EXCLUDED(mu_)
  {
    int res;
    mu_.Lock();
    a_ += i;
    res = a_;
    mu_.Unlock();
    return res;
  }

  float updateB(float f) LOCKS_EXCLUDED(mu_) SHARED_LOCKS_REQUIRED(mu1, mu3)
                         EXCLUSIVE_LOCKS_REQUIRED(mu2)
  {
    float res;

    incrementA(*gp);

    mu_.Lock();
    if (gx > 2)
      b_ -= f;
    else
      b_ -= ++gy;
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


  mu1.ReaderLock();

  if (gx > 3) {
    la = gx + 1;
  }
  else {
    mu2.Lock();
    mu3.ReaderLock();

    *p = foo.updateB(gy);

    mu3.Unlock();
    mu2.Unlock();
  }

  mu1.Unlock();

  mu4.Lock();
  mu3.Lock();

  gp = &gx;
  gz += 2;

  mu3.Unlock();
  mu4.Unlock();
}
