// Test virtual method calls in cases where the static type is unknown
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class FooBase1 {
public:
  int dummy; 
};


class FooBase2 : public FooBase1 {
public:
  virtual ~FooBase2();
};


class Foo : public FooBase2 {
public:
  Mutex m;  
  
  Foo();
  virtual ~Foo();
  virtual void doSomething() EXCLUSIVE_LOCKS_REQUIRED(m) = 0;
};


class FooDerived : public Foo {
public:
  FooDerived(); 
  virtual ~FooDerived();
  virtual void doSomething();  
};


/* This is a test case for a bug wherein annotalysis would crash when analyzing 
   a virtual method call.    
  
   The following three functions represent cases where gcc loses the static  
   type of foo in the expression foo->doSomething() when it drops down to 
   gimple.  Annotalysis should technically issue a thread-safe warning in these 
   cases, but because the type is missing, it should silently accept them
   instead.  See tree-threadsafe-analyze.c::handle_call_gs.
 */

// reinterpret_cast
void foo1(void* ptr) 
{ 
  reinterpret_cast<Foo*>(ptr)->doSomething(); 
}

// downcast from structure type with no virtuals
void foo1(FooBase1* ptr) 
{ 
  reinterpret_cast<Foo*>(ptr)->doSomething(); 
}

// downcast from structure type with virtuals
void foo1(FooBase2* ptr) 
{ 
  reinterpret_cast<Foo*>(ptr)->doSomething(); 
}

// C-style cast 
void foo2(int* buf) 
{
  ((Foo*) buf)->doSomething();
}

// new expression, with no local variable
void foo3() 
{
  (new FooDerived)->doSomething();
}

