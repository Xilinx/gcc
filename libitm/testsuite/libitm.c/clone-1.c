/* Verify that we can look up tm clone of tm_callable and tm_pure.  */

#include <stdlib.h>

#ifdef __i386__
#define REGPARM  __attribute__((regparm(2)))
#else
#define REGPARM
#endif

extern void *_ITM_getTMCloneSafe (void *) REGPARM;

static int x;

int __attribute__((tm_pure)) pure(int i)
{
  return i+2;
}

int __attribute__((tm_callable)) callable(void)
{
  return ++x;
}

int main()
{
  if (_ITM_getTMCloneSafe (&pure) != &pure)
    abort ();

  if (_ITM_getTMCloneSafe (&callable) == NULL)
    abort ();

  return 0;
}
