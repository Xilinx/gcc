/*** file melt-mv-if-changed.c

     This file is part of GCC. It could be useful for the MELT plugin.

     Copyright (C) 2012 Free Software Foundation, Inc.
     Contributed by Basile Starynkevitch <basile@starynkevitch.net>

  GCC is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3, or (at your option) any later
  version.

  GCC is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License
  along with GCC; see the file COPYING3.  If not see
  <http://www.gnu.org/licenses/>.  
***/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int
main (int argc, char **argv)
{
  const char *srcpath = NULL;
  const char *dstpath = NULL;
  struct stat srcstat;
  struct stat dststat;
  bool needmove = false;
  if (argc != 3)
    goto usage;
  if (argv[1][0] == '-')
    goto usage;
  srcpath = argv[1];
  dstpath = argv[2];
  if (access (srcpath, R_OK))
    {
      perror (srcpath);
      exit (EXIT_FAILURE);
    };
  memset (&srcstat, 0, sizeof (srcstat));
  memset (&dststat, 0, sizeof (dststat));
  if (stat (srcpath, &srcstat))
    {
      perror (srcpath);
      exit (EXIT_FAILURE);
    };
  if (stat (dstpath, &dststat))
    needmove = true;
  if (srcstat.st_size != dststat.st_size)
    needmove = true;
  if (!needmove)
    {
      FILE *srcfil = fopen (srcpath, "rb");
      FILE *dstfil = fopen (dstpath, "rb");
      if (!srcfil || !dstfil)
	needmove = true;
      while (!needmove && !feof (srcfil) && !feof (dstfil))
	{
	  int srcb = getc (srcfil);
	  int dstb = getc (dstfil);
	  if (srcb == EOF || dstb == EOF || srcb != dstb)
	    needmove = true;
	}
      if (srcfil)
	fclose (srcfil);
      if (dstfil)
	fclose (dstfil);
    };
  if (needmove)
    {
      if (rename (srcpath, dstpath))
	{
	  perror (dstpath);
	  exit (EXIT_FAILURE);
	}
    }
  else 
    {
      if (unlink (srcpath))
	{
	  perror (srcpath);
	  exit (EXIT_FAILURE);
	}
    }

  return EXIT_SUCCESS;
usage:
  fprintf (stderr, "usage: %s source destination\n", argv[0]);
  return EXIT_FAILURE;
}
