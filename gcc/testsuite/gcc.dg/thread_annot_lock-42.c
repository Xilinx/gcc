// Test support of multiple lock attributes of the same kind on a decl.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common_c.h"

struct Mutex mu1, mu2, mu3;

int x GUARDED_BY(mu1) GUARDED_BY(mu3); // { dg-warning "ignored" }
int y;

void f2() LOCKS_EXCLUDED(mu1) LOCKS_EXCLUDED(mu2) LOCKS_EXCLUDED(mu3);

void f2()
{
  y = 2;
}

void f1() EXCLUSIVE_LOCKS_REQUIRED(mu2) EXCLUSIVE_LOCKS_REQUIRED(mu1)
{
  x = 5;
  f2(); // { dg-warning "Cannot call function 'f2' with lock 'mu1' held" "annotalysis disabled in google/main" { xfail *-*-* } }
}

void func()
{
  f1(); // { dg-warning "Calling function 'f1' requires lock 'mu2'" "annotalysis disabled in google/main" { xfail *-*-* } }
}
