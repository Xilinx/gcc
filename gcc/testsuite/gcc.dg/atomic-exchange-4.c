/* Test __atomic routines for existence and proper execution on 8 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_long_long } */
/* { dg-options "" } */

/* Test the execution of the __atomic_X builtin for a long_long.  */

extern void abort(void);

long long v, count;

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
