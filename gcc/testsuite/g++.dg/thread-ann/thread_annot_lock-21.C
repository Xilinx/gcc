// Test various usage of GUARDED_BY and PT_GUARDED_BY annotations, especially
// uses in class definitions.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu;

class Bar {
 public:
  int a_ GUARDED_BY(mu1_);
  int b_;
  int *q PT_GUARDED_BY(mu);
  Mutex mu1_ ACQUIRED_AFTER(mu);
};

Bar b1, *b3;
int *p GUARDED_BY(mu) PT_GUARDED_BY(mu);

int res GUARDED_BY(mu) = 5;

int func(int i)
{
  int x;
  b3->mu1_.Lock();
  res = b1.a_ + b3->b_; // { dg-warning "Reading variable 'b1.a_' requires lock 'b1.mu1_'" }
  *p = i; // { dg-warning "Reading variable 'p' requires lock 'mu'" }
  b1.a_ = res + b3->b_; // { dg-warning "Reading variable 'res' requires lock 'mu'" }
  b3->b_ = *b1.q; // { dg-warning "Access to memory location pointed to by variable 'b1.q' requires lock 'mu'" }
  b3->mu1_.Unlock();
  b1.b_ = res; // { dg-warning "Reading variable 'res' requires lock 'mu'" }
  x = res; // { dg-warning "Reading variable 'res' requires lock 'mu'" }
  return x;
}

// { dg-warning "Writing to variable 'res' requires lock 'mu'" "" { target *-*-* } 27 }
// { dg-warning "Access to memory location pointed to by variable 'p' requires lock 'mu'" "" { target *-*-* } 28 }
// { dg-warning "Writing to variable 'b1.a_' requires lock 'b1.mu1_'" "" { target *-*-* } 29 }
