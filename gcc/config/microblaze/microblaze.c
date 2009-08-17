/* Subroutines used for code generation on Xilinx MicroBlaze.
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include <signal.h>
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "insn-attr.h"
#include "integrate.h"
#include "recog.h"
#include "toplev.h"
#include "tree.h"
#include "function.h"
#include "expr.h"
#include "flags.h"
#include "reload.h"
#include "output.h"
#include "ggc.h"
#include "hashtab.h"
#include "target.h"
#include "target-def.h"
#include "tm_p.h"
#include "gstab.h"

/* Classifies an address.

ADDRESS_INVALID
An invalid address.

ADDRESS_REG

A natural register or a register + const_int offset address.  
The register satisfies microblaze_valid_base_register_p and the 
offset is a const_arith_operand.

ADDRESS_REG_INDEX

A natural register offset by the index contained in an index register. The base
register satisfies microblaze_valid_base_register_p and the index register
satisfies microblaze_valid_index_register_p

ADDRESS_CONST_INT

A signed 16/32-bit constant address.

ADDRESS_SYMBOLIC:

A constant symbolic address or a (register + symbol).  */

enum microblaze_address_type
{
  ADDRESS_INVALID,
  ADDRESS_REG,
  ADDRESS_REG_INDEX,
  ADDRESS_CONST_INT,
  ADDRESS_SYMBOLIC,
  ADDRESS_GOTOFF,
  ADDRESS_PLT
};

/* Classifies symbols

SYMBOL_TYPE_GENERAL
        
A general symbol. */
enum microblaze_symbol_type
{
  SYMBOL_TYPE_INVALID,
  SYMBOL_TYPE_GENERAL
};

/* Classification of a Microblaze address */
struct microblaze_address_info
{
  enum microblaze_address_type type;
  rtx regA; 	/* Contains valid values on ADDRESS_REG, ADDRESS_REG_INDEX, 
     		   ADDRESS_SYMBOLIC */
  rtx regB; 	/* Contains valid values on ADDRESS_REG_INDEX */
  rtx offset; 	/* Contains valid values on ADDRESS_CONST_INT and ADDRESS_REG */
  rtx symbol; 	/* Contains valid values on ADDRESS_SYMBOLIC */
  enum microblaze_symbol_type symbol_type;
};

static void microblaze_encode_section_info	(tree, rtx, int);
static void microblaze_globalize_label          (FILE*, const char*);
static void microblaze_function_prologue        (FILE*, int);
static void microblaze_function_epilogue        (FILE*, HOST_WIDE_INT);
static void microblaze_internal_label           (FILE *, const char*, 
					         unsigned long);
static bool microblaze_rtx_costs                (rtx, int, int, int*);
static int microblaze_address_cost              (rtx);
static int microblaze_address_insns             (rtx, enum machine_mode);
static void microblaze_asm_constructor          (rtx, int);
static void microblaze_asm_destructor           (rtx, int);
static void microblaze_select_section           (tree, int, 
						 unsigned HOST_WIDE_INT);
static void microblaze_select_rtx_section       (enum machine_mode, rtx, 
						 unsigned HOST_WIDE_INT);
static const char *microblaze_mode_to_mem_modifier (int, enum machine_mode);
static bool microblaze_valid_base_register_p    (rtx, enum machine_mode, int);
static bool microblaze_valid_index_register_p   (rtx, enum machine_mode, int);
static bool microblaze_classify_address         (struct microblaze_address_info *,
						 rtx, enum machine_mode, int);
HOST_WIDE_INT compute_frame_size                (HOST_WIDE_INT);
int microblaze_sched_use_dfa_pipeline_interface (void);
void microblaze_function_end_prologue           (FILE *);
static void microblaze_block_move_straight      (rtx, rtx, HOST_WIDE_INT);
static void microblaze_adjust_block_mem         (rtx, HOST_WIDE_INT, rtx *, 
						 rtx *);
static void microblaze_block_move_loop          (rtx, rtx, HOST_WIDE_INT);
static void save_restore_insns			(int);
static int microblaze_version_to_int            (const char *);
static int microblaze_version_compare           (const char *, const char *);
void final_prescan_insn 			(rtx, rtx *, int);
void microblaze_internal_label 			(FILE *, const char*, unsigned long);
void microblaze_output_float 			(FILE *, REAL_VALUE_TYPE value);
void trace 					(const char *, const char *, const char *);
int function_arg_partial_bytes 			(CUMULATIVE_ARGS *, 
						 enum machine_mode, tree, int);
void microblaze_output_lineno 			(FILE *, int);
void microblaze_internal_label 			(FILE *, const char*, unsigned long);
void microblaze_output_double 			(FILE *, REAL_VALUE_TYPE);
static int microblaze_save_volatiles 		(tree);
int microblaze_valid_machine_decl_attribute 	(tree, tree, tree, tree);
static bool microblaze_handle_option 		(size_t, const char *, int);
static int microblaze_must_save_register 	(int);
static bool microblaze_classify_unspec 		(struct microblaze_address_info *,
						 rtx);
static bool microblaze_elf_in_small_data_p      (tree);

/* Global variables for machine-dependent things.  */

static unsigned int microblaze_select_flags = 0;

/* Toggle which pipleline interface to use */
int microblaze_sched_use_dfa = 0;

/* Threshold for data being put into the small data/bss area, instead
   of the normal data area (references to the small data/bss area take
   1 instruction, and use the global pointer, references to the normal
   data area takes 2 instructions).  */
int microblaze_section_threshold = -1;

/* Prevent scheduling potentially exception causing instructions in 
   delay slots.  -mcpu=v3.00.a or v4.00.a turns this on.  */
int microblaze_no_unsafe_delay;

/* Count the number of .file directives, so that .loc is up to date.  */
int num_source_filenames = 0;

/* Count the number of sdb related labels are generated (to find block
   start and end boundaries).  */
int sdb_label_count = 0;

/* Next label # for each statement for Silicon Graphics IRIS systems. */
int sym_lineno = 0;

/* Non-zero if inside of a function, because the stupid asm can't
   handle .files inside of functions.  */
int inside_function = 0;

/* Name of the file containing the current function.  */
const char *current_function_file = "";

int file_in_function_warning = FALSE;

/* Whether to suppress issuing .loc's because the user attempted
   to change the filename within a function.  */
int ignore_line_number = FALSE;

/* Count of delay slots and how many are filled.  */
int dslots_load_total;
int dslots_load_filled;
int dslots_jump_total;
int dslots_jump_filled;

/* # of nops needed by previous insn */
int dslots_number_nops;

/* Number of 1/2/3 word references to data items (ie, not brlid's).  */
int num_refs[3];

/* registers to check for load delay */
rtx microblaze_load_reg, microblaze_load_reg2, microblaze_load_reg3,
  microblaze_load_reg4;

/* Cached operands, and operator to compare for use in set/branch on
   condition codes.  */
rtx branch_cmp[2];

/* what type of branch to use */
enum cmp_type branch_type;

/* Which CPU pipeline do we use. We haven't really standardized on a CPU 
   version having only a particular type of pipeline. There can still be 
   options on the CPU to scale pipeline features up or down. :( 
   Bad Presentation (??), so we let the MD file rely on the value of 
   this variable instead Making PIPE_5 the default. It should be backward 
   optimal with PIPE_3 MicroBlazes */
enum pipeline_type microblaze_pipe = MICROBLAZE_PIPE_5;

/* Generating calls to position independent functions?  */
enum microblaze_abicalls_type microblaze_abicalls;

/* High and low marks for floating point values which we will accept
   as legitimate constants for LEGITIMATE_CONSTANT_P.  These are
   initialized in override_options.  */
REAL_VALUE_TYPE dfhigh, dflow, sfhigh, sflow;

/* Mode used for saving/restoring general purpose registers.  */
static enum machine_mode gpr_mode;

/* Array giving truth value on whether or not a given hard register
   can support a given mode.  */
char microblaze_hard_regno_mode_ok[(int)MAX_MACHINE_MODE]
				  [FIRST_PSEUDO_REGISTER];

/* Current frame information calculated by compute_frame_size.  */
struct microblaze_frame_info current_frame_info;

/* Zero structure to initialize current_frame_info.  */
struct microblaze_frame_info zero_frame_info;

struct string_constant
{
  struct string_constant *next;
  char *label;
};

static struct string_constant *string_constants;

/* List of all MICROBLAZE punctuation characters used by print_operand.  */
char microblaze_print_operand_punct[256];

/* Map GCC register number to debugger register number.  */
int microblaze_dbx_regno[FIRST_PSEUDO_REGISTER];

/* Map hard register number to register class */
enum reg_class microblaze_regno_to_class[] =
{
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  GR_REGS,	GR_REGS,	GR_REGS,	GR_REGS,
  HI_REG,	LO_REG,		HILO_REG,	ST_REGS,
  ST_REGS,	ST_REGS,	ST_REGS,	ST_REGS,
  ST_REGS,	ST_REGS,	ST_REGS,	GR_REGS,
  GR_REGS,    GR_REGS
};

/* Map register constraint character to register class.  */
enum reg_class microblaze_char_to_class[256] =
{
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
  NO_REGS,	NO_REGS,	NO_REGS,	NO_REGS,
};

int get_base_reg (rtx);
static int printed = 0;

/* Microblaze specific machine attributes.
   interrupt_handler - Interrupt handler attribute to add interrupt prologue 
		       and epilogue and use appropriate interrupt return.
   save_volatiles    - Similiar to interrupt handler, but use normal return. */
int interrupt_handler;
int save_volatiles;

const struct attribute_spec microblaze_attribute_table[] = {
  /* name         min_len, max_len, decl_req, type_req, fn_type, req_handler */
  {"interrupt_handler", 0,       0,     true,    false,   false,        NULL},
  {"save_volatiles"   , 0,       0,     true,    false,   false,        NULL},
  { NULL,        	0,       0,    false,    false,   false,        NULL}
};

static int microblaze_interrupt_function_p (tree);
static int microblaze_save_volatiles (tree);

#undef TARGET_ENCODE_SECTION_INFO
#define TARGET_ENCODE_SECTION_INFO      microblaze_encode_section_info

#undef TARGET_ASM_GLOBALIZE_LABEL
#define TARGET_ASM_GLOBALIZE_LABEL      microblaze_globalize_label

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE    microblaze_function_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE    microblaze_function_epilogue

#undef TARGET_ASM_INTERNAL_LABEL
#define TARGET_ASM_INTERNAL_LABEL       microblaze_internal_label

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS                microblaze_rtx_costs

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST             microblaze_address_cost

#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE          microblaze_attribute_table

#undef TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR          microblaze_asm_constructor

#undef TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR           microblaze_asm_destructor

#undef TARGET_IN_SMALL_DATA_P
#define TARGET_IN_SMALL_DATA_P          microblaze_elf_in_small_data_p

#undef TARGET_ASM_SELECT_RTX_SECTION
#define TARGET_ASM_SELECT_RTX_SECTION   microblaze_select_rtx_section

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION       microblaze_select_section

#undef TARGET_HAVE_SRODATA_SECTION
#define TARGET_HAVE_SRODATA_SECTION     true

#undef TARGET_SCHED_USE_DFA_PIPELINE_INTERFACE
#define TARGET_SCHED_USE_DFA_PIPELINE_INTERFACE \
                                    microblaze_sched_use_dfa_pipeline_interface

#undef TARGET_ASM_FUNCTION_END_PROLOGUE
#define TARGET_ASM_FUNCTION_END_PROLOGUE \
                                        microblaze_function_end_prologue

#undef TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION		microblaze_handle_option

#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS	TARGET_DEFAULT

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES	function_arg_partial_bytes

#undef TARGET_PROMOTE_FUNCTION_RETURN
#define TARGET_PROMOTE_FUNCTION_RETURN 	hook_bool_tree_true

struct gcc_target targetm = TARGET_INITIALIZER;

/* Return truth value if a CONST_DOUBLE is ok to be a legitimate constant.  */

int
microblaze_const_double_ok (rtx op, enum machine_mode mode)
{
  REAL_VALUE_TYPE d;

  if (GET_CODE (op) != CONST_DOUBLE)
    return 0;

  if (mode == VOIDmode)
    return 1;

  if (mode != SFmode && mode != DFmode)
    return 0;

  if (op == CONST0_RTX (mode))
    return 1;

  REAL_VALUE_FROM_CONST_DOUBLE (d, op);

  if (REAL_VALUE_ISNAN (d))
    return FALSE;

  if (REAL_VALUE_NEGATIVE (d))
    d = REAL_VALUE_NEGATE (d);

  if (mode == DFmode)
    {
      if (REAL_VALUES_LESS (d, dfhigh) && REAL_VALUES_LESS (dflow, d))
	return 1;
    }
  else
    {
      if (REAL_VALUES_LESS (d, sfhigh) && REAL_VALUES_LESS (sflow, d))
	return 1;
    }

  return 0;
}

/* Return truth value if a memory operand fits in a single instruction
   (ie, register + small offset) or (register + register).  */

int
simple_memory_operand (rtx op, enum machine_mode mode ATTRIBUTE_UNUSED)
{
  rtx addr, plus0, plus1;

  /* Eliminate non-memory operations */
  if (GET_CODE (op) != MEM)
    return 0;

  /* dword operations really put out 2 instructions, so eliminate them.  */
  /* ??? This isn't strictly correct.  It is OK to accept multiword modes
     here, since the length attributes are being set correctly, but only
     if the address is offsettable.  */
  if (GET_MODE_SIZE (GET_MODE (op)) > UNITS_PER_WORD)
    return 0;


  /* Decode the address now.  */
  addr = XEXP (op, 0);
  switch (GET_CODE (addr))

    {
    case REG:
      return 1;

    case PLUS:
      plus0 = XEXP (addr, 0);
      plus1 = XEXP (addr, 1);

      if (GET_CODE (plus0) == REG && GET_CODE (plus1) == CONST_INT
	  && SMALL_INT (plus1))
	{
	  return 1;
	}
      else if (GET_CODE (plus1) == REG && GET_CODE (plus0) == CONST_INT)
	{
	  return 1;
	}
      else if (GET_CODE (plus0) == REG && GET_CODE (plus1) == REG)
	{
	  return 1;
	}
      else
	return 0;

    case SYMBOL_REF:
      return 0;

    default:
      break;
    }

  return 0;
}

