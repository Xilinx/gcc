// Test the support for use of point_to_guarded{_by} on smart/scoped pointers.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

template<class T>
class scoped_ptr {
 public:
  typedef T element_type;

  explicit scoped_ptr(T * p = 0);
  ~scoped_ptr();

  void reset(T * p = 0);

  T & operator*() const;
  T * operator->() const;
  T * get() const;
};

class Foo {
 public:
  int x;
};

Mutex mu1, mu2;
scoped_ptr<int> a PT_GUARDED_BY(mu1);
scoped_ptr<Foo> b GUARDED_BY(mu2) PT_GUARDED_VAR;

main()
{
  MutexLock l1(&mu1);
  MutexLock l2(&mu2);
  *a = 5;
  a.reset();
  b->x = 3 + *a;
}
