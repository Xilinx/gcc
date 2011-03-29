// Test lock/trylock/unlock annotations with unsupported or unrecognized lock
// names/expressions
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

namespace ns1 {

template <typename T>
class Bar {
 public:
  void Lock() EXCLUSIVE_LOCK_FUNCTION(mu_);
  void Unlock() UNLOCK_FUNCTION(mu_);
  void MyLock() EXCLUSIVE_LOCK_FUNCTION(mu1); // { dg-warning "Unsupported argument of 'exclusive_lock' attribute ignored" }
  void MyUnlock() UNLOCK_FUNCTION(mu1); // { dg-warning "Unsupported argument of 'unlock' attribute ignored" }

  T get_foo() {
    T ret;
    MyLock();
    ret = foo;
    MyUnlock();
    return ret;
  }

  void set_foo(T a) {
    Lock();
    foo = a;
    Unlock();
  }

 private:
  Mutex mu_;
  T foo GUARDED_BY(mu_);
};

int pthread_mutex_lock(Mutex *mu) EXCLUSIVE_LOCK_FUNCTION(mu)
{
  return 5;
}

int pthread_mutex_unlock(Mutex *mu) UNLOCK_FUNCTION(mu);
int pthread_mutex_trylock() EXCLUSIVE_TRYLOCK_FUNCTION(0, (t->mu1)); // { dg-warning "Unsupported argument of 'exclusive_trylock' attribute ignored" }

Mutex mu2;
int p GUARDED_BY(mu2);
int r;

Bar<float> *b1;

void func1()
{
  b1->set_foo(3.5);
  pthread_mutex_lock(&mu2);
  r = b1->get_foo();
  p = r + 5;
  pthread_mutex_unlock(&mu2);
  if (!pthread_mutex_trylock())
  {
    p = 2;
    // Since the annotation on pthread_mutex_trylock contains an unrecognized
    // lock (t->mu1), we don't warn if it is not released at the end of the
    // scope.
  }
  pthread_mutex_lock(&mu2);
  // Since the annotation on Bar<float>::MyUnlock contains an unrecognized
  // lock name, the analysis would conservatively disable the check for
  // mismatched lock acquire/release. Therefore even though mu2 is not
  // released, we don't emit a warning.
  b1->MyUnlock();
}

}
