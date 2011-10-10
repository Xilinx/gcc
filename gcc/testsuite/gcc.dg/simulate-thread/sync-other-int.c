/* { dg-do link } */
/* { dg-require-effective-target sync_int_long } */
/* { dg-final { simulate-thread } } */


#include <stdio.h>
#include "simulate-thread.h"

/* Test all the __sync routines for proper atomicity on 4 byte values.  */

unsigned int zero = 0;
unsigned int max = ~0;

unsigned int changing_value = 0;
unsigned int value = 0;
unsigned int ret;

void test_abort()
{
  static int reported = 0;
  if (!reported)
    {
      printf ("FAIL: improper execution of __sync builtin.\n");
      reported = 1;
    }
}

void simulate_thread_other_threads ()
{
}

int simulate_thread_step_verify ()
{
  if (value != zero && value != max)
    {
      printf ("FAIL: invalid intermediate result for value.\n");
      return 1;
    }
  return 0;
}

int simulate_thread_final_verify ()
{
  if (value != 0)
    {
      printf ("FAIL: invalid final result for value.\n");
      return 1;
    }
  return 0;
}

/* All values written to 'value' alternate between 'zero' and
   'max'. Any other value detected by simulate_thread_step_verify()
   between instructions would indicate that the value was only
   partially written, and would thus fail this atomicity test.

   This function tests each different __sync_mem routine once, with
   the exception of the load instruction which requires special
   testing.  */
__attribute__((noinline))
void simulate_thread_main()
{
  
  ret = __sync_mem_exchange (&value, max, __SYNC_MEM_SEQ_CST);
  if (ret != zero || value != max)
    test_abort();

  __sync_mem_store (&value, zero, __SYNC_MEM_SEQ_CST);
  if (value != zero)
    test_abort();

  ret = __sync_mem_fetch_add (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != max || ret != zero)
    test_abort ();

  ret = __sync_mem_fetch_sub (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != zero || ret != max)
    test_abort ();

  ret = __sync_mem_fetch_or (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != max || ret != zero)
    test_abort ();

  ret = __sync_mem_fetch_and (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != max || ret != max)
    test_abort ();

  ret = __sync_mem_fetch_xor (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != zero || ret != max)
    test_abort ();

  ret = __sync_mem_add_fetch (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != max || ret != max)
    test_abort ();

  ret = __sync_mem_sub_fetch (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != zero || ret != zero)
    test_abort ();

  ret = __sync_mem_or_fetch (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != max || ret != max)
    test_abort ();

  ret = __sync_mem_and_fetch (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != max || ret != max)
    test_abort ();

  ret = __sync_mem_xor_fetch (&value, max, __SYNC_MEM_SEQ_CST);
  if (value != zero || ret != zero)
    test_abort ();
}

main ()
{
  simulate_thread_main ();
  simulate_thread_done ();
  return 0;
}
