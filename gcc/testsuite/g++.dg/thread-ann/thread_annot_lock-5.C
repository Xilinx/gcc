// Test lock annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

extern Mutex mu3;

Mutex mu1 ACQUIRED_AFTER(mu3);
Mutex mu2 ACQUIRED_AFTER(mu1);
Mutex mu4;
Mutex mu3 ACQUIRED_AFTER(mu2, mu4);

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

  float updateB(float f) LOCKS_EXCLUDED(mu_, mu3) SHARED_LOCKS_REQUIRED(mu1)
                         EXCLUSIVE_LOCKS_REQUIRED(mu2)
  {
    float res;

    mu3.Lock(); // { dg-warning "There is a cycle in the acquisition order between locks 'mu2' and 'mu3'" }
    incrementA(*gp);
    mu3.Unlock();

    mu1.Lock(); // { dg-warning "Try to acquire lock 'mu1' that is already held \\(at function entry\\)" }
    mu_.Lock();
    if (gx > 2)
      b_ -= f;
    else
      b_ -= ++gy;
    res = b_;
    mu_.Unlock();
    mu1.Unlock();
    return res;
  }
};

void func1(void) LOCKS_EXCLUDED(mu1, mu2, mu3);

void func1(void)
{
  int la;
  float *p PT_GUARDED_BY(mu2) = &gy;
  Foo foo;

  if (la > 3) {
    mu1.ReaderLock();
    la = gx + 1;
    mu1.Unlock();
  }
  else {
    mu2.Lock();
    mu3.ReaderLock(); // { dg-warning "There is a cycle in the acquisition order between locks 'mu2' and 'mu3'" }

    *p = foo.updateB(gy); // { dg-warning "Calling function 'updateB' requires lock 'mu1'" }

    mu3.Unlock();
  }

  mu2.Unlock(); // { dg-warning "Try to unlock 'mu2' that was not acquired" }

  mu3.Lock();
  mu4.Lock();

  gp = &gx;
  gz += 2;

  mu4.Unlock();
}

// { dg-warning "There is a cycle in the acquisition order between locks 'mu1' and 'mu3'" "" { target *-*-* } 46 }
// { dg-warning "Lock 'mu2' \\(acquired at line 77\\) is not released at the end of its scope in function 'func1'" "" { target *-*-* } 77 }
// { dg-warning "Cannot call function 'updateB' with lock 'mu3' held \\(previously acquired at line 78\\)" "" { target *-*-* } 80 }
// { dg-warning "Lock 'mu3' \\(acquired at line 87\\) is not released at the end of function 'func1'" "" { target *-*-* } 87 }
// { dg-warning "Lock 'mu4' is acquired after lock 'mu3' \\(acquired at line 87\\) but is annotated otherwise" "" { target *-*-* } 88 }
