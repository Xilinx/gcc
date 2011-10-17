/* Test __atomic routines for existence and proper execution on 4 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_long } */

extern void abort(void);

int v, count;


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

