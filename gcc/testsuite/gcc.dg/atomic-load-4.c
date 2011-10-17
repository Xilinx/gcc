/* Test __atomic routines for existence and proper execution on 8 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_long_long } */
/* { dg-options "" } */

extern void abort(void);

long long v, count;

main ()
{
  v = 0;
  count = 0;

  if (__atomic_load (&v, __ATOMIC_RELAXED) != count++) 
    abort(); 
  else 
    v++;

  if (__atomic_load (&v, __ATOMIC_ACQUIRE) != count++) 
    abort(); 
  else 
    v++;

  if (__atomic_load (&v, __ATOMIC_CONSUME) != count++) 
    abort(); 
  else 
    v++;

  if (__atomic_load (&v, __ATOMIC_SEQ_CST) != count++) 
    abort(); 
  else 
    v++;

  return 0;
}

