/* Test __atomic routines for existence and proper execution on 16 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_128 } */
/* { dg-options "-mcx16" { target { x86_64-*-* } } } */

/* Test the execution of the __atomic_X builtin for a 16 byte value.  */

extern void abort(void);

__int128_t v, count;

main ()
{
  v = 0;
  count = 0;

  if (__atomic_exchange (&v, count + 1, __ATOMIC_RELAXED) !=  count++) 
    abort ();

  if (__atomic_exchange (&v, count + 1, __ATOMIC_ACQUIRE) !=  count++) 
    abort ();

  if (__atomic_exchange (&v, count + 1, __ATOMIC_RELEASE) !=  count++) 
    abort ();

  if (__atomic_exchange (&v, count + 1, __ATOMIC_ACQ_REL) !=  count++) 
    abort ();

  if (__atomic_exchange (&v, count + 1, __ATOMIC_SEQ_CST) !=  count++) 
    abort ();

  return 0;
}
