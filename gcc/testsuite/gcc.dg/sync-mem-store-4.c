/* Test __sync_mem routines for existence and proper execution on 8 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_long_long } */
/* { dg-options "" } */

/* Test the execution of the __sync_mem_store builtin for a long long.  */

extern void abort(void);

long long v, count;

main ()
{
  v = 0;
  count = 0;

  __sync_mem_store (&v, count + 1, __SYNC_MEM_RELAXED);
  if (v != ++count)
    abort ();

  __sync_mem_store (&v, count + 1, __SYNC_MEM_RELEASE);
  if (v != ++count)
    abort ();

  __sync_mem_store (&v, count + 1, __SYNC_MEM_SEQ_CST);
  if (v != ++count)
    abort ();

  return 0;
}

