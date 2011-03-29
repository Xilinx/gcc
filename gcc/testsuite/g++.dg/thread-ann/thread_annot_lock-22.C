// Test various usage of GUARDED_BY and PT_GUARDED_BY annotations, especially
// uses in class definitions.
// This is a "good" test case that should not incur any thread safety warning.
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
  mu.Lock();
  b1.mu1_.Lock();
  res = b1.a_ + b3->b_;
  *p = i;
  b1.a_ = res + b3->b_;
  b3->b_ = *b1.q;
  b1.mu1_.Unlock();
  b1.b_ = res;
  x = res;
  mu.Unlock();
  return x;
}
