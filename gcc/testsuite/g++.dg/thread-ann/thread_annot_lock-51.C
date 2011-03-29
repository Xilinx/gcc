// Test the support for allowing non-const but non-modifying methods to be
// protected by reader locks.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Foo {
 public:
  int GetVal1(int a) const {
    return a + val1;
  }

  int GetVal1(int a) {
    return a + val1;
  }

  int GetVal1(int a, float b) {
    return a + b + val1;
  }

  int GetVal2(int a) {
    return a + val2;
  }

  int GetVal2(float a) {
    return val2;
  }


 private:
  int val1;
  int val2;
};

Mutex mu;
Foo foo GUARDED_BY(mu);

int main() {
  mu.ReaderLock();
  int x = foo.GetVal1(3); // should not warn
  int y = foo.GetVal2(3); // { dg-warning "Writing to variable" }
  mu.Unlock();
}
