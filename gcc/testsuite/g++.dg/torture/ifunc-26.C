/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

template <class T1, class T2>
class ifunc
{
private:
  void foo1 (T1);
  void foo1 (T2);

public:
  void foo (T1);
  void foo (T2);
};

template <class T1, class T2>
void
__attribute__ ((ifunc))
ifunc<T1, T2>::foo (T1)
{ 
  return &ifunc<T1, T2>::foo1;
}

template <class T1, class T2>
void
__attribute__ ((ifunc))
ifunc<T1, T2>::foo (T2)
{ 
  return &ifunc<T1, T2>::foo1;
}

void
bar (int x)
{
  ifunc<int, float> i;
  i.foo (x);
}

void
bar (float x)
{
  ifunc<int, float> i;
  i.foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifuncIifE3fooEf, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifuncIifE3fooEi, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifuncIifE3fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifuncIifE3fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifuncIifE3fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifuncIifE3fooEf, .gnu_indirect_function" } } */
