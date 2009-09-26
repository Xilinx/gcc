# x86/x86_64 support for -fsplit-stack.
# Copyright (C) 2009 Free Software Foundation, Inc.
# Contributed by Ian Lance Taylor <iant@google.com>.

# This file is part of GCC.

# GCC is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3, or (at your option) any later
# version.

# GCC is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.

# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the GCC Runtime Library Exception, version
# 3.1, as published by the Free Software Foundation.

# You should have received a copy of the GNU General Public License and
# a copy of the GCC Runtime Library Exception along with this program;
# see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
# <http://www.gnu.org/licenses/>.


# Support for allocating more stack space when using -fsplit-stack.
# When a function discovers that it needs more stack space, it will
# call __morestack with the size of the stack frame and the size of
# the parameters to copy from the old stack frame to the new one.
# The __morestack function preserves the parameter registers and
# calls __generic_morestack to actually allocate the stack space.

# When this is called stack space is very low, but we ensure that
# there is enough space to push the parameter registers and to call
# __generic_morestack.

# When calling __generic_morestack, FRAME_SIZE points to the size of
# the desired frame when the function is called, and the function
# sets it to the size of the allocated stack.  OLD_STACK points to
# the parameters on the old stack and PARAM_SIZE is the number of
# bytes of parameters to copy to the new stack.  These are the
# parameters of the function that called __morestack.  The
# __generic_morestack function returns the new stack pointer,
# pointing to the address of the first copied parameter.  The return
# value minus the returned *FRAME_SIZE will be the first address on
# the stack which we should not use.

# void *__generic_morestack (size_t *frame_size, void *old_stack,
#			      size_t param_size);

# The __morestack routine arranges for the caller to return to a stub
# on the new stack.  The stub is responsible for restoring the old
# stack pointer and returning to the caller's caller.  This calls
# __generic_releasestack to retrieve the old stack pointer and
# release the newly allocated stack.

# void *__generic_releasestack (void);

	.global	__morestack

#ifdef __ELF__
	.type	__morestack,@function
#endif

__morestack:

#ifndef __x86_64__

	# In 32-bit mode the parameters are pushed on the stack.  The
	# argument size is pushed then the new stack frame size is
	# pushed.

	# In 32-bit mode the registers %eax, %edx, and %ecx may be
	# used for parameters, depending on the regparm and fastcall
	# attributes.  However, we don't have to worry about %ecx, as
	# the function will have saved it on the stack if necessary.

	pushl	%eax
	pushl	%edx

	pushl	16(%esp)		# The size of the parameters.
	leal	28(%esp),%eax		# The caller's	parameters.
	pushl	%eax
	leal	20(%esp),%eax		# The address of the new frame size.
	pushl	%eax

#ifdef __PIC__
	call	__generic_morestack@PLT
#else
	call	__generic_morestack
#endif

	movl	%eax,%ecx		# Copy the new stack pointer.
	subl	24(%esp),%ecx		# The end of the stack space.
	addl	$256,%ecx		# Back off 256 bytes.

	# FIXME: The offset must match
	# TARGET_THREAD_SPLIT_STACK_OFFSET in
	# gcc/config/i386/linux.h.
	movl	%ecx,%gs:0x14		# Save the new stack boundary.

	movl	16(%esp),%eax		# Restore registers.
	movl	12(%esp),%edx

	# We do a little dance here so that the processor's
        # call/return prediction works out right.

	movl	20(%esp),%ecx		# Where we are in the caller.
	movl	%eax,%esp		# Switch to the new stack.
	call	*%ecx			# Call our caller!

	# The caller will return here, as predicted.

	# Save the registers which may hold a return value.  We
	# assume that __generic_releasestack does not touch any
	# floating point or vector registers.
	pushl	%eax
	pushl	%edx

#ifdef __PIC__
	call	__generic_releasestack@PLT
#else
	call	__generic_releasestack
#endif

	movl	%eax,%ecx		# Hold onto old stack pointer.

	popl	%edx			# Restore possible return value
	popl	%eax

	movl	%ecx,%esp		# Switch back to the old stack.

	ret				# Return to caller's caller.

#else /* defined(__x86_64__) */

	# In 64-bit mode the new stack frame size is passed in r10
        # and the argument size is passed in r11.

	# In 64-bit mode the registers %rdi, %rsi, %rdx, %rcx, %r8,
	# and %r9 may be used for parameters.

	pushq	%rdi
	pushq	%rsi
	pushq	%rdx
	pushq	%rcx
	pushq	%r8
	pushq	%r9

	pushq	%r10			# The new frame size.
	leaq	0(%rsp),%rdi		# Its address.
	leaq	64(%rsp),%rsi		# The caller's parameters.
	movq	%r11,%rdx		# The size of the parameters.

#ifdef __PIC__
	call	__generic_morestack@PLT
#else
	call	__generic_morestack
#endif

	movq	%rax,%rcx		# Copy the new stack pointer.
	subq	0(%rsp),%rcx		# The end of the stack space.
	addq	$256,%rcx		# Back off 256 bytes.

	# FIXME: The offset must match
	# TARGET_THREAD_SPLIT_STACK_OFFSET in
	# gcc/config/i386/linux64.h.
	movq	%rcx,%fs:0x28		# Save the new stack boundary.

	movq	48(%rsp),%rdi		# Restore registers.
	movq	40(%rsp),%rsi
	movq	32(%rsp),%rdx
	movq	24(%rsp),%rcx
	movq	16(%rsp),%r8
	movq	8(%rsp),%r9

	# We do a little dance here so that the processor's
        # call/return prediction works out right.

	movq	56(%rsp),%rcx		# Where we are in the caller.
	movq	%rax,%rsp		# Switch to the new stack.
	call	*%rcx			# Call our caller!

	# The caller will return here, as predicted.

	# Save the registers which may hold a return value.  We
	# assume that __generic_releasestack does not touch any
	# floating point or vector registers.
	pushq	%rax
	pushq	%rdx

#ifdef __PIC__
	call	__generic_releasestack@PLT
#else
	call	__generic_releasestack
#endif

	movq	%rax,%rcx		# Hold onto old stack pointer.

	popq	%rdx			# Restore possible return value
	popq	%rax

	movq	%rcx,%rsp		# Switch back to the old stack.

	ret				# Return to caller's caller.

#endif /* defined(__x86_64__) */

#ifdef __ELF__
	.size	__morestack, . - __morestack
#endif

# Initialize the stack test value when the program starts.  We don't
# know how large the main stack is, so we guess conservatively.  We
# might be able to use getrlimit here.

#ifdef __ELF__
	.type	init, @function
#endif

init:

#ifndef __x86_64__

	leal	-16000(%esp),%eax	# We should have at least 16K.
	movl	%eax,%gs:0x14
	ret

#else /* defined(__x86_64__) */

	leaq	-16000(%rsp),%rax	# We should have at least 16K.
	movq	%rax,%fs:0x28
	ret

#endif /* defined(__x86_64__) */

#ifdef __ELF__
	.size	init, . - init
#endif

# Make init a high priority constructor.  FIXME: This is ELF
# specific.

	.section	.ctors.65535,"aw",@progbits

#ifndef __x86_64__
	.align	4
	.long	init
#else
	.align	8
	.quad	init
#endif

#ifdef __ELF__
	.section	.note.GNU-stack,"",@progbits
#endif
