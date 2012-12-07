/* {dg-do compile } */
/* {dg-options } */

int main2 (int argc, char **argv)
{
  int array[10][10], array2[10];
  
  array2[:1:1] = array2[:] ;  /* { dg-error "start index not found" } */ 

  return 0;
} 
