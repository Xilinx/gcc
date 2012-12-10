/* { dg-final { scan-assembler "simdsimd"  } } */

/* This test will insert the clone for the function ef_add inside the function
 * main (the non-masked version).
 */

#include <stdlib.h>
#define My_Type float
__attribute__ ((vector(vectorlength(4), processor (pentium_4), uniform (x,y)))) My_Type ef_add (My_Type x, My_Type y);

My_Type vhx2[10];
int
main (int argc, char **argv)
{
  My_Type vhx[10];
  int ii = 9;

  if (argc == 1) 
    for (ii = 0; ii < 10; ii++) 
      vhx[ii] = argc;

  for (ii = 0; ii < 10; ii++)
    vhx2[ii] = ef_add(vhx[ii], vhx[ii]);
 
  for (ii = 0; ii < 10; ii++)
    if (vhx2[ii] != (argc + argc))
      abort ();
  return 0;
}

