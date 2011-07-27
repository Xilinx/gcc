/* Test __sync_mem routines for existence and proper execution on 4 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_long } */

/* Test the execution of the __sync_mem_store builtin for an int.  */

extern void abort(void);

int v, count;

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

