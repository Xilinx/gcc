/* Test guarded_by/pt_guarded_by annotations with unsupported or unrecognized
   lock names/expressions.  */
/* { dg-do compile } */
/* { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" } */

#include "thread_annot_common_c.h"

int pthread_mutex_lock(struct Mutex *mu, int i) EXCLUSIVE_LOCK_FUNCTION(1);
int pthread_mutex_unlock(struct Mutex *mu) UNLOCK_FUNCTION(1);
int pthread_mutex_trylock() EXCLUSIVE_TRYLOCK_FUNCTION(0, t->mu1); /* { dg-warning "Unsupported argument of 'exclusive_trylock' attribute ignored" } */

struct Bar {
  float x_ GUARDED_BY(((mu1_))); /* { dg-warning "'guarded_by' attribute downgraded to 'guarded'" } */
  struct Mutex mu1_ ACQUIRED_AFTER(mu2);
  float a_ GUARDED_BY(foo_->mu_); /* { dg-warning "'guarded_by' attribute downgraded to 'guarded'" } */
};

int p GUARDED_BY(a->mu); /* { dg-warning "'guarded_by' attribute downgraded to 'guarded'" } */
int *r PT_GUARDED_BY(f1.mu_); /* { dg-warning "'point_to_guarded_by' attribute downgraded to 'point_to_guarded'" } */

struct Bar *b1;
struct Mutex *mu3;

int foo(int i, int j, struct Mutex *mu) EXCLUSIVE_LOCKS_REQUIRED(3)
{
  int *q PT_GUARDED_BY(mu);
  int t = *q;
  *q = 4;
  return t;
}

int main()
{
  pthread_mutex_lock(&b1->mu1_, 2);
  p = *r + 5;
  b1->x_ = 3;
  pthread_mutex_unlock(&b1->mu1_);
}
