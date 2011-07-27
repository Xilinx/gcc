/* Test __sync_mem routines for existence and proper execution on 8 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_long_long } */
/* { dg-options "" } */

/* Test the execution of the __sync_mem_fetch_sub builtin for a long long.  */

extern void abort(void);

long long v, count, res;

main ()
{
  v = res = 20;
  count = 0;

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_RELAXED) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_CONSUME) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_ACQUIRE) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_RELEASE) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_ACQ_REL) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_SEQ_CST) !=  res--) 
    abort ();

  return 0;
}
