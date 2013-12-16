/* go-callers.c -- get callers for Go.

   Copyright 2012 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

#include "config.h"

#include "backtrace.h"

#include "runtime.h"
#include "array.h"

/* This is set to non-zero when calling backtrace_full.  This is used
   to avoid getting hanging on a recursive lock in dl_iterate_phdr on
   older versions of glibc when a SIGPROF signal arrives while
   collecting a backtrace.  */

uint32 runtime_in_callers;

/* Argument passed to callback function.  */

struct callers_data
{
  Location *locbuf;
  int skip;
  int index;
  int max;
};

/* Callback function for backtrace_full.  Just collect the locations.
   Return zero to continue, non-zero to stop.  */

static int
callback (void *data, uintptr_t pc, const char *filename, int lineno,
	  const char *function)
{
  struct callers_data *arg = (struct callers_data *) data;
  Location *loc;

  /* Skip split stack functions.  */
  if (function != NULL)
    {
      const char *p;

      p = function;
      if (__builtin_strncmp (p, "___", 3) == 0)
	++p;
      if (__builtin_strncmp (p, "__morestack_", 12) == 0)
	return 0;
    }
  else if (filename != NULL)
    {
      const char *p;

      p = strrchr (filename, '/');
      if (p == NULL)
	p = filename;
      if (__builtin_strncmp (p, "/morestack.S", 12) == 0)
	return 0;
    }

  /* Skip thunks and recover functions.  There is no equivalent to
     these functions in the gc toolchain, so returning them here means
     significantly different results for runtime.Caller(N).  */
  if (function != NULL)
    {
      const char *p;

      p = __builtin_strchr (function, '.');
      if (p != NULL && __builtin_strncmp (p + 1, "$thunk", 6) == 0)
	return 0;
      p = __builtin_strrchr (function, '$');
      if (p != NULL && __builtin_strcmp(p, "$recover") == 0)
	return 0;
    }

  if (arg->skip > 0)
    {
      --arg->skip;
      return 0;
    }

  loc = &arg->locbuf[arg->index];
  loc->pc = pc;

  /* The libbacktrace library says that these strings might disappear,
     but with the current implementation they won't.  We can't easily
     allocate memory here, so for now assume that we can save a
     pointer to the strings.  */
  loc->filename = runtime_gostringnocopy ((const byte *) filename);
  loc->function = runtime_gostringnocopy ((const byte *) function);

  loc->lineno = lineno;
  ++arg->index;
  return arg->index >= arg->max;
}

/* Error callback.  */

static void
error_callback (void *data __attribute__ ((unused)),
		const char *msg, int errnum)
{
  if (errnum != 0)
    runtime_printf ("%s errno %d\n", msg, errnum);
  runtime_throw (msg);
}

/* Gather caller PC's.  */

int32
runtime_callers (int32 skip, Location *locbuf, int32 m)
{
  struct callers_data data;

  data.locbuf = locbuf;
  data.skip = skip + 1;
  data.index = 0;
  data.max = m;
  runtime_xadd (&runtime_in_callers, 1);
  backtrace_full (__go_get_backtrace_state (), 0, callback, error_callback,
		  &data);
  runtime_xadd (&runtime_in_callers, -1);
  return data.index;
}

int Callers (int, struct __go_open_array)
  __asm__ (GOSYM_PREFIX "runtime.Callers");

int
Callers (int skip, struct __go_open_array pc)
{
  Location *locbuf;
  int ret;
  int i;

  locbuf = (Location *) runtime_mal (pc.__count * sizeof (Location));

  /* In the Go 1 release runtime.Callers has an off-by-one error,
     which we can not correct because it would break backward
     compatibility.  Normally we would add 1 to SKIP here, but we
     don't so that we are compatible.  */
  ret = runtime_callers (skip, locbuf, pc.__count);

  for (i = 0; i < ret; i++)
    ((uintptr *) pc.__values)[i] = locbuf[i].pc;

  return ret;
}
