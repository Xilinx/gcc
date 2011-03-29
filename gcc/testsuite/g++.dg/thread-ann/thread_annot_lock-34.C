// Test lockable objects wrapped in smart pointers.
// This is a "good" program that should not incur any compiler warnings.
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
    result = a;
    lock->Unlock();
    lock->Lock();
    return result;
  }

  int GetValue1() {
    int result;
    lock->Lock();
    result = GetA();
    lock->Unlock();
    return result;
  }

  int GetValue2() {
    int result;
    MutexLock l(lock.get());
    result = GetA();
    return result;
  }
};

Foo *foo;
int x;

main()
{
  x = foo->GetValue1();
  x += foo->GetValue2();
}
