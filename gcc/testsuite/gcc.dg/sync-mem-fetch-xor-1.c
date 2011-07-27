/* Test __sync_mem routines for existence and proper execution on 1 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */

/* Test the execution of the __sync_mem_fetch_xor builtin for a char.  */

extern void abort(void);


char v, count, init = ~0;

main ()
{
  v = init;
  count = 0;

  if (__sync_mem_fetch_xor (&v, count, __SYNC_MEM_RELAXED) !=  init) 
    abort ();

  if (__sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_CONSUME) !=  init) 
    abort ();

  if (__sync_mem_fetch_xor (&v, count, __SYNC_MEM_ACQUIRE) !=  0) 
    abort ();

  if (__sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_RELEASE) !=  0) 
    abort ();

  if (__sync_mem_fetch_xor (&v, count, __SYNC_MEM_ACQ_REL) !=  init) 
    abort ();

  if (__sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_SEQ_CST) !=  init) 
    abort ();

  return 0;
}
