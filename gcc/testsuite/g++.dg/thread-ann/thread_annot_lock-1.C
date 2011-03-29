// Test guarded_by/guarded/pt_guarded_by annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
Mutex mu2 ACQUIRED_AFTER(mu1);
Mutex mu3 ACQUIRED_AFTER(mu2);

int gx  GUARDED_BY(mu1) = 3;
float gy GUARDED_BY(mu2) = 5.0;
int *gp GUARDED_BY(mu3) PT_GUARDED_BY(mu1);
int gw GUARDED_VAR = 4;

class Foo {
 private:
  Mutex mu_ ACQUIRED_AFTER(mu2);
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);

 public:

  Foo() : a_(1) { b_ = 5.0; }

  ~Foo() { a_ = 0; b_ = 0.0; }

  void incrementA(int i)
  {
    a_ += i; // { dg-warning "Reading variable" }
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

  gp = &gx; // { dg-warning "Writing to variable 'gp' requires lock 'mu3'" }

  if (gx > 3) { // { dg-warning "Reading variable 'gx' requires lock 'mu1'" }
    la = gx + gw; // { dg-warning "Reading variable 'gw' requires a lock" }
  }
  else {
    *p = foo.decrementB(gy); // { dg-warning "Reading variable 'gy' requires lock 'mu2'" }
  }

  foo.incrementA(la);

  if (la < 10) {
    *gp = 7; // { dg-warning "Reading variable 'gp' requires lock 'mu3'" }
  }
}

// { dg-warning "Writing to variable" "" { target *-*-* } 30 }
// { dg-warning "Reading variable 'gx' requires lock 'mu1'" "" { target *-*-* } 58 }
// { dg-warning "Calling function 'decrementB' requires lock 'mu1'" "" { target *-*-* } 61 }
// { dg-warning "Access to memory location pointed to by variable 'p' requires lock 'mu2'" "" { target *-*-* } 61 }
// { dg-warning "Access to memory location pointed to by variable 'gp' requires lock 'mu1'" "" { target *-*-* } 67 }
