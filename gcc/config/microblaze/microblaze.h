/* Definitions of target machine for GNU compiler for Xilinx MicroBlaze.
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
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */


/* Standard GCC variables that we reference.  */

/* MICROBLAZE external variables defined in microblaze.c.  */

/* comparison type */
enum cmp_type
{
  CMP_SI,			/* compare four byte integers */
  CMP_DI,			/* compare eight byte integers */
  CMP_SF,			/* compare single precision floats */
  CMP_DF,			/* compare double precision floats */
  CMP_MAX			/* max comparison type */
};

/* types of shift */
enum shift_type
{
  NONE,				/* Invalid shift type */
  ASHL,				/* Arithmetic shift left */
  ASHR,				/* Arithmetic shift right */
  LSHR				/* Logical shift right */
};

/* Which pipeline to schedule for. */
enum pipeline_type
{
  MICROBLAZE_PIPE_3 = 0,
  MICROBLAZE_PIPE_5 = 1
};

/* Which ABI to use.  */
#define ABI_32  0
#define microblaze_abi ABI_32

/* Whether to emit abicalls code sequences or not.  */
enum microblaze_abicalls_type
{
  MICROBLAZE_ABICALLS_NO,
  MICROBLAZE_ABICALLS_YES
};

/* Recast the abicalls class to be the abicalls attribute.  */
#define microblaze_abicalls_attr ((enum attr_abicalls)microblaze_abicalls)

#define MICROBLAZE_MASK_NO_UNSAFE_DELAY         0x00000001

/* register names (a0 vs. $4). */
extern char microblaze_reg_names[][8];

/* print_operand punctuation chars */
extern char microblaze_print_operand_punct[];

/* filename current function is in */
extern const char *current_function_file;

/* current .file # */
extern int num_source_filenames;

/* != 0 if inside of a function */
extern int inside_function;

/* != 0 if we are to ignore next .loc */
extern int ignore_line_number;

/* warning given about .file in func */
extern int file_in_function_warning;

/* block start/end next label # */
extern int sdb_label_count;

/* Starting Line of current function */
extern int sdb_begin_function_line;

/* # bytes of data/sdata cutoff */
extern int microblaze_section_threshold;

/* sgi next label # for each stmt */
extern int sym_lineno;

/* Map register # to debug register # */
extern int microblaze_dbx_regno[];

/* operands for compare */
extern struct rtx_def *branch_cmp[2];

/* for svr4 abi pic calls */
extern enum microblaze_abicalls_type microblaze_abicalls;

/* architectural level */
extern int microblaze_isa;

/* total # load related delay slots */
extern int dslots_load_total;

/* # filled load delay slots */
extern int dslots_load_filled;

/* total # jump related delay slots */
extern int dslots_jump_total;

/* # filled jump delay slots */
extern int dslots_jump_filled;

/* # of nops needed by previous insn */
extern int dslots_number_nops;

/* # 1/2/3 word references */
extern int num_refs[3];

/* register to check for load delay */
extern struct rtx_def *microblaze_load_reg;

/* 2nd reg to check for load delay */
extern struct rtx_def *microblaze_load_reg2;

/* 3rd reg to check for load delay */
extern struct rtx_def *microblaze_load_reg3;

/* 4th reg to check for load delay */
extern struct rtx_def *microblaze_load_reg4;

extern const char *asm_file_name;
extern char call_used_regs[];
extern int current_function_calls_alloca;
extern char *language_string;
extern int may_call_alloca;
extern int target_flags;
extern struct microblaze_cpu_select microblaze_select;
extern int microblaze_no_unsafe_delay;
extern enum pipeline_type microblaze_pipe;
extern enum cmp_type branch_type;
extern char *microblaze_no_clearbss;

/* Functions within microblaze.c that we reference.  Some of these return
   type HOST_WIDE_INT, so define that here.  */

#include "hwint.h"

/* This is the only format we support */
#define OBJECT_FORMAT_ELF

/* Run-time compilation parameters selecting different hardware subsets.  */

/* Macros used in the machine description to test the flags.  */
#define TARGET_UNIX_ASM		0
#define TARGET_MICROBLAZE_AS		TARGET_UNIX_ASM
#define TARGET_MICROBLAZE_ASM   0

#define TARGET_GP_OPT            0

/* Optimize for Sdata/Sbss */
#define TARGET_XLGP_OPT		(target_flags & MASK_XLGPOPT)

/* We must disable the function end stabs when doing the file switching trick,
   because the Lscope stabs end up in the wrong place, making it impossible
   to debug the resulting code.  */
#define NO_DBX_FUNCTION_END 0

/* Just to preserve old code */
#define TARGET_SINGLE_FLOAT             0

/* This table intercepts weirdo options whose names would interfere
   with normal driver conventions, and either translates them into
   standardly-named options, or adds a 'Z' so that they can get to
   specs processing without interference.

   Do not expand a linker option to "-Xlinker -<option>", since that
   forfeits the ability to control via spec strings later.  However,
   as a special exception, do this translation with -filelist, because
   otherwise the driver will think there are no input files and quit.
   (The alternative would be to hack the driver to recognize -filelist
   specially, but it's simpler to use the translation table.)

   Note that an option name with a prefix that matches another option
   name, that also takes an argument, needs to be modified so the
   prefix is different, otherwise a '*' after the shorter option will
   match with the longer one.  */
#define TARGET_OPTION_TRANSLATE_TABLE \
  { "-xl-mode-executable", "-Zxl-mode-executable" }, \
  { "-xl-mode-xmdstub", "-Zxl-mode-xmdstub" },  \
  { "-xl-mode-bootstrap", "-Zxl-mode-bootstrap" }, \
  { "-xl-mode-novectors", "-Zxl-mode-novectors" }, \
  { "-xl-mode-xilkernel", "-Zxl-mode-xilkernel" },  \
  { "-xl-blazeit", "-Zxl-blazeit" },    \
  { "-xl-no-libxil", "-Zxl-no-libxil" }


/* Default target_flags if no switches are specified  */
#define TARGET_DEFAULT      (MASK_SOFT_MUL | MASK_SOFT_DIV | MASK_SOFT_FLOAT)

#ifndef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT 0
#endif

#ifndef TARGET_ENDIAN_DEFAULT
#define TARGET_ENDIAN_DEFAULT MASK_BIG_ENDIAN
#endif

/* What is the default setting for -mcpu= . We set it to v4.00.a even though 
   we are actually ahead. This is safest version that has generate code 
   compatible for the original ISA */
#define MICROBLAZE_DEFAULT_CPU      "v4.00.a"

#define LINKER_ENDIAN_SPEC ""

/* Macros to decide whether certain features are available or not,
   depending on the instruction set architecture level.  */

#ifdef SWITCH_TAKES_ARG
#undef SWITCH_TAKES_ARG
#endif

#define SWITCH_TAKES_ARG(CHAR)						\
  (DEFAULT_SWITCH_TAKES_ARG (CHAR) || (CHAR) == 'G')

/* On the MICROBLAZE, override_optoins is used to handle -G.  
   We also use it to set up all of the tables referenced in the 
   other macros.  */

#define OVERRIDE_OPTIONS override_options ()

#define CONDITIONAL_REGISTER_USAGE					\
do									\
  {									\
	int regno;							\
	for (regno = ST_REG_FIRST; regno <= ST_REG_LAST; regno++)	\
	  fixed_regs[regno] = call_used_regs[regno] = 1;		\
    SUBTARGET_CONDITIONAL_REGISTER_USAGE				\
  }									\
while (0)

#define SUBTARGET_CONDITIONAL_REGISTER_USAGE

#define CAN_DEBUG_WITHOUT_FP

#define DRIVER_SELF_SPECS    				\
	"%{mxl-soft-mul:%<mno-xl-soft-mul}", 		\
	"%{mno-xl-multiply-high:%<mxl-multiply-high}", 	\
	"%{mno-xl-barrel-shift:%<mxl-barrel-shift}", 	\
	"%{mno-xl-pattern-compare:%<mxl-pattern-compare}", \
	"%{mxl-soft-div:%<mno-xl-soft-div}", 		\
	"%{msoft-float:%<mhard-float}",

/* Tell collect what flags to pass to nm.  */
#ifndef NM_FLAGS
#define NM_FLAGS "-Bn"
#endif

/* Names to predefine in the preprocessor for this target machine.  */

/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()				\
  do								\
    {								\
        builtin_define ("microblaze");                          \
        builtin_define ("_BIG_ENDIAN");                         \
        builtin_define ("__MICROBLAZE__");                      \
                                                                \
        builtin_assert ("system=unix");                         \
        builtin_assert ("system=bsd");                          \
        builtin_assert ("cpu=microblaze");                      \
        builtin_assert ("machine=microblaze");                  \
} while (0)

/* Assembler specs.  */

/* MICROBLAZE_AS_ASM_SPEC is passed when using the MICROBLAZE assembler rather
   than gas.  */

#define MICROBLAZE_AS_ASM_SPEC "\
%{!.s:-nocpp} %{.s: %{cpp} %{nocpp}} \
%{pipe: %e-pipe is not supported.} \
%{K} %(subtarget_microblaze_as_asm_spec)"

/* SUBTARGET_MICROBLAZE_AS_ASM_SPEC is passed when using the MICROBLAZE 
   assembler rather than gas.  It may be overridden by subtargets.  */

#ifndef SUBTARGET_MICROBLAZE_AS_ASM_SPEC
#define SUBTARGET_MICROBLAZE_AS_ASM_SPEC "%{v}"
#endif

