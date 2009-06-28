/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

static int
foo1 (int x) 
{
  return x;
}

int
__attribute__ ((ifunc))
foo (int)
{ 
  return foo1;
}

extern int foo (int);

int
bar (int i)
{
  return foo (i);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+foo, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+foo" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+foo, .gnu_indirect_function" } } */
