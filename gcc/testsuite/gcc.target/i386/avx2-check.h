#include <stdlib.h>
#include "cpuid.h"
#include "m256-check.h"

static void avx2_test (void);

static void
__attribute__ ((noinline))
do_test (void)
{
  avx2_test ();
}

int
main ()
{
  unsigned int eax, ebx, ecx, edx;

  if (!__get_cpuid (1, &eax, &ebx, &ecx, &edx))
    return 0;

  /* Run AVX2 test only if host has AVX2 support.  */
  if ((ecx & bit_OSXSAVE) == (bit_OSXSAVE))
    {
      __asm__ ("xchg{l}\t{%%}ebx, %1\n\t"
	       "cpuid\n\t"
	       "xchg{l}\t{%%}ebx, %1\n\t"
	       : "=a" (eax), "=r" (ebx), "=c" (ecx), "=d" (edx)
	       : "0" (7), "2" (0));

      if ((ebx & bit_AVX2) == bit_AVX2) {
	do_test ();
#ifdef DEBUG
	printf ("PASSED\n");
#endif
	return 0;
      }
#ifdef DEBUG
      printf ("SKIPPED\n");
#endif
    }
#ifdef DEBUG
  else
    printf ("SKIPPED\n");
#endif

  return 0;
}
