/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

template <class T1, class T2>
class ifunc
{
private:
  virtual int foo1 (T1);
  virtual int foo1 (T2);

public:
  virtual int foo (T1);
  virtual int foo (T2);
};

template <class T1, class T2>
class ifunc2 : public ifunc<T1, T2>
{
private:
  int foo1 (T1);
  int foo1 (T2);

public:
  int foo (T1);
  int foo (T2);
};

template <class T1, class T2>
int
__attribute__ ((ifunc))
ifunc2<T1, T2>::foo (T1)
{ 
  return &ifunc2<T1, T2>::foo1;
}

template <class T1, class T2>
int
__attribute__ ((ifunc))
ifunc2<T1, T2>::foo (T2)
{ 
  return &ifunc2<T1, T2>::foo1;
}

int
bar (int x)
{
  ifunc2<int, float> i;
  return i.foo (x);
}

int
bar (float x)
{
  ifunc2<int, float> i;
  return i.foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc2IifE3fooEi, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc2IifE3fooEf, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc2IifE3fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc2IifE3fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc2IifE3fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc2IifE3fooEf, .gnu_indirect_function" } } */
