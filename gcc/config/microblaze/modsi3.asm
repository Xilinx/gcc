###################################
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
# modsi3.asm 
# 
# modulo operation for 32 bit integers.
#	Input :	op1 in Reg r5
#		op2 in Reg r6
#	Output: op1 mod op2 in Reg r3
# 
#######################################

	.globl	__modsi3
	.ent	__modsi3
	.type	__modsi3,@function
__modsi3:
	.frame	r1,0,r15	

	addik	r1,r1,-16
	swi	r28,r1,0
	swi	r29,r1,4
	swi	r30,r1,8
	swi	r31,r1,12

	BEQI	r6,$LaDiv_By_Zero       # Div_by_Zero   # Division Error
	BEQI	r5,$LaResult_Is_Zero    # Result is Zero 
	BGEId	r5,$LaR5_Pos 
	ADD	r28,r5,r0               # Get the sign of the result [ Depends only on the first arg]
	RSUBI	r5,r5,0	                # Make r5 positive
$LaR5_Pos:
	BGEI	r6,$LaR6_Pos
	RSUBI	r6,r6,0	    # Make r6 positive
$LaR6_Pos:
	ADDIK	r3,r0,0      # Clear mod
	ADDIK	r30,r0,0     # clear div
	ADDIK	r29,r0,32    # Initialize the loop count
   # First part try to find the first '1' in the r5
$LaDIV1:
	ADD	r5,r5,r5         # left shift logical r5
	BGEID	r5,$LaDIV1       #
	ADDIK	r29,r29,-1
$LaDIV2:
	ADD	r5,r5,r5         # left shift logical  r5 get the '1' into the Carry
	ADDC	r3,r3,r3         # Move that bit into the Mod register
	rSUB	r31,r6,r3        # Try to subtract (r30 a r6)
	BLTi	r31,$LaMOD_TOO_SMALL
	OR	r3,r0,r31       # Move the r31 to mod since the result was positive
	ADDIK	r30,r30,1
$LaMOD_TOO_SMALL:
	ADDIK	r29,r29,-1
	BEQi	r29,$LaLOOP_END
	ADD	r30,r30,r30         # Shift in the '1' into div
	BRI	$LaDIV2          # Div2
$LaLOOP_END:
	BGEI	r28,$LaRETURN_HERE
	BRId	$LaRETURN_HERE
	rsubi	r3,r3,0 # Negate the result
$LaDiv_By_Zero:
$LaResult_Is_Zero:
	or	r3,r0,r0        # set result to 0 [Both mod as well as div are 0]
$LaRETURN_HERE:
# Restore values of CSRs and that of r3 and the divisor and the dividend
	lwi	r28,r1,0
	lwi	r29,r1,4
	lwi	r30,r1,8
	lwi	r31,r1,12
	rtsd	r15,8
	addik	r1,r1,16
        .end __modsi3
	.size	__modsi3, . - __modsi3

