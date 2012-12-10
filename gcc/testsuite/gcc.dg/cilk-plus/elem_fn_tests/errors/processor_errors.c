/* { dg-do compile } */

__attribute__ ((vector(vectorlength(2), processor (pentixhaum_4), uniform (y)))) int ef_add (int x, int y); /* { dg-error "processor pentixhaum_4 not" } */
__attribute__ ((vector(vectorlength(2), processor (, uniform (y)))) int ef_add (int x, int y); /* { dg-error "expected processor-name before" } */
__attribute__ ((vector(vectorlength(2), processor (), uniform (y)))) int ef_add (int x, int y); /* { dg-error "expected '\\(' and CPUID before '\\)' token" } */
__attribute__ ((vector(vectorlength(2), processor ), uniform (y)))) int ef_add (int x, int y); /* { dg-error "expected '\\(' before '\\)' token"  } */
__attribute__ ((vector(vectorlength(2), uniform (x), processor (pentixhaum_4)))) int ef_add (int x, int y); /* { dg-error "processor pentixhaum_4 not" } */
__attribute__ ((vector(vectorlength(2), processor ( ))) int ef_add (int x, int y); /* { dg-error "expected '\\(' and CPUID before '\\)' token" } */
__attribute__ ((vector(vectorlength(2), processor () ))) int ef_add (int x, int y); /* { dg-error "expected '\\(' and CPUID before '\\)' token" } */
__attribute__ ((vector(vectorlength(2), processor )))) int ef_add (int x, int y); /* { dg-error "expected '\\(' before '\\)' token"  } */
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
