// Test lock expressions that contain function calls.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"


class Foo {
 private:
  Mutex mu;

 public:
  void Lock() EXCLUSIVE_LOCK_FUNCTION(mu);
  void Unlock() UNLOCK_FUNCTION(mu);
};


class DerivedFoo : public Foo {
 private:
  Mutex mu1;
  int b_;
};

class Bar {
 protected:
  DerivedFoo *foo_;
  DerivedFoo *GetFoo(int y);
};

class DerivedBar : public Bar {
 private:
  int a_;

 public:
  void UpdateA(int x);
  void SelectA(int x);
};

void DerivedBar::UpdateA(int x) {
  GetFoo(2)->Lock(); // { dg-warning "is not released at the end of function" }
  a_ += x;
  GetFoo(3)->Unlock(); // { dg-warning "Try to unlock" }
}

void DerivedBar::SelectA(int x) {
  GetFoo(5)->Lock();
  GetFoo(x)->Lock(); // { dg-warning "is not released at the end of function" }
  a_ = x;
  GetFoo(a_)->Unlock(); // { dg-warning "Try to unlock" }
  GetFoo(5)->Unlock();
}

int g;

int func(int x, DerivedFoo * (*getfoo1)(int), DerivedFoo * (*getfoo2)(int)) {
  getfoo1(5)->Lock(); // { dg-warning "is not released at the end of function" }
  getfoo2(x)->Lock(); // { dg-warning "is not released at the end of function" }
  g += x;
  getfoo2(5)->Unlock(); // { dg-warning "Try to unlock" }
  getfoo1(g)->Unlock(); // { dg-warning "Try to unlock" }
}

DerivedBar *bar;

main() {
  bar->UpdateA(3);
  bar->SelectA(2);
}
