// Test the support for releasable scoped lock (e.g std::unique_lock).
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

extern void bar();

Mutex mu1, mu2;
int a GUARDED_BY(mu1);

void foo(int x) {
  if (x > 2) {
    ReleasableMutexLock l(&mu1);
    if (a < 3) {
      a = x + 1;
      l.Release();
      bar();
    }
    else {
      a = x + 2;
    }
  }
}

void func(int x) {
  ReleasableMutexLock l(&mu1);
  ReleasableMutexLock m(&mu2);
  switch (x) {
    case 1:
      {
        a = x + 1;
        l.Release();
        bar();
        break;
      }
    case 3:
      {
        a = x + 3;
        m.Release();
        break;
      }
    case 2:
      {
        a = x + 2;
        l.Release();
        bar();
        break;
      }
    default:
      {
        a = x + 3;
        break;
      }
  }
}
