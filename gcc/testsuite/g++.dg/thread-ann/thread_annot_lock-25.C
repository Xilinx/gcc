// Test function lock requirement annotations with unsupported or unrecognized
// lock names/expressions
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

namespace ns1 {

struct Foo {
  Mutex mu_;
};

template <typename T>
class Bar {
 private:
  Mutex mu_;
  void Lock() EXCLUSIVE_LOCK_FUNCTION(mu_);
  void Unlock() UNLOCK_FUNCTION(mu_);
  T foo GUARDED_BY(mu_);
  Foo *x;

 public:
  Mutex *GetLock1() LOCK_RETURNED(mu_);
  Mutex *GetLock2() LOCK_RETURNED(x->mu_);
  Mutex *GetLock3() LOCK_RETURNED(y->mu_); // { dg-warning "'lock_returned' attribute ignored due to the unsupported argument" }

  T get_foo() SHARED_LOCKS_REQUIRED(x->mu_, (mu2)) { // { dg-warning "Unsupported argument of 'shared_locks_required' attribute ignored" }
    T ret;
    ret = foo;
    return ret;
  }

  void set_foo(T a) EXCLUSIVE_LOCKS_REQUIRED((mu_), GetLock2()) { // { dg-warning "Unsupported argument of 'exclusive_locks_required' attribute ignored" }
    foo = a;
  }
};

Mutex mu2;
float r;
Bar<float> *b1;

void func1()
{
  {
    MutexLock ml(b1->GetLock1());
    b1->set_foo(3.5);
    r = b1->get_foo(); // { dg-warning "Calling function 'get_foo' requires lock 'x->mu_'" }
  }
  b1->GetLock2()->Lock();
  r += b1->get_foo();
  b1->GetLock2()->Unlock();
}

}
