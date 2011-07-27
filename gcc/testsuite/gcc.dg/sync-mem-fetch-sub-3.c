/* Test __sync_mem routines for existence and proper execution on 4 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_long } */

/* Test the execution of the __sync_mem_fetch_sub builtin for an int.  */

extern void abort(void);

int v, count, res;

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