/* Return nonzero for a memory address that can be used to load or store
   a doubleword.  */

int
double_memory_operand (rtx op, enum machine_mode mode)
{
  rtx addr;

  if (GET_CODE (op) != MEM || !memory_operand (op, mode))
    {
      /* During reload, we accept a pseudo register if it has an
         appropriate memory address.  If we don't do this, we will
         wind up reloading into a register, and then reloading that
         register from memory, when we could just reload directly from
         memory.  */
      if (reload_in_progress
	  && GET_CODE (op) == REG
	  && REGNO (op) >= FIRST_PSEUDO_REGISTER
	  && reg_renumber[REGNO (op)] < 0
	  && reg_equiv_mem[REGNO (op)] != 0
	  && double_memory_operand (reg_equiv_mem[REGNO (op)], mode))
	return 1;
      return 0;
    }

  /* Make sure that 4 added to the address is a valid memory address.
     This essentially just checks for overflow in an added constant.  */

  addr = XEXP (op, 0);

  if (CONSTANT_ADDRESS_P (addr))
    return 1;

  return memory_address_p ((GET_MODE_CLASS (mode) == MODE_INT
			    ? SImode : SFmode), plus_constant (addr, 4));
}


/* This hook is called many times during insn scheduling. If the hook 
   returns nonzero, the automaton based pipeline description is used 
   for insn scheduling. Otherwise the traditional pipeline description 
   is used. The default is usage of the traditional pipeline description. */
int
microblaze_sched_use_dfa_pipeline_interface (void)
{
  return microblaze_sched_use_dfa;
}

/* Write a message to stderr (for use in macros expanded in files that do not
   include stdio.h).  */

void
trace (const char *s, const char *s1, const char *s2)
{
  fprintf (stderr, s, s1, s2);
}

/* Implement REG_OK_FOR_BASE_P -and- REG_OK_FOR_INDEX_P  */
int
microblaze_regno_ok_for_base_p (int regno, int strict)
{
  if (regno >= FIRST_PSEUDO_REGISTER)
    {
      if (!strict)
	return true;
      regno = reg_renumber[regno];
    }

  /* These fake registers will be eliminated to either the stack or
     hard frame pointer, both of which are usually valid base registers.
     Reload deals with the cases where the eliminated form isn't valid.  */
  if (regno == ARG_POINTER_REGNUM || regno == FRAME_POINTER_REGNUM)
    return true;

  return GP_REG_P (regno);
}

/* Return true if X is a valid base register for the given mode.
   Allow only hard registers if STRICT.  */

static bool
microblaze_valid_base_register_p (rtx x,
				  enum machine_mode mode ATTRIBUTE_UNUSED,
				  int strict)
{
  if (!strict && GET_CODE (x) == SUBREG)
    x = SUBREG_REG (x);

  return (GET_CODE (x) == REG
	  && microblaze_regno_ok_for_base_p (REGNO (x), strict));
}

static bool
microblaze_classify_unspec (struct microblaze_address_info *info, rtx x)
{
  info->symbol_type = SYMBOL_TYPE_GENERAL;
  info->symbol = XVECEXP (x, 0, 0);

  if (XINT (x, 1) == UNSPEC_GOTOFF)
    {
      info->regA = gen_rtx_REG (SImode, PIC_OFFSET_TABLE_REGNUM);
      info->type = ADDRESS_GOTOFF;
    }
  else if (XINT (x, 1) == UNSPEC_PLT)
    {
      info->type = ADDRESS_PLT;
    }
  else
    {
      return false;
    }
  return true;
}


/* Return true if X is a valid index register for the given mode.
   Allow only hard registers if STRICT.  */

static bool
microblaze_valid_index_register_p (rtx x,
				   enum machine_mode mode ATTRIBUTE_UNUSED,
				   int strict)
{
  if (!strict && GET_CODE (x) == SUBREG)
    x = SUBREG_REG (x);

  return (GET_CODE (x) == REG
	  /* A base register is good enough to be an index register on Microblaze */
	  && microblaze_regno_ok_for_base_p (REGNO (x), strict));
}

/* Return true if X is a valid address for machine mode MODE.  If it is,
   fill in INFO appropriately.  STRICT is true if we should only accept
   hard base registers.  

      type                     regA      regB    offset      symbol

   ADDRESS_INVALID             NULL      NULL     NULL        NULL

   ADDRESS_REG                 %0        NULL     const_0 /   NULL
                                                  const_int
   ADDRESS_REG_INDEX           %0        %1       NULL        NULL

   ADDRESS_SYMBOLIC            r0 /      NULL     NULL        symbol    
                           sda_base_reg 

   ADDRESS_CONST_INT           r0       NULL      const       NULL

   For modes spanning multiple registers (DFmode in 32-bit GPRs,
   DImode, TImode), indexed addressing cannot be used because
   adjacent memory cells are accessed by adding word-sized offsets
   during assembly output.  */

static bool
microblaze_classify_address (struct microblaze_address_info *info, rtx x,
			     enum machine_mode mode, int strict)
{
  rtx xplus0;
  rtx xplus1;

  info->type = ADDRESS_INVALID;
  info->regA = NULL;
  info->regB = NULL;
  info->offset = NULL;
  info->symbol = NULL;
  info->symbol_type = SYMBOL_TYPE_INVALID;

  switch (GET_CODE (x))
    {
    case REG:
    case SUBREG:
      {
	info->type = ADDRESS_REG;
	info->regA = x;
	info->offset = const0_rtx;
	return microblaze_valid_base_register_p (info->regA, mode, strict);
      }
    case PLUS:
      {
	xplus0 = XEXP (x, 0);
	xplus1 = XEXP (x, 1);

	if (microblaze_valid_base_register_p (xplus0, mode, strict))
	  {
	    info->type = ADDRESS_REG;
	    info->regA = xplus0;

	    if (GET_CODE (xplus1) == CONST_INT)
	      {
		info->offset = xplus1;
		return true;
	      }
	    else if (GET_CODE (xplus1) == UNSPEC)
	      {
		return microblaze_classify_unspec (info, xplus1);
	      }
	    else if ((GET_CODE (xplus1) == SYMBOL_REF ||
		      GET_CODE (xplus1) == LABEL_REF) && flag_pic == 2)
	      {
		return false;
	      }
	    else if (GET_CODE (xplus1) == SYMBOL_REF ||
		     GET_CODE (xplus1) == LABEL_REF ||
		     GET_CODE (xplus1) == CONST)
	      {
		if (GET_CODE (XEXP (xplus1, 0)) == UNSPEC)
		  return microblaze_classify_unspec (info, XEXP (xplus1, 0));
		else if (flag_pic == 2)
		  {
		    return false;
		  }
		info->type = ADDRESS_SYMBOLIC;
		info->symbol = xplus1;
		info->symbol_type = SYMBOL_TYPE_GENERAL;
		return true;
	      }
	    else if (GET_CODE (xplus1) == REG
		     && microblaze_valid_index_register_p (xplus1, mode,
							   strict)
		     && (GET_MODE_SIZE (mode) <= UNITS_PER_WORD))
	      {
		/* Restrict larger than word-width modes from using an index register */
		info->type = ADDRESS_REG_INDEX;
		info->regB = xplus1;
		return true;
	      }
	  }
	break;
      }
    case CONST_INT:
      {
	info->regA = gen_rtx_raw_REG (mode, 0);
	info->type = ADDRESS_CONST_INT;
	info->offset = x;
	return true;
      }
    case CONST:
    case LABEL_REF:
    case SYMBOL_REF:
      {
	info->type = ADDRESS_SYMBOLIC;
	info->symbol_type = SYMBOL_TYPE_GENERAL;
	info->symbol = x;
	info->regA = gen_rtx_raw_REG (mode, get_base_reg (x));

	if (GET_CODE (x) == CONST)
	  {
	    return !(flag_pic && pic_address_needs_scratch (x));
	  }
	else if (flag_pic == 2)
	  {
	    return false;
	  }

	return true;
      }

    case UNSPEC:
      {
	if (reload_in_progress)
	  regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
	return microblaze_classify_unspec (info, x);
      }

    default:
      return false;
    }

  return false;
}

/* This function is used to implement GO_IF_LEGITIMATE_ADDRESS.  It
   returns a nonzero value if X is a legitimate address for a memory
   operand of the indicated MODE.  STRICT is nonzero if this function
   is called during reload.  */

bool
microblaze_legitimate_address_p (enum machine_mode mode, rtx x, int strict)
{
  struct microblaze_address_info addr;

  return microblaze_classify_address (&addr, x, mode, strict);
}


/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This is used from only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was
   called.  In some cases it is useful to look at this to decide what
   needs to be done.

   MODE is passed so that this function can use GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this function to do nothing.  It exists to
   recognize opportunities to optimize the output.

   For the MICROBLAZE, transform:

   memory(X + <large int>)

   into:

   Y = <large int> & ~0x7fff;
   Z = X + Y
   memory (Z + (<large int> & 0x7fff));

   This is for CSE to find several similar references, and only use one Z.

   When PIC, convert addresses of the form memory (symbol+large int) to
   memory (reg+large int).  */
rtx
microblaze_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
			       enum machine_mode mode ATTRIBUTE_UNUSED)
{
  register rtx xinsn = x, result;

  if (TARGET_DEBUG_B_MODE)
    {
      GO_PRINTF ("\n========== LEGITIMIZE_ADDRESS\n");
      GO_DEBUG_RTX (xinsn);
    }

  if (GET_CODE (xinsn) == CONST
      && flag_pic && pic_address_needs_scratch (xinsn))
    {
      rtx ptr_reg = gen_reg_rtx (Pmode);
      rtx constant = XEXP (XEXP (xinsn, 0), 1);

      emit_move_insn (ptr_reg, XEXP (XEXP (xinsn, 0), 0));

      result = gen_rtx_PLUS (Pmode, ptr_reg, constant);
      if (SMALL_INT (constant))
	return result;
      /* Otherwise we fall through so the code below will fix the 
         constant.  */
      xinsn = result;
    }

  if (GET_CODE (xinsn) == PLUS)
    {
      register rtx xplus0 = XEXP (xinsn, 0);
      register rtx xplus1 = XEXP (xinsn, 1);
      register enum rtx_code code0 = GET_CODE (xplus0);
      register enum rtx_code code1 = GET_CODE (xplus1);

      if (code0 != REG && code1 == REG)
	{
	  xplus0 = XEXP (xinsn, 1);
	  xplus1 = XEXP (xinsn, 0);
	  code0 = GET_CODE (xplus0);
	  code1 = GET_CODE (xplus1);
	}

      if (code0 == REG && REG_OK_FOR_BASE_P (xplus0)
	  && code1 == CONST_INT && !SMALL_INT (xplus1))
	{
	  rtx int_reg = gen_reg_rtx (Pmode);
	  rtx ptr_reg = gen_reg_rtx (Pmode);

	  emit_move_insn (int_reg, GEN_INT (INTVAL (xplus1) & ~0x7fff));

	  emit_insn (gen_rtx_SET (VOIDmode,
				  ptr_reg,
				  gen_rtx_PLUS (Pmode, xplus0, int_reg)));

	  result = gen_rtx_PLUS (Pmode, ptr_reg,
				 GEN_INT (INTVAL (xplus1) & 0x7fff));
	  return result;
	}

      if (code0 == REG && REG_OK_FOR_BASE_P (xplus0) && flag_pic == 2)
	{
	  if (reload_in_progress)
	    regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
	  if (code1 == CONST)
	    {
	      xplus1 = XEXP (xplus1, 0);
	      code1 = GET_CODE (xplus1);
	    }
	  if (code1 == SYMBOL_REF)
	    {
	      result =
		gen_rtx_UNSPEC (Pmode, gen_rtvec (1, xplus1), UNSPEC_GOTOFF);
	      result = gen_rtx_CONST (Pmode, result);
	      result = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, result);
	      result = gen_const_mem (Pmode, result);
	      result = gen_rtx_PLUS (Pmode, xplus0, result);
	      return result;
	    }
	}
    }

  if (GET_CODE (xinsn) == SYMBOL_REF)
    {
      if (reload_in_progress)
	regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
      result = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, xinsn), UNSPEC_GOTOFF);
      result = gen_rtx_CONST (Pmode, result);
      result = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, result);
      result = gen_const_mem (Pmode, result);
      return result;
    }

  if (TARGET_DEBUG_B_MODE)
    GO_PRINTF ("LEGITIMIZE_ADDRESS could not fix.\n");

  return NULL_RTX;
}

/* Block Moves */

#define MAX_MOVE_REGS 8
#define MAX_MOVE_BYTES (MAX_MOVE_REGS * UNITS_PER_WORD)

/* Emit straight-line code to move LENGTH bytes from SRC to DEST.
   Assume that the areas do not overlap.  */

static void
microblaze_block_move_straight (rtx dest, rtx src, HOST_WIDE_INT length)
{
  HOST_WIDE_INT offset, delta;
  unsigned HOST_WIDE_INT bits;
  int i;
  enum machine_mode mode;
  rtx *regs;

  bits = BITS_PER_WORD;
  mode = mode_for_size (bits, MODE_INT, 0);
  delta = bits / BITS_PER_UNIT;

  /* Allocate a buffer for the temporary registers.  */
  regs = alloca (sizeof (rtx) * length / delta);

  /* Load as many BITS-sized chunks as possible.  Use a normal load if
     the source has enough alignment, otherwise use left/right pairs.  */
  for (offset = 0, i = 0; offset + delta <= length; offset += delta, i++)
    {
      regs[i] = gen_reg_rtx (mode);
      emit_move_insn (regs[i], adjust_address (src, mode, offset));
    }

  /* Copy the chunks to the destination.  */
  for (offset = 0, i = 0; offset + delta <= length; offset += delta, i++)
    emit_move_insn (adjust_address (dest, mode, offset), regs[i]);

  /* Mop up any left-over bytes.  */
  if (offset < length)
    {
      src = adjust_address (src, BLKmode, offset);
      dest = adjust_address (dest, BLKmode, offset);
      move_by_pieces (dest, src, length - offset,
		      MIN (MEM_ALIGN (src), MEM_ALIGN (dest)), 0);
    }
}

