/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

class ifunc
{
private:
  virtual int foo1 (int);
  virtual float foo1 (float);

public:
  virtual int foo (int);
  virtual float foo (float);
};

class ifunc2 : public ifunc
{
private:
  int foo1 (int);
  float foo1 (float);

public:
  int foo (int);
  float foo (float);
};

int
__attribute__ ((ifunc))
ifunc2::foo (int)
{ 
  return &ifunc2::foo1;
}

float
__attribute__ ((ifunc))
ifunc2::foo (float)
{ 
  return &ifunc2::foo1;
}

int
bar (int x)
{
  ifunc2 i;
  return i.foo (x);
}

float
bar (float x)
{
  ifunc2 i;
  return i.foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc23fooEi, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN6ifunc23fooEf, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc23fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN6ifunc23fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc23fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN6ifunc23fooEf, .gnu_indirect_function" } } */
