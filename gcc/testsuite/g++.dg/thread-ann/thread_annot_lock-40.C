// Test the case where the member function of a class member object protected 
// by a lock is invoked.
// This is a "good" test that should not incur any compiler warnings.
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
    MutexLock l(&lock);
    if (data.get())
      result = *data.get();
    return result;
  }
};

class Bar {
 private:
  Foo *foo;

 public:
  int GetB() {
    MutexLock l(&foo->lock);
    if (foo->data.get())
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
