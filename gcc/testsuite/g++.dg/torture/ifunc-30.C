/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

class ifunc
{
private:
  virtual void foo1 (int);
  virtual void foo1 (float);

public:
  virtual void foo (int);
  virtual void foo (float);
};

class ifunc2 : public ifunc
{
private:
  void foo1 (int);
  void foo1 (float);

public:
  void foo (int);
  void foo (float);
};

void
__attribute__ ((ifunc))
ifunc2::foo (int)
{ 
  return &ifunc2::foo1;
}

void
__attribute__ ((ifunc))
ifunc2::foo (float)
{ 
  return &ifunc2::foo1;
}

void
bar (int x)
{
  ifunc2 i;
  i.foo (x);
}

void
bar (float x)
{
  ifunc2 i;
  i.foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc23fooEi, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc23fooEf, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc23fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc23fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc23fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc23fooEf, .gnu_indirect_function" } } */
