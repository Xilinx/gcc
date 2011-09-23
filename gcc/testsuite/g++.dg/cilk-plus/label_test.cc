#include <stdio.h>

int main(void)
{
  int jj = 0;

  _Cilk_for (int ii = 0; ii < 10; ii++)
    {
      if ((ii % 2) == 0)
	goto hello_label;
      else
	goto world_label;

hello_label:
      printf("Hello ");
world_label:
      printf("World\n");
    }
  return 0;
}
