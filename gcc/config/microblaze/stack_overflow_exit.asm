###################################-*-asm*- 
# 
#    Copyright 2009 Free Software Foundation, Inc.
# 
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any
# later version.
# 
# In addition to the permissions in the GNU General Public License, the
# Free Software Foundation gives you unlimited permission to link the
# compiled version of this file with other programs, and to distribute
# those programs without any restriction coming from the use of this
# file.  (The General Public License restrictions do apply in other
# respects; for example, they cover modification of the file, and
# distribution when not linked into another program.)
# 
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.  
#
#    As a special exception, if you link this library with other files,
#    some of which are compiled with GCC, to produce an executable,
#    this library does not by itself cause the resulting executable
#    to be covered by the GNU General Public License.
#    This exception does not however invalidate any other reasons why
#    the executable file might be covered by the GNU General Public License.  
# 
# stack_overflow_exit.asm
# 
# Checks for stack overflows and sets the global variable 
# stack_overflow_error with the value of current stack pointer
#
# This routine exits from the program
# 
#######################################

	.globl	_stack_overflow_error
	.data
	.align	2
	.type	_stack_overflow_error,@object
	.size	_stack_overflow_error,4
_stack_overflow_error:
	.data32	0

	.text 
	.globl	_stack_overflow_exit	
	.ent	_stack_overflow_exit
	.type	_stack_overflow_exit,@function

_stack_overflow_exit:
#ifdef __PIC__
	mfs	r20,rpc
	addik	r20,r20,_GLOBAL_OFFSET_TABLE_+8
	swi	r1,r20,_stack_overflow_error@GOTOFF
	bri	exit@PLT
#else
	swi	r1,r0,_stack_overflow_error
	bri	exit
#endif

	.end 	_stack_overflow_exit
	.size	_stack_overflow_exit,. - _stack_overflow_exit
