/* Test __sync_mem routines for existence and execution with each valid 
   memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_char_short } */


/* Test that __sync_mem_{thread,signal}_fence builtins execute.  */

main ()
{
  __sync_mem_thread_fence (__SYNC_MEM_RELAXED);
  __sync_mem_thread_fence (__SYNC_MEM_CONSUME);
  __sync_mem_thread_fence (__SYNC_MEM_ACQUIRE);
  __sync_mem_thread_fence (__SYNC_MEM_RELEASE);
  __sync_mem_thread_fence (__SYNC_MEM_ACQ_REL);
  __sync_mem_thread_fence (__SYNC_MEM_SEQ_CST);

  __sync_mem_signal_fence (__SYNC_MEM_RELAXED);
  __sync_mem_signal_fence (__SYNC_MEM_CONSUME);
  __sync_mem_signal_fence (__SYNC_MEM_ACQUIRE);
  __sync_mem_signal_fence (__SYNC_MEM_RELEASE);
  __sync_mem_signal_fence (__SYNC_MEM_ACQ_REL);
  __sync_mem_signal_fence (__SYNC_MEM_SEQ_CST);

  return 0;
}