/* GAS_ASM_SPEC is passed when using gas, rather than the MICROBLAZE
   assembler.  */

#define GAS_ASM_SPEC "%{v}"

/* TARGET_ASM_SPEC is used to select either MICROBLAZE_AS_ASM_SPEC or
   GAS_ASM_SPEC as the default, depending upon the value of
   TARGET_DEFAULT.  */

#if ((TARGET_CPU_DEFAULT | TARGET_DEFAULT) & MASK_GAS) != 0
/* GAS */

#define TARGET_ASM_SPEC "\
%{mmicroblaze-as: %(microblaze_as_asm_spec)} \
%{!mmicroblaze-as: %(gas_asm_spec)}"

#else /* not GAS */

#define TARGET_ASM_SPEC ""
/*#define TARGET_ASM_SPEC "\
  %{!mgas: %(microblaze_as_asm_spec)} \
  %{mgas: %(gas_asm_spec)}"
*/
#endif /* not GAS */

/* SUBTARGET_ASM_OPTIMIZING_SPEC handles passing optimization options
   to the assembler.  It may be overridden by subtargets.  */
#ifndef SUBTARGET_ASM_OPTIMIZING_SPEC
#define SUBTARGET_ASM_OPTIMIZING_SPEC " "
#endif

/* SUBTARGET_ASM_DEBUGGING_SPEC handles passing debugging options to
   the assembler.  It may be overridden by subtargets.  */
#ifndef SUBTARGET_ASM_DEBUGGING_SPEC
#define SUBTARGET_ASM_DEBUGGING_SPEC "\
%{g} %{g0} %{g1} %{g2} %{g3} \
%{ggdb:-g} %{ggdb0:-g0} %{ggdb1:-g1} %{ggdb2:-g2} %{ggdb3:-g3} \
%{gstabs:-g} %{gstabs0:-g0} %{gstabs1:-g1} %{gstabs2:-g2} %{gstabs3:-g3} \
%{gstabs+:-g} %{gstabs+0:-g0} %{gstabs+1:-g1} %{gstabs+2:-g2} %{gstabs+3:-g3}"
#endif

#ifndef SUBTARGET_ASM_SPEC
#define SUBTARGET_ASM_SPEC ""
#endif

#define ASM_SPEC "\
%{microblaze1} \
%(target_asm_spec) \
%(subtarget_asm_spec)"

/* Specify to run a post-processor, microblaze-tfile after the assembler
   has run to stuff the microblaze debug information into the object file.
   This is needed because the $#!%^ MICROBLAZE assembler provides no way
   of specifying such information in the assembly file.  If we are
   cross compiling, disable microblaze-tfile unless the user specifies
   -mmicroblaze-tfile.  */

#ifndef ASM_FINAL_SPEC
#define ASM_FINAL_SPEC ""
#endif /* ASM_FINAL_SPEC */

/* Extra switches sometimes passed to the linker.  */
/* ??? The bestGnum will never be passed to the linker, because the gcc driver
   will interpret it as a -b option.  */

#define LINK_SPEC "%{shared:-shared} -N -relax %{Zxl-mode-xmdstub:-defsym \
  _TEXT_START_ADDR=0x800} %{mxl-gp-opt:%{G*}} %{!mxl-gp-opt: -G 0} \
  %{!Wl,-T*: %{!T*: -T xilinx.ld%s}}"

/* Specs for the compiler proper */

#ifndef SUBTARGET_CC1_SPEC
#define SUBTARGET_CC1_SPEC ""
#endif

#ifndef CC1_SPEC
#define CC1_SPEC " \
%{G*} %{gline:%{!g:%{!g0:%{!g1:%{!g2: -g1}}}}} \
%{save-temps: } \
%(subtarget_cc1_spec) \
%{Zxl-blazeit: -mno-xl-soft-mul -mno-xl-soft-div -mxl-barrel-shift \
-mxl-pattern-compare -mxl-multiply-high} \
"
#endif

/* Preprocessor specs.  */

#ifndef SUBTARGET_CPP_SIZE_SPEC
#define SUBTARGET_CPP_SIZE_SPEC "-D__SIZE_TYPE__=unsigned\\ \
  int -D__PTRDIFF_TYPE__=int"
#endif

#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC ""
#endif

#ifndef CPP_SPEC
#define CPP_SPEC "\
%{.S: -D__LANGUAGE_ASSEMBLY -D_LANGUAGE_ASSEMBLY \
  %{!ansi:-DLANGUAGE_ASSEMBLY}} \
%{.s: -D__LANGUAGE_ASSEMBLY -D_LANGUAGE_ASSEMBLY \
  %{!ansi:-DLANGUAGE_ASSEMBLY}} \
%{!.S: %{!.s: %{!.cc: %{!.cxx: %{!.C: %{!.m: -D__LANGUAGE_C -D_LANGUAGE_C \
  %{!ansi:-DLANGUAGE_C}}}}}}} \
%{mno-xl-soft-mul: -DHAVE_HW_MUL}       	\
%{mxl-multiply-high: -DHAVE_HW_MUL_HIGH}    	\
%{mno-xl-soft-div: -DHAVE_HW_DIV}       	\
%{mxl-barrel-shift: -DHAVE_HW_BSHIFT}   	\
%{mxl-pattern-compare: -DHAVE_HW_PCMP}  	\
%{mhard-float: -DHAVE_HW_FPU}           	\
%{mxl-float-convert: -DHAVE_HW_FPU_CONVERT} \
%{mxl-float-sqrt: -DHAVE_HW_FPU_SQRT}   \
"
#endif

#define EXTRA_SPECS							\
  { "subtarget_cc1_spec", SUBTARGET_CC1_SPEC },				\
  { "subtarget_cpp_spec", SUBTARGET_CPP_SPEC },				\
  { "subtarget_cpp_size_spec", SUBTARGET_CPP_SIZE_SPEC },		\
  { "microblaze_as_asm_spec", MICROBLAZE_AS_ASM_SPEC },				\
  { "gas_asm_spec", GAS_ASM_SPEC },					\
  { "target_asm_spec", TARGET_ASM_SPEC },				\
  { "subtarget_microblaze_as_asm_spec", SUBTARGET_MICROBLAZE_AS_ASM_SPEC }, 	\
  { "subtarget_asm_optimizing_spec", SUBTARGET_ASM_OPTIMIZING_SPEC },	\
  { "subtarget_asm_debugging_spec", SUBTARGET_ASM_DEBUGGING_SPEC },	\
  { "subtarget_asm_spec", SUBTARGET_ASM_SPEC },				\
  { "linker_endian_spec", LINKER_ENDIAN_SPEC },				\
  SUBTARGET_EXTRA_SPECS

#ifndef MD_EXEC_PREFIX
#define MD_EXEC_PREFIX "/usr/lib/cmplrs/cc/"
#endif

#ifndef MD_STARTFILE_PREFIX
#define MD_STARTFILE_PREFIX "/home/sid/comp/tests/gcctest/"
#endif

/* Print subsidiary information on the compiler version in use.  */
#define MICROBLAZE_VERSION MICROBLAZE_DEFAULT_CPU

#ifndef MACHINE_TYPE
#define MACHINE_TYPE "MicroBlaze/ELF"
#endif

#ifndef TARGET_VERSION_INTERNAL
#define TARGET_VERSION_INTERNAL(STREAM)					\
  fprintf (STREAM, " %s %s", MACHINE_TYPE, MICROBLAZE_VERSION)
#endif

#ifndef TARGET_VERSION
#define TARGET_VERSION TARGET_VERSION_INTERNAL (stderr)
#endif


/* If we are passing smuggling stabs through the MICROBLAZE ECOFF object
   format, put a comment in front of the .stab<x> operation so
   that the MICROBLAZE assembler does not choke.  The microblaze-tfile program
   will correctly put the stab into the object file.  */

#define ASM_STABS_OP	".stabs "
#define ASM_STABN_OP	".stabn "
#define ASM_STABD_OP	".stabd "

/* Local compiler-generated symbols must have a prefix that the assembler
   understands.   */

#ifndef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX	"$"
#endif

/* fixed registers */
#define MB_ABI_BASE_REGNUM                   0
#define MB_ABI_STACK_POINTER_REGNUM          1
#define MB_ABI_GPRO_REGNUM                   2
#define MB_ABI_GPRW_REGNUM                  13
#define MB_ABI_INTR_RETURN_ADDR_REGNUM      14
#define MB_ABI_SUB_RETURN_ADDR_REGNUM       15
#define MB_ABI_DEBUG_RETURN_ADDR_REGNUM     16
#define MB_ABI_EXCEPTION_RETURN_ADDR_REGNUM 17
#define MB_ABI_ASM_TEMP_REGNUM              18	
  /* This is our temp register. We use it heavily for compares and shifts */
#define MB_ABI_FRAME_POINTER_REGNUM         19
#define MB_ABI_PIC_ADDR_REGNUM              20
#define MB_ABI_PIC_FUNC_REGNUM              21
/* volatile registers */
#define MB_ABI_INT_RETURN_VAL_REGNUM         3
#define MB_ABI_INT_RETURN_VAL2_REGNUM        4
#define MB_ABI_FIRST_ARG_REGNUM              5
#define MB_ABI_LAST_ARG_REGNUM              10
#define MB_ABI_MAX_ARG_REGS                 (MB_ABI_LAST_ARG_REGNUM 		\
					     - MB_ABI_FIRST_ARG_REGNUM + 1)
