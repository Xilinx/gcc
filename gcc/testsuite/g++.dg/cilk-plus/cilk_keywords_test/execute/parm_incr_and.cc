/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */


#include <cstdlib>
#include <string.h>
#define N 1025

int main2 (int argc, char **argv);
int a[N], a_serial[N];
unsigned int b[N], c[N];

void func (int start, int end)
{
  _Cilk_for (int ii = end-1; ii >= start ; ii--)
    a[ii] += b[ii] & c[ii];
}

void func_serial (int start, int end)
{
  for (int ii = end-1; ii >= start; ii--)
    a_serial[ii] += b[ii] & c[ii];
}

int main (int argc, char **argv)
{
  int x = 5;
  if (argc == 1)
    {
      char *array[3];

      array[0] = strdup ("a.out");
      array[1] = strdup ("1");
      array[2] = strdup ("1025");
      x = main2 (3, array);
    }
  else if (argc == 3) 
    x = main2 (argc, argv);
  else
    abort ();

  return x;
}

int main2 (int argc, char **argv)
{
  int start = atoi (argv[1]);
  int end = atoi (argv[2]);
  
  for (int ii = 0; ii < N; ii++)
    {
      a[ii] = 0;
      a_serial[ii] = 0;
      b[ii] = 0xAAAAAAAA;   /* b[ii] & c[ii] = 0x00000002 */
      c[ii] = 0x55555556;
    }

  for (int ii = 0; ii < N; ii++)
    {
      func (end, start-ii);
      func_serial (end, start-ii);
      for (int jj = 0; jj < N; jj++) {
	if (a[jj] != a_serial[jj]) {
	  abort ();
	}
      }
    }
  return 0;
}
