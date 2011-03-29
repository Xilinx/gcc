// Test lock annotations
// This is a "good" test case that should not incur any thread safety warning.
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
    mu.Unlock();
}


void foo()
{
  mu1.Lock();
  bar(mu1);
  mu1.Unlock();
}
