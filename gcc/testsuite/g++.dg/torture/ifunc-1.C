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

int
bar (int i)
{
  return foo (i);
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_Z3fooi, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_Z3fooi" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_Z3fooi, .gnu_indirect_function" } } */
