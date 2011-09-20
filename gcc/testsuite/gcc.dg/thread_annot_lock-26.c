/* Test lock annotations applied to function definitions.  */
/* { dg-do compile } */
/* { dg-options "-Wthread-safety -O" } */

#include "thread_annot_common_c.h"

struct Mutex mu1;
struct Mutex mu2 ACQUIRED_AFTER(mu1);

static int foo(int i) EXCLUSIVE_LOCKS_REQUIRED(mu2);

int bar(int i) LOCKS_EXCLUDED(mu1)
{
  return i;
}

static int foo(int i) SHARED_LOCKS_REQUIRED(mu1)
{
  return bar(i); /* { dg-warning "Cannot call function 'bar' with lock 'mu1' held" "annotalysis disabled in google/main" { xfail *-*-* } } */
}

main()
{
  foo(2); /* { dg-warning "Calling function 'foo' requires lock 'mu2'" "annotalysis disabled in google/main" { xfail *-*-* } } */
}
