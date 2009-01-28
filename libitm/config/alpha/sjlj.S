/* Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
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

	.text
	.align 4
	.globl	_ITM_beginTransaction
	.ent	_ITM_beginTransaction

#define FRAME	144

_ITM_beginTransaction:
	ldgp	$29, 0($27)
	subq	$30, FRAME, $30
	.frame	$30, FRAME, $26, 0
	.mask	0x04000000, 0
	stq	$26, 0($30)
	.prologue 1

	stq	$9, 8($30)
	stq	$10, 16($30)
	addq	$30, FRAME, $0
	stq	$11, 24($30)

	stq	$12, 32($30)
	stq	$13, 40($30)
	stq	$14, 48($30)
	stq	$15, 56($30)

	stq	$0, 64($30)
	stt	$f2, 72($30)
	stt	$f3, 80($30)
	stt	$f4, 88($30)

	stt	$f5, 96($30)
	stt	$f6, 104($30)
	stt	$f7, 112($30)
	stt	$f8, 120($30)

	stt	$f9, 128($30)
	mov	$30, $17
#ifdef __PIC__
	unop
	bsr	$26, GTM_begin_transaction !samegp
#else
	jsr	$26, GTM_begin_transaction
	ldgp	$29, 0($26)
#endif

	ldq	$26, 0($30)
	addq	$30, FRAME $30
	ret
.end _ITM_beginTransaction

	.align 4
	.globl	GTM_longjmp
	.hidden	GTM_longjmp
	.ent	GTM_longjmp

GTM_longjmp:
	.prologue 0
	ldq	$26, 0($16)
	ldq	$9, 8($16)
	ldq	$10, 16($16)
	ldq	$11, 24($16)

	ldq	$12, 32($16)
	ldq	$13, 40($16)
	ldq	$14, 48($16)
	ldq	$15, 56($16)

	ldq	$1, 64($16)
	ldt	$f2, 72($16)
	ldt	$f3, 80($16)
	ldt	$f4, 88($16)

	ldt	$f5, 96($16)
	ldt	$f6, 104($16)
	ldt	$f7, 112($16)
	ldt	$f8, 120($16)

	ldt	$f9, 128($16)
	mov	$17, $0
	mov	$1, $30
	ret
.end GTM_longjmp

.section .note.GNU-stack, "", @progbits
