/* Test __sync_mem routines for invalid memory model errors. This only needs
   to be tested on a single size.  */
/* { dg-do compile } */
/* { dg-require-effective-target sync_int_long } */

int i;

main ()
{
  __sync_mem_exchange (&i, 1, __SYNC_MEM_CONSUME); /* { dg-error "invalid memory model" } */

  __sync_mem_compare_exchange (&i, 1, 2, __SYNC_MEM_SEQ_CST, __SYNC_MEM_RELEASE); /* { dg-error "invalid failure memory model" } */
  __sync_mem_compare_exchange (&i, 1, 2, __SYNC_MEM_SEQ_CST, __SYNC_MEM_ACQ_REL); /* { dg-error "invalid failure memory model" } */
  __sync_mem_compare_exchange (&i, 1, 2, __SYNC_MEM_ACQUIRE, __SYNC_MEM_SEQ_CST); /* { dg-error "failure memory model" } */
  __sync_mem_compare_exchange (&i, 1, 2, __SYNC_MEM_RELAXED, __SYNC_MEM_ACQUIRE); /* { dg-error "failure memory model" } */

  __sync_mem_load (&i, __SYNC_MEM_RELEASE); /* { dg-error "invalid memory model" } */
  __sync_mem_load (&i, __SYNC_MEM_ACQ_REL); /* { dg-error "invalid memory model" } */

  __sync_mem_store (&i, 1, __SYNC_MEM_ACQUIRE); /* { dg-error "invalid memory model" } */
  __sync_mem_store (&i, 1, __SYNC_MEM_CONSUME); /* { dg-error "invalid memory model" } */
  __sync_mem_store (&i, 1, __SYNC_MEM_ACQ_REL); /* { dg-error "invalid memory model" } */

  __sync_mem_flag_clear (&i, __SYNC_MEM_ACQUIRE); /* { dg-error "invalid memory model" } */
  __sync_mem_flag_clear (&i, __SYNC_MEM_ACQ_REL); /* { dg-error "invalid memory model" } */
}
