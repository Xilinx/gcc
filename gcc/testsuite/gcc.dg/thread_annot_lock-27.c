/* Test lock annotations applied to function definitions. This is a "good"
   test that should not incur any compilation warnings.  */
/* { dg-do compile } */
/* { dg-options "-Wthread-safety -O" } */

#include "thread_annot_common_c.h"

int pthread_mutex_lock(struct Mutex *mu) EXCLUSIVE_LOCK_FUNCTION(1);
int pthread_mutex_unlock(struct Mutex *mu) UNLOCK_FUNCTION(1);

struct Mutex mu1;
struct Mutex mu2 ACQUIRED_AFTER(mu1);

static int foo(int i) EXCLUSIVE_LOCKS_REQUIRED(mu2);

int bar(int i) LOCKS_EXCLUDED(mu2)
{
  return i;
}

static int foo(int i) SHARED_LOCKS_REQUIRED(mu1)
{
  int result;
  pthread_mutex_unlock(&mu2);
  result = bar(i);
  pthread_mutex_lock(&mu2);
  return result;
}

main()
{
  pthread_mutex_lock(&mu1);
  pthread_mutex_lock(&mu2);
  foo(2);
  pthread_mutex_unlock(&mu2);
  pthread_mutex_unlock(&mu1);
}