#define MB_ABI_STATIC_CHAIN_REGNUM           3
#define MB_ABI_TEMP1_REGNUM                 11
#define MB_ABI_TEMP2_REGNUM                 12
#define MB_ABI_MSR_SAVE_REG                 11	
  /* Volatile register used to save MSR in interrupt handlers */


/* Debug stuff */

/* How to renumber registers for dbx and gdb. */
#define DBX_REGISTER_NUMBER(REGNO) microblaze_dbx_regno[ (REGNO) ]

/* Generate DWARF exception handling info. */
#define DWARF2_UNWIND_INFO 1

/* Don't generate .loc operations. */
#define DWARF2_ASM_LINE_DEBUG_INFO 0

/* The DWARF 2 CFA column which tracks the return address.  */
#define DWARF_FRAME_RETURN_COLUMN \
	(GP_REG_FIRST + MB_ABI_SUB_RETURN_ADDR_REGNUM)

/* Initial state of return address on entry to func = R15.
   Actually, the RA is at R15+8, but gcc doesn't know how 
   to generate this. */
#define INCOMING_RETURN_ADDR_RTX  			\
  gen_rtx_REG (VOIDmode, GP_REG_FIRST + MB_ABI_SUB_RETURN_ADDR_REGNUM)

/* Use DWARF 2 debugging information by default.  */
#define DWARF2_DEBUGGING_INFO
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG

/* Correct the offset of automatic variables and arguments.  Note that
   the MICROBLAZE debug format wants all automatic variables and arguments
   to be in terms of the virtual frame pointer (stack pointer before
   any adjustment in the function), while the MICROBLAZE linker wants
   the frame pointer to be the stack pointer after the initial
   adjustment.  */

#define DEBUGGER_AUTO_OFFSET(X)  \
  microblaze_debugger_offset (X, (HOST_WIDE_INT) 0)
#define DEBUGGER_ARG_OFFSET(OFFSET, X)  \
  microblaze_debugger_offset (X, (HOST_WIDE_INT) OFFSET)

/* Target machine storage layout */

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 1
#define WORDS_BIG_ENDIAN 1
#define LIBGCC2_WORDS_BIG_ENDIAN 1
#define BITS_PER_UNIT           8
#define BITS_PER_WORD           32
#define UNITS_PER_WORD          4
#define MIN_UNITS_PER_WORD      4
#define INT_TYPE_SIZE           32
#define SHORT_TYPE_SIZE         16
#define LONG_TYPE_SIZE          32
#define LONG_LONG_TYPE_SIZE     64
#define CHAR_TYPE_SIZE BITS_PER_UNIT
#define FLOAT_TYPE_SIZE         32
#define DOUBLE_TYPE_SIZE        64
#define LONG_DOUBLE_TYPE_SIZE   64
#ifndef POINTER_SIZE
#define POINTER_SIZE            32
#endif
#define PARM_BOUNDARY           32
#define FUNCTION_BOUNDARY       32
#define EMPTY_FIELD_BOUNDARY    32
#define STRUCTURE_SIZE_BOUNDARY 8
#define BIGGEST_ALIGNMENT       32
#define STRICT_ALIGNMENT        1
#define PCC_BITFIELD_TYPE_MATTERS 1

#define CONSTANT_ALIGNMENT(EXP, ALIGN)					\
  ((TREE_CODE (EXP) == STRING_CST  || TREE_CODE (EXP) == CONSTRUCTOR)	\
   && (ALIGN) < BITS_PER_WORD						\
	? BITS_PER_WORD							\
	: (ALIGN))

#undef DATA_ALIGNMENT
#define DATA_ALIGNMENT(TYPE, ALIGN)					\
  ((((ALIGN) < BITS_PER_WORD)						\
    && (TREE_CODE (TYPE) == ARRAY_TYPE					\
	|| TREE_CODE (TYPE) == UNION_TYPE				\
	|| TREE_CODE (TYPE) == RECORD_TYPE)) ? BITS_PER_WORD : (ALIGN))

#define LOCAL_ALIGNMENT(TYPE, ALIGN)     				\
    (((TREE_CODE (TYPE) == ARRAY_TYPE 					\
       && TYPE_MODE (TREE_TYPE (TYPE)) == QImode)			\
     && (ALIGN) < BITS_PER_WORD) ? BITS_PER_WORD : (ALIGN))

#define WORD_REGISTER_OPERATIONS

#define LOAD_EXTEND_OP(MODE)  ZERO_EXTEND

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < 4)		\
    (MODE) = SImode;

/* Standard register usage.  */

/* On the MicroBlaze, we have 32 integer registers */

#define FIRST_PSEUDO_REGISTER 46

#define FIXED_REGISTERS							\
{									\
  1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,			\
  1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,			\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 				\
}

#define CALL_USED_REGISTERS						\
{									\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			\
  1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,			\
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 				\
}

#define GP_REG_FIRST    0
#define GP_REG_LAST     31
#define GP_REG_NUM      (GP_REG_LAST - GP_REG_FIRST + 1)
#define GP_DBX_FIRST    0

#define MD_REG_FIRST    32
#define MD_REG_LAST     34
#define MD_REG_NUM      (MD_REG_LAST - MD_REG_FIRST + 1)

#define ST_REG_FIRST    35
#define ST_REG_LAST     42
#define ST_REG_NUM      (ST_REG_LAST - ST_REG_FIRST + 1)

#define AP_REG_NUM      43
#define RAP_REG_NUM     44
#define FRP_REG_NUM     45

#define GP_REG_P(REGNO) ((unsigned) ((REGNO) - GP_REG_FIRST) < GP_REG_NUM)
#define MD_REG_P(REGNO) ((unsigned) ((REGNO) - MD_REG_FIRST) < MD_REG_NUM)
#define ST_REG_P(REGNO) ((unsigned) ((REGNO) - ST_REG_FIRST) < ST_REG_NUM)

