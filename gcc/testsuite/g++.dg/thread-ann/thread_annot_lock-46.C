// Test the support for annotations on virtual functions.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Base {
 public:
  virtual void func1() EXCLUSIVE_LOCKS_REQUIRED(mu_);
  virtual void func2() LOCKS_EXCLUDED(mu_);
  Mutex mu_;
};

class Child : public Base {
 public:
  virtual void func1() EXCLUSIVE_LOCKS_REQUIRED(mu_);
  virtual void func2() LOCKS_EXCLUDED(mu_);
};

main() {
  Child *c;
  Base *b = c;

  b->func1();        // { dg-warning "Calling function 'func1' requires lock" }
  b->mu_.Lock();
  b->func2();        // { dg-warning "Cannot call function" }
  b->mu_.Unlock();

  c->func1();        // { dg-warning "Calling function 'func1' requires lock" }
  c->mu_.Lock();
  c->func2();        // { dg-warning "Cannot call function" }
  c->mu_.Unlock();
}
