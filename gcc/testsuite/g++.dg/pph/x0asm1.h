#ifndef __X0ASM1_H
#define __X0ASM1_H

struct arg1 {
  int value;
  static const int info = 99;
};

struct arg2 {
  int value;
  static const int info = 11;
};

template<int j>
int foo (void)
{
  int i;
  asm ("# foo on %[third] %[second] %[fourth] %[first]"
       : [first] "=r" (i)
       : [second] "i" (j),
         [third] "i" (j + 2),
         [fourth] "i" (100));
  return i;
}

template<class TYPE>
TYPE bar (TYPE t)
{
  asm ("# bar on %[first] %[second] %[third]"
       : [first] "=r" (t.value)
       : [second] "i[first]" (t.value),
         [third] "i" (t.info));
  return t;
}

template<class TYPE>
struct S {
  static void frob (TYPE t)
  {
    asm ("# frob on %[arg]" :: [arg] "i" (t.info));
  }
};

void test ()
{
  arg1 x;
  arg2 y;

  foo<42> ();
  bar (x);
  bar (y);
  S<arg1>::frob (x);
}

template <class T> class  I {
public:
 void f() { asm ("# mov %edi, %esi" ); }
};

inline int cas(volatile int* ptr, int old_value, int new_value)
{
  int prev;
  __asm__ __volatile__("lock; cmpxchgl %1,%2"
                       : "=a" (prev)
                       : "q" (new_value), "m" (*ptr), "0" (old_value)
                       : "memory");
  return prev;
}
#endif
