/* Test function lock requirement annotations with unsupported or unrecognized
   lock names/expressions.  */
/* { dg-do compile } */
/* { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" } */

#include "thread_annot_common_c.h"

int pthread_mutex_lock(int i) EXCLUSIVE_LOCK_FUNCTION(((&f)->mu)); /* { dg-warning "Unsupported argument of 'exclusive_lock' attribute ignored" } */
int pthread_mutex_unlock() UNLOCK_FUNCTION(f[0].mu); /* { dg-warning "Unsupported argument of 'unlock' attribute ignored" } */
int pthread_mutex_trylock() EXCLUSIVE_TRYLOCK_FUNCTION(0, t->mu1); /* { dg-warning "Unsupported argument of 'exclusive_trylock' attribute ignored" } */

struct Mutex *mu3;
int p GUARDED_BY(mu3);
int r GUARDED_BY(mu3);

void my_lock() EXCLUSIVE_LOCK_FUNCTION(mu3);
void my_unlock() UNLOCK_FUNCTION(mu3);

extern void foo(int i, int j, struct Mutex *mu) EXCLUSIVE_LOCKS_REQUIRED(3);

void foo(int i, int j, struct Mutex *mu) LOCKS_EXCLUDED(t->mu3)
{ /* { dg-warning "Unsupported argument of 'locks_excluded' attribute ignored" } */
  int *q PT_GUARDED_BY(mu);
  my_lock();
  p = *q;
  *q = 4;
  my_unlock();
}

int bar() SHARED_LOCKS_REQUIRED(y->mu); /* { dg-warning "Unsupported argument of 'shared_locks_required' attribute ignored" } */

int bar()
{
  int t = r;
  return t;
}

main()
{
  foo(2, 3, mu3); /* { dg-warning "Calling function 'foo' requires lock 'mu3'" } */
  my_lock();
  foo(2, 3, mu3);
  bar();
  my_unlock();
  bar(); /* { dg-warning "Calling function 'bar' requires a lock" } */
}
