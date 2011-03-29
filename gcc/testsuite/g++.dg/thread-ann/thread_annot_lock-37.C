// Test the case where a template member function is annotated with lock
// attributes in a non-template class.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  void func1(int y) EXCLUSIVE_LOCKS_REQUIRED(mu_);
  template <typename T> void func2(T x) LOCKS_EXCLUDED(mu_);
  Mutex mu_;
};

Foo *foo;

int main()
{
  foo->mu_.Lock();
  foo->func1(5);
  foo->func2(5); // { dg-warning "Cannot call function 'func2' with lock 'foo->mu_' held" }
  foo->mu_.Unlock();
}