#define HARD_REGNO_NREGS(REGNO, MODE)					\
	((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Value is 1 if hard register REGNO can hold a value of machine-mode
   MODE.  In 32 bit mode, require that DImode and DFmode be in even
   registers.  For DImode, this makes some of the insns easier to
   write, since you don't have to worry about a DImode value in
   registers 3 & 4, producing a result in 4 & 5.
   FIXME: Can we avoid restricting odd-numbered register bases 
          for DImode and DFmode ?

   To make the code simpler HARD_REGNO_MODE_OK now just references an
   array built in override_options.  Because machmodes.h is not yet
   included before this file is processed, the MODE bound can't be
   expressed here.  */
extern char microblaze_hard_regno_mode_ok[][FIRST_PSEUDO_REGISTER];
#define HARD_REGNO_MODE_OK(REGNO, MODE)					\
            microblaze_hard_regno_mode_ok[ (int)(MODE) ][ (REGNO)]

#define MODES_TIEABLE_P(MODE1, MODE2)					\
  ((GET_MODE_CLASS (MODE1) == MODE_FLOAT ||				\
    GET_MODE_CLASS (MODE1) == MODE_COMPLEX_FLOAT)			\
   == (GET_MODE_CLASS (MODE2) == MODE_FLOAT ||				\
       GET_MODE_CLASS (MODE2) == MODE_COMPLEX_FLOAT))

#define STACK_POINTER_REGNUM   (GP_REG_FIRST + MB_ABI_STACK_POINTER_REGNUM)

#define STACK_POINTER_OFFSET   FIRST_PARM_OFFSET(FNDECL)

/* Base register for access to local variables of the function.  We
   pretend that the frame pointer is
   MB_ABI_INTR_RETURN_ADDR_REGNUM, and then eliminate it
   to HARD_FRAME_POINTER_REGNUM.  We can get away with this because
   rMB_ABI_INTR_RETUREN_ADDR_REGNUM is a fixed
   register(return address for interrupt), and will not be used for
   anything else.  */
   
#define FRAME_POINTER_REGNUM 		FRP_REG_NUM
#define HARD_FRAME_POINTER_REGNUM       \
        (GP_REG_FIRST + MB_ABI_FRAME_POINTER_REGNUM)
#define FRAME_POINTER_REQUIRED 		current_function_calls_alloca
#define ARG_POINTER_REGNUM		AP_REG_NUM
#define RETURN_ADDRESS_POINTER_REGNUM	RAP_REG_NUM
#define STATIC_CHAIN_REGNUM             \
        (GP_REG_FIRST + MB_ABI_STATIC_CHAIN_REGNUM)
#define STRUCT_VALUE    0

/* registers used in prologue/epilogue code when the stack frame
   is larger than 32K bytes.  These registers must come from the
   scratch register set, and not used for passing and returning
   arguments and any other information used in the calling sequence
   (such as pic).  */

#define MICROBLAZE_TEMP1_REGNUM         \
        (GP_REG_FIRST + MB_ABI_TEMP1_REGNUM)

#define MICROBLAZE_TEMP2_REGNUM         \
        (GP_REG_FIRST + MB_ABI_TEMP2_REGNUM)

#define NO_FUNCTION_CSE                 1

#define PIC_OFFSET_TABLE_REGNUM         \
        (flag_pic ? (GP_REG_FIRST + MB_ABI_PIC_ADDR_REGNUM) : \
        INVALID_REGNUM)

#define PIC_FUNCTION_ADDR_REGNUM        \
        (GP_REG_FIRST + MB_ABI_PIC_FUNC_REGNUM)

enum reg_class
{
  NO_REGS,			/* no registers in set */
  GR_REGS,			/* integer registers */
  HI_REG,			/* hi register */
  LO_REG,			/* lo register */
  HILO_REG,			/* hilo register pair for 64 bit mode mult */
  MD_REGS,			/* multiply/divide registers (hi/lo) */
  HI_AND_GR_REGS,		/* union classes */
  LO_AND_GR_REGS,
  HILO_AND_GR_REGS,
  ST_REGS,			/* status registers (fp status) */
  ALL_REGS,			/* all registers */
  LIM_REG_CLASSES		/* max value + 1 */
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

#define GENERAL_REGS GR_REGS

#define REG_CLASS_NAMES							\
{									\
  "NO_REGS",								\
  "GR_REGS",								\
  "HI_REG",								\
  "LO_REG",								\
  "HILO_REG",								\
  "MD_REGS",								\
  "HI_AND_GR_REGS",							\
  "LO_AND_GR_REGS",							\
  "HILO_AND_GR_REGS",							\
  "ST_REGS",								\
  "ALL_REGS"								\
}

#define REG_CLASS_CONTENTS						\
{									\
  { 0x00000000, 0x00000000 },		/* no registers */	\
  { 0xffffffff, 0x00000000 },		/* integer registers */	\
  { 0x00000000, 0x00000001 },		/* hi register */	\
  { 0x00000000, 0x00000002 },		/* lo register */	\
  { 0x00000000, 0x00000004 },		/* hilo register */	\
  { 0x00000000, 0x00000003 },		/* mul/div registers */	\
  { 0xffffffff, 0x00000001 },		/* union classes */     \
  { 0xffffffff, 0x00000002 },					\
  { 0xffffffff, 0x00000004 },					\
  { 0x00000000, 0x000007f8 },		/* status registers */	\
  { 0xffffffff, 0x000007ff }		/* all registers */	\
}

extern enum reg_class microblaze_regno_to_class[];

#define REGNO_REG_CLASS(REGNO) microblaze_regno_to_class[ (REGNO) ]

#define BASE_REG_CLASS  GR_REGS

#define INDEX_REG_CLASS GR_REGS

#define GR_REG_CLASS_P(CLASS) ((CLASS) == GR_REGS)

/* REG_ALLOC_ORDER is to order in which to allocate registers.  This
   is the default value (allocate the registers in numeric order).  We
   define it just so that we can override it if necessary in
   ORDER_REGS_FOR_LOCAL_ALLOC.  */
#define REG_ALLOC_ORDER							\
{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 16,	\
  17, 18, 19, 20, 21, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 15,	\
  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43			\
}

#define ORDER_REGS_FOR_LOCAL_ALLOC microblaze_order_regs_for_local_alloc ()

/* REGISTER AND CONSTANT CLASSES */

/* Get reg_class from a letter such as appears in the machine
   description.

   DEFINED REGISTER CLASSES:

   'd'  General (aka integer) registers
   Normally this is GR_REGS
   'y'  General registers 
   'h'	Hi register
   'l'	Lo register
   'x'	Multiply/divide registers
   'a'	HILO_REG
   'z'	FP Status register
   'b'	All registers */

extern enum reg_class microblaze_char_to_class[];

#define REG_CLASS_FROM_LETTER(C) microblaze_char_to_class[(C)]

/* 
`I'	is used for the range of constants an arithmetic insn can
actually contain (16 bits signed integers).

`J'	is used for the range which is just zero (ie, $r0).

'K'	is used for positive numbers.

'L'	is used for negative numbers.

`M'	is used for the range of constants that take two words to load.

`N'	is used for negative 16 bit constants other than -65536.

`O'	is a 15 bit signed integer.

`P'	is used for positive 16 bit constants.  */

#define SMALL_INT(X) ((unsigned HOST_WIDE_INT) (INTVAL (X) + 0x8000) < 0x10000)
#define SMALL_INT_UNSIGNED(X) ((unsigned HOST_WIDE_INT) (INTVAL (X)) < 0x10000)
#define PLT_ADDR_P(X) (GET_CODE (X) == UNSPEC && XINT (X,1) == UNSPEC_PLT)
/* Test for a valid operand for a call instruction.
   Don't allow the arg pointer register or virtual regs
   since they may change into reg + const, which the patterns
   can't handle yet.  */
#define CALL_INSN_OP(X) (CONSTANT_ADDRESS_P (X) \
                         || (GET_CODE (X) == REG && X != arg_pointer_rtx\
                             && ! (REGNO (X) >= FIRST_PSEUDO_REGISTER	\
                             && REGNO (X) <= LAST_VIRTUAL_REGISTER)))

/* Deifinition of K changed for MicroBlaze specific code */

#define CONST_OK_FOR_LETTER_P(VALUE, C)					\
  ((C) == 'I' ? ((unsigned HOST_WIDE_INT) ((VALUE) + 0x8000) < 0x10000)	\
   : (C) == 'J' ? ((VALUE) == 0)					\
   : (C) == 'K' ? ((VALUE) > 0)					\
   : (C) == 'L' ? ((VALUE) < 0)					\
   : (C) == 'M' ? ((((VALUE) & ~0x0000ffff) != 0)			\
		   && (((VALUE) & ~0x0000ffff) != ~0x0000ffff)		\
		   && (((VALUE) & 0x0000ffff) != 0			\
		       || (((VALUE) & ~2147483647) != 0			\
			   && ((VALUE) & ~2147483647) != ~2147483647)))	\
   : (C) == 'N' ? ((unsigned HOST_WIDE_INT) ((VALUE) + 0xffff) < 0xffff) \
   : (C) == 'O' ? ((unsigned HOST_WIDE_INT) ((VALUE) + 0x4000) < 0x8000) \
   : (C) == 'P' ? ((VALUE) != 0 && (((VALUE) & ~0x0000ffff) == 0))	\
   : 0)

/* Similar, but for floating constants, and defining letters G and H.
   Here VALUE is the CONST_DOUBLE rtx itself.  */

/* 'G'	: Floating point 0 */

#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)				\
  ((C) == 'G'								\
   && (VALUE) == CONST0_RTX (GET_MODE (VALUE)))

/* `R'	is for memory references which take 1 word for the instruction.
   `T'	is for memory addresses that can be used to load two words.  */

#define EXTRA_CONSTRAINT(OP,CODE)					\
  (((CODE) == 'T')	  ? double_memory_operand (OP, GET_MODE (OP))	\
   : ((CODE) == 'Q')	  ? FALSE                                       \
   : (GET_CODE (OP) != MEM) ? FALSE					\
   : ((CODE) == 'R')	  ? simple_memory_operand (OP, GET_MODE (OP))	\
   : ((CODE) == 's')      ? ST_REG_P(INTVAL(OP))   			\
   : FALSE)

/* Say which of the above are memory constraints.  */
#define EXTRA_MEMORY_CONSTRAINT(C, STR) ((C) == 'R' || (C) == 'T')

#define PREFERRED_RELOAD_CLASS(X,CLASS)					\
  ((CLASS) != ALL_REGS							\
   ? (CLASS)							\
   : ((GET_MODE_CLASS (GET_MODE (X)) == MODE_FLOAT			\
       || GET_MODE_CLASS (GET_MODE (X)) == MODE_COMPLEX_FLOAT)		\
      ? (GR_REGS)			\
      : ((GET_MODE_CLASS (GET_MODE (X)) == MODE_INT			\
	  || GET_MODE (X) == VOIDmode)					\
	 ? (GR_REGS) : (CLASS))))

#define SECONDARY_MEMORY_NEEDED(CLASS1, CLASS2, MODE)			\
  (!TARGET_DEBUG_H_MODE	&& GET_MODE_CLASS (MODE) == MODE_INT)

/* The HI and LO registers can only be reloaded via the general
   registers.  Condition code registers can only be loaded to the
   general registers, and from the floating point registers.  */

#define SECONDARY_INPUT_RELOAD_CLASS(CLASS, MODE, X)			\
  microblaze_secondary_reload_class (CLASS, MODE, X, 1)
#define SECONDARY_OUTPUT_RELOAD_CLASS(CLASS, MODE, X)			\
  microblaze_secondary_reload_class (CLASS, MODE, X, 0)

#define CLASS_UNITS(mode, size)						\
  ((GET_MODE_SIZE (mode) + (size) - 1) / (size))

#define CLASS_MAX_NREGS(CLASS, MODE)					\
   CLASS_UNITS (MODE, UNITS_PER_WORD)

/* Stack layout; function entry, exit and calling.  */

#define STACK_GROWS_DOWNWARD

/* Changed the starting frame offset to including the new link stuff */
#define STARTING_FRAME_OFFSET						\
  (current_function_outgoing_args_size					\
   +  (FIRST_PARM_OFFSET(FNDECL)))

/* The return address for the current frame is in r31 if this is a leaf
   function.  Otherwise, it is on the stack.  It is at a variable offset
   from sp/fp/ap, so we define a fake hard register rap which is a
   poiner to the return address on the stack.  This always gets eliminated
   during reload to be either the frame pointer or the stack pointer plus
   an offset.  */

/* ??? This definition fails for leaf functions.  There is currently no
   general solution for this problem.  */

/* ??? There appears to be no way to get the return address of any previous
   frame except by disassembling instructions in the prologue/epilogue.
   So currently we support only the current frame.  */

#define RETURN_ADDR_RTX(count, frame)			\
  microblaze_return_addr(count,frame)

/* Structure to be filled in by compute_frame_size with register
   save masks, and offsets for the current function.  */

struct microblaze_frame_info
{
  long total_size;		/* # bytes that the entire frame takes up */
  long var_size;		/* # bytes that variables take up */
  long args_size;		/* # bytes that outgoing arguments take up */
  int link_debug_size;		/* # bytes for the link reg and back pointer */
  int gp_reg_size;		/* # bytes needed to store gp regs */
  long gp_offset;		/* offset from new sp to store gp registers */
  long mask;			/* mask of saved gp registers */
  int initialized;		/* != 0 if frame size already calculated */
  int num_gp;			/* number of gp registers saved */
  long insns_len;		/* length of insns */
  int alloc_stack;		/* Flag to indicate if the current function 
				   must not create stack space. (As an optimization) */
};

extern struct microblaze_frame_info current_frame_info;

#define ELIMINABLE_REGS							\
{{ ARG_POINTER_REGNUM,   STACK_POINTER_REGNUM},				\
 { ARG_POINTER_REGNUM,   GP_REG_FIRST + MB_ABI_FRAME_POINTER_REGNUM},	\
 { RETURN_ADDRESS_POINTER_REGNUM, STACK_POINTER_REGNUM},		\
 { RETURN_ADDRESS_POINTER_REGNUM, 					\
   GP_REG_FIRST + MB_ABI_FRAME_POINTER_REGNUM},				\
 { RETURN_ADDRESS_POINTER_REGNUM, 					\
   GP_REG_FIRST + MB_ABI_SUB_RETURN_ADDR_REGNUM},			\
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},				\
 { FRAME_POINTER_REGNUM, GP_REG_FIRST + MB_ABI_FRAME_POINTER_REGNUM}}

/* We can always eliminate to the frame pointer.  We can eliminate 
   the stack pointer unless a frame pointer is needed.  */

#define CAN_ELIMINATE(FROM, TO)						\
  (((FROM) == RETURN_ADDRESS_POINTER_REGNUM && (! leaf_function_p()	\
   || (TO == GP_REG_FIRST + MB_ABI_SUB_RETURN_ADDR_REGNUM 		\
       && leaf_function_p()))) 						\
  || ((FROM) != RETURN_ADDRESS_POINTER_REGNUM				\
   && ((TO) == HARD_FRAME_POINTER_REGNUM 				\
   || ((TO) == STACK_POINTER_REGNUM && ! frame_pointer_needed))))

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			 \
        (OFFSET) = microblaze_initial_elimination_offset ((FROM), (TO))

#define ACCUMULATE_OUTGOING_ARGS        1

/* On the MICROBLAZE, we must skip the first argument position if we are
   returning a structure or a union, to account for its address being
   passed in $4.  However, at the current time, this produces a compiler
   that can't bootstrap, so comment it out for now.  */

#define FIRST_PARM_OFFSET(FNDECL)       (UNITS_PER_WORD)

#define ARG_POINTER_CFA_OFFSET(FNDECL) 0

#define REG_PARM_STACK_SPACE(FNDECL)  (MAX_ARGS_IN_REGISTERS * UNITS_PER_WORD)

#define OUTGOING_REG_PARM_STACK_SPACE       1

#define STACK_BOUNDARY                      32

#define NUM_OF_ARGS                         6

#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) 0

#define GP_RETURN (GP_REG_FIRST + MB_ABI_INT_RETURN_VAL_REGNUM)

#define GP_ARG_FIRST (GP_REG_FIRST + MB_ABI_FIRST_ARG_REGNUM)
#define GP_ARG_LAST  (GP_REG_FIRST + MB_ABI_LAST_ARG_REGNUM)

#define MAX_ARGS_IN_REGISTERS	MB_ABI_MAX_ARG_REGS

#define LIBCALL_VALUE(MODE)						\
  gen_rtx_REG (								\
	   ((GET_MODE_CLASS (MODE) != MODE_INT				\
	     || GET_MODE_SIZE (MODE) >= 4)				\
	    ? (MODE)							\
	    : SImode), GP_RETURN)

#define FUNCTION_VALUE(VALTYPE, FUNC) LIBCALL_VALUE (TYPE_MODE (VALTYPE))

/* 1 if N is a possible register number for a function value.
   On the MICROBLAZE, R2 R3 and F0 F2 are the only register thus used.
   Currently, R2 and F0 are only implemented  here (C has no complex type)  */

#define FUNCTION_VALUE_REGNO_P(N) ((N) == GP_RETURN)

#define FUNCTION_ARG_REGNO_P(N)					\
  (((N) >= GP_ARG_FIRST && (N) <= GP_ARG_LAST))

/* GCC normally converts 1 byte structures into chars, 2 byte
   structs into shorts, and 4 byte structs into ints, and returns
   them this way.  Defining the following macro overrides this,
   to give us MICROBLAZE cc compatibility.  */

#define RETURN_IN_MEMORY(TYPE)	\
  (TYPE_MODE (TYPE) == BLKmode)

#define TARGET_FLOAT_FORMAT IEEE_FLOAT_FORMAT

typedef struct microblaze_args
{
  int gp_reg_found;		/* whether a gp register was found yet */
  int arg_number;		/* argument number */
  int arg_words;		/* # total words the arguments take */
  int fp_arg_words;		/* # words for FP args (MICROBLAZE_EABI only) */
  int last_arg_fp;		/* nonzero if last arg was FP (EABI only) */
  int fp_code;			/* Mode of FP arguments */
  int num_adjusts;		/* number of adjustments made */
  /* Adjustments made to args pass in regs.  */
  /* ??? The size is doubled to work around a bug in the code that sets the 
     adjustments in function_arg.  */
  struct rtx_def *adjust[MAX_ARGS_IN_REGISTERS * 2];
} CUMULATIVE_ARGS;

#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS)	\
  init_cumulative_args (&CUM, FNTYPE, LIBNAME)

#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)			\
  function_arg_advance (&CUM, MODE, TYPE, NAMED)

#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  function_arg( &CUM, MODE, TYPE, NAMED)

#define MUST_SAVE_REGISTER(regno) microblaze_must_save_register(regno)

#define NO_PROFILE_COUNTERS         1

#define FUNCTION_PROFILER(FILE, LABELNO) { \
  {                                        \
    fprintf (FILE, "\tbrki\tr16,_mcount\n");           \
  }                                                    \
 }