/* Helper function for doing a loop-based block operation on memory
   reference MEM.  Each iteration of the loop will operate on LENGTH
   bytes of MEM.

   Create a new base register for use within the loop and point it to
   the start of MEM.  Create a new memory reference that uses this
   register.  Store them in *LOOP_REG and *LOOP_MEM respectively.  */

static void
microblaze_adjust_block_mem (rtx mem, HOST_WIDE_INT length,
			     rtx * loop_reg, rtx * loop_mem)
{
  *loop_reg = copy_addr_to_reg (XEXP (mem, 0));

  /* Although the new mem does not refer to a known location,
     it does keep up to LENGTH bytes of alignment.  */
  *loop_mem = change_address (mem, BLKmode, *loop_reg);
  set_mem_align (*loop_mem,
		 MIN ((HOST_WIDE_INT) MEM_ALIGN (mem),
		      length * BITS_PER_UNIT));
}


/* Move LENGTH bytes from SRC to DEST using a loop that moves MAX_MOVE_BYTES
   per iteration.  LENGTH must be at least MAX_MOVE_BYTES.  Assume that the
   memory regions do not overlap.  */

static void
microblaze_block_move_loop (rtx dest, rtx src, HOST_WIDE_INT length)
{
  rtx label, src_reg, dest_reg, final_src;
  HOST_WIDE_INT leftover;

  leftover = length % MAX_MOVE_BYTES;
  length -= leftover;

  /* Create registers and memory references for use within the loop.  */
  microblaze_adjust_block_mem (src, MAX_MOVE_BYTES, &src_reg, &src);
  microblaze_adjust_block_mem (dest, MAX_MOVE_BYTES, &dest_reg, &dest);

  /* Calculate the value that SRC_REG should have after the last iteration
     of the loop.  */
  final_src = expand_simple_binop (Pmode, PLUS, src_reg, GEN_INT (length),
				   0, 0, OPTAB_WIDEN);

  /* Emit the start of the loop.  */
  label = gen_label_rtx ();
  emit_label (label);

  /* Emit the loop body.  */
  microblaze_block_move_straight (dest, src, MAX_MOVE_BYTES);

  /* Move on to the next block.  */
  emit_move_insn (src_reg, plus_constant (src_reg, MAX_MOVE_BYTES));
  emit_move_insn (dest_reg, plus_constant (dest_reg, MAX_MOVE_BYTES));

  /* Emit the loop condition.  */
  emit_insn (gen_cmpsi (src_reg, final_src));
  emit_jump_insn (gen_bne (label));

  /* Mop up any left-over bytes.  */
  if (leftover)
    microblaze_block_move_straight (dest, src, leftover);
}

/* Expand a movmemsi instruction.  */

bool
microblaze_expand_block_move (rtx dest, rtx src, rtx length, rtx align_rtx)
{

  if (GET_CODE (length) == CONST_INT)
    {
      HOST_WIDE_INT bytes = INTVAL (length);
      int align = INTVAL (align_rtx);

      if (align > UNITS_PER_WORD)
	{
	  align = UNITS_PER_WORD;	/* We can't do any better */
	}
      else if (align < UNITS_PER_WORD)
	{
	  if (INTVAL (length) <= MAX_MOVE_BYTES)
	    {
	      move_by_pieces (dest, src, bytes, align, 0);
	      return true;
	    }
	  else
	    return false;
	}

      if (INTVAL (length) <= 2 * MAX_MOVE_BYTES)
	{
	  microblaze_block_move_straight (dest, src, INTVAL (length));
	  return true;
	}
      else if (optimize)
	{
	  microblaze_block_move_loop (dest, src, INTVAL (length));
	  return true;
	}
    }
  return false;
}

bool
microblaze_rtx_costs (rtx x,
		      int code, int outer_code ATTRIBUTE_UNUSED, int *total)
{
  enum machine_mode mode = GET_MODE (x);

  switch (code)
    {
    case MEM:
      {
	int num_words = (GET_MODE_SIZE (mode) > UNITS_PER_WORD) ? 2 : 1;
	if (simple_memory_operand (x, mode))
	  *total = COSTS_N_INSNS (2 * num_words);
	else
	  *total = COSTS_N_INSNS (2 * (2 * num_words));

	return true;
      }
    case NOT:
      {
	if (mode == DImode)
	  {
	    *total = COSTS_N_INSNS (2);
	  }
	else
	  *total = COSTS_N_INSNS (1);
	return false;
      }
    case AND:
    case IOR:
    case XOR:
      {
	if (mode == DImode)
	  {
	    *total = COSTS_N_INSNS (2);
	  }
	else
	  *total = COSTS_N_INSNS (1);

	return false;
      }
    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
      {
	if (TARGET_BARREL_SHIFT)
	  {
	    if (microblaze_version_compare (microblaze_select_cpu, "v5.00.a")
		>= 0)
	      *total = COSTS_N_INSNS (1);
	    else
	      *total = COSTS_N_INSNS (2);
	  }
	else if (!TARGET_SOFT_MUL)
	  *total = COSTS_N_INSNS (1);
	else if (GET_CODE (XEXP (x, 1)) == CONST_INT)
	  {
	    /* Add 1 to make shift slightly more expensive than add. */
	    *total = COSTS_N_INSNS (INTVAL (XEXP (x, 1))) + 1;
	    /* Reduce shift costs for for special circumstances. */
	    if (optimize_size && INTVAL (XEXP (x, 1)) > 5)
	      *total -= 2;
	    if (!optimize_size && INTVAL (XEXP (x, 1)) > 17)
	      *total -= 2;
	  }
	else
	  /* Double the worst cost of shifts when there is no barrel shifter and 
	     the shift amount is in a reg */
	  *total = COSTS_N_INSNS (32 * 4);
	return true;
      }
    case PLUS:
    case MINUS:
      {
	if (mode == SFmode || mode == DFmode)
	  {
	    if (TARGET_HARD_FLOAT)
	      *total = COSTS_N_INSNS (6);
	    return true;
	  }
	else if (mode == DImode)
	  {
	    *total = COSTS_N_INSNS (4);
	    return true;
	  }
	else
	  {
	    *total = COSTS_N_INSNS (1);
	    return true;
	  }

	return false;
      }
    case NEG:
      {
	if (mode == DImode)
	  *total = COSTS_N_INSNS (4);

	return false;
      }
    case MULT:
      {
	if (mode == SFmode)
	  {
	    if (TARGET_HARD_FLOAT)
	      *total = COSTS_N_INSNS (6);
	  }
	else if (!TARGET_SOFT_MUL)
	  {
	    if (microblaze_version_compare (microblaze_select_cpu, "v5.00.a")
		>= 0)
	      *total = COSTS_N_INSNS (1);
	    else
	      *total = COSTS_N_INSNS (3);
	  }
	else
	  *total = COSTS_N_INSNS (10);
	return true;
      }
    case DIV:
    case UDIV:
      {
	if (mode == SFmode)
	  {
	    if (TARGET_HARD_FLOAT)
	      *total = COSTS_N_INSNS (23);
	  }
	return false;
      }
    case SIGN_EXTEND:
      {
	*total = COSTS_N_INSNS (1);
	return false;
      }
    case ZERO_EXTEND:
      {
	*total = COSTS_N_INSNS (1);
	return false;
      }
    }

  return false;
}

/* Return the number of instructions needed to load or store a value
   of mode MODE at X.  Return 0 if X isn't valid for MODE.  */

int
microblaze_address_insns (rtx x, enum machine_mode mode)
{
  struct microblaze_address_info addr;

  if (microblaze_classify_address (&addr, x, mode, false))
    {
      switch (addr.type)
	{
	case ADDRESS_REG:
	  if (SMALL_INT (addr.offset))
	    return 1;
	  else
	    return 2;
	case ADDRESS_CONST_INT:
	  if (SMALL_INT (x))
	    return 1;
	  else
	    return 2;
	case ADDRESS_REG_INDEX:
	case ADDRESS_SYMBOLIC:
	  return 1;
	case ADDRESS_GOTOFF:
	  return 2;
	default:
	  break;
	}
    }
  return 0;
}

/* Provide the costs of an addressing mode that contains ADDR.
   If ADDR is not a valid address, its cost is irrelevant.  */
int
microblaze_address_cost (rtx addr)
{
  return COSTS_N_INSNS (microblaze_address_insns (addr, GET_MODE (addr)));
}

/* Return nonzero if X is an address which needs a temporary register when 
   reloaded while generating PIC code.  */

/* XLNX [08/16/01] Need to look into this*/
int
pic_address_needs_scratch (rtx x)
{
  /* An address which is a symbolic plus a non SMALL_INT needs a temp reg.  */
  if (GET_CODE (x) == CONST && GET_CODE (XEXP (x, 0)) == PLUS
      && GET_CODE (XEXP (XEXP (x, 0), 0)) == SYMBOL_REF
      && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
      && (flag_pic == 2 || !SMALL_INT (XEXP (XEXP (x, 0), 1))))
    return 1;

  return 0;
}

/* Emit the common code for doing conditional branches.
   operand[0] is the label to jump to.
   The comparison operands are saved away by cmp{si,di,sf,df}.  */

void
microblaze_gen_conditional_branch (rtx operands[], enum rtx_code test_code)
{
  enum cmp_type type = branch_type;
  rtx cmp0 = branch_cmp[0];
  rtx cmp1 = branch_cmp[1];
  enum machine_mode mode;
  rtx reg0;
  rtx label1, label2;

  switch (type)
    {
    case CMP_SI:
    case CMP_DI:
      mode = type == CMP_SI ? SImode : DImode;
      break;
    case CMP_SF:
      if (TARGET_HARD_FLOAT)
	{
	  reg0 = gen_reg_rtx (SImode);
	  emit_insn (gen_rtx_SET (VOIDmode, reg0,
				  gen_rtx_fmt_ee ((test_code ==
						   NE ? EQ : test_code),
						  SImode, cmp0, cmp1)));

	  /* Setup test and branch for following instruction
	     Setup a test for zero as opposed to non-zero.
	     This is more optimally implemented. */
	  test_code = (test_code == NE) ? EQ : NE;
	  mode = SImode;
	  cmp0 = reg0;
	  cmp1 = const0_rtx;
	  break;
	}
      else
	fatal_insn ("gen_conditional_branch:",
		    gen_rtx_fmt_ee (test_code, VOIDmode, cmp0, cmp1));
    default:
      fatal_insn ("gen_conditional_branch:",
		  gen_rtx_fmt_ee (test_code, VOIDmode, cmp0, cmp1));
    }

  /* Generate the branch.  */

  label1 = gen_rtx_LABEL_REF (VOIDmode, operands[0]);
  label2 = pc_rtx;

  if (!(GET_CODE (cmp1) == CONST_INT && INTVAL (cmp1) == 0))
    {
      /* Except for branch_zero */
      emit_jump_insn (gen_rtx_PARALLEL (VOIDmode,
					gen_rtvec (2,
						   gen_rtx_SET (VOIDmode,
								pc_rtx,
								gen_rtx_IF_THEN_ELSE
								(VOIDmode,
								 gen_rtx_fmt_ee
								 (test_code,
								  mode, cmp0,
								  cmp1),
								 label1,
								 label2)),
						   gen_rtx_CLOBBER (VOIDmode,
								    gen_rtx_REG
								    (SImode,
								     MB_ABI_ASM_TEMP_REGNUM)))));
    }
  else
    emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx,
				 gen_rtx_IF_THEN_ELSE (VOIDmode,
						       gen_rtx_fmt_ee
						       (test_code, mode, cmp0,
							cmp1), label1,
						       label2)));
}

/* Argument support functions.  */

/* Initialize CUMULATIVE_ARGS for a function.  */

void
init_cumulative_args (CUMULATIVE_ARGS * cum,	/* argument info to initialize */
		      tree fntype,	/* tree ptr for function decl */
		      rtx libname ATTRIBUTE_UNUSED)	/* SYMBOL_REF of library name or 0 */
{
  static CUMULATIVE_ARGS zero_cum;
  tree param, next_param;

  if (TARGET_DEBUG_E_MODE)
    {
      fprintf (stderr,
	       "\ninit_cumulative_args, fntype = 0x%.8lx", (long) fntype);

      if (!fntype)
	fputc ('\n', stderr);

      else
	{
	  tree ret_type = TREE_TYPE (fntype);
	  fprintf (stderr, ", fntype code = %s, ret code = %s\n",
		   tree_code_name[(int) TREE_CODE (fntype)],
		   tree_code_name[(int) TREE_CODE (ret_type)]);
	}
    }

  *cum = zero_cum;

  /* Determine if this function has variable arguments.  This is
     indicated by the last argument being 'void_type_mode' if there
     are no variable arguments.  The standard MICROBLAZE calling sequence
     passes all arguments in the general purpose registers in this case. */

  for (param = fntype ? TYPE_ARG_TYPES (fntype) : 0;
       param != 0; param = next_param)
    {
      next_param = TREE_CHAIN (param);
      if (next_param == 0 && TREE_VALUE (param) != void_type_node)
	cum->gp_reg_found = 1;
    }
}

/* Advance the argument to the next argument position.  */

