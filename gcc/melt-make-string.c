/*** 
   File melt-make-string.c [making C constant strings from arguments]
   Copyright (C) 2011 Free Software Foundation, Inc.
   Indented with GNU indent.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.   If not see
<http://www.gnu.org/licenses/>.
***/

/** 
  This is a self contained utility program. When given a list of pairs
  as program argument, it outputs on stdout a sequence of C constant
  strings. So, if invoked as:
        melt-make-string foo abc bar bcd
  it produces on stdout:
     const char foo[]="abc";
     const char bar[]="bcd";
  However, it handles special characters correctly, e.g.
       melt-make-string foo 'a"' 
   gives rightly, since the single quote is handled by the shell
       const char foo[]="a\"";
   and so on.
**/
#include <stdio.h>
#include <ctype.h>


/* output a C string with escapes when needed. */
void
output_cstr (const char *s)
{
  char c;
  for (s = s; (c = (*s)) != (char) 0; s++)
    {
      switch (c)
	{
	case '\n':
	  fputs ("\\n", stdout);
	  break;
	case '\r':
	  fputs ("\\r", stdout);
	  break;
	case '\t':
	  fputs ("\\t", stdout);
	  break;
	case '\b':
	  fputs ("\\b", stdout);
	  break;
	case '\v':
	  fputs ("\\v", stdout);
	  break;
	case '\f':
	  fputs ("\\f", stdout);
	  break;
	case '\\':
	  fputs ("\\\\", stdout);
	  break;
	case '\'':
	  fputs ("\\\'", stdout);
	  break;
	case '\"':
	  fputs ("\\\"", stdout);
	  break;
	case '/':
	case '*':
	case '+':
	case '-':
	case '%':
	case '$':
	case ' ':
	  putc (c, stdout);
	  break;
	default:
	  if (c < 0x7f && isprint (c))
	    putc (c, stdout);
	  else
	    printf ("\\%03o", (int) (c & 0xff));
	  break;
	}
    }
}

int
main (int argc, char **argv)
{
  int ix = 0;
  for (ix = 1; (ix + 1) < argc; ix += 2)
    {
      const char *name = argv[ix];
      const char *str = argv[ix + 1];
      const char *ps = NULL;
      int badname = 0;
      if (!name || !str)
	break;
      badname = !isalpha (name[0]);
      for (ps = name; !badname && *ps; ps++)
	if (isalnum (*ps) || *ps == '_')
	  continue;
	else
	  badname = 1;
      if (badname)
	{
	  printf("#error bad name %s\n", name);
	  fprintf (stderr, "bad name #%d: %s\n", ix, name);
	  exit (1);
	}
      printf ("const char %s[]=\"", name);
      output_cstr (str);
      fputs ("\";\n", stdout);
    }
  if (fflush (stdout))
    {
      perror ("failed to flush stdout");
      exit (1);
    };
  return 0;
}