#define EXIT_IGNORE_STACK 1

#define TRAMPOLINE_TEMPLATE(STREAM)					 \
{									 \
  fprintf (STREAM, "\t.word\t0x03e00821\t\t# move   $1,$31\n");		\
  fprintf (STREAM, "\t.word\t0x04110001\t\t# bgezal $0,.+8\n");		\
  fprintf (STREAM, "\t.word\t0x00000000\t\t# nop\n");			\
  fprintf (STREAM, "\t.word\t0x8fe30014\t\t# lw     $3,20($31)\n");	\
  fprintf (STREAM, "\t.word\t0x8fe20018\t\t# lw     $2,24($31)\n");	\
  fprintf (STREAM, "\t.word\t0x0060c821\t\t# move   $25,$3 (abicalls)\n"); \
  fprintf (STREAM, "\t.word\t0x00600008\t\t# jr     $3\n");		\
  fprintf (STREAM, "\t.word\t0x0020f821\t\t# move   $31,$1\n");		\
  fprintf (STREAM, "\t.word\t0x00000000\t\t# <function address>\n"); \
  fprintf (STREAM, "\t.word\t0x00000000\t\t# <static chain value>\n"); \
}

#define TRAMPOLINE_SIZE (32 + (8))

#define TRAMPOLINE_ALIGNMENT    32

#define INITIALIZE_TRAMPOLINE(ADDR, FUNC, CHAIN)			    \
{									    \
  rtx addr = ADDR;							    \
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (addr, 32)), FUNC);   \
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant (addr, 36)), CHAIN);  \
}

#define REGNO_OK_FOR_BASE_P(regno)   \
  microblaze_regno_ok_for_base_p ((regno), 1)

#define REGNO_OK_FOR_INDEX_P(regno)  \
  microblaze_regno_ok_for_base_p ((regno), 1)

#ifndef REG_OK_STRICT
#define REG_STRICT_FLAG         0
#else
#define REG_STRICT_FLAG         1
#endif

#define REG_OK_FOR_BASE_P(X)    \
  microblaze_regno_ok_for_base_p (REGNO (X), REG_STRICT_FLAG)

#define REG_OK_FOR_INDEX_P(X)   \
  microblaze_regno_ok_for_base_p (REGNO (X), REG_STRICT_FLAG)

#define MAX_REGS_PER_ADDRESS 2

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR)                     \
{                                                                   \
  if (microblaze_legitimate_address_p (MODE, X, REG_STRICT_FLAG))   \
    goto ADDR;                                                      \
}

#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL) {}

#if 1
#define GO_PRINTF(x)	trace(x, 0, 0)
#define GO_PRINTF2(x,y)	trace(x,y, 0)
#define GO_DEBUG_RTX(x) debug_rtx(x)
#else
#define GO_PRINTF(x)
#define GO_PRINTF2(x,y)
#define GO_DEBUG_RTX(x)
#endif

/* When pic, we must reject addresses of the form symbol+large int.
   This is because an instruction `sw $4,s+70000' needs to be converted
   by the assembler to `lw $at,s($gp);sw $4,70000($at)'.  Normally the
   assembler would use $at as a temp to load in the large offset.  In this
   case $at is already in use.  We convert such problem addresses to
   `la $5,s;sw $4,70000($5)' via LEGITIMIZE_ADDRESS.  */
#define CONSTANT_ADDRESS_P(X)						\
  (GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF		\
    || GET_CODE (X) == CONST_INT 		                        \
    || (GET_CODE (X) == CONST						\
	&& ! (flag_pic && pic_address_needs_scratch (X))))

/* Define this, so that when PIC, reload won't try to reload invalid
   addresses which require two reload registers.  */
