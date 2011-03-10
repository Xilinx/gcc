/* { dg-do link } */
/* { dg-options "-O2" } */
/* { dg-final { memmodel-gdb-test } } */

/* FIXME: I can't get this to fail.  */

#include <stdio.h>
#include "memmodel.h"

/* This file tests that speculative store movement out of a loop doesn't 
   happen.  This is disallowed when -fno-allow-store-data-races is on.  */

int global = 100;

/* Other thread makes sure global is 100 before the next instruction is
 * exceuted.  */
int memmodel_other_threads() 
{
  global = 100;
}

int memmodel_step_verify()
{
  if (global != 100)
    {
      printf("FAIL: global variable was assigned to.  \n");
      return 1;
    }
}

int memmodel_final_verify()
{
  return 0;
}

/* The variable global should never be assigned if func(0) is called.
   This tests store movement out of loop thats never executed. */
void test (int y)
{
  int x;
  for (x=0; x< y; x++)
    {
       global = y;   /* This should never speculatively execute.  */
    }
}

int main()
{
  test(0);
  memmodel_done();
}
