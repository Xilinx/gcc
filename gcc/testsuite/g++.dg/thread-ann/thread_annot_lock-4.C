// Test guarded_by/guarded/pt_guarded_by/pt_guarded annotations
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
Mutex mu2 ACQUIRED_AFTER(mu1);
Mutex mu3 ACQUIRED_AFTER(mu2);
Mutex mu4[5];

class Foo {
 private:
  int a_ GUARDED_BY(mu_);
  float b_ GUARDED_BY(mu_);

 public:
  Mutex mu_ ACQUIRED_AFTER(mu2);
  Mutex *get_lock() LOCK_RETURNED(mu_) __attribute__((pure)) { return &mu_; }
  void incrementA(int i);
  float decrementB(float f);
};

class Bar {
 public:
  Mutex mu_ ACQUIRED_AFTER ((mu4[1]));
  void method1() const;
  void method2();
};

Foo *foo;
Bar bar[2];
int gx GUARDED_BY((mu4[1])) = 3;
float gy GUARDED_BY((foo->mu_)) = 5.0;
int *gp PT_GUARDED_VAR GUARDED_BY((bar[0].mu_));

void func1(void) LOCKS_EXCLUDED(mu1, mu2, mu3);

void func1(void)
{
  int la;
  float *p PT_GUARDED_BY((foo->mu_)) = &gy;

  bar[0].mu_.Lock();
  gp = &gx;
  bar[0].mu_.Unlock();

  mu4[1].ReaderLock();

  if (gx > 3) {
    la = gx + 1;
    mu4[1].Unlock();
  }
  else {
    mu4[1].Unlock();
    foo->get_lock()->Lock();
    *p = foo->decrementB(gy);
    foo->get_lock()->Unlock();
  }

  if (la < 10) {
    MutexLock rl(&bar[0].mu_);
    *gp = 7;
  }
}
