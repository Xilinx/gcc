// Test guarded_by/guarded/pt_guarded_by/pt_guarded annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
Mutex mu2 ACQUIRED_AFTER(mu1);
Mutex mu3 ACQUIRED_AFTER(mu2);
Mutex mu4[5];

class Foo {
 private:
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);

 public:
  Mutex mu_ ACQUIRED_AFTER(mu2);
  Mutex *get_lock() __attribute__((pure)) LOCK_RETURNED(mu_) { return &mu_; }
  void incrementA(int i);
  float decrementB(float f);
};

class Bar {
 public:
  Mutex mu_ ACQUIRED_AFTER ((mu4[1]));
  void method1() const;
  void method2();
};

Foo *foo;
Bar bar[2];
int gx GUARDED_BY((mu4[1])) = 3;
float gy GUARDED_BY((foo->mu_)) = 5.0;
int *gp PT_GUARDED_VAR GUARDED_BY((bar[0].mu_));

void func1(void) LOCKS_EXCLUDED(mu1, mu2, mu3);

void func1(void)
{
  int la;
  float *p PT_GUARDED_BY((foo->mu_)) = &gy;

  gp = &gx; // { dg-warning "Writing to variable 'gp' requires lock 'bar\\\[0\\\].mu_'" }

  if (gx > 3) { // { dg-warning "Reading variable 'gx' requires lock 'mu4\\\[1\\\]'" }
    la = gx + 1; // { dg-warning "Reading variable 'gx' requires lock 'mu4\\\[1\\\]'" }
  }
  else {
    *p = foo->decrementB(gy); // { dg-warning "Reading variable 'gy' requires lock 'foo->mu_'" }
  }

  if (la < 10) {
    *gp = 7; // { dg-warning "Reading variable 'gp' requires lock 'bar\\\[0\\\].mu_'" }
  }
}

// { dg-warning "Access to memory location pointed to by variable 'p' requires lock 'foo->mu_'" "" { target *-*-* } 50 }
// { dg-warning "Access to memory location pointed to by variable 'gp' requires a lock" "" { target *-*-* } 54 }
