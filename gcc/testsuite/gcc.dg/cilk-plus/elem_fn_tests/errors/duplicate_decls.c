/* { dg-do compile } */

__attribute__ ((vector(vectorlength(2), linear (x:2), processor (pentium_4), uniform (x,y)))) int ef_add (int x, int y); 

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
/* { dg-error "variable x defined in both uniform and linear clause" "" { target *-*-*} 0 } */
