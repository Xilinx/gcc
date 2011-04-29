// { dg-do compile }
// { dg-options "-Wnonnull" }

#include <stddef.h>

class Foo {
  char *name;
 public:
  void func1(const int *ptr) __attribute__((nonnull(2))) {}
  Foo(char *str) __attribute__((nonnull)) : name(str) {}
};

void Bar() {
  Foo foo(NULL);   // { dg-warning "null argument where non-null required" }
  foo.func1(NULL); // { dg-warning "null argument where non-null required" }
}
