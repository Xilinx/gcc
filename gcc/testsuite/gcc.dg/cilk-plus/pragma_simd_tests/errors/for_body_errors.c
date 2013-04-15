/* { dg-do compile } */
/* { dg-options "-O3 -fcilkplus" } */

#include <setjmp.h>

int main(void)
{
  int ii = 0;

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    break; /* { dg-error "goto/break/continue statments are" } */

#pragma simd linear (ii)
  for (ii = 0; ii < 1000; ii++)
    if (ii)
      continue; /* { dg-error "goto/break/continue statments are" } */

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    {
L5:
      goto L5;  /* { dg-error "goto/break/continue statments are" } */ 
    }

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    {
      jmp_buf x;
      if (!setjmp (x)) /* { dg-error "setjmps are not allowed inside"  } */
	return 5;
    }
  return 5;
}
