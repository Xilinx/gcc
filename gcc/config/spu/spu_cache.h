/* Copyright (C) 2008 Free Software Foundation, Inc.

   This file is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option)
   any later version.

   This file is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* As a special exception, if you include this header file into source files
   compiled by GCC, this header file does not by itself cause  the resulting
   executable to be covered by the GNU General Public License.  This exception
   does not however invalidate any other reasons why the executable file might be
   covered by the GNU General Public License.  */

#ifndef SPU_CACHEH
#define SPU_CACHEH

void *__cache_fetch_dirty (__ea void *ea, int n_bytes_dirty);
void *__cache_fetch (__ea void *ea);
void __cache_evict (__ea void *ea);
void __cache_flush (void);
void __cache_touch (__ea void *ea);

#define cache_fetch_dirty(_ea, _n_bytes_dirty) \
     __cache_fetch_dirty(_ea, _n_bytes_dirty)

#define cache_fetch(_ea) __cache_fetch(_ea)
#define cache_touch(_ea) __cache_touch(_ea)
#define cache_evict(_ea) __cache_evict(_ea)
#define cache_flush() __cache_flush()

#endif
