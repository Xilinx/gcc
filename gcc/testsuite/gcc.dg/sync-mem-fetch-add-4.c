/* Test __sync_mem routines for existence and proper execution on 8 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_long_long } */
/* { dg-options "" } */

/* Test the execution of the __sync_mem_fetch_add builtin for a long_long.  */

extern void abort(void);

long long v, count;

main ()
{
  v = 0;
  count = 1;

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_RELAXED) != 0)
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_CONSUME) != 1) 
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_ACQUIRE) != 2)
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_RELEASE) != 3) 
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_ACQ_REL) != 4) 
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_SEQ_CST) != 5) 
    abort ();

  return 0;
}
