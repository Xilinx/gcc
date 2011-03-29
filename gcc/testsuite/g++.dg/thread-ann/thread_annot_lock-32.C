// Test lock expressions that contain function calls.
// This is a "good" test that should not incur any compilation wanrings.
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
  GetFoo(2)->Lock();
  a_ += x;
  GetFoo(2)->Unlock();
}

void DerivedBar::SelectA(int x) {
  GetFoo(x)->Lock();
  GetFoo(a_)->Lock();
  a_ = x;
  GetFoo(a_)->Unlock();
  GetFoo(x)->Unlock();
}

int g;

int func(int x, DerivedFoo * (*getfoo1)(int), DerivedFoo * (*getfoo2)(int)) {
  getfoo1(5)->Lock();
  getfoo2(x)->Lock();
  getfoo1(g)->Lock();
  g += x;
  getfoo1(g)->Unlock();
  getfoo2(x)->Unlock();
  getfoo1(5)->Unlock();
}

DerivedBar *bar;

main() {
  bar->UpdateA(3);
  bar->SelectA(2);
}
