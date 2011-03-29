// Test delay parsing of lock attribute arguments with nested classes.
// This is a "good" test that should not incur any compilation wanrings.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

int a = 0;

class Bar {
  struct Foo;

 public:
  Foo *my_trylock() EXCLUSIVE_TRYLOCK_FUNCTION(true, mu);

  int func() {
    const Foo *foo = my_trylock();

    if (foo == 0) {
      return 0;
    }

    a = 5;

    mu.Unlock();

    return 1;
  }

  class FooBar {
    int x;
    int y;
  };

 private:
  Mutex mu;
};

Bar *bar;

main()
{
  bar->func();
}
