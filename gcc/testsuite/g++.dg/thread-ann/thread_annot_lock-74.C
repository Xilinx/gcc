// Test a bug fix that resolves a compiler complaint on using a private member
// of a friend class in the lock annotations.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

namespace ns {
class Bar;
}

class Foo {
 private:
  Mutex *mymu_;
  friend class ns::Bar;
};

namespace ns {

class Bar {
  Foo *foo;
  int data;

 public:
  void func(Foo *fu);
};

void Bar::func(Foo *fu) EXCLUSIVE_LOCKS_REQUIRED(fu->mymu_) {
  data = 5;
}

void Test() {
  Foo foo;
  Bar bar;
  bar.func(&foo); // { dg-warning "requires lock" }
}

}
