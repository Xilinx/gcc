/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

class ifunc
{
private:
  void foo1 (int);
  void foo2 (float);

public:
  void foo (int);
  void foo (float);
  void bar (int);
  void bar (float);
};

void
__attribute__ ((ifunc))
ifunc::foo (int)
{ 
  return &ifunc::foo1;
}

void
__attribute__ ((ifunc))
ifunc::foo (float)
{ 
  return &ifunc::foo2;
}

void
ifunc::bar (int x)
{
  foo (x);
}

void
ifunc::bar (float x)
{
  foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifunc3fooEi, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifunc3fooEf, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifunc3fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifunc3fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifunc3fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifunc3fooEf, .gnu_indirect_function" } } */
