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

	.text
	.p2align 4
	.globl	_ITM_beginTransaction
	.type	_ITM_beginTransaction, @function

_ITM_beginTransaction:
	.cfi_startproc
#ifdef __LP64__
	leaq	8(%rsp), %rax
	movq	(%rsp), %r8
	subq	$72, %rsp
	.cfi_def_cfa_offset 80
	movq	%rax, (%rsp)
	movq	%r8, 8(%rsp)
	movq	%rbx, 16(%rsp)
	movq	%rbp, 24(%rsp)
	movq	%r12, 32(%rsp)
	movq	%r13, 40(%rsp)
	movq	%r14, 48(%rsp)
	movq	%r15, 56(%rsp)
	movq	%rsp, %rsi
	call	GTM_begin_transaction
	addq	$72, %rsp
	.cfi_def_cfa_offset 8
	ret
#else
	leal	4(%esp), %ecx
	subl	$28, %esp
	.cfi_def_cfa_offset 32
	movl	%ecx, 8(%esp)
	movl	%ebx, 12(%esp)
	movl	%esi, 16(%esp)
	movl	%edi, 20(%esp)
	movl	%ebp, 24(%esp)
	leal	8(%esp), %edx
	call	GTM_begin_transaction
	addl	$28, %esp
	.cfi_def_cfa_offset 4
	ret
#endif
	.cfi_endproc
	.size	_ITM_beginTransaction, .-_ITM_beginTransaction

	.p2align 4
	.globl	GTM_longjmp
	.type	GTM_longjmp, @function
	.hidden	GTM_longjmp

GTM_longjmp:
	.cfi_startproc
#ifdef __LP64__
	movq	(%rsi), %rcx
	movq	8(%rsi), %rdx
	movq	16(%rsi), %rbx
	movq	24(%rsi), %rbp
	movq	32(%rsi), %r12
	movq	40(%rsi), %r13
	movq	48(%rsi), %r14
	movq	56(%rsi), %r15
	movl	%edi, %eax
	.cfi_def_cfa %rcx, 0
	.cfi_register %rip, %rdx
	movq	%rcx, %rsp
	jmp	*%rdx
#else
	xchgl	%eax, %edx
	movl	(%edx), %ecx
	movl	4(%edx), %ebx
	movl	8(%edx), %esi
	movl	12(%edx), %edi
	movl	16(%edx), %ebp
	movl	20(%edx), %edx
	.cfi_def_cfa %ecx, 0
	.cfi_register %eip, %edx
	movl	%ecx, %esp
	jmp	*%edx
#endif
	.cfi_endproc
	.size	GTM_longjmp, .-GTM_longjmp

.section .note.GNU-stack, "", @progbits
