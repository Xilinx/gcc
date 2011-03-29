// Test the handling of a method with lock annotations accessed through a
// smart/scoped pointer.
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

class LOCKABLE Foo {
 public:
  Mutex *mutex_;
  int x;
  int GetValue() EXCLUSIVE_LOCKS_REQUIRED(mutex);
};

scoped_ptr<Foo> b;

main()
{
  int a;
  a = b->GetValue(); // { dg-warning "Calling function 'GetValue' requires" }
}
