/* Test __sync_mem routines for existence and execution with each valid 
   memory model.  */
/* { dg-options "-w" } */
/* { dg-do run } */
/* { dg-additional-sources "sync-mem-lockfree-aux.c" } */

/* Test that __sync_mem_{is,always}_lock_free builtins execute.
   sync-mem-lockfree-aux.c supplies and external entry point for 
   __sync_mem_is_lock_free which always returns a 2. We can detect the 
   external routine was called if 2 is returned since that is not a valid
   result normally.  */

#include <stdlib.h>

extern void abort();

int r1, r2;

/* Test for consistency on sizes 1, 2, 4, 8, 16 and 32.  */
main ()
{
  
  r1 = __sync_mem_always_lock_free (sizeof(char));
  r2 = __sync_mem_is_lock_free (sizeof(char));
  /* If always lock free, then is_lock_free must also be true.  */
  if (r1)
    { 
      if (r2 != 1)  
	abort ();
    }
  else
    {
      /* If it is not lock free, then the external routine must be called.  */
      if (r2 != 2) 
	abort ();
    }
  
  r1 = __sync_mem_always_lock_free (2);
  r2 = __sync_mem_is_lock_free (2);
  /* If always lock free, then is_lock_free must also be true.  */
  if (r1)
    { 
      if (r2 != 1)  
	abort ();
    }
  else
    {
      /* If it is not lock free, then the external routine must be called.  */
      if (r2 != 2) 
	abort ();
    }

   
  r1 = __sync_mem_always_lock_free (4);
  r2 = __sync_mem_is_lock_free (4);     /* Try passing in a variable.  */
  /* If always lock free, then is_lock_free must also be true.  */
  if (r1)
    { 
      if (r2 != 1)  
	abort ();
    }
  else
    {
      /* If it is not lock free, then the external routine must be called.  */
      if (r2 != 2) 
	abort ();
    }

   
  r1 = __sync_mem_always_lock_free (8);
  r2 = __sync_mem_is_lock_free (8);
  /* If always lock free, then is_lock_free must also be true.  */
  if (r1)
    { 
      if (r2 != 1)  
	abort ();
    }
  else
    {
      /* If it is not lock free, then the external routine must be called.  */
      if (r2 != 2) 
	abort ();
    }

   
  r1 = __sync_mem_always_lock_free (16);
  r2 = __sync_mem_is_lock_free (16);
  /* If always lock free, then is_lock_free must also be true.  */
  if (r1)
    { 
      if (r2 != 1)  
	abort ();
    }
  else
    {
      /* If it is not lock free, then the external routine must be called.  */
      if (r2 != 2) 
	abort ();
    }

   
  r1 = __sync_mem_always_lock_free (32);
  r2 = __sync_mem_is_lock_free (32);
  /* If always lock free, then is_lock_free must also be true.  */
  if (r1)
    { 
      if (r2 != 1)  
	abort ();
    }
  else
    {
      /* If it is not lock free, then the external routine must be called.  */
      if (r2 != 2) 
	abort ();
    }

 
  return 0;
}

