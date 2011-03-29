// Test lock annotations and analysis escape hatches
// This is a "good" test case that should not incur any thread safety warning.
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
    Lock();
    ret = foo;
    Unlock();
    return ret;
  }

  // Thread safety analysis will skip this function
  void set_foo(float a) NO_THREAD_SAFETY_ANALYSIS {
    // Lock();
    foo = a;
    Unlock();
  }
};

int pthread_mutex_lock(int i, int j, Mutex *mutex, int k) EXCLUSIVE_LOCK_FUNCTION(3);
int pthread_mutex_unlock(int i, int j, Mutex *mutex, int k) UNLOCK_FUNCTION(3);

Bar *x;
Mutex fastmutex;
float val GUARDED_BY(fastmutex);

main()
{
  pthread_mutex_lock(1, 2, &fastmutex, 3);
  x->set_foo(2.5);
  val = x->get_foo();
  pthread_mutex_unlock(4, 5, &fastmutex, 6);
}
