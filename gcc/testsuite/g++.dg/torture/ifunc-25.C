/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

template <class T1, class T2>
class ifunc
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
ifunc<T1, T2>::foo (T1)
{ 
  return &ifunc<T1, T2>::foo1;
}

template <class T1, class T2>
int
__attribute__ ((ifunc))
ifunc<T1, T2>::foo (T2)
{ 
  return &ifunc<T1, T2>::foo1;
}

int
bar (int x)
{
  ifunc<int, float> i;
  return i.foo (x);
}

int
bar (float x)
{
  ifunc<int, float> i;
  return i.foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifuncIifE3fooEi, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifuncIifE3fooEf, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifuncIifE3fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifuncIifE3fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifuncIifE3fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifuncIifE3fooEf, .gnu_indirect_function" } } */
