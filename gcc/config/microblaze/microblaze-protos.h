/* Definitions of target machine for GNU compiler, for Xilinx MicroBlaze.
   Copyright 2009 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the
   the Free Software Foundation, 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#ifndef __MICROBLAZE_PROTOS__
#define __MICROBLAZE_PROTOS__

#ifdef RTX_CODE
extern void barrel_shift_left_imm(rtx operands[]);
extern void shift_left_imm(rtx operands[]);
extern void shift_right_imm(rtx operands[]);
extern rtx embedded_pic_offset       (rtx);
extern int pic_address_needs_scratch (rtx);
extern void expand_block_move        (rtx *);
extern void shift_left_imm  (rtx []);
extern void microblaze_expand_prologue (void);
extern void microblaze_expand_epilogue (void);
extern void shift_double_left_imm    (rtx *);
extern void override_options (void);
extern int microblaze_expand_shift (enum shift_type, rtx *);
extern bool microblaze_expand_move (enum machine_mode, rtx *);
extern bool microblaze_expand_block_move (rtx, rtx, rtx, rtx);
extern void rodata_section(void);
extern void sbss_section(void);
extern void sbss2_section(void);
extern void bss_section(void);
extern void sdata2_section(void);
extern int microblaze_can_use_return_insn (void);
extern rtx  microblaze_legitimize_address (rtx, rtx, enum machine_mode);
extern int microblaze_const_double_ok (rtx, enum machine_mode);
extern void print_operand (FILE *, rtx, int);
extern void print_operand_address (FILE *, rtx);
extern void init_cumulative_args (CUMULATIVE_ARGS *,tree, rtx);
extern void output_ascii (FILE *, const char *, int);
extern bool microblaze_legitimate_address_p (enum machine_mode, rtx, int );
extern void microblaze_gen_conditional_branch (rtx *, enum rtx_code);
extern int microblaze_is_interrupt_handler (void);
extern rtx microblaze_return_addr (int, rtx);
extern HOST_WIDE_INT microblaze_debugger_offset (rtx, HOST_WIDE_INT);
extern void microblaze_order_regs_for_local_alloc (void);
extern int simple_memory_operand (rtx, enum machine_mode);
extern int double_memory_operand (rtx, enum machine_mode);
extern enum reg_class microblaze_secondary_reload_class
   (enum reg_class, enum machine_mode, rtx, int);
extern int microblaze_regno_ok_for_base_p (int, int);
extern HOST_WIDE_INT microblaze_initial_elimination_offset (int, int);
extern void microblaze_output_filename (FILE*, const char*);
extern void microblaze_declare_object (FILE *, const char *, const char *,
   const char *, int);
#endif  /* RTX_CODE */

#ifdef TREE_CODE
extern void function_arg_advance (CUMULATIVE_ARGS *, enum machine_mode,
				  tree, int);
extern rtx function_arg (CUMULATIVE_ARGS *, enum machine_mode, tree, int);
#endif /* TREE_CODE */

#endif  /* __MICROBLAZE_PROTOS__ */
