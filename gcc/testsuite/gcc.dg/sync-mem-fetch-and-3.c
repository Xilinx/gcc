/* Test __sync_mem routines for existence and proper execution on 4 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_long } */

/* Test the execution of the __sync_mem_fetch_and builtin for an int.  */

extern void abort(void);

int v;
#define INIT (int)0xffffffff

main ()
{
  v = INIT;

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_RELAXED) !=  INIT) 
    abort ();

  if (__sync_mem_fetch_and (&v, INIT, __SYNC_MEM_CONSUME) !=  0) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQUIRE) !=  0)
    abort ();

  v = ~v;
  if (__sync_mem_fetch_and (&v, INIT, __SYNC_MEM_RELEASE) !=  INIT)
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQ_REL) !=  INIT) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_SEQ_CST) !=  0) 
    abort ();

  return 0;
}
