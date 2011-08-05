	.file	"c3rawstruct.cc"
	.text
.globl _Z1gv
	.type	_Z1gv, @function
_Z1gv:
.LFB0:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movl	$1, -16(%rbp)
	movl	$2, -12(%rbp)
	movl	-16(%rbp), %eax
	leave
	ret
	.cfi_endproc
.LFE0:
	.size	_Z1gv, .-_Z1gv
.globl _Z1hv
	.type	_Z1hv, @function
_Z1hv:
.LFB1:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	movl	$3, -16(%rbp)
	movl	$4, -12(%rbp)
	movl	-12(%rbp), %eax
	leave
	ret
	.cfi_endproc
.LFE1:
	.size	_Z1hv, .-_Z1hv
.globl s
	.data
	.align 4
	.type	s, @object
	.size	s, 8
s:
	.long	5
	.long	6
	.text
.globl main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	call	_Z1gv
	movl	%eax, %ebx
	call	_Z1hv
	leal	(%rbx,%rax), %edx
	movl	s(%rip), %eax
	addl	%eax, %edx
	movl	s+4(%rip), %eax
	leal	(%rdx,%rax), %eax
	addq	$8, %rsp
	popq	%rbx
	leave
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
