/* <feature> loop control variable must have integer, pointer or class type
   </feature>
*/

#define ARRAY_SIZE 10000
int a[ARRAY_SIZE];

int main(void)
{ 
  int *aa = 0;
  int ii = 0;

  for (ii =0; ii < ARRAY_SIZE; ii++)
    a[ii] = 5;

  _Cilk_for(aa = a; aa < a + ARRAY_SIZE; aa++) 
    *aa = 0;

  for (ii = 0; ii < ARRAY_SIZE; ii++) 
    if (a[ii] != 0) 
      return 1;

  return 0;
}
