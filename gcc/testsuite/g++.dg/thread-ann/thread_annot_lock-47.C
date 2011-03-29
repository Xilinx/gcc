// Test the support for annotations on virtual functions.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
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

  b->mu_.Lock();
  b->func1();
  b->mu_.Unlock();
  b->func2();

  c->mu_.Lock();
  c->func1();
  c->mu_.Unlock();
  c->func2();
}
