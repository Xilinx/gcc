/* {dg-do compile } */
/* {dg-options } */

int main2 (int argc, char **argv)
{
  int array[10][10], array2[10];
  
  array[:] = argv[:][:];  /* { dg-error "records or pointers using array notation must specify the start and length" } */ 

  return 0;
} 
