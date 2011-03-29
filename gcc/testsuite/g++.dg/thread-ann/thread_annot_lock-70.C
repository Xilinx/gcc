// Test a fix to a bug when handling calls to virtual functions that are
// annotated with LOCK/UNLOCK_FUNCTION. More specifically, the bug happens
// when we tried to assert the function decl of a gimple call statement
// returned by gimple_call_fndecl is non-NULL, which is not true when the call
// is a virtual function call. Instead, we should either get the function decl
// through the reference object, or (as is the fix) simply pass the function
// decl that we have extracted earlier all the way to
// handle_lock_primitive_attrs where the assertion fails.
//
// This is a good test case. (i.e. There should be no error/warning/ICE
// triggered.)
//
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Base {
 protected:
  virtual void Lock() EXCLUSIVE_LOCK_FUNCTION(mu_) { mu_.Lock(); }
  virtual void Unlock() UNLOCK_FUNCTION(mu_) { mu_.Unlock(); }
  Mutex mu_;
};

class Child: public Base {
  int a;
 public:
  void func1() {
    Lock();
    a += 1;
    Unlock();
  }
};

main() {
  Child c;
  c.func1();
}
