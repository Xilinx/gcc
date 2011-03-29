// Test annotations on out-of-line definitions of member functions where the
// annotations refer to locks that are also data members in the class.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu;

class Foo {
 public:
  int method1(int i);
  int data GUARDED_BY(mu1);
  Mutex *mu1;
  Mutex *mu2;
};

int Foo::method1(int i) SHARED_LOCKS_REQUIRED(mu1, mu, mu2)
{
  return data + i;
}

main()
{
  Foo a;

  MutexLock l(a.mu2);
  a.mu1->Lock();
  mu.Lock();
  a.method1(1);
  mu.Unlock();
  a.mu1->Unlock();
}
