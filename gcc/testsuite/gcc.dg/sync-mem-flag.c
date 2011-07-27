/* Test __sync_mem routines for existence and proper execution with each valid 
   memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */


/* Test the execution of __sync_mem_flag_{test_and_set,clear} builtins.  */

extern void abort(void);

char v;

main ()
{
  v = 0;

  if (__sync_mem_flag_test_and_set (&v, __SYNC_MEM_RELAXED) != 0) 
    abort(); 
  if (v != 1)
    abort();
  __sync_mem_flag_clear (&v, __SYNC_MEM_RELAXED);

  if (__sync_mem_flag_test_and_set (&v, __SYNC_MEM_RELEASE) != 0) 
    abort(); 
  if (v != 1)
    abort();
  __sync_mem_flag_clear (&v, __SYNC_MEM_RELEASE);

  if (__sync_mem_flag_test_and_set (&v, __SYNC_MEM_SEQ_CST) != 0) 
    abort(); 
  if (v != 1)
    abort();
  __sync_mem_flag_clear (&v, __SYNC_MEM_SEQ_CST);

  return 0;
}

