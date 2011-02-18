/* A string to string mapping. 

   Copyright (C) 2011 Free Software Foundation, Inc.
   Contributed by Lawrence Crowl <crowl@google.com>.
   
   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

typedef struct strstrmap_d strstrmap_t;

extern strstrmap_t* strstrmap_create (void);
extern void strstrmap_destroy (strstrmap_t *tbl);
extern const char* strstrmap_insert (strstrmap_t *tbl, const char *key,
                                     const char *val);
extern const char* strstrmap_lookup (strstrmap_t *tbl, const char *key);
