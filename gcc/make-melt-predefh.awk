## generate the header file melt-predef.h

#	Copyright (C)  2009  Free Software Foundation, Inc.
#	Contributed by Basile Starynkevitch  <basile@starynkevitch.net>
#
# This file is part of GCC.
#
# GCC is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3, or (at your option) any later
# version.
#
# GCC is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GCC; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.
BEGIN {
    predefcount = 0;
    print "/* generated file melt-predef.h -- DO NOT EDIT */"
    print "/* generated from melt-predef.list by make-melt-predefh.awk */"
}

# Remove comment and blank lines.
/^ *#/ || /^ *$/ {
  next;
}

{
    predefcount++;
    printf("  MELTGLOB_%s=%d,\n", $1, predefcount);
}

END {
    printf("/* end of generated file melt-predef.h for %d predefined */\n", predefcount);
}