/* {dg-do compile } */
/* {dg-options } */

int main (int argc, char **argv)
{
  int array[10][10], array2[10];
  
  array2[:] = array2[1:2:] ;  /* { dg-error "expected primary-expression before" } */

  return 0;
} 
