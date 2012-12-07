/* {dg-do compile } */
/* {dg-options } */

int main (int argc, char **argv)
{
  int array[10][10], array2[10];
  
  array[:][:] = __sec_implicit_index(argc) + array[:][:]; /* { dg-error "__sec_implicit_index parameter must be a constant" } */
  return 0;
}
