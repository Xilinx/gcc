/* Test __atomic routines for existence and proper execution on 2 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */

/* Test the execution of the __atomic_store builtin for a short.  */

extern void abort(void);

short v, count;

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

