// Test annotations on out-of-line definitions of member functions where the
// annotations refer to locks that are also data members in the class.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex mu;

class Foo {
 public:
  int method1(int i);
  int data GUARDED_BY(mu1);
  Mutex *mu1;
  Mutex *mu2;
};

int Foo::method1(int i) SHARED_LOCKS_REQUIRED(mu1, mu, mu2, mu3)
{
  return data + i;
}

main()
{
  Foo a;

  a.method1(1); // { dg-warning "Calling function 'method1' requires lock" }
}

// { dg-warning "Calling function 'method1' requires lock 'mu'" { target *-*-* } 27 }
// { dg-warning "Calling function 'method1' requires lock 'a.mu2'" { target *-*-* } 27 }
// { dg-warning "Calling function 'method1' requires lock 'mu3'" { target *-*-* } 27 }
