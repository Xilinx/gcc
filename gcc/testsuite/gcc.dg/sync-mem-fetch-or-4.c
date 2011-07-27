/* Test __sync_mem routines for existence and proper execution on 8 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_long_long } */
/* { dg-options "" } */

/* Test the execution of the __sync_mem_fetch_or builtin for a long long.  */

extern void abort(void);

long long v, count;

main ()
{
  v = 0;
  count = 1;

  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_RELAXED) !=  0) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_CONSUME) !=  1) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_ACQUIRE) !=  3) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_RELEASE) !=  7) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_ACQ_REL) !=  15) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_SEQ_CST) !=  31) 
    abort ();

  return 0;
}
