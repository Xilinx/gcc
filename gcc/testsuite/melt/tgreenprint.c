// file tgreenprint.c
// a testcase for the makegreen command and pass
/*** 
      run in buildir/gcc 
  ./cc1 -fmelt=makegreen \
   -fmelt-module-path=. \
   -fmelt-compile-script=./built-melt-cc-script \
   -fmelt-source-path=.:...melt-source -fmelt-tempdir=/tmp \
   -fmelt-debug \
   -Wall -O3 .../tgreenprint.c

 ***/

#include <stdio.h>

#include <stdlib.h>

extern FILE *tGreenPrint_otherfile;

void
tGreenPrint_simple_fprint_stdout (int p)
{
  fprintf (stdout, "simple fprint stdout p=%d\n", p);
}

static inline void
tGreenPrint_say_str_int (FILE * f, const char *str, int n)
{
  if (f)
    {
      fprintf (f, "say str=%s n=%d\n", str, n);
      fflush (f);
    }
}


void
tGreenPrint_green_test (void)
{
  if (!tGreenPrint_otherfile)
    tGreenPrint_otherfile = tmpfile ();
  tGreenPrint_say_str_int (stdout, "green_test stdout", __LINE__);
  tGreenPrint_say_str_int (stderr, "green_test stderr", __LINE__);
  tGreenPrint_say_str_int ((FILE *) 0, "green_test nullfile", __LINE__);
  tGreenPrint_say_str_int (tGreenPrint_otherfile, "green_test otherfile",
			   __LINE__);
}