void
function_arg_advance (CUMULATIVE_ARGS * cum,	/* current arg information */
		      enum machine_mode mode,	/* current arg mode */
		      tree type,	/* type of the argument or 0 if lib support */
		      int named)	/* whether or not the argument was named */
{
  if (TARGET_DEBUG_E_MODE)
    {
      fprintf (stderr,
	       "function_adv({gp reg found = %d, arg # = %2d, words = %2d}, %4s, ",
	       cum->gp_reg_found, cum->arg_number, cum->arg_words,
	       GET_MODE_NAME (mode));
      fprintf (stderr, "%p", (void *) type);
      fprintf (stderr, ", %d )\n\n", named);
    }

  cum->arg_number++;
  switch (mode)
    {
    case VOIDmode:
      break;

    default:
      if (GET_MODE_CLASS (mode) != MODE_COMPLEX_INT
	  && GET_MODE_CLASS (mode) != MODE_COMPLEX_FLOAT)
	abort ();

      cum->gp_reg_found = 1;
      cum->arg_words += ((GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1)
			 / UNITS_PER_WORD);
      break;

    case BLKmode:
      cum->gp_reg_found = 1;
      cum->arg_words += ((int_size_in_bytes (type) + UNITS_PER_WORD - 1)
			 / UNITS_PER_WORD);
      break;

    case SFmode:
      cum->arg_words++;
      if (!cum->gp_reg_found && cum->arg_number <= 2)
	cum->fp_code += 1 << ((cum->arg_number - 1) * 2);
      break;

    case DFmode:
      cum->arg_words += 2;
      if (!cum->gp_reg_found && !TARGET_SINGLE_FLOAT && cum->arg_number <= 2)
	cum->fp_code += 2 << ((cum->arg_number - 1) * 2);
      break;

    case DImode:
      cum->gp_reg_found = 1;
      cum->arg_words += 2;
      break;

    case QImode:
    case HImode:
    case SImode:
    case TImode:
      cum->gp_reg_found = 1;
      cum->arg_words++;
      break;
    }
}

/* Return an RTL expression containing the register for the given mode,
   or 0 if the argument is to be passed on the stack.  */

rtx
function_arg (CUMULATIVE_ARGS * cum,	/* current arg information */
	      enum machine_mode mode,	/* current arg mode */
	      tree type,	/* type of the argument or 0 if lib support */
	      int named)	/* != 0 for normal args, == 0 for ... args */
{
  rtx ret;
  int regbase = -1;
  int *arg_words = &cum->arg_words;
  int struct_p = (type != 0
		  && (TREE_CODE (type) == RECORD_TYPE
		      || TREE_CODE (type) == UNION_TYPE
		      || TREE_CODE (type) == QUAL_UNION_TYPE));

  if (TARGET_DEBUG_E_MODE)
    {
      fprintf (stderr,
	       "function_arg( {gp reg found = %d, arg # = %2d, words = %2d}, %4s, ",
	       cum->gp_reg_found, cum->arg_number, cum->arg_words,
	       GET_MODE_NAME (mode));
      fprintf (stderr, "%p", (void *) type);
      fprintf (stderr, ", %d ) = ", named);
    }

  cum->last_arg_fp = 0;
  switch (mode)
    {
    case SFmode:
    case DFmode:
    case VOIDmode:
    case QImode:
    case HImode:
    case SImode:
    case DImode:
    case TImode:
      regbase = GP_ARG_FIRST;
      break;
    default:
      if (GET_MODE_CLASS (mode) != MODE_COMPLEX_INT
	  && GET_MODE_CLASS (mode) != MODE_COMPLEX_FLOAT)
	abort ();
      /* Drops through.  */
    case BLKmode:
      regbase = GP_ARG_FIRST;
      break;
    }

  if (*arg_words >= MAX_ARGS_IN_REGISTERS)
    {
      if (TARGET_DEBUG_E_MODE)
	fprintf (stderr, "<stack>%s\n", struct_p ? ", [struct]" : "");

      ret = 0;
    }
  else
    {
      if (regbase == -1)
	abort ();

      ret = gen_rtx_REG (mode, regbase + *arg_words);

      if (TARGET_DEBUG_E_MODE)
	fprintf (stderr, "%s%s\n", reg_names[regbase + *arg_words],
		 struct_p ? ", [struct]" : "");
    }

  if (mode == VOIDmode)
    {
      if (cum->num_adjusts > 0)
	ret = gen_rtx_PARALLEL ((enum machine_mode) cum->fp_code,
				gen_rtvec_v (cum->num_adjusts, cum->adjust));
    }

  return ret;
}

/* Return number of bytes of argument to put in registers. */
int
function_arg_partial_bytes (CUMULATIVE_ARGS * cum,  /* current arg info */
			    enum machine_mode mode,	/* current arg mode */
			    tree type,	
				/* type of the argument or 0 if lib support */
			    int named ATTRIBUTE_UNUSED)	
				/* != 0 for normal args, == 0 for ... args */
{
  if ((mode == BLKmode
       || GET_MODE_CLASS (mode) != MODE_COMPLEX_INT
       || GET_MODE_CLASS (mode) != MODE_COMPLEX_FLOAT)
      && cum->arg_words < MAX_ARGS_IN_REGISTERS)
    {
      int words;
      if (mode == BLKmode)
	words = ((int_size_in_bytes (type) + UNITS_PER_WORD - 1)
		 / UNITS_PER_WORD);
      else
	words = (GET_MODE_SIZE (mode) + UNITS_PER_WORD - 1) / UNITS_PER_WORD;

      if (words + cum->arg_words <= MAX_ARGS_IN_REGISTERS)
	return 0;		/* structure fits in registers */

      if (TARGET_DEBUG_E_MODE)
	fprintf (stderr, "function_arg_partial_nregs = %d\n",
		 MAX_ARGS_IN_REGISTERS - cum->arg_words);

      return (MAX_ARGS_IN_REGISTERS - cum->arg_words) * UNITS_PER_WORD;
    }

  else if (mode == DImode && cum->arg_words == MAX_ARGS_IN_REGISTERS - 1)
    {
      if (TARGET_DEBUG_E_MODE)
	fprintf (stderr, "function_arg_partial_nregs = 1\n");

      return UNITS_PER_WORD;
    }

  return 0;
}

/*  Convert a version number of the form "vX.YY.Z" to an integer encoding 
    for easier range comparison */
static int
microblaze_version_to_int (const char *version)
{
  const char *p, *v;
  const char *tmpl = "vX.YY.Z";
  int iver = 0;

  p = (char *) version;
  v = tmpl;

  while (*v)
    {
      if (*v == 'X')
	{			/* Looking for major */
	  if (!(*p >= '0' && *p <= '9'))
	    return -1;
	  iver += (int) (*p - '0');
	  iver *= 10;
	}
      else if (*v == 'Y')
	{			/* Looking for minor */
	  if (!(*p >= '0' && *p <= '9'))
	    return -1;
	  iver += (int) (*p - '0');
	  iver *= 10;
	}
      else if (*v == 'Z')
	{			/* Looking for compat */
	  if (!(*p >= 'a' && *p <= 'z'))
	    return -1;
	  iver *= 10;
	  iver += (int) (*p - 'a');
	}
      else
	{
	  if (*p != *v)
	    return -1;
	}

      v++;
      p++;
    }

  if (*p)
    return -1;

  return iver;
}

/* Compare two given microblaze versions and return a verdict */
static int
microblaze_version_compare (const char *va, const char *vb)
{
  return strcasecmp (va, vb);
}

/* Set up the threshold for data to go into the small data area, instead
   of the normal data area, and detect any conflicts in the switches.  */

static bool
microblaze_handle_option (size_t code,
			  const char *arg ATTRIBUTE_UNUSED,
			  int value ATTRIBUTE_UNUSED)
{
  switch (code)
    {
      /* Check if we are asked to not clear BSS 
         If YES, we do not place zero initialized in BSS  */
    case OPT_mno_clearbss:
      flag_zero_initialized_in_bss = 0;
      break;
    }
  return true;
}


void
override_options (void)
{
  register int i, start;
  register int regno;
  register enum machine_mode mode;
  int ver;

  microblaze_section_threshold =
    g_switch_set ? g_switch_value : MICROBLAZE_DEFAULT_GVALUE;

  /* Check the Microblaze CPU version for any special action to be done */
  if (microblaze_select_cpu == NULL)
    microblaze_select_cpu = MICROBLAZE_DEFAULT_CPU;
  ver = microblaze_version_to_int (microblaze_select_cpu);
  if (ver == -1)
    {
      error ("(%s) is an invalid argument to -mcpu=", microblaze_select_cpu);
    }

  ver = microblaze_version_compare (microblaze_select_cpu, "v3.00.a");
  if (ver < 0)
    {
      /* No hardware exceptions in earlier versions. So no worries */
      microblaze_select_flags &= ~(MICROBLAZE_MASK_NO_UNSAFE_DELAY);
      microblaze_no_unsafe_delay = 0;
      microblaze_pipe = MICROBLAZE_PIPE_3;
    }
  else if (ver == 0
	   || (microblaze_version_compare (microblaze_select_cpu, "v4.00.b")
	       == 0))
    {
      microblaze_select_flags |= (MICROBLAZE_MASK_NO_UNSAFE_DELAY);
      microblaze_no_unsafe_delay = 1;
      microblaze_pipe = MICROBLAZE_PIPE_3;
    }
  else
    {
      /* We agree to use 5 pipe-stage model even on area optimized 3 
         pipe-stage variants. */
      microblaze_select_flags &= ~(MICROBLAZE_MASK_NO_UNSAFE_DELAY);
      microblaze_no_unsafe_delay = 0;
      microblaze_pipe = MICROBLAZE_PIPE_5;
      if (microblaze_version_compare (microblaze_select_cpu, "v5.00.a") == 0
	  || microblaze_version_compare (microblaze_select_cpu,
					 "v5.00.b") == 0
	  || microblaze_version_compare (microblaze_select_cpu,
					 "v5.00.c") == 0)
	{
	  /* Pattern compares are to be turned on by default only when 
 	     compiling for MB v5.00.'z' */
	  target_flags |= MASK_PATTERN_COMPARE;
	}
    }

  ver = microblaze_version_compare (microblaze_select_cpu, "v6.00.a");
  if (ver < 0)
    {
      if (TARGET_MULTIPLY_HIGH)
	warning (0,
		 "-mxl-multiply-high can be used only with -mcpu=v6.00.a or greater");
    }

  if (TARGET_MULTIPLY_HIGH && TARGET_SOFT_MUL)
    error ("-mxl-multiply-high requires -mno-xl-soft-mul");

  /* Always use DFA scheduler */
  microblaze_sched_use_dfa = 1;

  microblaze_abicalls = MICROBLAZE_ABICALLS_NO;

  /* Initialize the high, low values for legit floating point constants. */
  real_maxval (&dfhigh, 0, DFmode);
  real_maxval (&dflow, 1, DFmode);
  real_maxval (&sfhigh, 0, SFmode);
  real_maxval (&sflow, 1, SFmode);

  microblaze_print_operand_punct['?'] = 1;
  microblaze_print_operand_punct['#'] = 1;
  microblaze_print_operand_punct['&'] = 1;
  microblaze_print_operand_punct['!'] = 1;
  microblaze_print_operand_punct['*'] = 1;
  microblaze_print_operand_punct['@'] = 1;
  microblaze_print_operand_punct['.'] = 1;
  microblaze_print_operand_punct['('] = 1;
  microblaze_print_operand_punct[')'] = 1;
  microblaze_print_operand_punct['['] = 1;
  microblaze_print_operand_punct[']'] = 1;
  microblaze_print_operand_punct['<'] = 1;
  microblaze_print_operand_punct['>'] = 1;
  microblaze_print_operand_punct['{'] = 1;
  microblaze_print_operand_punct['}'] = 1;
  microblaze_print_operand_punct['^'] = 1;
  microblaze_print_operand_punct['$'] = 1;
  microblaze_print_operand_punct['+'] = 1;

  microblaze_char_to_class['d'] = GR_REGS;
  microblaze_char_to_class['f'] = NO_REGS;
  microblaze_char_to_class['h'] = HI_REG;
  microblaze_char_to_class['l'] = LO_REG;
  microblaze_char_to_class['a'] = HILO_REG;
  microblaze_char_to_class['x'] = MD_REGS;
  microblaze_char_to_class['b'] = ALL_REGS;
  microblaze_char_to_class['y'] = GR_REGS;
  microblaze_char_to_class['z'] = ST_REGS;

  /* Set up array to map GCC register number to debug register number.
     Ignore the special purpose register numbers.  */

  for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)
    microblaze_dbx_regno[i] = -1;

  start = GP_DBX_FIRST - GP_REG_FIRST;
  for (i = GP_REG_FIRST; i <= GP_REG_LAST; i++)
    microblaze_dbx_regno[i] = i + start;

  /* Save GPR registers in word_mode sized hunks.  word_mode hasn't been
     initialized yet, so we can't use that here.  */
  gpr_mode = SImode;

  /* Set up array giving whether a given register can hold a given mode. */

  for (mode = VOIDmode;
       mode != MAX_MACHINE_MODE; mode = (enum machine_mode) ((int) mode + 1))
    {
      register int size = GET_MODE_SIZE (mode);

      for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
	{
	  register int ok;

	  if (mode == CCmode)
	    {
	      ok = (ST_REG_P (regno) || GP_REG_P (regno));
	    }
	  else if (GP_REG_P (regno))
	    ok = ((regno & 1) == 0 || size <= UNITS_PER_WORD);
	  else
	    ok = 0;

	  microblaze_hard_regno_mode_ok[(int) mode][regno] = ok;
	}
    }
}

void
microblaze_order_regs_for_local_alloc (void)
{
  register int i;

  for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)
    reg_alloc_order[i] = i;
}

/* The MICROBLAZE debug format wants all automatic variables and arguments
   to be in terms of the virtual frame pointer (stack pointer before
   any adjustment in the function), while the MICROBLAZE 3.0 linker wants
   the frame pointer to be the stack pointer after the initial
   adjustment.  So, we do the adjustment here.  The arg pointer (which
   is eliminated) points to the virtual frame pointer, while the frame
   pointer (which may be eliminated) points to the stack pointer after
   the initial adjustments.  */

HOST_WIDE_INT
microblaze_debugger_offset (rtx addr, HOST_WIDE_INT offset)
{
  rtx offset2 = const0_rtx;
  rtx reg = eliminate_constant_term (addr, &offset2);

  if (offset == 0)
    offset = INTVAL (offset2);

  if (reg == stack_pointer_rtx || reg == frame_pointer_rtx
      || reg == hard_frame_pointer_rtx)
    {
      HOST_WIDE_INT frame_size = (!current_frame_info.initialized)
	? compute_frame_size (get_frame_size ())
	: current_frame_info.total_size;

      offset = offset - frame_size;
    }

  return offset;
}

