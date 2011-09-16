/* Test __sync_mem routines for invalid memory model errors. This only needs
   to be tested on a single size.  */
/* { dg-do compile } */
/* { dg-require-effective-target sync_int_long } */

int i;

main ()
{

  __sync_mem_exchange (&i, 1); /* { dg-error "too few arguments" } */
  __sync_mem_exchange (&i, 1, __SYNC_MEM_SEQ_CST, __SYNC_MEM_SEQ_CST); /* { dg-error "too many arguments" } */
}
