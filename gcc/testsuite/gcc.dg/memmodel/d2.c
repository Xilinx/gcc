/* { dg-do link } */
/* { dg-options "-O2" } */
/* { dg-final { memmodel-gdb-test } } */

#include <stdio.h>
#include "memmodel.h"

/* This is a variation on global-hoist.c where instead of a loop, we
   store global into different elements of an array in straightline
   code with an if condition.  This will catch cases where commoning
   should be disabled when -fno-allow-load-data-races is on.  */

/* Test the FALSE path in test.  */

int global = 0;
int sum[4] = { 0, 0, 0, 0 };

void memmodel_other_threads() 
{
  global++;
}

int memmodel_step_verify()
{
  return 0;
}

int memmodel_final_verify()
{
  int ret = 0;
  int x, y;
  for (x = 0; x < 3; x++)
    for (y = x + 1; y < 4; y++)
      {
	if (sum[x] == sum[y])
	  {
	    printf("FAIL: sum[%d] and sum[%d] have the same value : %d\n",
		   x, y, sum[x]);
	    ret = 1;
	  }
      }
}

/* Since global is always being increased by the 'other' thread, all
   elements of sum should be different.  (no cse) */
int test (int y)
{
  sum[0] = global;
  if (y)
    sum[1] = global;
  else
    sum[2] = global;
  sum[3] = global;
}

int main()
{
  test(0);
  memmodel_done();
}
