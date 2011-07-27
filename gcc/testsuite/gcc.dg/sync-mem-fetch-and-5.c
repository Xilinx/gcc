/* Test __sync_mem routines for existence and proper execution on 16 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_128 } */
/* { dg-options "-mcx16" { target { x86_64-*-* } } } */

/* Test execution of the __sync_mem_fetch_and builtin for a 16 byte value.  */

extern void abort(void);

__int128_t v;
__int128_t init = ~0;

main ()
{
  v = init;

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_RELAXED) !=  init) 
    abort ();

  if (__sync_mem_fetch_and (&v, init, __SYNC_MEM_CONSUME) !=  0) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQUIRE) !=  0 ) 
    abort ();

  v = ~v;
  if (__sync_mem_fetch_and (&v, init, __SYNC_MEM_RELEASE) !=  init)
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQ_REL) !=  init) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_SEQ_CST) !=  0) 
    abort ();

  return 0;
}