/* Implement INITIAL_ELIMINATION_OFFSET.  FROM is either the frame
   pointer or argument pointer or the return address pointer.  TO is either 
   the stack pointer or hard frame pointer.  */

HOST_WIDE_INT
microblaze_initial_elimination_offset (int from, int to)
{
  HOST_WIDE_INT offset;

  switch (from)
    {
    case FRAME_POINTER_REGNUM:
      offset = 0;
      break;
    case ARG_POINTER_REGNUM:
      if (to == STACK_POINTER_REGNUM || to == HARD_FRAME_POINTER_REGNUM)
	offset = compute_frame_size (get_frame_size ());
      else
	{
	  abort ();
	}
      break;
    case RETURN_ADDRESS_POINTER_REGNUM:
      if (current_function_is_leaf)
	offset = 0;
      else
	offset = current_frame_info.gp_offset +
	  ((UNITS_PER_WORD - (POINTER_SIZE / BITS_PER_UNIT)));
      break;
    default:
      abort ();
    }
  return offset;
}

/* A C compound statement to output to stdio stream STREAM the
   assembler syntax for an instruction operand X.  X is an RTL
   expression.

   CODE is a value that can be used to specify one of several ways
   of printing the operand.  It is used when identical operands
   must be printed differently depending on the context.  CODE
   comes from the `%' specification that was used to request
   printing of the operand.  If the specification was just `%DIGIT'
   then CODE is 0; if the specification was `%LTR DIGIT' then CODE
   is the ASCII code for LTR.

   If X is a register, this macro should print the register's name.
   The names can be found in an array `reg_names' whose type is
   `char *[]'.  `reg_names' is initialized from `REGISTER_NAMES'.

   When the machine description has a specification `%PUNCT' (a `%'
   followed by a punctuation character), this macro is called with
   a null pointer for X and the punctuation character for CODE.

   The MICROBLAZE specific codes are:

   'X'  X is CONST_INT, prints 32 bits in hexadecimal format = "0x%08x",
   'x'  X is CONST_INT, prints 16 bits in hexadecimal format = "0x%04x",
   'F'  op is CONST_DOUBLE, print 32 bits in hex,
   'd'  output integer constant in decimal,
   'z'	if the operand is 0, use $0 instead of normal operand.
   'D'  print second register of double-word register operand.
   'L'  print low-order register of double-word register operand.
   'M'  print high-order register of double-word register operand.
   'C'  print part of opcode for a branch condition.
   'N'  print part of opcode for a branch condition, inverted.
   'S'  X is CODE_LABEL, print with prefix of "LS" (for embedded switch).
   'B'  print 'z' for EQ, 'n' for NE
   'b'  print 'n' for EQ, 'z' for NE
   'T'  print 'f' for EQ, 't' for NE
   't'  print 't' for EQ, 'f' for NE
   'Z'  print register and a comma, but print nothing for $fcc0
   'm'  Print 1<<operand.
   'i'  Print 'i' if MEM operand has immediate value
   'o'	Print operand address+4
   '?'	Print 'd' if we use a branch with delay slot instead of normal branch.
   'h'  Print high word of const_double (int or float) value as hex
   'j'  Print low word of const_double (int or float) value as hex
   's'  Print -1 if operand is negative, 0 if positive (sign extend)
   '@'	Print the name of the temporary register (rMB_ABI_ASM_TEMP_REGNUM).
   '#'	Print nop if the delay slot of a branch is not filled. 
*/

void
print_operand (FILE * file,	/* file to write to */
	       rtx op,		/* operand to print */
	       int letter)	/* %<letter> or 0 */
{
  register enum rtx_code code;

  if (PRINT_OPERAND_PUNCT_VALID_P (letter))
    {
      switch (letter)
	{
	case '?':
	  /* Conditionally add a 'd' to indicate filled delay slot */
	  if (final_sequence != NULL)
	    fputs ("d", file);
	  break;

	case '#':
	  /* Conditionally add a nop in unfilled delay slot */
	  if (final_sequence == NULL)
	    fputs ("nop\t\t# Unfilled delay slot\n", file);
	  break;

	case '@':
	  fputs (reg_names[GP_REG_FIRST + MB_ABI_ASM_TEMP_REGNUM], file);
	  break;

	default:
	  error ("PRINT_OPERAND: Unknown punctuation '%c'", letter);
	  break;
	}

      return;
    }

  if (!op)
    {
      error ("PRINT_OPERAND null pointer");
      return;
    }

  code = GET_CODE (op);

  if (code == SIGN_EXTEND)
    op = XEXP (op, 0), code = GET_CODE (op);

  if (letter == 'C')
    switch (code)
      {
      case EQ:
	fputs ("eq", file);
	break;
      case NE:
	fputs ("ne", file);
	break;
      case GT:
	fputs ("gt", file);
	break;
      case GE:
	fputs ("ge", file);
	break;
      case LT:
	fputs ("lt", file);
	break;
      case LE:
	fputs ("le", file);
	break;
      case GTU:
	fputs ("gtu", file);
	break;
      case GEU:
	fputs ("geu", file);
	break;
      case LTU:
	fputs ("ltu", file);
	break;
      case LEU:
	fputs ("leu", file);
	break;
      default:
	fatal_insn ("PRINT_OPERAND, invalid insn for %%C", op);
      }

  else if (letter == 'N')
    switch (code)
      {
      case EQ:
	fputs ("ne", file);
	break;
      case NE:
	fputs ("eq", file);
	break;
      case GT:
	fputs ("le", file);
	break;
      case GE:
	fputs ("lt", file);
	break;
      case LT:
	fputs ("ge", file);
	break;
      case LE:
	fputs ("gt", file);
	break;
      case GTU:
	fputs ("leu", file);
	break;
      case GEU:
	fputs ("ltu", file);
	break;
      case LTU:
	fputs ("geu", file);
	break;
      case LEU:
	fputs ("gtu", file);
	break;
      default:
	fatal_insn ("PRINT_OPERAND, invalid insn for %%N", op);
      }

  else if (letter == 'S')
    {
      char buffer[100];

      ASM_GENERATE_INTERNAL_LABEL (buffer, "LS", CODE_LABEL_NUMBER (op));
      assemble_name (file, buffer);
    }

  else if (letter == 'Z')
    {
      register int regnum;

      if (code != REG)
	abort ();

      regnum = REGNO (op);
      if (!ST_REG_P (regnum))
	abort ();

      if (regnum != ST_REG_FIRST)
	fprintf (file, "%s,", reg_names[regnum]);
    }

  /* Print 'i' for memory operands which have immediate values. */
  else if (letter == 'i')
    {
      if (code == MEM)
	{
	  struct microblaze_address_info info;

	  if (!microblaze_classify_address
	      (&info, XEXP (op, 0), GET_MODE (op), 1))
	    fatal_insn ("insn contains an invalid address !", op);

	  switch (info.type)
	    {
	    case ADDRESS_REG:
	    case ADDRESS_CONST_INT:
	    case ADDRESS_SYMBOLIC:
	    case ADDRESS_GOTOFF:
	      fputs ("i", file);
	      break;
	    case ADDRESS_REG_INDEX:
	      break;
	    case ADDRESS_INVALID:
	    case ADDRESS_PLT:
	      fatal_insn ("Invalid address", op);
	    }
	}
    }

  else if (code == REG || code == SUBREG)
    {
      register int regnum;

      if (code == REG)
	regnum = REGNO (op);
      else
	regnum = true_regnum (op);

      if ((letter == 'M' && !WORDS_BIG_ENDIAN)
	  || (letter == 'L' && WORDS_BIG_ENDIAN) || letter == 'D')
	regnum++;

      fprintf (file, "%s", reg_names[regnum]);
    }

  else if (code == MEM)
    if (letter == 'o')
      {
	rtx op4 = adjust_address (op, GET_MODE (op), 4);
	output_address (XEXP (op4, 0));
      }
    else
      output_address (XEXP (op, 0));

  else if (code == CONST_DOUBLE)
    {
      if (letter == 'h' || letter == 'j')
	{
	  int val[2];
	  if (GET_MODE (op) == DFmode)
	    {
	      REAL_VALUE_TYPE value;
	      REAL_VALUE_FROM_CONST_DOUBLE (value, op);
	      REAL_VALUE_TO_TARGET_DOUBLE (value, &val);
	    }
	  else
	    {
	      val[0] = CONST_DOUBLE_HIGH (op);
	      val[1] = CONST_DOUBLE_LOW (op);
	    }
	  fprintf (file, "0x%8.8x", (letter == 'h') ? val[0] : val[1]);
	}
      else if (letter == 'F')
	{
	  unsigned int value_long;
	  REAL_VALUE_TYPE value;
	  REAL_VALUE_FROM_CONST_DOUBLE (value, op);
	  REAL_VALUE_TO_TARGET_SINGLE (value, value_long);
	  fprintf (file, HOST_WIDE_INT_PRINT_HEX, value_long);
	}
      else
	{
	  char s[60];
	  real_to_decimal (s, CONST_DOUBLE_REAL_VALUE (op), sizeof (s), 0, 1);
	  fputs (s, file);
	}
    }

  else if (code == UNSPEC)
    {
      print_operand_address (file, op);
    }

  else if (letter == 'x' && GET_CODE (op) == CONST_INT)
    fprintf (file, HOST_WIDE_INT_PRINT_HEX, 0xffff & INTVAL (op));

  else if (letter == 'X' && GET_CODE (op) == CONST_INT)
    fprintf (file, HOST_WIDE_INT_PRINT_HEX, INTVAL (op));

  else if (letter == 'd' && GET_CODE (op) == CONST_INT)
    fprintf (file, HOST_WIDE_INT_PRINT_DEC, (INTVAL (op)));

  else if (letter == 'z' && GET_CODE (op) == CONST_INT && INTVAL (op) == 0)
    fputs (reg_names[GP_REG_FIRST], file);

  else if (letter == 's' && GET_CODE (op) == CONST_INT)
    if (INTVAL (op) < 0)
      fputs ("-1", file);
    else
      fputs ("0", file);

  else if (letter == 'd' || letter == 'x' || letter == 'X' || letter == 's')
    error ("PRINT_OPERAND: letter %c was found & insn was not CONST_INT",
	   letter);

  else if (letter == 'B')
    fputs (code == EQ ? "z" : "n", file);
  else if (letter == 'b')
    fputs (code == EQ ? "n" : "z", file);
  else if (letter == 'T')
    fputs (code == EQ ? "f" : "t", file);
  else if (letter == 't')
    fputs (code == EQ ? "t" : "f", file);

  else if (code == CONST && GET_CODE (XEXP (op, 0)) == REG)
    {
      print_operand (file, XEXP (op, 0), letter);
    }
  else if (letter == 'm')
    fprintf (file, HOST_WIDE_INT_PRINT_DEC, (1L << INTVAL (op)));
  else
    output_addr_const (file, op);
}

/* A C compound statement to output to stdio stream STREAM the
   assembler syntax for an instruction operand that is a memory
   reference whose address is ADDR.  ADDR is an RTL expression.

   Possible address classifications and output formats are,
   
   ADDRESS_REG                  "%0, r0"

   ADDRESS_REG with non-zero    "%0, <addr_const>"
   offset       

   ADDRESS_REG_INDEX            "rA, RB"    
                                (if rA is r0, rA and rB are swapped)

   ADDRESS_CONST_INT            "r0, <addr_const>"

   ADDRESS_SYMBOLIC             "rBase, <addr_const>"   
                                (rBase is a base register suitable for the 
				 symbol's type)
*/

void
print_operand_address (FILE * file, rtx addr)
{
  struct microblaze_address_info info;
  enum microblaze_address_type type;
  if (!microblaze_classify_address (&info, addr, GET_MODE (addr), 1))
    fatal_insn ("insn contains an invalid address !", addr);

  type = info.type;
  switch (info.type)
    {
    case ADDRESS_REG:
      fprintf (file, "%s,", reg_names[REGNO (info.regA)]);
      output_addr_const (file, info.offset);
      break;
    case ADDRESS_REG_INDEX:
      if (REGNO (info.regA) == 0)
	/* Make rB == r0 instead of rA == r0. This helps reduce read port 
           congestion */
	fprintf (file, "%s,%s", reg_names[REGNO (info.regB)],
		 reg_names[REGNO (info.regA)]);
      else if (REGNO (info.regB) != 0)
	/* This is a silly swap to help Dhrystone */
	fprintf (file, "%s,%s", reg_names[REGNO (info.regB)],
		 reg_names[REGNO (info.regA)]);
      break;
    case ADDRESS_CONST_INT:
      fprintf (file, "%s,", reg_names[REGNO (info.regA)]);
      output_addr_const (file, info.offset);
      break;
    case ADDRESS_SYMBOLIC:
    case ADDRESS_GOTOFF:
    case ADDRESS_PLT:
      if (info.regA)
	fprintf (file, "%s,", reg_names[REGNO (info.regA)]);
      output_addr_const (file, info.symbol);
      if (type == ADDRESS_GOTOFF)
	{
	  fputs ("@GOT", file);
	}
      else if (type == ADDRESS_PLT)
	{
	  fputs ("@PLT", file);
	}
      break;
    case ADDRESS_INVALID:
      fatal_insn ("invalid address", addr);
      break;
    }
}

/* Emit a new filename to a stream.  If this is MICROBLAZE ECOFF, watch out
   for .file's that start within a function.  If we are smuggling stabs, try to
   put out a MICROBLAZE ECOFF file and a stab.  */