#define LEGITIMATE_PIC_OPERAND_P(X)  (!pic_address_needs_scratch (X))

/* At present, GAS doesn't understand li.[sd], so don't allow it
   to be generated at present.  Also, the MICROBLAZE assembler does not
   grok li.d Infinity.  */
#define LEGITIMATE_CONSTANT_P(X)				\
  (GET_CODE (X) != CONST_DOUBLE					\
    || microblaze_const_double_ok (X, GET_MODE (X)))

#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN)			\
{  rtx result = microblaze_legitimize_address (X, OLDX, MODE);	\
   if (result != NULL_RTX) {					\
       (X) = result;						\
       goto WIN;						\
     }								\
}

/* If you are changing this macro, you should look at
   microblaze_select_section and see if it needs a similar change.  */
#ifndef UNIQUE_SECTION_P
#define UNIQUE_SECTION_P(DECL) (0)
#endif

#define TREE_STRING_RTL(NODE) (STRING_CST_CHECK (NODE)->string.rtl)

#define CONSTANT_POOL_BEFORE_FUNCTION TRUE

#define CASE_VECTOR_MODE    (SImode)

#ifndef DEFAULT_SIGNED_CHAR
#define DEFAULT_SIGNED_CHAR 1
#endif

#define MOVE_MAX        4
#define MAX_MOVE_MAX    8

#define SLOW_BYTE_ACCESS 1

#define STORE_FLAG_VALUE 1

#define SHIFT_COUNT_TRUNCATED 1

/* ??? This results in inefficient code for 64 bit to 32 conversions.
   Something needs to be done about this.  Perhaps not use any 32 bit
   instructions?  Perhaps use PROMOTE_MODE?  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC)  1

#ifndef Pmode
#define Pmode SImode
#endif

#define FUNCTION_MODE   SImode


/* Mode should alwasy be SImode */
#define REGISTER_MOVE_COST(MODE, FROM, TO)	\
  ( GR_REG_CLASS_P (FROM) && GR_REG_CLASS_P (TO) ? 2 \
   : (((FROM) == HI_REG || (FROM) == LO_REG				\
       || (FROM) == MD_REGS || (FROM) == HILO_REG)			\
      && GR_REG_CLASS_P (TO)) ? 6		\
   : (((TO) == HI_REG || (TO) == LO_REG					\
       || (TO) == MD_REGS || (TO) == HILO_REG)				\
      && GR_REG_CLASS_P (FROM)) ? 6		\
   : (FROM) == ST_REGS && GR_REG_CLASS_P (TO) ? 4			\
   : 12)

#define MEMORY_MOVE_COST(MODE,CLASS,TO_P) \
  (4 + memory_move_secondary_cost ((MODE), (CLASS), (TO_P)))

/* This is needed for the MICROBLAZE because reload_outcc is not complete;
it needs to handle cases where the source is a general or another
condition code register.  */
#define AVOID_CCMODE_COPIES

#define BRANCH_COST   2

#define ADJUST_COST(INSN,LINK,DEP_INSN,COST)				\
  if (REG_NOTE_KIND (LINK) != 0)					\
    (COST) = 0;			/* Anti or output dependence.  */


/* Control the assembler format that we output.  */

#define ASM_APP_ON " #APP\n"

#define ASM_APP_OFF " #NO_APP\n"

#define REGISTER_NAMES {						\
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",		\
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",	\
  "r16",  "r17",  "r18",  "r19",  "r20",  "r21",  "r22",  "r23",	\
  "r24",  "r25",  "r26",  "r27",  "r28",  "r29",  "r30",  "r31",	\
  "hi",   "lo",   "accum","rmsr", "$fcc1","$fcc2","$fcc3","$fcc4",	\
  "$fcc5","$fcc6","$fcc7","$ap",  "$rap", "$frp" }

/* print-rtl.c can't use REGISTER_NAMES, since it depends on microblaze.c.
   So define this for it.  */
#define DEBUG_REGISTER_NAMES						\
{									\
  "$0",   "sp", "rogp",   "v0",   "v1",   "a0",   "a1",   "a2",		\
  "a3",   "a4",   "a5",   "t0",   "t1",   "rwgp", "k0",   "k1",		\
  "k2",   "k3",   "at",   "s0",   "s1",   "s2",   "s3",   "s4",		\
  "s5",   "s6",   "s7",   "s8",   "s9",   "s10",  "s11",  "s12",	\
  "hi",   "lo",   "accum","rmsr","$fcc1","$fcc2","$fcc3","$fcc4",	\
  "$fcc5","$fcc6","$fcc7","$rap"					\
}

#define ADDITIONAL_REGISTER_NAMES					\
{									\
  { "r0",	 0 + GP_REG_FIRST },					\
  { "r1",	 1 + GP_REG_FIRST },					\
  { "r2",	 2 + GP_REG_FIRST },					\
  { "r3",	 3 + GP_REG_FIRST },					\
  { "r4",	 4 + GP_REG_FIRST },					\
  { "r5",	 5 + GP_REG_FIRST },					\
  { "r6",	 6 + GP_REG_FIRST },					\
  { "r7",	 7 + GP_REG_FIRST },					\
  { "r8",	 8 + GP_REG_FIRST },					\
  { "r9",	 9 + GP_REG_FIRST },					\
  { "r10",	10 + GP_REG_FIRST },					\
  { "r11",	11 + GP_REG_FIRST },					\
  { "r12",	12 + GP_REG_FIRST },					\
  { "r13",	13 + GP_REG_FIRST },					\
  { "r14",	14 + GP_REG_FIRST },					\
  { "r15",	15 + GP_REG_FIRST },					\
  { "r16",	16 + GP_REG_FIRST },					\
  { "r17",	17 + GP_REG_FIRST },					\
  { "r18",	18 + GP_REG_FIRST },					\
  { "r19",	19 + GP_REG_FIRST },					\
  { "r20",	20 + GP_REG_FIRST },					\
  { "r21",	21 + GP_REG_FIRST },					\
  { "r22",	22 + GP_REG_FIRST },					\
  { "r23",	23 + GP_REG_FIRST },					\
  { "r24",	24 + GP_REG_FIRST },					\
  { "r25",	25 + GP_REG_FIRST },					\
  { "r26",	26 + GP_REG_FIRST },					\
  { "r27",	27 + GP_REG_FIRST },					\
  { "r28",	28 + GP_REG_FIRST },					\
  { "r29",	29 + GP_REG_FIRST },					\
  { "r30",	30 + GP_REG_FIRST },					\
  { "r31",	31 + GP_REG_FIRST },					\
  { "rmsr",     ST_REG_FIRST} \
}

#define PRINT_OPERAND(FILE, X, CODE) print_operand (FILE, X, CODE)

#define PRINT_OPERAND_PUNCT_VALID_P(CODE) microblaze_print_operand_punct[CODE]

#define PRINT_OPERAND_ADDRESS(FILE, ADDR) print_operand_address (FILE, ADDR)

#define DBR_OUTPUT_SEQEND(STREAM)					\
do									\
  {									\
    dslots_jump_filled++;						\
    fputs ("\n", STREAM);						\
  }									\
while (0)

#ifndef SET_FILE_NUMBER
#define SET_FILE_NUMBER() ++num_source_filenames
#endif

#define ASM_OUTPUT_SOURCE_FILENAME(STREAM, NAME)			\
  microblaze_output_filename (STREAM, NAME)

#ifndef LABEL_AFTER_LOC
#define LABEL_AFTER_LOC(STREAM)
#endif

/* The MICROBLAZE implementation uses some labels for its own purpose.  The
   following lists what labels are created, and are all formed by the
   pattern $L[a-z].*.  The machine independent portion of GCC creates
   labels matching:  $L[A-Z][0-9]+ and $L[0-9]+.

   LM[0-9]+	Silicon Graphics/ECOFF stabs label before each stmt.
   $Lb[0-9]+	Begin blocks for MICROBLAZE debug support
   $Lc[0-9]+	Label for use in s<xx> operation.
   $Le[0-9]+	End blocks for MICROBLAZE debug support
   $Lp\..+		Half-pic labels. */

/* If we are optimizing the gp, remember that this label has been put
   out, so we know not to emit an .extern for it in microblaze_asm_file_end.
   We use one of the common bits in the IDENTIFIER tree node for this,
   since those bits seem to be unused, and we don't have any method
   of getting the decl nodes from the name.  */
#define ASM_OUTPUT_LABEL(STREAM,NAME)					\
do {									\
  assemble_name (STREAM, NAME);						\
  fputs (":\n", STREAM);						\
} while (0)

/* All of these below can be replaced or overridden from elfos.h.  */
#define COMMON_ASM_OP	"\t.comm\t"
#define LCOMMON_ASM_OP	"\t.lcomm\t"

/* ASM_OUTPUT_ALIGNED_COMMON and ASM_OUTPUT_ALIGNED_LOCAL

   Unfortunately, we still need to set the section explicitly. Somehow,
   our binutils assign .comm and .lcomm variables to the "current" section 
   in the assembly file, rather than where they implicitly belong. We need to
   remove this explicit setting in GCC when binutils can understand sections
   better. */
#undef	ASM_OUTPUT_ALIGNED_COMMON
#define	ASM_OUTPUT_ALIGNED_COMMON(FILE, NAME, SIZE, ALIGN)		\
do {									\
  if (SIZE > 0 && SIZE <= microblaze_section_threshold                  \
      && TARGET_XLGP_OPT)                                               \
    {                                                                   \
      sbss_section ();							\
    }									\
  else									\
    {									\
      bss_section();                                                    \
    }                                                                   \
  fprintf (FILE, "%s", COMMON_ASM_OP);                                  \
  assemble_name ((FILE), (NAME));					\
  fprintf ((FILE), ","HOST_WIDE_INT_PRINT_UNSIGNED",%u\n",		\
           (SIZE), (ALIGN) / BITS_PER_UNIT);                            \
  ASM_OUTPUT_TYPE_DIRECTIVE (FILE, NAME, "object");			\
} while (0)

