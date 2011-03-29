// Test handling of arguments passed to reference parameters.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

struct Foo {
  int a;
};

void func1(Foo &f);

void func2(Foo *f);

Mutex mu;

Foo foo GUARDED_BY(mu);

main() {
  func1(foo);  // { dg-warning "Reading variable 'foo' requires lock" }
  func2(&foo); // should not warn
}