void
microblaze_output_filename (FILE * stream, const char *name)
{
  static int first_time = 1;
  char ltext_label_name[100];

  if (first_time)
    {
      first_time = 0;
      SET_FILE_NUMBER ();
      current_function_file = name;
      fprintf (stream, "\t.file\t%d ", num_source_filenames);
      output_quoted_string (stream, name);
      putc ('\n', stream);
    }

  else if (write_symbols == DBX_DEBUG)
    {
      ASM_GENERATE_INTERNAL_LABEL (ltext_label_name, "Ltext", 0);
      fprintf (stream, "%s ", ASM_STABS_OP);
      output_quoted_string (stream, name);
      fprintf (stream, ",%d,0,0,%s\n", N_SOL, &ltext_label_name[1]);
    }

  else if (name != current_function_file
	   && strcmp (name, current_function_file) != 0)
    {
      SET_FILE_NUMBER ();
      current_function_file = name;
      fprintf (stream, "\t.file\t%d", num_source_filenames);
      output_quoted_string (stream, name);
    }
}

/* Emit a linenumber.  For encapsulated stabs, we need to put out a stab
   as well as a .loc, since it is possible that MICROBLAZE ECOFF might not be
   able to represent the location for inlines that come from a different
   file.  */

void
microblaze_output_lineno (FILE * stream, int line)
{
  if (write_symbols == DBX_DEBUG)
    {
      ++sym_lineno;
      fprintf (stream, "%sLM%d:\n\t%s %d,0,%d,%sLM%d",
	       LOCAL_LABEL_PREFIX, sym_lineno, ASM_STABN_OP, N_SLINE, line,
	       LOCAL_LABEL_PREFIX, sym_lineno);
      fputc ('-', stream);
      assemble_name (stream,
		     XSTR (XEXP (DECL_RTL (current_function_decl), 0), 0));
      fprintf (stream, "\n");
    }

  else
    {
      fprintf (stream, "\n\t%s.loc\t%d %d\n",
	       (ignore_line_number) ? "#" : "", num_source_filenames, line);

      LABEL_AFTER_LOC (stream);
    }
}

/* Output an element in the table of global constructors. */
void
microblaze_asm_constructor (rtx symbol ATTRIBUTE_UNUSED, int priority)
{

  const char *section = ".ctors";
  char buf[16];

  if (priority != DEFAULT_INIT_PRIORITY)
    {
      sprintf (buf, ".ctors.%.5u",
	       /* Invert the numbering so the linker puts us in the proper
	          order; constructors are run from right to left, and the
	          linker sorts in increasing order.  */
	       MAX_INIT_PRIORITY - priority);
      section = buf;
    }

  named_section_flags (section, SECTION_WRITE);
  fputs ("\t.word\t", asm_out_file);
  output_addr_const (asm_out_file, symbol);
  fputs ("\n", asm_out_file);
}

/* Output an element in the table of global destructors. */
void
microblaze_asm_destructor (rtx symbol, int priority)
{
  const char *section = ".dtors";
  char buf[16];

  if (priority != DEFAULT_INIT_PRIORITY)
    {
      sprintf (buf, ".dtors.%.5u",
	       /* Invert the numbering so the linker puts us in the proper
	          order; constructors are run from right to left, and the
	          linker sorts in increasing order.  */
	       MAX_INIT_PRIORITY - priority);
      section = buf;
    }

  named_section_flags (section, SECTION_WRITE);
  fputs ("\t.word\t", asm_out_file);
  output_addr_const (asm_out_file, symbol);
  fputs ("\n", asm_out_file);
}


/* A function to output to the stdio stream stream a label whose name is made 
   from the string prefix and the number labelno. */
void
microblaze_internal_label (FILE * STREAM,
			   const char *prefix, unsigned long labelno)
{
  fprintf (STREAM, "%s%s%ld:\n", LOCAL_LABEL_PREFIX, prefix, labelno);
}

/* Emit either a label, .comm, or .lcomm directive, and mark that the symbol
   is used, so that we don't emit an .extern for it in 
   microblaze_asm_file_end.  */

void
microblaze_declare_object (FILE * stream, const char *name,
			   const char *section, const char *fmt, int size)
{

  fputs (section, stream);	/* "", "\t.comm\t", or "\t.lcomm\t" */
  assemble_name (stream, name);
  fprintf (stream, fmt, size);	/* ":\n", ",%u\n", ",%u\n" */

  if (TARGET_GP_OPT)
    {
      tree name_tree = get_identifier (name);
      TREE_ASM_WRITTEN (name_tree) = 1;
    }
}

/* Output a double precision value to the assembler.  If both the
   host and target are IEEE, emit the values in hex.  */

void
microblaze_output_double (FILE * stream, REAL_VALUE_TYPE value)
{
  union
  {
    double d;
    REAL_VALUE_TYPE value;
  } val;
#ifdef REAL_VALUE_TO_TARGET_DOUBLE
  long value_long[2];
  val.value = value;
  REAL_VALUE_TO_TARGET_DOUBLE (value, value_long);

  fprintf (stream, "\t.word\t0x%08lx\t\t# %.20g\n\t.word\t0x%08lx\n",
	   value_long[0], val.d, value_long[1]);
#else
  val.value = value;
  fprintf (stream, "\t.double\t%.20g\n", val.d);
#endif
}

/* Output a single precision value to the assembler.  If both the
   host and target are IEEE, emit the values in hex.  */

void
microblaze_output_float (FILE * stream, REAL_VALUE_TYPE value)
{
  union
  {
    double d;
    REAL_VALUE_TYPE value;
  } val;
#ifdef REAL_VALUE_TO_TARGET_SINGLE
  long value_long;
  val.value = value;
  REAL_VALUE_TO_TARGET_SINGLE (value, value_long);

  fprintf (stream, "\t.word\t0x%08lx\t\t# %.12g (float)\n", value_long,
	   val.d);
#else
  val.value = value;
  fprintf (stream, "\t.float\t%.12g\n", val.d);
#endif
}

/* Return the bytes needed to compute the frame pointer from the current
   stack pointer.

   MicroBlaze stack frames look like:



             Before call		        After call
        +-----------------------+	+-----------------------+
   high |			|       |      			|
   mem. |  local variables,     |	|  local variables,	|
        |  callee saved and     |       |  callee saved and    	|
	|  temps     		|       |  temps     	        |
        +-----------------------+	+-----------------------+
        |  arguments for called	|       |  arguments for called |
	|  subroutines		|	|  subroutines  	|
        |  (optional)           |       |  (optional)           |
        +-----------------------+	+-----------------------+
	|  Link register 	|	|  Link register        |
    SP->|                       |       |                       |
	+-----------------------+       +-----------------------+
					|		        |
                                        |  local variables,     |
                                        |  callee saved and     |
                                        |  temps                |
					+-----------------------+
                                        |   MSR (optional if,   |
                                        |   interrupt handler)  |
					+-----------------------+
					|			|
                                        |  alloca allocations   |
        				|			|
					+-----------------------+
					|			|
                                        |  arguments for called |
                                        |  subroutines          |
                                        |  (optional)           |
        				|		        |
					+-----------------------+
                                        |  Link register        |
   low                           FP,SP->|                       |
   memory        			+-----------------------+

*/

HOST_WIDE_INT
compute_frame_size (HOST_WIDE_INT size)	/* # of var. bytes allocated */
{
  int regno;
  HOST_WIDE_INT total_size;	/* # bytes that the entire frame takes up */
  HOST_WIDE_INT var_size;	/* # bytes that local variables take up */
  HOST_WIDE_INT args_size;	/* # bytes that outgoing arguments take up */
  int link_debug_size;		/* # bytes for link register */
  HOST_WIDE_INT gp_reg_size;	/* # bytes needed to store calle-saved gp regs */
  long mask;			/* mask of saved gp registers */

  interrupt_handler =
    (microblaze_interrupt_function_p (current_function_decl));
  save_volatiles = (microblaze_save_volatiles (current_function_decl));

  gp_reg_size = 0;
  mask = 0;
  var_size = size;
  args_size = current_function_outgoing_args_size;

  if ((args_size == 0) && current_function_calls_alloca)
    args_size = NUM_OF_ARGS * UNITS_PER_WORD;

  total_size = var_size + args_size;

  if (flag_pic == 2)
    regs_ever_live[MB_ABI_PIC_ADDR_REGNUM] = 1;	/* force setting GOT */

  /* Calculate space needed for gp registers.  */
  for (regno = GP_REG_FIRST; regno <= GP_REG_LAST; regno++)
    {
      if (MUST_SAVE_REGISTER (regno))
	{

	  if (regno != MB_ABI_SUB_RETURN_ADDR_REGNUM)
	    /* Don't account for link register. It is accounted specially below */
	    gp_reg_size += GET_MODE_SIZE (gpr_mode);

	  mask |= (1L << (regno - GP_REG_FIRST));
	}
    }

  total_size += gp_reg_size;

  /* Add 4 bytes for MSR */
  if (interrupt_handler)
    total_size += 4;

  /* No space to be allocated for link register in leaf functions with no other
     stack requirements */
  if (total_size == 0 && current_function_is_leaf)
    link_debug_size = 0;
  else
    link_debug_size = UNITS_PER_WORD;

  total_size += link_debug_size;

  /* Save other computed information.  */
  current_frame_info.total_size = total_size;
  current_frame_info.var_size = var_size;
  current_frame_info.args_size = args_size;
  current_frame_info.gp_reg_size = gp_reg_size;
  current_frame_info.mask = mask;
  current_frame_info.initialized = reload_completed;
  current_frame_info.num_gp = gp_reg_size / UNITS_PER_WORD;
  current_frame_info.link_debug_size = link_debug_size;

  if (mask)
    /* Offset from which to callee-save GP regs */
    current_frame_info.gp_offset = (total_size - gp_reg_size);
  else
    current_frame_info.gp_offset = 0;

  /* Ok, we're done.  */
  return total_size;
}

/* Common code to emit the insns (or to write the instructions to a file)
   to save/restore registers.

   Other parts of the code assume that MICROBLAZE_TEMP1_REGNUM (aka large_reg)
   is not modified within save_restore_insns.  */

#define BITSET_P(VALUE,BIT) (((VALUE) & (1L << (BIT))) != 0)

/* Save or restore instructions based on whether this is the prologue or 
   epilogue.  prologue is 1 for the prologue */
static void
save_restore_insns (int prologue)
{
  rtx base_reg_rtx, reg_rtx, mem_rtx, /* msr_rtx, */ isr_reg_rtx =
    0, isr_mem_rtx = 0;
  rtx isr_msr_rtx = 0, insn;
  long mask = current_frame_info.mask;
  HOST_WIDE_INT base_offset, gp_offset;
  int regno;

  if (frame_pointer_needed
      && !BITSET_P (mask, HARD_FRAME_POINTER_REGNUM - GP_REG_FIRST))
    abort ();

  if (mask == 0)
    return;

  /* Save registers starting from high to low.  The debuggers prefer at least
     the return register be stored at func+4, and also it allows us not to
     need a nop in the epilog if at least one register is reloaded in
     addition to return address.  */

  /* Pick which pointer to use as a base register.  For small frames, just
     use the stack pointer.  Otherwise, use a temporary register.  Save 2
     cycles if the save area is near the end of a large frame, by reusing
     the constant created in the prologue/epilogue to adjust the stack
     frame.  */

  gp_offset = current_frame_info.gp_offset;

  if (gp_offset <= 0)
    error ("gp_offset (%ld) is less than or equal to zero", (long) gp_offset);

  base_reg_rtx = stack_pointer_rtx;
  base_offset = 0;

  /* For interrupt_handlers, need to save/restore the MSR */
  if (interrupt_handler)
    {
      isr_mem_rtx = gen_rtx_MEM (gpr_mode,
				 gen_rtx_PLUS (Pmode, base_reg_rtx,
					       GEN_INT (current_frame_info.
							gp_offset -
							UNITS_PER_WORD)));

      /* Do not optimize in flow analysis */
      MEM_VOLATILE_P (isr_mem_rtx) = 1;
      isr_reg_rtx = gen_rtx_REG (gpr_mode, MB_ABI_MSR_SAVE_REG);
      isr_msr_rtx = gen_rtx_REG (gpr_mode, ST_REG_FIRST);
    }

  if (interrupt_handler && !prologue)
    {
      emit_move_insn (isr_reg_rtx, isr_mem_rtx);
      emit_move_insn (isr_msr_rtx, isr_reg_rtx);
      /* Do not optimize in flow analysis */
      emit_insn (gen_rtx_USE (SImode, isr_reg_rtx));
      emit_insn (gen_rtx_USE (SImode, isr_msr_rtx));
    }

  for (regno = GP_REG_FIRST; regno <= GP_REG_LAST; regno++)
    {
      if (BITSET_P (mask, regno - GP_REG_FIRST))
	{
	  if (regno == MB_ABI_SUB_RETURN_ADDR_REGNUM)
	    /* Don't handle here. Already handled as the first register */
	    continue;

	  reg_rtx = gen_rtx_REG (gpr_mode, regno);
	  insn = gen_rtx_PLUS (Pmode, base_reg_rtx, GEN_INT (gp_offset));
	  mem_rtx = gen_rtx_MEM (gpr_mode, insn);
	  if (interrupt_handler || save_volatiles)
	    /* Do not optimize in flow analysis */
	    MEM_VOLATILE_P (mem_rtx) = 1;

	  if (prologue)
	    {
	      insn = emit_move_insn (mem_rtx, reg_rtx);
	      RTX_FRAME_RELATED_P (insn) = 1;
	    }
	  else
	    {
	      insn = emit_move_insn (reg_rtx, mem_rtx);
	    }

	  REG_NOTES (insn) =
	    gen_rtx_EXPR_LIST (REG_MAYBE_DEAD, const0_rtx, NULL_RTX);

	  gp_offset += GET_MODE_SIZE (gpr_mode);
	}
    }

  if (interrupt_handler && prologue)
    {
      emit_move_insn (isr_reg_rtx, isr_msr_rtx);
      emit_move_insn (isr_mem_rtx, isr_reg_rtx);

      /* Do not optimize in flow analysis */
      emit_insn (gen_rtx_USE (SImode, isr_reg_rtx));
      emit_insn (gen_rtx_USE (SImode, isr_msr_rtx));
    }

  /* Done saving and restoring */
}