#undef ASM_OUTPUT_ALIGNED_LOCAL
#define	ASM_OUTPUT_ALIGNED_LOCAL(FILE, NAME, SIZE, ALIGN)		\
do {									\
  if (SIZE > 0 && SIZE <= microblaze_section_threshold                  \
      && TARGET_XLGP_OPT)                                               \
    {                                                                   \
      sbss_section ();							\
    }									\
  else									\
    {									\
      bss_section();                                                    \
    }                                                                   \
  fprintf (FILE, "%s", LCOMMON_ASM_OP);                                 \
  assemble_name ((FILE), (NAME));					\
  fprintf ((FILE), ","HOST_WIDE_INT_PRINT_UNSIGNED",%u\n",		\
           (SIZE), (ALIGN) / BITS_PER_UNIT);                            \
  ASM_OUTPUT_TYPE_DIRECTIVE (FILE, NAME, "object");			\
} while (0)

#define	ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN)		\
do {									\
  ASM_OUTPUT_ALIGNED_LOCAL (FILE, NAME, SIZE, ALIGN);			\
} while (0)

#define ASM_DECLARE_FUNCTION_NAME(STREAM,NAME,DECL)                     \
{                                                                       \
}

#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)			\
  sprintf ((LABEL), "*%s%s%ld", (LOCAL_LABEL_PREFIX), (PREFIX), (long)(NUM))

#define ASM_OUTPUT_DOUBLE(STREAM,VALUE)					\
  microblaze_output_double (STREAM, VALUE)

#define ASM_OUTPUT_FLOAT(STREAM,VALUE)					\
  microblaze_output_float (STREAM, VALUE)

#define ASM_OUTPUT_INT(STREAM,VALUE)					\
do {									\
  fprintf (STREAM, "\t.data32\t");					\
  output_addr_const (STREAM, (VALUE));					\
  fprintf (STREAM, "\n");						\
} while (0)

#define ASM_OUTPUT_DOUBLE_INT(STREAM,VALUE)				\
do {									\
      assemble_integer (operand_subword ((VALUE), 0, 0, DImode),	\
			UNITS_PER_WORD, 1);				\
      assemble_integer (operand_subword ((VALUE), 1, 0, DImode),	\
			UNITS_PER_WORD, 1);				\
} while (0)

#define ASM_OUTPUT_SHORT(STREAM,VALUE)					\
{									\
  fprintf (STREAM, "\t.data16\t");					\
  output_addr_const (STREAM, (VALUE));					\
  fprintf (STREAM, "\n");						\
}

#define ASM_OUTPUT_CHAR(STREAM,VALUE)					\
{									\
  fprintf (STREAM, "\t.data8\t");					\
  output_addr_const (STREAM, (VALUE));					\
  fprintf (STREAM, "\n");						\
}

#define ASM_OUTPUT_BYTE(STREAM,VALUE)					\
  fprintf (STREAM, "\t.data8\t0x%x\n", (VALUE))

#define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE)				\
  fprintf (STREAM, "\t%s\t%sL%d\n",					\
	   ".gpword",                                                   \
	   LOCAL_LABEL_PREFIX, VALUE)

#define ASM_OUTPUT_ADDR_DIFF_ELT(STREAM, BODY, VALUE, REL)		\
do {									\
  if (flag_pic == 2)                                               \
    fprintf (STREAM, "\t%s\t%sL%d@GOTOFF\n",                            \
	     ".gpword",                                                 \
	     LOCAL_LABEL_PREFIX, VALUE);				\
  else                                                                  \
    fprintf (STREAM, "\t%s\t%sL%d\n",					\
	     ".gpword",                                                 \
	     LOCAL_LABEL_PREFIX, VALUE);				\
} while (0)

/* We want to put the jump table in the .rdata section.  Unfortunately, 
   we can't use JUMP_TABLES_IN_TEXT_SECTION, because it is not conditional.
   Instead, we use ASM_OUTPUT_CASE_LABEL to switch back to the .text
   section if appropriate.  */
#define ASM_OUTPUT_CASE_LABEL(FILE, PREFIX, NUM, INSN)			\
do {									\
  (*targetm.asm_out.internal_label) (FILE, PREFIX, NUM);		\
} while (0)

#define ASM_OUTPUT_ALIGN(STREAM,LOG)					\
  fprintf (STREAM, "\t.align\t%d\n", (LOG))

#define ASM_OUTPUT_SKIP(STREAM,SIZE)					\
  fprintf (STREAM, "\t.space\t%lu\n", (SIZE))

#if TARGET_MICROBLAZE_ASM
#define ASM_OUTPUT_ASCII(STREAM, STRING, LEN)				\
do {									\
  register int i, c, len = (LEN) ;  				        \
  register unsigned char *string = (unsigned char *)(STRING);		\
  for (i = 0; i < len; i++)						\
       fprintf((STREAM),".data8 %d\n",string[i]);			\
  }                                                                     \
} while (0)
#else
#define ASM_OUTPUT_ASCII(STREAM, STRING, LEN) output_ascii(STREAM,STRING,LEN)
#endif

#define ASM_OUTPUT_IDENT(FILE, STRING)					\
{									\
  const char *p = STRING;							\
  int size = strlen (p) + 1;						\
  if(size <= microblaze_section_threshold)				\
     sdata2_section ();							\
  else 									\
     rodata_section ();							\
  assemble_string (p, size);						\
}

/* Default to -G 8 */
#ifndef MICROBLAZE_DEFAULT_GVALUE
#define MICROBLAZE_DEFAULT_GVALUE 8
#endif

/* Given a decl node or constant node, choose the section to output it in
   and select that section.  */

/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */
#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)			\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10),			\
  sprintf ((OUTPUT), "%s.%d", (NAME), (LABELNO)))

/* How to start an assembler comment.
   The leading space is important (the microblaze assembler requires it).  */
#ifndef ASM_COMMENT_START
#define ASM_COMMENT_START " #"
#endif

/* Macros for microblaze-tfile.c to encapsulate stabs in ECOFF, and for
   and microblaze-tdump.c to print them out.

   These must match the corresponding definitions in gdb/microblazeread.c.
   Unfortunately, gcc and gdb do not currently share any directories. */
#define CODE_MASK 0x8F300
#define MICROBLAZE_IS_STAB(sym) (((sym)->index & 0xFFF00) == CODE_MASK)
#define MICROBLAZE_MARK_STAB(code) ((code)+CODE_MASK)
#define MICROBLAZE_UNMARK_STAB(code) ((code)-CODE_MASK)

#undef TARGET_FP_CALL_32

#define BSS_VAR         1
#define SBSS_VAR        2
#define DATA_VAR        4
#define SDATA_VAR       5
#define RODATA_VAR      6
#define SDATA2_VAR      7

/* These definitions are used in with the shift_type flag in the rtl */
#define SHIFT_CONST     1
#define SHIFT_REG       2
#define USE_ADDK        3

/* handle interrupt attribute */
extern int interrupt_handler;
extern int save_volatiles;

#define INTERRUPT_HANDLER_NAME "_interrupt_handler"

/* these #define added for C++ */
#ifdef OBJECT_FORMAT_ELF
#define UNALIGNED_SHORT_ASM_OP          ".data16"
#define UNALIGNED_INT_ASM_OP            ".data32"
#define UNALIGNED_DOUBLE_INT_ASM_OP     ".data8"
#endif /* OBJECT_FORMAT_ELF */

#define ASM_BYTE_OP                     ".data8"

/* the following #defines are used in the headers files. Always retain these */

/* Added for declaring size at the end of the function*/
#undef ASM_DECLARE_FUNCTION_SIZE
#define ASM_DECLARE_FUNCTION_SIZE(FILE, FNAME, DECL)			\
  do {									\
    if (!flag_inhibit_size_directive)					\
      {									\
        char label[256];						\
	static int labelno;						\
	labelno++;							\
	ASM_GENERATE_INTERNAL_LABEL (label, "Lfe", labelno);		\
        (*targetm.asm_out.internal_label) (FILE, "Lfe", labelno);	\
	fprintf (FILE, "%s", SIZE_ASM_OP);				\
	assemble_name (FILE, (FNAME));					\
        fprintf (FILE, ",");						\
	assemble_name (FILE, label);					\
        fprintf (FILE, "-");						\
	assemble_name (FILE, (FNAME));					\
	putc ('\n', FILE);						\
      }									\
  } while (0)

#define GLOBAL_ASM_OP "\t.globl\t"

#define MAX_OFILE_ALIGNMENT (32768*8)

#define TYPE_OPERAND_FMT        "@%s"

#undef TYPE_ASM_OP
#undef SIZE_ASM_OP
#define TYPE_ASM_OP	"\t.type\t"
#define SIZE_ASM_OP	"\t.size\t"

