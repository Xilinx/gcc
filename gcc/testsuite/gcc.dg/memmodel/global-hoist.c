/* { dg-do link } */
/* { dg-options "-O2 --param allow-load-data-races=0" } */
/* { dg-final { memmodel-gdb-test } } */

/* Verify that a load of a global is not hoisted out of a loop.  This
   can introduce a data race, and is dissallowed if
   --param allow-load-data-races is 0. */

#include <stdio.h>
#include <stdlib.h>
#include "memmodel.h"

int global = 0;
int sum[5];


/* The other thread in the system increments the value of global by
   one each time it is executed.  By accessing 'global' in a loop, we can
   check if the load has been hoisted out of the loop by seeing if
   iterations of the loop get the same or different values of
   global.  */
void
memmodel_other_threads() 
{
  global++;
}

/* Nothing to verify at each step. */
int
memmodel_step_verify()
{
  return 0;
}

/* Every element of the array should have a different value of global if the 
 * load is left in the loop like it is supposed to.  */
int
memmodel_final_verify()
{
  int ret = 0;
  int x, y;

  for (x = 0; x < 4; x++)
    for (y = x + 1; y < 5; y++)
      {
	if (sum[x] == sum[y])
	  {
	    printf("FAIL: sum[%d] and sum[%d] have the same value : %d\n",
		   x, y, sum[x]);
	    ret = 1;
	  }
      }
  return ret;
}

/* 'global' is bumped by "the other thread" every insn.  Test that all
   elements of 'sum' are different, otherwise load of 'global' has
   been hoisted.  */
void
test()
{
  int x;

  for (x=0; x< 5; x++)
    sum[x] =  global;
}

int
main()
{
  test();
  memmodel_done();
}
