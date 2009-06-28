/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

class ifunc
{
private:
  static int foo1 (int);
  static int foo2 (float);

public:
  static int foo (int);
  static int foo (float);
};

int
__attribute__ ((ifunc))
ifunc::foo (int)
{ 
  return &ifunc::foo1;
}

int
__attribute__ ((ifunc))
ifunc::foo (float)
{ 
  return &ifunc::foo2;
}

int
bar (int x)
{
  return ifunc::foo (x);
}

int
bar (float x)
{
  return ifunc::foo (x);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifunc3fooEf, .function" } } */
/* { dg-final { scan-assembler-not ".type\[ 	\]\+_ZN5ifunc3fooEi, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifunc3fooEi" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_ZN5ifunc3fooEf" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifunc3fooEi, .gnu_indirect_function" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_ZN5ifunc3fooEf, .gnu_indirect_function" } } */
