/* Test __atomic routines for invalid memory model errors. This only needs
   to be tested on a single size.  */
/* { dg-do compile } */
/* { dg-require-effective-target sync_int_long } */

#include <stddef.h>

int i;
size_t s;

main ()
{
  __atomic_exchange (&i, 1, __ATOMIC_CONSUME); /* { dg-error "invalid memory model" } */

  __atomic_load (&i, __ATOMIC_RELEASE); /* { dg-error "invalid memory model" } */
  __atomic_load (&i, __ATOMIC_ACQ_REL); /* { dg-error "invalid memory model" } */

  __atomic_store (&i, 1, __ATOMIC_ACQUIRE); /* { dg-error "invalid memory model" } */
  __atomic_store (&i, 1, __ATOMIC_CONSUME); /* { dg-error "invalid memory model" } */
  __atomic_store (&i, 1, __ATOMIC_ACQ_REL); /* { dg-error "invalid memory model" } */

  i = __atomic_always_lock_free (s); /* { dg-error "non-constant argument" } */

}
