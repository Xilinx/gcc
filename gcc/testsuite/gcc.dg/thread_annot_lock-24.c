/* Test lock/trylock/unlock annotations with unsupported or unrecognized lock
   names/expressions.  */
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

static int foo(int i, int j, struct Mutex *mu) EXCLUSIVE_LOCKS_REQUIRED(3)
{
  int *q PT_GUARDED_BY(mu);
  int t = *q;
  *q = 4;
  pthread_mutex_unlock();
  /* Since the annotation on pthread_mutex_unlock contains an
     unrecognized lock name, the analysis would conservatively disable
     the check for mismatched lock acquire/release. Therefore even though
     my_unlock is trying to release mu3 which is never acquired, we don't
     emit a warning.  */
  my_unlock();
  return t;
}

int main()
{
  my_lock();
  foo(2, 3, mu3);
  my_unlock();
  pthread_mutex_lock(2);
  p = r + 5;
  /* Because the annotation on pthread_mutex_lock contains an unrecognized
     lock, we don't emit a warning even though there is no corresponding
     unlock call.  */
}
