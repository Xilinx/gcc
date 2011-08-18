/* { dg-do link } */
/* { dg-options "--param allow-packed-store-data-races=0" } */
/* { dg-final { memmodel-gdb-test } } */

#include <stdio.h>
#include "memmodel.h"

/* This test verifies writes to globals do not write to adjacent
   globals.  This mostly happens on strict-align targets that are not
   byte addressable (old Alphas, etc).  */

char a = 0;
char b = 77;

void memmodel_other_threads() 
{
}

int memmodel_step_verify()
{
  if (b != 77)
    {
      printf("FAIL: Unexpected value.  <b> is %d, should be 77\n", b);
      return 1;
    }
  return 0;
}

/* Verify that every variable has the correct value.  */
int memmodel_final_verify()
{
  int ret = memmodel_step_verify ();
  if (a != 66)
    {
      printf("FAIL: Unexpected value.  <a> is %d, should be 66\n", a);
      return 1;
    }
  return ret;
}

int main ()
{
  a = 66;
  memmodel_done();
  return 0;
}
