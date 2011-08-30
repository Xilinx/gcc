## generator of the MELT file melt/warmeltmelt-predef.melt

#	Copyright (C)  2009,2011  Free Software Foundation, Inc.
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
    print ";; generated file warmmelt-predef.melt -- DO NOT EDIT "
    print ";; generated from melt-predef.list by make-warmelt-predef.awk"
    print "(defun fill_initial_predefmap (predefmap)"
    print "  (let ( (curpredef ()) )"
}

# Remove comment and blank lines.
/^ *#/ || /^ *$/ {
  next;
}

{
## special hack to ignore some obsolete predefined
    predefcount++;
    if ($2 == "@POISON")
    {
	printf (" (compile_warning \"poisoned predefined %s %d\")\n", 
		$1, predefcount);
	printf ("@@ Poisoned MELT predefined %s %d\n", $1, predefcount) > "/dev/stderr"
	next;
    }
    printf("  (code_chunk %s_chk #{ $curpredef = MELT_PREDEF(%s) /*%d*/; }#)\n",
	   $1, $1, predefcount);
    printf("  (mapobject_put predefmap curpredef '%s) ; %d\n",
	   $1, predefcount)
}

END {
    print "));; end of generated fill_initial_predefmap\n";
    printf(";; end of generated file warmelt-predef.melt for %d predefined objects.\n", predefcount);
}
