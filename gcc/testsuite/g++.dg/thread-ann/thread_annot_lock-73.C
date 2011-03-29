// Test the support for users to specify in the annotations a lock that is a
// function parameter in templates.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

template<typename T>
class Foo {
 public:
  T func1(T a, Mutex *mutex) EXCLUSIVE_LOCKS_REQUIRED(mutex);
};

template<typename T>
T Foo<T>::func1(T a, Mutex *mutex) {
  if (a > 1) {
    mutex->Unlock();
    a = 0;
    mutex->Lock();
  }
  else {
    a += 1;
  }
  return a;
}

Mutex *mu;

main() {
  Foo<int> foo;
  mu->Lock();
  foo.func1(20, mu);
  mu->Unlock();
}
