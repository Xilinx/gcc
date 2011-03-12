/* { dg-do link { target { x32 || lp64 } } } */
/* { dg-options "-O2 -mbmi" } */

#include <x86intrin.h>

/* Test that a constant operand 0 to tzcnt gets folded.  */
extern void link_error(void);
int main()
{
  if (__tzcnt_u64(0) != 64)
    link_error();
  return 0;
}
