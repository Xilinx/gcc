#include <stdio.h>

void abort (void);
void exit (int);

int main(void)
{
  int jj = 0, error = 0;
  int Array[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int Array_Serial[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

  _Cilk_for (int ii = 0; ii < 10; ii++)
    {
      if ((ii % 2) == 0)
	goto hello_label;
      else
	goto world_label;

hello_label:
      Array[ii]++;
world_label:
      Array[ii]++;
    }
  
  for (int ii = 0; ii < 10; ii++)
    {
      if ((ii % 2) == 0)
	goto hello_label2;
      else
	goto world_label2;

hello_label2:
      Array_Serial[ii]++;
world_label2:
      Array_Serial[ii]++;
    }

  for (int ii = 0; ii < 10; ii++)
    {
      if (Array_Serial[ii] != Array[ii])
	error = 1;
    }
  if (error)
    abort ();
  else
    exit (0);

  return 0;
}