/* Write the extra assembler code needed to declare an object properly.  */
#undef ASM_DECLARE_OBJECT_NAME
#define ASM_DECLARE_OBJECT_NAME(FILE, NAME, DECL)			\
  do {									\
    fprintf (FILE, "%s", TYPE_ASM_OP);			         	\
    assemble_name (FILE, NAME);						\
    putc (',', FILE);							\
    fprintf (FILE, TYPE_OPERAND_FMT, "object");				\
    putc ('\n', FILE);							\
    size_directive_output = 0;						\
    if (!flag_inhibit_size_directive && DECL_SIZE (DECL))		\
      {									\
	size_directive_output = 1;					\
	fprintf (FILE, "%s", SIZE_ASM_OP);				\
	assemble_name (FILE, NAME);					\
	fprintf (FILE, ",%d\n",  int_size_in_bytes (TREE_TYPE (DECL)));	\
      }									\
    microblaze_declare_object (FILE, NAME, "", ":\n", 0);			\
  } while (0)

#undef ASM_FINISH_DECLARE_OBJECT
#define ASM_FINISH_DECLARE_OBJECT(FILE, DECL, TOP_LEVEL, AT_END)	 \
do {									 \
     char *name = XSTR (XEXP (DECL_RTL (DECL), 0), 0);			 \
     if (!flag_inhibit_size_directive && DECL_SIZE (DECL)		 \
         && ! AT_END && TOP_LEVEL					 \
	 && DECL_INITIAL (DECL) == error_mark_node			 \
	 && !size_directive_output)					 \
       {								 \
	 size_directive_output = 1;					 \
	 fprintf (FILE, "%s", SIZE_ASM_OP);			         \
	 assemble_name (FILE, name);					 \
	 fprintf (FILE, ",%d\n", int_size_in_bytes (TREE_TYPE (DECL)));  \
       }								 \
   } while (0)

#define ASM_OUTPUT_DEF(FILE,LABEL1,LABEL2)                            \
 do { fputc ( '\t', FILE);                                            \
      assemble_name (FILE, LABEL1);                                   \
      fputs ( " = ", FILE);                                           \
      assemble_name (FILE, LABEL2);                                   \
      fputc ( '\n', FILE);                                            \
 } while (0)


/* No support for weak in MicroBlaze. Hence commenting out this part */
#define ASM_WEAKEN_LABEL(FILE,NAME) ASM_OUTPUT_WEAK_ALIAS(FILE,NAME,0)
#define ASM_OUTPUT_WEAK_ALIAS(FILE,NAME,VALUE)	\
  do {						\
  fputs ("\t.weakext\t", FILE);		\
  assemble_name (FILE, NAME);			\
  if (VALUE)					\
  {						\
  fputc (' ', FILE);			\
  assemble_name (FILE, VALUE);		\
  }						\
  fputc ('\n', FILE);				\
  } while (0)

#define MAKE_DECL_ONE_ONLY(DECL) (DECL_WEAK (DECL) = 1)
#undef UNIQUE_SECTION_P
#define UNIQUE_SECTION_P(DECL) (DECL_ONE_ONLY (DECL))

#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION        default_elf_asm_named_section

#undef TARGET_UNIQUE_SECTION
#define TARGET_UNIQUE_SECTION microblaze_unique_section

/* Define the strings to put out for each section in the object file.  
   
   Note: For ctors/dtors, we want to give these sections the SHF_WRITE 
   attribute to allow shared libraries to patch/resolve addresses into 
   these locations.  On Microblaze, there is no concept of shared libraries 
   yet, so this is for future use. */
#define TEXT_SECTION_ASM_OP	"\t.text"
#define DATA_SECTION_ASM_OP	"\t.data"
#define READONLY_DATA_SECTION_ASM_OP    \
                                "\t.rodata"
#define BSS_SECTION_ASM_OP      "\t.bss"
#define CTORS_SECTION_ASM_OP    "\t.section\t.ctors,\"aw\""
#define DTORS_SECTION_ASM_OP    "\t.section\t.dtors,\"aw\""
#define INIT_SECTION_ASM_OP     "\t.section\t.init,\"ax\""
#define FINI_SECTION_ASM_OP     "\t.section\t.fini,\"ax\""

#define SDATA_SECTION_ASM_OP	"\t.sdata"	/* Small RW initialized data   */
#define SDATA2_SECTION_ASM_OP	"\t.sdata2"	/* Small RO initialized data   */
#define SBSS_SECTION_ASM_OP     "\t.sbss"	/* Small RW uninitialized data */
#define SBSS2_SECTION_ASM_OP    "\t.sbss2"	/* Small RO uninitialized data */

#define HOT_TEXT_SECTION_NAME   ".text.hot"
#define UNLIKELY_EXECUTED_TEXT_SECTION_NAME \
                                ".text.unlikely"

#define READONLY_DATA_SECTION   rodata_section
#define SDATA_SECTION           sdata_section
#define READONLY_SDATA_SECTION  sdata2_section
#define SBSS_SECTION            sbss_section
#define READONLY_SBSS_SECTION   sbss2_section
#define BSS_SECTION             bss_section

/* A list of sections which the compiler might be "in" at any given time.  */
#undef EXTRA_SECTIONS
#define EXTRA_SECTIONS in_rodata, in_sdata, in_sdata2, in_sbss, in_sbss2, \
   in_init, in_fini

#undef EXTRA_SECTION_FUNCTIONS
#define EXTRA_SECTION_FUNCTIONS 					\
  SECTION_FUNCTION_TEMPLATE(rodata_section, in_rodata, 			\
			    READONLY_DATA_SECTION_ASM_OP)		\
  SECTION_FUNCTION_TEMPLATE(sdata_section,  in_sdata,  			\
			    SDATA_SECTION_ASM_OP)        		\
  SECTION_FUNCTION_TEMPLATE(sdata2_section, in_sdata2, 			\
			    SDATA2_SECTION_ASM_OP)       		\
  SECTION_FUNCTION_TEMPLATE(sbss_section,   in_sbss,   			\
			    SBSS_SECTION_ASM_OP)         		\
  SECTION_FUNCTION_TEMPLATE(sbss2_section,  in_sbss2,  			\
			    SBSS2_SECTION_ASM_OP)        		\
  SECTION_FUNCTION_TEMPLATE(init_section,  in_init,  			\
			    INIT_SECTION_ASM_OP)           		\
  SECTION_FUNCTION_TEMPLATE(fini_section,  in_fini,  			\
			    FINI_SECTION_ASM_OP)
		\
			    
#define SECTION_FUNCTION_TEMPLATE(FN, ENUM, OP)                		\
void FN (void)								\
{									\
  if (in_section != ENUM)						\
    {									\
      fprintf (asm_out_file, "%s\n", OP);				\
      in_section = ENUM;						\
    }									\
}

/* We do this to save a few 10s of code space that would be taken up
   by the call_FUNC () wrappers, used by the generic CRT_CALL_STATIC_FUNCTION
   definition in crtstuff.c */
#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC)	\
    asm ( SECTION_OP "\n"                               \
          "\tbrlid   r15, " #FUNC "\n\t nop\n"         \
          TEXT_SECTION_ASM_OP);

/* Don't set the target flags, this is done by the linker script */
#undef LIB_SPEC
#define LIB_SPEC "%{!pg:%{!nostdlib:%{!Zxl-no-libxil:-start-group -lgloss -lxil \
-lc -lm -end-group }}} %{pg:%{!nostdlib:-start-group -lxilprofile -lgloss -lxil \ 
-lc -lm -end-group }} 		\
%{Zxl-no-libxil: %{!nostdlib: -start-group -lc -lm -end-group }}"
/* Xilinx: We need to group -lm as well, since some Newlib math functions 
   reference __errno! */

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"


#define STARTFILE_EXECUTABLE_SPEC   "crt0.o%s crti.o%s crtbegin.o%s"
#define STARTFILE_XMDSTUB_SPEC      "crt1.o%s crti.o%s crtbegin.o%s"
#define STARTFILE_BOOTSTRAP_SPEC    "crt2.o%s crti.o%s crtbegin.o%s"
#define STARTFILE_NOVECTORS_SPEC    "crt3.o%s crti.o%s crtbegin.o%s"
#define STARTFILE_XILKERNEL_SPEC    "crt4.o%s crti.o%s crtbegin.o%s"
#define STARTFILE_CRTINIT_SPEC      "%{!pg: %{!mno-clearbss: crtinit.o%s} \
%{mno-clearbss: sim-crtinit.o%s}} \
%{pg: %{!mno-clearbss: pgcrtinit.o%s} %{mno-clearbss: sim-pgcrtinit.o%s}}"

#define STARTFILE_DEFAULT_SPEC      STARTFILE_EXECUTABLE_SPEC

#undef SUBTARGET_EXTRA_SPECS
#define	SUBTARGET_EXTRA_SPECS						\
  { "startfile_executable",	STARTFILE_EXECUTABLE_SPEC },		\
  { "startfile_xmdstub",	STARTFILE_XMDSTUB_SPEC },		\
  { "startfile_bootstrap",	STARTFILE_BOOTSTRAP_SPEC },		\
  { "startfile_novectors",	STARTFILE_NOVECTORS_SPEC },		\
  { "startfile_xilkernel",	STARTFILE_XILKERNEL_SPEC },		\
  { "startfile_crtinit",        STARTFILE_CRTINIT_SPEC },               \
  { "startfile_default",	STARTFILE_DEFAULT_SPEC },

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC  "\
%{Zxl-mode-executable   : %(startfile_executable)  ; \
  Zxl-mode-xmdstub      : %(startfile_xmdstub)     ; \
  Zxl-mode-bootstrap    : %(startfile_bootstrap)   ; \
  Zxl-mode-novectors    : %(startfile_novectors)   ; \
  Zxl-mode-xilkernel    : %(startfile_xilkernel)   ; \
                        : %(startfile_default)       \
} \
%(startfile_crtinit)"
