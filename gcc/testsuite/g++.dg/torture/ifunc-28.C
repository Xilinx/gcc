/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

template <class T1, class T2>
class ifunc
{
private:
  virtual void foo1 (T1);
  virtual void foo1 (T2);

public:
  virtual void foo (T1);
  virtual void foo (T2);
};

template <class T1, class T2>
class ifunc2 : public ifunc<T1, T2>
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
ifunc2<T1, T2>::foo (T1)
{ 
  return &ifunc2<T1, T2>::foo1;
}

template <class T1, class T2>
void
__attribute__ ((ifunc))
ifunc2<T1, T2>::foo (T2)
{ 
  return &ifunc2<T1, T2>::foo1;
}

void
bar (int x)
{
  ifunc2<int, float> i;
  i.foo (x);
}

void
bar (float x)
{
  ifunc2<int, float> i;
  i.foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc2IifE3fooEi, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc2IifE3fooEf, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc2IifE3fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc2IifE3fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc2IifE3fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc2IifE3fooEf, .gnu_indirect_function" } } */
