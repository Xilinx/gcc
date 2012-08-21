/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>

int main(void)
{
  int jj = 0;
  int total = 0;

  _Cilk_for (int ii = 0; ii < 10; ii++)
    {
      if ((ii % 2) == 0)
	goto hello_label;
      else
	goto world_label;

hello_label:
     total++;
world_label:
     total++;
    }
  if (total != 15)
    abort ();
  return 0;
}
