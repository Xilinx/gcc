/* This test will insert the clone for the function ef_add inside the function
 * main (the non-masked version).
 */

#define My_Type float
__attribute__ ((vector(vectorlength(4), processor (core_i7_sse4_2), uniform (x,y)))) My_Type ef_add (My_Type x, My_Type y);

My_Type vhx2[10];
int
main (int argc, char **argv)
{
  My_Type vhx[10];
  int ii = 9;

  for (ii = 0; ii < 10; ii++)
    vhx2[ii] = ef_add(vhx[ii], vhx[ii]);
 
  return 0;
}

