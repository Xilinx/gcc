/* Test __sync_mem routines for existence and proper execution on 2 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */

/* Test the execution of the __sync_mem_store builtin for a short.  */

extern void abort(void);

short v, count;

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