/* Set up the stack and frame (if desired) for the function.  */
static void
microblaze_function_prologue (FILE * file, int size ATTRIBUTE_UNUSED)
{
#ifndef FUNCTION_NAME_ALREADY_DECLARED
  const char *fnname;
#endif
  long fsiz = current_frame_info.total_size;

  inside_function = 1;

#ifndef FUNCTION_NAME_ALREADY_DECLARED
  /* Get the function name the same way that toplev.c does before calling
     assemble_start_function.  This is needed so that the name used here
     exactly matches the name used in ASM_DECLARE_FUNCTION_NAME.  */
  fnname = XSTR (XEXP (DECL_RTL (current_function_decl), 0), 0);
  if (!flag_inhibit_size_directive)
    {
      fputs ("\t.ent\t", file);
      if (interrupt_handler && strcmp (INTERRUPT_HANDLER_NAME, fnname))
	fputs ("_interrupt_handler", file);
      else
	assemble_name (file, fnname);
      fputs ("\n", file);
      if (!interrupt_handler)
	ASM_OUTPUT_TYPE_DIRECTIVE (file, fnname, "function");
    }

  assemble_name (file, fnname);
  fputs (":\n", file);

  if (interrupt_handler && strcmp (INTERRUPT_HANDLER_NAME, fnname))
    fputs ("_interrupt_handler:\n", file);
#endif

  if (!flag_inhibit_size_directive)
    {
      /* .frame FRAMEREG, FRAMESIZE, RETREG */
      fprintf (file,
	       "\t.frame\t%s,%ld,%s\t\t# vars= %ld, regs= %d, args= %d\n",
	       (reg_names[(frame_pointer_needed)
			  ? HARD_FRAME_POINTER_REGNUM :
			  STACK_POINTER_REGNUM]), fsiz,
	       reg_names[MB_ABI_SUB_RETURN_ADDR_REGNUM + GP_REG_FIRST],
	       current_frame_info.var_size, current_frame_info.num_gp,
	       current_function_outgoing_args_size);
      fprintf (file, "\t.mask\t0x%08lx\n", current_frame_info.mask);
    }
}

/* Output extra assembler code at the end of a prologue */
void
microblaze_function_end_prologue (FILE * file)
{
  if (TARGET_STACK_CHECK)
    {
      fprintf (file, "\t# Stack Check Stub -- Start.\n\t");
      fprintf (file, "ori\tr18,r0,_stack_end\n\t");
      fprintf (file, "cmpu\tr18,r1,r18\n\t");
      fprintf (file, "bgei\tr18,_stack_overflow_exit\n\t");
      fprintf (file, "# Stack Check Stub -- End.\n");
    }
}

/* Expand the prologue into a bunch of separate insns.  */

void
microblaze_expand_prologue (void)
{
  int regno;
  HOST_WIDE_INT fsiz;
  const char *arg_name = 0;
  tree fndecl = current_function_decl;
  tree fntype = TREE_TYPE (fndecl);
  tree fnargs = DECL_ARGUMENTS (fndecl);
  rtx next_arg_reg;
  int i;
  tree next_arg;
  tree cur_arg;
  CUMULATIVE_ARGS args_so_far;
  rtx mem_rtx, reg_rtx;

  /* If struct value address is treated as the first argument, make it so.  */
  if (aggregate_value_p (DECL_RESULT (fndecl), fntype)
      && !current_function_returns_pcc_struct)
    {
      tree type = build_pointer_type (fntype);
      tree function_result_decl = build_decl (PARM_DECL, NULL_TREE, type);

      DECL_ARG_TYPE (function_result_decl) = type;
      TREE_CHAIN (function_result_decl) = fnargs;
      fnargs = function_result_decl;
    }

  /* Determine the last argument, and get its name.  */

  INIT_CUMULATIVE_ARGS (args_so_far, fntype, NULL_RTX, 0, 0);
  regno = GP_ARG_FIRST;

  for (cur_arg = fnargs; cur_arg != 0; cur_arg = next_arg)
    {
      tree passed_type = DECL_ARG_TYPE (cur_arg);
      enum machine_mode passed_mode = TYPE_MODE (passed_type);
      rtx entry_parm;

      if (TREE_ADDRESSABLE (passed_type))
	{
	  passed_type = build_pointer_type (passed_type);
	  passed_mode = Pmode;
	}

      entry_parm = FUNCTION_ARG (args_so_far, passed_mode, passed_type, 1);

      if (entry_parm)
	{
	  int words;

	  /* passed in a register, so will get homed automatically */
	  if (GET_MODE (entry_parm) == BLKmode)
	    words = (int_size_in_bytes (passed_type) + 3) / 4;
	  else
	    words = (GET_MODE_SIZE (GET_MODE (entry_parm)) + 3) / 4;

	  regno = REGNO (entry_parm) + words - 1;
	}
      else
	{
	  regno = GP_ARG_LAST + 1;
	  break;
	}

      FUNCTION_ARG_ADVANCE (args_so_far, passed_mode, passed_type, 1);

      next_arg = TREE_CHAIN (cur_arg);
      if (next_arg == 0)
	{
	  if (DECL_NAME (cur_arg))
	    arg_name = IDENTIFIER_POINTER (DECL_NAME (cur_arg));

	  break;
	}
    }

  /* In order to pass small structures by value in registers compatibly with
     the MicroBlaze compiler, we need to shift the value into the high part 
     of the register.  Function_arg has encoded a PARALLEL rtx, holding a 
     vector of adjustments to be made as the next_arg_reg variable, so we 
     split up the insns, and emit them separately.  */

  /* IN MicroBlaze shift has been modified to be a combination of adds
     and shifts in other directions, Hence we need to change the code
     a bit */

  next_arg_reg = FUNCTION_ARG (args_so_far, VOIDmode, void_type_node, 1);
  if (next_arg_reg != 0 && GET_CODE (next_arg_reg) == PARALLEL)
    {
      rtvec adjust = XVEC (next_arg_reg, 0);
      int num = GET_NUM_ELEM (adjust);

      for (i = 0; i < num; i++)
	{
	  rtx pattern = RTVEC_ELT (adjust, i);
	  emit_insn (pattern);
	}
    }

  fsiz = compute_frame_size (get_frame_size ());

  /* If this function is a varargs function, store any registers that
     would normally hold arguments ($5 - $10) on the stack.  */
  if (((TYPE_ARG_TYPES (fntype) != 0
	&& (TREE_VALUE (tree_last (TYPE_ARG_TYPES (fntype)))
	    != void_type_node))
       || (arg_name != 0
	   && ((arg_name[0] == '_'
		&& strcmp (arg_name, "__builtin_va_alist") == 0)
	       || (arg_name[0] == 'v'
		   && strcmp (arg_name, "va_alist") == 0)))))
    {
      int offset = (regno - GP_ARG_FIRST + 1) * UNITS_PER_WORD;
      rtx ptr = stack_pointer_rtx;

      /* If we are doing svr4-abi, sp has already been decremented by fsiz. */
      for (; regno <= GP_ARG_LAST; regno++)
	{
	  if (offset != 0)
	    ptr = gen_rtx_PLUS (Pmode, stack_pointer_rtx, GEN_INT (offset));
	  emit_move_insn (gen_rtx_MEM (gpr_mode, ptr),
			  gen_rtx_REG (gpr_mode, regno));

	  offset += GET_MODE_SIZE (gpr_mode);
	}

    }

  if (fsiz > 0)
    {
      rtx fsiz_rtx = GEN_INT (fsiz);

      rtx insn = NULL;
      insn = emit_insn (gen_subsi3 (stack_pointer_rtx, stack_pointer_rtx,
				    fsiz_rtx));
      if (insn)
	RTX_FRAME_RELATED_P (insn) = 1;

      /* Handle SUB_RETURN_ADDR_REGNUM specially at first */
      if (!current_function_is_leaf || interrupt_handler)
	{
	  mem_rtx = gen_rtx_MEM (gpr_mode,
				 gen_rtx_PLUS (Pmode, stack_pointer_rtx,
					       const0_rtx));

	  if (interrupt_handler)
	    /* Do not optimize in flow analysis */
	    MEM_VOLATILE_P (mem_rtx) = 1;

	  reg_rtx = gen_rtx_REG (gpr_mode, MB_ABI_SUB_RETURN_ADDR_REGNUM);
	  insn = emit_move_insn (mem_rtx, reg_rtx);
	  RTX_FRAME_RELATED_P (insn) = 1;
	}

      /* _save_ registers for prologue */
      save_restore_insns (1);

      if (frame_pointer_needed)
	{
	  rtx insn = 0;

	  insn = emit_insn (gen_movsi (hard_frame_pointer_rtx,
				       stack_pointer_rtx));

	  if (insn)
	    RTX_FRAME_RELATED_P (insn) = 1;
	}
    }

  if (flag_pic == 2 && regs_ever_live[MB_ABI_PIC_ADDR_REGNUM])
    {
      rtx insn;
      REGNO (pic_offset_table_rtx) = MB_ABI_PIC_ADDR_REGNUM;
      insn = emit_insn (gen_set_got (pic_offset_table_rtx));	/* setting GOT */
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_MAYBE_DEAD, const0_rtx, NULL);
    }

  /* If we are profiling, make sure no instructions are scheduled before
     the call to mcount.  */

  if (profile_flag)
    emit_insn (gen_blockage ());
}

/* Do necessary cleanup after a function to restore stack, frame, and regs. */

#define RA_MASK ((long) 0x80000000)	/* 1 << 31 */
#define PIC_OFFSET_TABLE_MASK (1 << (PIC_OFFSET_TABLE_REGNUM - GP_REG_FIRST))

void
microblaze_function_epilogue (FILE * file ATTRIBUTE_UNUSED,
			      HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  const char *fnname;

#ifndef FUNCTION_NAME_ALREADY_DECLARED
  /* Get the function name the same way that toplev.c does before calling
     assemble_start_function.  This is needed so that the name used here
     exactly matches the name used in ASM_DECLARE_FUNCTION_NAME.  */
  fnname = XSTR (XEXP (DECL_RTL (current_function_decl), 0), 0);

  if (!flag_inhibit_size_directive)
    {
      fputs ("\t.end\t", file);
      if (interrupt_handler)
	fputs ("_interrupt_handler", file);
      else
	assemble_name (file, fnname);
      fputs ("\n", file);
    }
#endif

  /* Reset state info for each function.  */
  inside_function = 0;
  ignore_line_number = 0;
  dslots_load_total = 0;
  dslots_jump_total = 0;
  dslots_load_filled = 0;
  dslots_jump_filled = 0;
  num_refs[0] = 0;
  num_refs[1] = 0;
  num_refs[2] = 0;
  microblaze_load_reg = 0;
  microblaze_load_reg2 = 0;
  current_frame_info = zero_frame_info;

  while (string_constants != NULL)
    {
      struct string_constant *next;

      next = string_constants->next;
      free (string_constants);
      string_constants = next;
    }

  /* Restore the output file if optimizing the GP (optimizing the GP causes
     the text to be diverted to a tempfile, so that data decls come before
     references to the data).  */
}

/* Expand the epilogue into a bunch of separate insns.  */

void
microblaze_expand_epilogue (void)
{
  HOST_WIDE_INT fsiz = current_frame_info.total_size;
  rtx fsiz_rtx = GEN_INT (fsiz);
  rtx reg_rtx;
  rtx mem_rtx;

  /* In case of interrupt handlers use addki instead of addi for changing the 
     stack pointer value */

  if (microblaze_can_use_return_insn ())
    {
      emit_jump_insn (gen_return_internal (gen_rtx_REG (Pmode,
							GP_REG_FIRST +
							MB_ABI_SUB_RETURN_ADDR_REGNUM)));
      return;
    }

  if (fsiz > 0)
    {
      /* Restore SUB_RETURN_ADDR_REGNUM at first. This is to prevent the 
         sequence of load-followed by a use (in rtsd) in every prologue. Saves 
         a load-use stall cycle  :)   This is also important to handle alloca. 
         (See comments for if (frame_pointer_needed) below */

      if (!current_function_is_leaf || interrupt_handler)
	{
	  mem_rtx =
	    gen_rtx_MEM (gpr_mode,
			 gen_rtx_PLUS (Pmode, stack_pointer_rtx, const0_rtx));
	  if (interrupt_handler)
	    /* Do not optimize in flow analysis */
	    MEM_VOLATILE_P (mem_rtx) = 1;
	  reg_rtx = gen_rtx_REG (gpr_mode, MB_ABI_SUB_RETURN_ADDR_REGNUM);
	  emit_move_insn (reg_rtx, mem_rtx);
	}

      /* It is important that this is done after we restore the return address 
         register (above).  When alloca is used, we want to restore the 
	 sub-routine return address only from the current stack top and not 
	 from the frame pointer (which we restore below). (frame_pointer + 0) 
	 might have been over-written since alloca allocates memory on the 
	 current stack */
      if (frame_pointer_needed)
	emit_insn (gen_movsi (stack_pointer_rtx, hard_frame_pointer_rtx));

      /* _restore_ registers for epilogue */
      save_restore_insns (0);
      emit_insn (gen_blockage ());
      emit_insn (gen_addsi3 (stack_pointer_rtx, stack_pointer_rtx, fsiz_rtx));
    }

  emit_jump_insn (gen_return_internal (gen_rtx_REG (Pmode, GP_REG_FIRST +
						    MB_ABI_SUB_RETURN_ADDR_REGNUM)));
}


/* Return nonzero if this function is known to have a null epilogue.
   This allows the optimizer to omit jumps to jumps if no stack
   was created.  */

int
microblaze_can_use_return_insn (void)
{
  if (!reload_completed)
    return 0;

  if (regs_ever_live[MB_ABI_SUB_RETURN_ADDR_REGNUM] || profile_flag)
    return 0;

  if (current_frame_info.initialized)
    return current_frame_info.total_size == 0;

  return compute_frame_size (get_frame_size ()) == 0;
}

/* This function returns the register class required for a secondary
   register when copying between one of the registers in CLASS, and X,
   using MODE.  If IN_P is nonzero, the copy is going from X to the
   register, otherwise the register is the source.  A return value of
   NO_REGS means that no secondary register is required.  */

