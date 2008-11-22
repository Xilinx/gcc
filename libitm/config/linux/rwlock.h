/* Copyright (C) 2008 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libgomp; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

#ifndef GTM_RWLOCK_H
#define GTM_RWLOCK_H

/* The read-write summary definition.  */

#define RWLOCK_S_LOCK		1
#define RWLOCK_A_WRITER		2
#define RWLOCK_W_WRITER		4
#define RWLOCK_A_READER		8
#define RWLOCK_W_READER		16
#define RWLOCK_RW_UPGRADE	32

typedef struct {
  int summary;
  int a_readers;
  int w_readers;
  int w_writers;
} gtm_rwlock;

extern void REGPARM gtm_rwlock_read_lock (gtm_rwlock *);
extern void REGPARM gtm_rwlock_write_lock (gtm_rwlock *);
extern bool REGPARM gtm_rwlock_write_upgrade (gtm_rwlock *);
extern void REGPARM gtm_rwlock_read_unlock (gtm_rwlock *);
extern void REGPARM gtm_rwlock_write_unlock (gtm_rwlock *);

#endif /* GTM_RWLOCK_H */
