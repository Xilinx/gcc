/* { dg-do compile } */

 __attribute__ ((vector(vectorlength(2, processor (pentium_4), uniform (x,y))))) int ef_add (int x, int y); /* { dg-error "after vectorlength before" } */

__attribute__ ((vector(vectorlength 2, processor (pentium_4), uniform (x,y)))) int ef_add (int x, int y); /* { dg-error "expected '\\(' before numeric constant" } */

__attribute__ ((vector(vectorlength(5), processor (pentium_4), uniform (x,y)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be a power of 2" } */

__attribute__ ((vector(vectorlength(6), processor (pentium_4)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be a power of 2" } */

__attribute__ ((vector(vectorlength(7), uniform (x,y)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be a power of 2" } */

__attribute__ ((vector(uniform (x,y)))) int ef_add (int x, int y); /* { "This is OK!" } */

__attribute__ ((vector(vectorlength(2.0005)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be an integer." } */

__attribute__ ((vector(vectorlength(5), processor (pentium_4), uniform (x,y)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be a power of 2" } */

__attribute__ ((vector(vectorlength(1)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be between 2 and 8." } */

__attribute__ ((vector(vectorlength(16), processor (pentium_4)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be between 2 and 8." } */

__attribute__ ((vector(vectorlength(32), processor (pentium_4), linear (x:1)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be between 2 and 8." } */

__attribute__ ((vector(vectorlength(32), processor (pentium_4), uniform (y), linear (x:1)))) int ef_add (int x, int y); /* { dg-error "vectorlength must be between 2 and 8." } */

int vhx2[10];
int
main (int argc, char **argv)
{
  int vhx[10];
  int ii = 9;

  if (argc == 1) 
    for (ii = 0; ii < 10; ii++) 
      vhx[ii] = argc;

  for (ii = 0; ii < 10; ii++)
    vhx2[ii] = ef_add(vhx[ii], vhx[ii]);
 
  for (ii = 0; ii < 10; ii++)
    if (vhx2[ii] != (argc + argc))
      return 1;
  return 0;
}
