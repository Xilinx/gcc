// Test the support for use of point_to_guarded{_by} on smart/scoped pointers.
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
  *a = 5;        // { dg-warning "Access to memory location pointed to" }
  a.reset();
  b->x = 3 + *a; // { dg-warning "Reading variable" }
}

// { dg-warning "Access to memory location pointed to by variable 'b'" "" { target *-*-* } 35 }
// { dg-warning "Access to memory location pointed to by variable 'a'" "" { target *-*-* } 35 }
