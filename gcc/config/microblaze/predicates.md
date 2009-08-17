;; Predicate definitions for Xilinx MicroBlaze
;; Copyright 2009 Free Software Foundation, Inc.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING.   not, write to
;; the Free Software Foundation, 51 Franklin Street, Fifth Floor,
;; Boston, MA 02110-1301, USA.


;; Return whether OP can be used as an operands in arithmetic.
(define_predicate "arith_operand"
  (ior (match_code "const_int")
       (match_operand 0 "register_operand")))

;; FIXME:  Should this include test INTVAL(op)==0
(define_predicate "const_0_operand"
  (and (match_code "const_int,const_double")
       (match_test "op == CONST0_RTX (GET_MODE (op))")))

;; Return whether OP is a register or the constant 0.
(define_predicate "reg_or_0_operand"
  (ior (match_operand 0 "const_0_operand")
       (match_operand 0 "register_operand")))

;; Return whether OP is a integer which fits in 16 bits.
(define_predicate "small_int"
  (and (match_code "const_int")
       (match_test "SMALL_INT (op)")))

;; Return if the code of this rtx pattern is EQ or NE.
(define_predicate "equality_operator"
  (match_code "eq,ne"))

;; Return if the code of this rtx pattern is LT or LTU.
(define_predicate "lessthan_operator"
  (match_code "lt,ltu"))

;; Return if the code of this rtx pattern is a comparison.
(define_predicate "cmp_op"
  (match_code "eq,ne,gt,ge,gtu,geu,lt,le,ltu,leu"))

;; Return if the code of this rtx pattern is a signed comparison.
(define_predicate "signed_cmp_op"
  (match_code "eq,ne,gt,ge,lt,le"))

;; Return if the code of this rtx pattern is an unsigned comparison.
(define_predicate "unsigned_cmp_op"
  (match_code "gtu,geu,ltu,leu"))

;;  Return if the operand is either the PC or a label_ref.  
(define_special_predicate "pc_or_label_operand"
  (ior (match_code "pc,label_ref")
       (and (match_code "symbol_ref")
            (match_test "!(strcmp ((XSTR (op, 0)), \"_stack_overflow_exit\"))"))))

;; Test for valid call operand
(define_predicate "call_insn_operand"
  (match_test "CALL_INSN_OP (op)"))

;; Return if OPERAND is valid as a source operand for a move instruction.
(define_predicate "move_operand"
  (and (
     not (
       and (match_code "plus")
           (not (match_test "(GET_CODE (XEXP (op, 0)) == REG) ^ (GET_CODE (XEXP (op,1)) == REG)"))
	 )
       )
       (match_operand 0 "general_operand")))

;; Return if OPERAND is valid as a source operand for movdi.
(define_predicate "movdi_operand"
  (match_operand 0 "general_operand"))

;; Return if X is a SIGN or ZERO extend operator. 
(define_predicate "extend_operator"
  (match_code "sign_extend,zero_extend"))

;; Accept any operator that can be used to shift the high half of the
;;   input value to the lower half, suitable for truncation.  The
;;   remainder (the lower half of the input, and the upper half of the
;;   output) will be discarded.  
(define_predicate "highpart_shift_operator"
  (match_code "ashiftrt,rotatert,rotate"))

;; Test for valid PIC call operand
(define_predicate "call_insn_plt_operand"
  (match_test "PLT_ADDR_P (op)"))
