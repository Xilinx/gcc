// Test lock annotations
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Bar {
 private:
  Mutex mu_;
  void Lock() EXCLUSIVE_LOCK_FUNCTION(mu_) { mu_.Lock(); }
  void Unlock() UNLOCK_FUNCTION(mu_) { mu_.Unlock(); }
  float foo GUARDED_BY(mu_);
 public:
  float get_foo() {
    float ret;
    ret = foo; // { dg-warning "Reading variable 'foo' requires lock 'mu_'" }
    Unlock(); // { dg-warning "Try to unlock 'mu_' that was not acquired" }
    return ret;
  }
};

int pthread_mutex_lock(int i, int j, Mutex *mutex, int k) EXCLUSIVE_LOCK_FUNCTION(3);
int pthread_mutex_unlock(int i, int j, Mutex *mutex, int k) UNLOCK_FUNCTION(3);

Bar *x;
Mutex fastmutex;
float val GUARDED_BY(fastmutex);

main()
{
  val = x->get_foo(); // { dg-warning "Writing to variable 'val' requires lock 'fastmutex'" }
  pthread_mutex_unlock(4, 5, &fastmutex, 6); // { dg-warning "Try to unlock 'fastmutex' that was not acquired" }
}
