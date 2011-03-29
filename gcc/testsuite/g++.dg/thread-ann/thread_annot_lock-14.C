// Test lock annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1, mu2;
int x;
int a GUARDED_BY(mu);

void bar(Mutex &mu) EXCLUSIVE_LOCKS_REQUIRED(mu);

void bar(Mutex &mu)
{
  if (x) {
    a = x + 1;
    mu.Unlock();
  }
  else
  {
    // mu.Unlock();
  }
}


void foo()
{
  mu2.Lock();
  bar(mu1); // { dg-warning "Calling function 'bar' requires lock 'mu1'" }
  mu2.Unlock();
}

// { dg-warning "Lock 'mu' \\(held at entry\\) is released on some but not all control flow paths in function 'bar'" "" { target *-*-* } 13 }
