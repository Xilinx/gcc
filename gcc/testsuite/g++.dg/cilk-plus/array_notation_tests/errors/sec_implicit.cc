/* {dg-do compile } */
/* {dg-options } */

int main (int argc, char **argv)
{
  int array[10][10], array2[10];

  array[:][:] = __sec_implicit_index(5) + array[:][:]; /* { dg-error "__sec_implicit_index argument" } */

  return 0;
}