enum reg_class
microblaze_secondary_reload_class (enum reg_class class,
				   enum machine_mode mode ATTRIBUTE_UNUSED,
				   rtx x, int in_p ATTRIBUTE_UNUSED)
{
  enum reg_class gr_regs = GR_REGS;
  int regno = -1;
  int gp_reg_p;

  if (GET_CODE (x) == SIGN_EXTEND)
    {
      int off = 0;

      x = XEXP (x, 0);

      /* We may be called with reg_renumber NULL from regclass.
         ??? This is probably a bug.  */
      if (reg_renumber)
	regno = true_regnum (x);
      else
	{
	  while (GET_CODE (x) == SUBREG)
	    x = SUBREG_REG (x);

	  if (GET_CODE (x) == REG)
	    regno = REGNO (x) + off;
	}
    }

  else if (GET_CODE (x) == REG || GET_CODE (x) == SUBREG)
    regno = true_regnum (x);

  gp_reg_p = GP_REG_P (regno);

  if (MD_REG_P (regno))
    {
      return class == gr_regs ? NO_REGS : gr_regs;
    }

  /* We can only copy a value to a condition code register from a
     floating point register, and even then we require a scratch
     floating point register.  We can only copy a value out of a
     condition code register into a general register.  */
  if (class == ST_REGS)
    {
      return GP_REG_P (regno) ? NO_REGS : GR_REGS;
    }
  if (ST_REG_P (regno))
    {
      return class == GR_REGS ? NO_REGS : GR_REGS;
    }

  return NO_REGS;
}

/* Get the base register for accessing a value from the memory or
   Symbol ref. Used for Microblaze Small Data Area Pointer Optimization */
int
get_base_reg (rtx x)
{
  tree decl;
  int base_reg = (flag_pic ? MB_ABI_PIC_ADDR_REGNUM : MB_ABI_BASE_REGNUM);

  if (TARGET_XLGP_OPT
      && GET_CODE (x) == SYMBOL_REF
      && SYMBOL_REF_SMALL_P (x) && (decl = SYMBOL_REF_DECL (x)) != NULL)
    {
      if (TREE_READONLY (decl))
	base_reg = MB_ABI_GPRO_REGNUM;
      else
	base_reg = MB_ABI_GPRW_REGNUM;
    }

  return base_reg;
}

/* Added to handle Xilinx interrupt handler for MicroBlaze */

int
microblaze_valid_machine_decl_attribute (tree decl,
					 tree attributes ATTRIBUTE_UNUSED,
					 tree attr, tree args)
{
  if (args != NULL_TREE)
    return 0;

  if (is_attribute_p ("interrupt_handler", attr) ||
      is_attribute_p ("save_volatiles", attr))
    {
      return TREE_CODE (decl) == FUNCTION_DECL;
    }

  return 0;
}

/* Return nonzero if FUNC is an interrupt function as specified
   by the "interrupt" attribute.  */

/* Xilinx
 * Eventually remove both the functions below 
 */
static int
microblaze_interrupt_function_p (tree func)
{
  tree a;

  if (TREE_CODE (func) != FUNCTION_DECL)
    return 0;

  a = lookup_attribute ("interrupt_handler", DECL_ATTRIBUTES (func));
  return a != NULL_TREE;
}

static int
microblaze_save_volatiles (tree func)
{
  tree a;

  if (TREE_CODE (func) != FUNCTION_DECL)
    return 0;

  a = lookup_attribute ("save_volatiles", DECL_ATTRIBUTES (func));
  return a != NULL_TREE;
}

int
microblaze_is_interrupt_handler (void)
{
  return interrupt_handler;
}

static void
microblaze_globalize_label (FILE * stream, const char *name)
{
  fputs ("\t.globl\t", stream);
  if (interrupt_handler && strcmp (name, INTERRUPT_HANDLER_NAME))
    {
      fputs (INTERRUPT_HANDLER_NAME, stream);
      fputs ("\n\t.globl\t", stream);
    }
  assemble_name (stream, name);
  fputs ("\n", stream);
}

/* Returns true if decl should be placed into a "small data" section. */
static bool
microblaze_elf_in_small_data_p (tree decl)
{
  if (!TARGET_XLGP_OPT)
    return false;

  /* We want to merge strings, so we never consider them small data.  */
  if (TREE_CODE (decl) == STRING_CST)
    return false;

  /* Functions are never in the small data area.  */
  if (TREE_CODE (decl) == FUNCTION_DECL)
    return false;

  if (TREE_CODE (decl) == VAR_DECL && DECL_SECTION_NAME (decl))
    {
      const char *section = TREE_STRING_POINTER (DECL_SECTION_NAME (decl));
      if (strcmp (section, ".sdata") == 0
	  || strcmp (section, ".sdata2") == 0
	  || strcmp (section, ".sbss") == 0
	  || strcmp (section, ".sbss2") == 0)
	return true;
    }

  HOST_WIDE_INT size = int_size_in_bytes (TREE_TYPE (decl));

  return (size > 0 && size <= microblaze_section_threshold);
}

/* A C statement or statements to switch to the appropriate section
   for output of RTX in mode MODE.  You can assume that RTX is some
   kind of constant in RTL.  The argument MODE is redundant except in
   the case of a `const_int' rtx.  Select the section by calling
   `text_section' or one of the alternatives for other sections.

   Do not define this macro if you put all constants in the read-only
   data section.  */

static void
microblaze_select_rtx_section (enum machine_mode mode, rtx x,
			       unsigned HOST_WIDE_INT align)
{
  default_elf_select_rtx_section (mode, x, align);
}

/* A C statement or statements to switch to the appropriate
   section for output of DECL.  DECL is either a `VAR_DECL' node
   or a constant of some sort.  RELOC indicates whether forming
   the initial value of DECL requires link-time relocations.  */

static void
microblaze_select_section (tree decl, int reloc, unsigned HOST_WIDE_INT align)
{
  switch (categorize_decl_for_section (decl, reloc, align))
    {
    case SECCAT_RODATA_MERGE_STR:
    case SECCAT_RODATA_MERGE_STR_INIT:
      /* MB binutils have various issues with mergeable string sections and
         relaxation/relocation. Currently, turning mergeable sections 
         into regular readonly sections. */
      readonly_data_section ();
      return;

    default:
      default_elf_select_section (decl, reloc, align);
      return;
    }
}

/*
  Encode info about sections into the RTL based on a symbol's declaration.
  The default definition of this hook, default_encode_section_info in 
  `varasm.c', sets a number of commonly-useful bits in SYMBOL_REF_FLAGS. */

static void
microblaze_encode_section_info (tree decl, rtx rtl, int first)
{
  default_encode_section_info (decl, rtl, first);
}

/* Determine of register must be saved/restored in call. */
static int
microblaze_must_save_register (int regno)
{
  if (pic_offset_table_rtx &&
      (regno == MB_ABI_PIC_ADDR_REGNUM) && regs_ever_live[regno])
    return 1;

  if (regs_ever_live[regno] && !call_used_regs[regno])
    return 1;

  if (frame_pointer_needed && (regno == HARD_FRAME_POINTER_REGNUM))
    return 1;

  if (!current_function_is_leaf)
    {
      if (regno == MB_ABI_SUB_RETURN_ADDR_REGNUM)
	return 1;
      if ((interrupt_handler || save_volatiles) &&
	  (regno >= 3 && regno <= 12))
	return 1;
    }

  if (interrupt_handler)
    {
      if ((regs_ever_live[regno]) ||
	  (regno == MB_ABI_MSR_SAVE_REG) ||
	  (regno == MB_ABI_ASM_TEMP_REGNUM) ||
	  (regno == MB_ABI_EXCEPTION_RETURN_ADDR_REGNUM))
	return 1;
    }

  if (save_volatiles)
    {
      if ((regs_ever_live[regno]) ||
	  (regno == MB_ABI_ASM_TEMP_REGNUM) ||
	  (regno == MB_ABI_EXCEPTION_RETURN_ADDR_REGNUM))
	return 1;
    }

  return 0;
}

/* Output .ascii string. */
void
output_ascii (FILE * file, const char *string, int len)
{
  register int i, cur_pos = 17;
  fprintf (file, "\t.ascii\t\"");
  for (i = 0; i < len; i++)
    {
      register int c = (unsigned char) string[i];

      switch (c)
	{
	case '\"':
	case '\\':
	  putc ('\\', file);
	  putc (c, file);
	  cur_pos += 2;
	  break;

	case '\n':
	  fputs ("\\n", file);
	  if (i + 1 < len
	      && (((c = string[i + 1]) >= '\040' && c <= '~') || c == '\t'))
	    cur_pos = 32767;	/* break right here */
	  else
	    cur_pos += 2;
	  break;

	case '\t':
	  fputs ("\\t", file);
	  cur_pos += 2;
	  break;

	case '\f':
	  fputs ("\\f", file);
	  cur_pos += 2;
	  break;

	case '\b':
	  fputs ("\\b", file);
	  cur_pos += 2;
	  break;

	case '\r':
	  fputs ("\\r", file);
	  cur_pos += 2;
	  break;

	default:
	  if (c >= ' ' && c < 0177)
	    {
	      putc (c, file);
	      cur_pos++;
	    }
	  else
	    {
	      fprintf (file, "\\%03o", c);
	      cur_pos += 4;
	    }
	}

      if (cur_pos > 72 && i + 1 < len)
	{
	  cur_pos = 17;
	  fprintf (file, "\"\n\t.ascii\t\"");
	}
    }
  fprintf (file, "\"\n");
}

static rtx
expand_pic_symbol_ref (enum machine_mode mode ATTRIBUTE_UNUSED, rtx op)
{
  rtx result;
  result = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, op), UNSPEC_GOTOFF);
  result = gen_rtx_CONST (Pmode, result);
  result = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, result);
  result = gen_const_mem (Pmode, result);
  return result;
}

bool
microblaze_expand_move (enum machine_mode mode, rtx operands[])
{
  /* If operands[1] is a constant address invalid for pic, then we need to
     handle it just like LEGITIMIZE_ADDRESS does.  */
  if (flag_pic)
    {
      if (GET_CODE (operands[0]) == MEM)
	{
	  rtx addr = XEXP (operands[0], 0);
	  if (GET_CODE (addr) == SYMBOL_REF)
	    {
	      if (reload_in_progress)
		{
		  regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
		}
	      rtx ptr_reg, result;

	      addr = expand_pic_symbol_ref (mode, addr);
	      ptr_reg = gen_reg_rtx (Pmode);
	      emit_move_insn (ptr_reg, addr);
	      result = gen_rtx_MEM (mode, ptr_reg);
	      operands[0] = result;
	    }
	}
      if (GET_CODE (operands[1]) == SYMBOL_REF
	  || GET_CODE (operands[1]) == LABEL_REF)
	{
	  rtx result;
	  if (reload_in_progress)
	    {
	      regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
	    }
	  result = expand_pic_symbol_ref (mode, operands[1]);
	  if (GET_CODE (operands[0]) != REG)
	    {
	      rtx ptr_reg = gen_reg_rtx (Pmode);
	      emit_move_insn (ptr_reg, result);
	      emit_move_insn (operands[0], ptr_reg);
	    }
	  else
	    {
	      emit_move_insn (operands[0], result);
	    }
	  return true;
	}
      else if (GET_CODE (operands[1]) == MEM &&
	       GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF)
	{
	  rtx result;
	  rtx ptr_reg;
	  if (reload_in_progress)
	    {
	      regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
	    }
	  result = expand_pic_symbol_ref (mode, XEXP (operands[1], 0));

	  ptr_reg = gen_reg_rtx (Pmode);

	  emit_move_insn (ptr_reg, result);
	  result = gen_rtx_MEM (mode, ptr_reg);
	  emit_move_insn (operands[0], result);
	  return true;
	}
      else if (pic_address_needs_scratch (operands[1]))
	{
	  rtx temp = force_reg (SImode, XEXP (XEXP (operands[1], 0), 0));
	  rtx temp2 = XEXP (XEXP (operands[1], 0), 1);

	  if (reload_in_progress)
	    regs_ever_live[PIC_OFFSET_TABLE_REGNUM] = 1;
	  emit_move_insn (operands[0], gen_rtx_PLUS (SImode, temp, temp2));
	  return true;
	}
    }

  if ((reload_in_progress | reload_completed) == 0
      && !register_operand (operands[0], SImode)
      && !register_operand (operands[1], SImode)
      && (GET_CODE (operands[1]) != CONST_INT || INTVAL (operands[1]) != 0))
    {
      rtx temp = force_reg (SImode, operands[1]);
      emit_move_insn (operands[0], temp);
      return true;
    }
  return false;
}

/* Expand shift operations. */
int
microblaze_expand_shift (enum shift_type dir ATTRIBUTE_UNUSED, rtx operands[])
{
  gcc_assert ((GET_CODE (operands[2]) == CONST_INT)
	      || (GET_CODE (operands[2]) == REG)
	      || (GET_CODE (operands[2]) == SUBREG));

  /* Shift by one -- generate pattern. */
  if ((GET_CODE (operands[2]) == CONST_INT) && (INTVAL (operands[2]) == 1))
    return 0;

  /* Have barrel shifter and shift > 1: use it. */
  if (TARGET_BARREL_SHIFT)
    return 0;

  gcc_assert ((GET_CODE (operands[0]) == REG)
	      || (GET_CODE (operands[0]) == SUBREG)
	      || (GET_CODE (operands[1]) == REG)
	      || (GET_CODE (operands[1]) == SUBREG));

  /* Shift by zero -- copy regs if necessary. */
  if ((GET_CODE (operands[2]) == CONST_INT) && (INTVAL (operands[2]) == 0))
    {
      if (REGNO (operands[0]) != REGNO (operands[1]))
	emit_insn (gen_movsi (operands[0], operands[1]));
      return 1;
    }

  return 0;
}

/* Return an RTX indicating where the return address to the
   calling function can be found.  */
rtx
microblaze_return_addr (int count, rtx frame ATTRIBUTE_UNUSED)
{
  if (count != 0)
    return NULL_RTX;

  return gen_rtx_PLUS (Pmode,
		       get_hard_reg_initial_val (Pmode,
						 MB_ABI_SUB_RETURN_ADDR_REGNUM),
		       GEN_INT (8));
}
