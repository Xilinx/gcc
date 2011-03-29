// Test guarded_by/pt_guarded_by annotations with unsupported or unrecognized
// lock names/expressions
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

namespace ns1 {

int p GUARDED_BY(a->mu); // { dg-warning "'guarded_by' attribute downgraded to 'guarded'" }
int q GUARDED_BY(f2()->get_lock()); // { dg-warning "'guarded_by' attribute downgraded to 'guarded'" }

class Foo {
 public:
  Mutex mu_;
};

int r GUARDED_BY(ns2::Foo::mu_); // { dg-warning "'guarded_by' attribute downgraded to 'guarded'" }

template <typename T>
class Bar {
 public:

  T func1() {
    T ret;
    mu1_.Lock();
    a_ = 5; // { dg-warning "Writing to variable 'a_' requires lock 'foo_->mu_'" }
    x_ = 3 + q;
    ret = x_ - *y_;
    mu1_.Unlock();
    return ret;
  }

  T x_ GUARDED_BY(((mu1_)));
  Mutex mu1_ ACQUIRED_AFTER(mu2); // { dg-warning "Unsupported argument of 'acquired_after' attribute ignored" }
  T a_ GUARDED_BY(foo_->mu_);
  T *y_ PT_GUARDED_BY(array[2]->mu_); // { dg-warning "'point_to_guarded_by' attribute downgraded to 'point_to_guarded'" }
  Foo *foo_;
};

Bar<int> *b1 GUARDED_BY((a->mu)); // { dg-warning "'guarded_by' attribute downgraded to 'guarded'" }
Foo *f2;

int main()
{
  f2->mu_.Lock();
  b1->mu1_.Lock();
  b1->func1();
  p = r + 5;
  b1->x_ = 3;
  b1->mu1_.Unlock();
  f2->mu_.Unlock();
}

}
