/* { dg-do link } */
/* { dg-options "--param allow-load-data-races=0 --param allow-store-data-races=0" } */
/* { dg-final { memmodel-gdb-test } } */

/* Test that setting <var.a> does not touch either <var.b> or <var.c>.
   In the C++ memory model, non contiguous bitfields ("a" and "c"
   here) should be considered as distinct memory locations, so we
   can't use bit twiddling to set either one.  */

#include <stdio.h>
#include "memmodel.h"

#define CONSTA 12

static int global;
struct S
{
  /* On x86-64, the volatile causes us to access <a> with a 32-bit
     access, and thus trigger this test.  */
  volatile unsigned int a : 4;

  unsigned char b;
  unsigned int c : 6;
} var;

__attribute__((noinline))
void set_a()
{
  var.a = CONSTA;
}

void memmodel_other_threads()
{
  ++global;
  var.b = global;
  var.c = global;
}

int memmodel_step_verify()
{
  int ret = 0;
  if (var.b != global)
    {
      printf ("FAIL: Unexpected value: var.b is %d, should be %d\n",
	      var.b, global);
      ret = 1;
    }
  if (var.c != global)
    {
      printf ("FAIL: Unexpected value: var.c is %d, should be %d\n",
	      var.c, global);
      ret = 1;
    }
  return ret;
}

int memmodel_final_verify()
{
  int ret = memmodel_step_verify();
  if (var.a != CONSTA)
    {
      printf ("FAIL: Unexpected value: var.a is %d, should be %d\n",
	      var.a, CONSTA);
      ret = 1;
    }
  return ret;
}

int main()
{
  set_a();
  memmodel_done();
  return 0;
}
