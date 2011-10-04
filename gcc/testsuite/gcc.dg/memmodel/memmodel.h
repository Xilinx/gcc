int __gdb_memmodel_fini = 0;

void __attribute__((noinline))
memmodel_done ()
{
  __gdb_memmodel_fini = 1;
}


/* A hostile thread is one which changes a memory location so quickly that 
   another thread may never see the same value again.  This is simulated when
   memmodel_other_thread() is defined to modify a memory location every cycle.

   A process implementing a dependency on this value can run into difficulties
   with such a hostile thread.  for instance, implementing an add with a 
   compare_and_swap loop goes something like:
     expected = *mem;
   loop:
     new = expected += value;
     if (!succeed (expected = compare_and_swap (mem, expected, new)))
       goto loop;

   if the content of 'mem' are changed every cycle by memodel_other_thread ()
   this will become an infinite loop since the value *mem will never be 
   'expected' by the time the compare_and_swap is executed.

   HOSTILE_THREAD_THRESHOLD defines the number of intructions which a program 
   will execute before triggering the hostile threead pause. The pause will 
   last for HOSTILE_THREAD_PAUSE instructions, and then the counter will reset
   and begin again.  During the pause period, memodel_other_thread will not
   be called. 

   This provides a chance for forward progress to be made and the infinite loop
   to be avoided.

   if the testcase defines HOSTILE_PAUSE_ERROR, then it will be considered an
   RUNTIME FAILURE if the hostile pause is triggered.  This will allow to test
   for guaranteed forward progress routines.

   If the default values for HOSTILE_THREAD_THRESHOLD or HOSTILE_THREAD_PAUSE
   are insufficient, then the testcase may override these by defining the
   values before including this file.   

   Most testcase are intended to run for very short periods of time, so these
   defaults are considered to be high enough to not trigger on a typical case,
   but not drag the test time out too much if a hostile condition is 
   interferring.  */

  
/* Define the threshold to start pausing the hostile thread.  */
#if !defined (HOSTILE_THREAD_THRESHOLD)
#define HOSTILE_THREAD_THRESHOLD 	500
#endif

/* Define the length of pause in cycles for the hostile thread to pause to
   allow forward progress to be made.  */
#if !defined (HOSTILE_THREAD_PAUSE)
#define HOSTILE_THREAD_PAUSE	20
#endif

void memmodel_other_threads (void);
int memmodel_final_verify (void);

static int __gdb_hostile_pause = 0;

/* This function wraps memodel_other_threads an monitors for an infinite loop.
   If the threshold value HOSTILE_THREAD_THRESHOLD is reached, the other_thread
   process is paused for HOSTILE_THREAD_PAUSE cycles before resuming, and the
   counters start again.  */
void
__gdb_wrapper_other_threads()
{
  static int count = 0;
  static int pause = 0;

  if (++count >= HOSTILE_THREAD_THRESHOLD)
    {
      if (!__gdb_hostile_pause)
        __gdb_hostile_pause = 1;

      /* Count cycles before calling the hostile thread again.  */
      if (pause++ < HOSTILE_THREAD_PAUSE)
	return;

      /* Reset the pause counter, as well as the thread counter.  */
      pause = 0;
      count = 0;
    }
  memmodel_other_threads ();
}


/* If the test case defines HOSTILE_PAUSE_ERROR, then the test case will 
   fail execution if it had a hostile pause.  */
int
__gdb_wrapper_final_verify ()
{
  int ret = memmodel_final_verify ();
#if defined (HOSTILE_PAUSE_ERROR)
  if (__gdb_hostile_pause)
    {
      printf ("FAIL: Forward progress made only by pausing hostile thread\n");
      ret = ret | 1;    /* 0 indicates proper comnpletion.  */
    }
#endif
  return ret;
}
