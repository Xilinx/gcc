/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-fPIC -Wall -Wextra" } */

static int
foo1 (int x, int y) 
{
  return x - y;
}

int
__attribute__ ((ifunc, visibility ("hidden")))
foo (int, int y)
{ 
  return foo1;
}

int
bar (int i)
{
  return foo (i, 1);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+foo, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+foo@PLT" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+foo, .gnu_indirect_function" } } */
