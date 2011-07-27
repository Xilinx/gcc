/* Test __sync_mem routines for existence and proper execution on 2 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */

/* Test the execution of the __sync_mem_fetch_add builtin for a short.  */

extern void abort(void);

short v, count;

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
