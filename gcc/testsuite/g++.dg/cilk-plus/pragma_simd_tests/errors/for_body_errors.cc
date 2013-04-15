/* { dg-do compile } */
/* { dg-options "-O3 -fcilkplus" } */

#include <setjmp.h>
int main(void)
{
  int ii = 0;

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    break; /* { dg-error "break statments are not allowed" } */

#pragma simd linear (ii)
  for (ii = 0; ii < 1000; ii++)
    if (ii)
      continue; /* { dg-error "continue statments are not allowed" } */

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    {
L5:
      goto L5;  /* { dg-error "goto statments are not allowed" } */ 
    }

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    {
      try { /* { dg-error "try statements are not allowed" } */
	  ii = ii %2;
      }
      catch (...)
	{
	}
    }

#pragma simd
  for (ii = 0; ii < 1000; ii++)
    {
      jmp_buf x;
      if (!setjmp (x)) /* { dg-error "setjmps are not allowed inside"  } */
	return 5;
    }

  for (ii = 0; ii < 1000; ii++)
    {
      try { /*  This is OK! */
	  ii = ii %2;
      }
      catch (...)
	{
	}
    }
  return 5;
}
