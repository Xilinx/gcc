###################################-*-asm*- 
# 
#   Copyright 2009 Free Software Foundation, Inc.
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
# 
# mulsi3.asm 
# 
# Multiply operation for 32 bit integers.
#	Input :	Operand1 in Reg r5
#		Operand2 in Reg r6
#	Output: Result [op1 * op2] in Reg r3
# 
#######################################

	.globl	__mulsi3
	.ent	__mulsi3
	.type	__mulsi3,@function
__mulsi3:
	.frame	r1,0,r15
	add	r3,r0,r0
	BEQI	r5,$L_Result_Is_Zero      # Multiply by Zero
	BEQI	r6,$L_Result_Is_Zero      # Multiply by Zero
	BGEId	r5,$L_R5_Pos 
	XOR	r4,r5,r6                  # Get the sign of the result
	RSUBI	r5,r5,0	                  # Make r5 positive
$L_R5_Pos:
	BGEI	r6,$L_R6_Pos
	RSUBI	r6,r6,0	                  # Make r6 positive
$L_R6_Pos:	
	bri	$L1
$L2:	
	add	r5,r5,r5
$L1:	
	srl	r6,r6
	addc	r7,r0,r0
	beqi	r7,$L2
	bneid	r6,$L2
	add	r3,r3,r5	
	blti	r4,$L_NegateResult			
	rtsd	r15,8
	nop
$L_NegateResult:
	rtsd	r15,8
	rsub	r3,r3,r0
$L_Result_Is_Zero:
	rtsd	r15,8
	addi	r3,r0,0
	.end __mulsi3
	.size	__mulsi3, . - __mulsi3
