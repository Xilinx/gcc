// Test the case where a template member function is annotated with lock
// attributes in a non-template class.
// This is a "good" test that should not incur any compiler warnings.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  void func1(int y) LOCKS_EXCLUDED(mu_);
  template <typename T> void func2(T x) LOCKS_EXCLUDED(mu_);
 private:
  Mutex mu_;
};

Foo *foo;

int main()
{
  foo->func1(5);
  foo->func2(5);
}
