// Test the case where the member function of a class member object protected 
// by a lock is invoked.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"
#include <memory>

class Foo {
 public:
  Mutex lock;
  std::auto_ptr<int> data GUARDED_BY(lock);

  int GetValue() {
    int result;
    if (data.get()) // { dg-warning "Reading variable 'data' requires lock 'lock'" }
      result = *data.get(); // { dg-warning "Reading variable 'data' requires lock 'lock'" }
    return result;
  }
};

class Bar {
 private:
  Foo *foo;

 public:
  int GetB() {
    if (foo->data.get()) // { dg-warning "Reading variable 'foo->data' requires lock 'foo->lock'" }
      return 1;
    else
      return 2;
  }
};

Foo *foo;
Bar *bar;
int x;

int func()
{
  x = foo->GetValue();
  x += bar->GetB();
}
