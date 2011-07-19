// Test cases where the lock is accessed via a simple method call. 
// This commonly arises when using smart pointer classes that define operator->
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

template<class T> 
class SharedPtr {
public:
  T* ptr;
  inline T* operator->() { return ptr; }
  
  inline SharedPtr(T* p) : ptr(p) { } 
};


class Foo {
public:
  Mutex _mu;

  int data GUARDED_BY(_mu);
}; 


// use a smart pointer
int foo1(Foo* ptr) {
  SharedPtr<Foo> sptr(ptr);
  
  MutexLock l(&sptr->_mu);
  return sptr->data;
}


// use a smart pointer
int foo2(Foo* ptr) {
  SharedPtr<Foo> sptr(ptr);
  
  sptr->_mu.Lock();
  int d = sptr->data;
  sptr->_mu.Unlock();
  return d;
}


// use a smart pointer
// test to ensure there are no "lock not acquired" errors on the unlock.  
int foo3(SharedPtr<Foo>& fpp) {
  fpp->_mu.Lock();
  fpp->_mu.Unlock();
}

