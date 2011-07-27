/* Test __sync_mem routines for existence and proper execution on 2 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */

/* Test the execution of the __sync_mem_fetch_and builtin for a short.  */

extern void abort(void);

short v;
#define INIT (short)0xffff

main ()
{
  v = INIT;

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_RELAXED) !=  INIT) 
    abort ();

  if (__sync_mem_fetch_and (&v, INIT, __SYNC_MEM_CONSUME) !=  0) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQUIRE) !=  0 ) 
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
