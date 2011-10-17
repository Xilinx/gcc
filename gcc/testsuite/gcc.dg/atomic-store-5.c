/* Test __atomic routines for existence and proper execution on 16 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_128 } */
/* { dg-options "-mcx16" { target { x86_64-*-* } } } */

/* Test the execution of the __atomic_store builtin for a 16 byte value.  */

extern void abort(void);

__int128_t v, count;

main ()
{
  v = 0;
  count = 0;

  __atomic_store (&v, count + 1, __ATOMIC_RELAXED);
  if (v != ++count)
    abort ();

  __atomic_store (&v, count + 1, __ATOMIC_RELEASE);
  if (v != ++count)
    abort ();

  __atomic_store (&v, count + 1, __ATOMIC_SEQ_CST);
  if (v != ++count)
    abort ();

  return 0;
}

