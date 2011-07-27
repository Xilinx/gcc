/* Test __sync_mem routines for existence and proper execution on 16 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_128 } */
/* { dg-options "-mcx16" { target { x86_64-*-* } } } */

/* Test the execution of the __sync_mem_X builtin for a 16 byte value.  */

extern void abort(void);

__int128_t v, count;

main ()
{
  v = 0;
  count = 0;

  if (__sync_mem_exchange (&v, count + 1, __SYNC_MEM_RELAXED) !=  count++) 
    abort ();

  if (__sync_mem_exchange (&v, count + 1, __SYNC_MEM_ACQUIRE) !=  count++) 
    abort ();

  if (__sync_mem_exchange (&v, count + 1, __SYNC_MEM_RELEASE) !=  count++) 
    abort ();

  if (__sync_mem_exchange (&v, count + 1, __SYNC_MEM_ACQ_REL) !=  count++) 
    abort ();

  if (__sync_mem_exchange (&v, count + 1, __SYNC_MEM_SEQ_CST) !=  count++) 
    abort ();

  return 0;
}
