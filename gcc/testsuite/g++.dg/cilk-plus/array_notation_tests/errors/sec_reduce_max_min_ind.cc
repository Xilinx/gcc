/* {dg-do compile } */
/* {dg-options } */

int main (int argc, char **argv)
{
  int array[10][10], array2[10];
  int x, y;
  x = __sec_reduce_max_ind (array[:][:]); /* { dg-error "cannot have arrays with dimension greater than" } */

  y = __sec_reduce_max_ind (array2[:]); /* this should be OK. */

  x = __sec_reduce_min_ind (array[0:10][:]); /* { dg-error "cannot have arrays with dimension greater than" } */

  y = __sec_reduce_min_ind (array2[:]); /* this should be OK. */
  y = __sec_reduce_min_ind (array[10][:]); /* this should be OK also. */

  return 0;
}
