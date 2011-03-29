// Test lockable objects wrapped in smart pointers.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"
#include <memory>

using namespace std;

class Foo {
 private:
  auto_ptr<Mutex> lock;
  int a GUARDED_BY(lock);

 public:
   int GetA() EXCLUSIVE_LOCKS_REQUIRED(lock) {
    int result;
    lock->Unlock();
    result = a; // { dg-warning "Reading variable 'a' requires lock 'lock'" }
    lock->Lock();
    return result;
  }

  int GetValue() {
    int result;
    result = GetA(); // { dg-warning "Calling function 'GetA' requires lock 'lock'" }
    lock->Unlock(); // { dg-warning "Try to unlock 'lock' that was not acquired" }
    return result;
  }
};

Foo *foo;
int x;

main()
{
  x = foo->GetValue();
}
