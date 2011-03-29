// Test lock annotations applied to function definitions
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu1;
Mutex mu2 ACQUIRED_AFTER(mu1);

class Foo {
 public:
  int method1(int i) SHARED_LOCKS_REQUIRED(mu2);
};

int Foo::method1(int i) EXCLUSIVE_LOCKS_REQUIRED(mu1)
{
  return i;
}


int foo(int i) EXCLUSIVE_LOCKS_REQUIRED(mu2);
int foo(int i) SHARED_LOCKS_REQUIRED(mu1)
{
  return i;
}

static int bar(int i) EXCLUSIVE_LOCKS_REQUIRED(mu1)
{
  return i;
}

main()
{
  Foo a;

  a.method1(1); // { dg-warning "Calling function 'method1' requires lock 'mu1'" }
  foo(2); // { dg-warning "Calling function 'foo' requires lock 'mu2'" }
  bar(3); // { dg-warning "Calling function 'bar' requires lock 'mu1'" }
}

// { dg-warning "Calling function 'method1' requires lock 'mu2'" "" { target *-*-* } 36 }
// { dg-warning "Calling function 'foo' requires lock 'mu1'" "" { target *-*-* } 37 }
