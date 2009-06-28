/* { dg-do compile { target i?86-*-linux* x86_64-*-linux* } } */
/* { dg-options "-Wall -Wextra" } */

void
foo1 (void) 
{
}

void
__attribute__ ((ifunc))
foo (void)
{ 
  return foo1;
}

void
bar (void)
{
  foo ();
}

/* { dg-final { scan-assembler-not ".type\[ 	\]\+_Z3foov, .function" } } */
/* { dg-final { scan-assembler "(call|jmp)\[ 	\]\+_Z3foov" } } */
/* { dg-final { scan-assembler ".type\[ 	\]\+_Z3foov, .gnu_indirect_function" } } */
