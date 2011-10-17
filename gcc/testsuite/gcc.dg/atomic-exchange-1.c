/* Test __atomic routines for existence and proper execution on 1 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */

/* Test the execution of the __atomic_exchange builtin for a char.  */

extern void abort(void);

char v, count;

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
