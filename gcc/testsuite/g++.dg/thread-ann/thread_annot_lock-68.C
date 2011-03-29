// Test a fix to a bug in the delayed name binding with nested template
// instantiation. We use a stack to make sure a name is not resolved to an
// inner context.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

template <typename T>
class Bar {
  Mutex mu_;
};

template <typename T>
class Foo {
 public:
  void func(T x) {
    MutexLock l(&mu_);
    count_ = x;
  }

 private:
  T count_ GUARDED_BY(mu_);
  Bar<T> bar_;
  Mutex mu_;
};

int main()
{
  Foo<int> *foo;
  foo->func(5);
}
