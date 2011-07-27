/* Test __sync_mem routines for existence and proper execution on 16 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_128 } */
/* { dg-options "-mcx16" { target { x86_64-*-* } } } */

extern void abort(void);

__int128_t v, count;

main ()
{
  v = 0;
  count = 0;

  if (__sync_mem_load (&v, __SYNC_MEM_RELAXED) != count++) 
    abort(); 
  else 
    v++;

  if (__sync_mem_load (&v, __SYNC_MEM_ACQUIRE) != count++) 
    abort(); 
  else 
    v++;

  if (__sync_mem_load (&v, __SYNC_MEM_CONSUME) != count++) 
    abort(); 
  else 
    v++;

  if (__sync_mem_load (&v, __SYNC_MEM_SEQ_CST) != count++) 
    abort(); 
  else 
    v++;

  return 0;
}

