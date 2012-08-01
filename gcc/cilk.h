/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains Cilk Support files.
   Copyright (C) 2011, 2012  Free Software Foundation, Inc.
   Contributed by Balaji V. Iyer <balaji.v.iyer@intel.com>,
              Intel Corporation

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */


#ifndef GCC_CILK_H
#define GCC_CILK_H

#include "tree.h"

/* Frame status bits known to compiler.  */
#define CILK_FRAME_STOLEN    0x01
#define CILK_FRAME_UNSYNCHED 0x02
#define CILK_FRAME_DETACHED  0x04
#define CILK_FRAME_EXCEPTING 0x10
#define CILK_FRAME_SLOW      0xff

#define CILK_WORKER_TLS     (-1)
#define CILK_WORKER_PARM    (-2)
#define CILK_WORKER_INVALID (-3)

#define ZCA_MAJOR_VER_NUMBER (1)
#define ZCA_MINOR_VER_NUMBER (1)

enum cilk_tree_index
{
    CILK_TI_F_WORKER,        /* __cilkrts_get_worker() */
    CILK_TI_F_SYNC,          /* __cilkrts_sync() */
    CILK_TI_F_DETACH,        /* __cilkrts_detach() [builtin] */
    CILK_TI_F_OVERFLOW,      /* __cilkrts_ltq_overflow() */
    CILK_TI_HYPERCALL,
    CILK_TI_THROW,
    CILK_TI_RETHROW,
    CILK_TI_METADATA,
    CILK_TI_F_SYNCHED,       /* __cilkrts_synched() [builtin] */
    CILK_TI_F_STOLEN,        /* __cilkrts_was_stolen() [builtin] */
    CILK_TI_F_LOOP_32,       /* __cilkrts_cilk_for_32 */
    CILK_TI_F_LOOP_64,       /* __cilkrts_cilk_for_64 */
    CILK_TI_F_FREE_DYNAMIC,  /* __cilkrts_free_dynamic() */
    CILK_TI_F_ENTER,         /* __cilkrts_enter_frame() [builtin] */
    CILK_TI_F_LEAVE,         /* __cilkrts_leave_frame() [builtin] */
    CILK_TI_F_DEBUG,         /* __cilkrts_debug_s_frame_flags() */
    CILK_TI_F_POP,           /* __cilkrts_pop_frame() [builtin] */
    CILK_TI_F_ENTER_BEGIN,  
    CILK_TI_F_ENTER_H_BEGIN,
    CILK_TI_F_ENTER_END,
    CILK_TI_F_SPAWN_PREPARE,
    CILK_TI_F_SPAWN_OR_CONT,
    CILK_TI_F_DETACH_BEGIN,
    CILK_TI_F_DETACH_END,
    CILK_TI_F_SYNC_BEGIN,
    CILK_TI_F_SYNC_END,
    CILK_TI_F_LEAVE_BEGIN,
    CILK_TI_F_LEAVE_END,
    CILK_TI_F_RESUME,
    CILK_TI_F_LEAVE_STOLEN,
    CILK_TI_F_SYNC_ABANDON,
    CILK_TI_RUN,
    CILK_TI_FREE_DYNAMIC,
    CILK_TI_RUN_SERIAL_VOID,
    CILK_TI_RUN_SERIAL_ARG,

    CILK_TI_CILK_MAIN,

    CILK_TI_WORKER,
    CILK_TI_C_STACK,
    CILK_TI_REPLACEMENTS,
    
    /* Types */
    CILK_TI_FRAME,         /* __cilkrts_frame_t */
    CILK_TI_FRAME_PTR,     /* __cilkrts_frame_t * restrict */
    CILK_TI_WTYPE,         /* __cilkrts_worker_t */
    CILK_TI_METACALL_FRAME,
    CILK_TI_METACALL_FPTR,

    /* Fields of frame (layout must match runtime).  */
    CILK_TI_FRAME_EXTENSION,
    CILK_TI_BUCKET,
    CILK_TI_FRAME_FLAGS,
    CILK_TI_FRAME_PARENT,
    CILK_TI_FRAME_WORKER,
    CILK_TI_FRAME_EXCEPTION,
    CILK_TI_FRAME_CONTEXT,
    CILK_TI_STACK_POINTER,

    /* Fields of worker (layout must match runtime).  */
    CILK_TI_WORKER_TAIL,
    CILK_TI_WORKER_HEAD,
    CILK_TI_WORKER_EXC,
    CILK_TI_WORKER_PTAIL,
    CILK_TI_WORKER_LTQ_LIMIT,
    CILK_TI_WORKER_SELF,
    CILK_TI_WORKER_CUR,
    CILK_TI_WORKER_PARM,
    CILK_TI_WORKER_FREE,
    CILK_TI_WORKER_STACK,
    CILK_TI_WORKER_STATIC,
    CILK_TI_SAVEDREGS,
    CILK_TI_PENDING_FUNCTIONS,

    /* Layout of the fields in metacall struct.  */
    CILK_TI_METACALL_TOOL_FLAGS,
    CILK_TI_METACALL_CODE_FLAGS,
    CILK_TI_METACALL_DATA_FLAGS,

    /* Cilkscreen functions.  */
    CILKSCREEN_TI_F_METACALL,
    CILKSCREEN_TI_F_DIS_INSTR,
    CILKSCREEN_TI_F_EN_INSTR,
    CILKSCREEN_TI_F_DIS_CHECK,
    CILKSCREEN_TI_F_EN_CHECK,
    CILKSCREEN_TI_F_AQ_LOCK,
    CILKSCREEN_TI_F_REL_LOCK,
    NOTIFY_ZC_INTRINSIC,
    NOTIFY_INTRINSIC,
    CILK_TI_MAX
};

extern GTY(()) tree cilk_trees[(int) CILK_TI_MAX];

#define cilk_c_stack_decl               cilk_trees[CILK_TI_C_STACK]
#define cilk_worker_var_decl            cilk_trees[CILK_TI_WORKER]
#define cilk_worker_fndecl		cilk_trees[CILK_TI_F_WORKER]
#define cilk_sync_fndecl		cilk_trees[CILK_TI_F_SYNC]
#define cilk_synched_fndecl		cilk_trees[CILK_TI_F_SYNCHED]
#define cilk_stolen_fndecl		cilk_trees[CILK_TI_F_STOLEN]
#define cilk_detach_fndecl		cilk_trees[CILK_TI_F_DETACH]
#define cilk_overflow_fndecl		cilk_trees[CILK_TI_F_OVERFLOW]
#define cilk_free_dynamic_fndecl	cilk_trees[CILK_TI_F_FREE_DYNAMIC]
#define cilk_frame_type_decl		cilk_trees[CILK_TI_FRAME]
#define cilk_frame_ptr_type_decl	cilk_trees[CILK_TI_FRAME_PTR]
#define cilk_worker_type_decl		cilk_trees[CILK_TI_WTYPE]
#define cilk_metacall_frame_type_decl   cilk_trees[CILK_TI_METACALL_FRAME]
#define cilk_mcall_frame_ptr_type_decl  cilk_trees[CILK_TI_METACALL_FPTR]
#define cilk_for_32_fndecl		cilk_trees[CILK_TI_F_LOOP_32]
#define cilk_for_64_fndecl		cilk_trees[CILK_TI_F_LOOP_64]
#define cilk_enter_fndecl		cilk_trees[CILK_TI_F_ENTER]
#define cilk_leave_fndecl		cilk_trees[CILK_TI_F_LEAVE]
#define cilk_debug_fndecl               cilk_trees[CILK_TI_F_DEBUG]
#define cilk_pop_fndecl			cilk_trees[CILK_TI_F_POP]
#define cilk_run_fndecl			cilk_trees[CILK_TI_RUN]
#define cilk_loop_fndecl                cilk_trees[CILK_TI_F_LOOP_64]
#define cilk_metadata_fndecl            cilk_trees[CILK_TI_METADATA]
#define cilk_main_identifier_node       cilk_trees[CILK_TI_CILK_MAIN]
#define cilk_run_serial_arg_fndecl      cilk_trees[CILK_TI_RUN_SERIAL_ARG]
#define cilk_run_serial_void_fndecl     cilk_trees[CILK_TI_RUN_SERIAL_VOID]
#define cilk_rethrow_fndecl             cilk_trees[CILK_TI_RETHROW]
#define cilk_throw_fndecl               cilk_trees[CILK_TI_THROW]
#define cilk_replacement_functions      cilk_trees[CILK_TI_REPLACEMENTS]
#define cilk_enter_begin_fndecl         cilk_trees[CILK_TI_F_ENTER_BEGIN]
#define cilk_enter_h_begin_fndecl       cilk_trees[CILK_TI_F_ENTER_H_BEGIN]
#define cilk_enter_end_fndecl           cilk_trees[CILK_TI_F_ENTER_END]
#define cilk_spawn_prepare_fndecl       cilk_trees[CILK_TI_F_SPAWN_PREPARE]
#define cilk_spawn_or_cont_fndecl       cilk_trees[CILK_TI_F_SPAWN_OR_CONT]
#define cilk_detach_begin_fndecl        cilk_trees[CILK_TI_F_DETACH_BEGIN]
#define cilk_detach_end_fndecl          cilk_trees[CILK_TI_F_DETACH_END]
#define cilk_sync_begin_fndecl          cilk_trees[CILK_TI_F_SYNC_BEGIN]
#define cilk_sync_end_fndecl            cilk_trees[CILK_TI_F_SYNC_END]
#define cilk_leave_begin_fndecl         cilk_trees[CILK_TI_F_LEAVE_BEGIN]
#define cilk_leave_end_fndecl           cilk_trees[CILK_TI_F_LEAVE_END]
#define cilkscreen_metacall_fndecl      cilk_trees[CILKSCREEN_TI_F_METACALL]
#define cilk_resume_fndecl              cilk_trees[CILK_TI_F_RESUME]
#define cilk_leave_stolen_fndecl        cilk_trees[CILK_TI_F_LEAVE_STOLEN]
#define cilk_sync_abandon_fndecl        cilk_trees[CILK_TI_F_SYNC_ABANDON]
#define cilkscreen_disable_instr_fndecl cilk_trees[CILKSCREEN_TI_F_DIS_INSTR]
#define cilkscreen_enable_instr_fndecl  cilk_trees[CILKSCREEN_TI_F_EN_INSTR]
#define cilkscreen_disable_check_fndecl cilk_trees[CILKSCREEN_TI_F_DIS_CHECK]
#define cilkscreen_enable_check_fndecl  cilk_trees[CILKSCREEN_TI_F_EN_CHECK]
#define cilkscreen_aquire_lock_fndecl   cilk_trees[CILKSCREEN_TI_F_AQ_LOCK]
#define cilkscreen_release_lock_fndecl  cilk_trees[CILKSCREEN_TI_F_REL_LOCK]
#define notify_zc_intrinsic_fndecl      cilk_trees[NOTIFY_ZC_INTRINSIC]
#define notify_intrinsic_fndecl         cilk_trees[NOTIFY_INTRINSIC]

/* this is the max number of data we have have in elem-function arrays */
#define MAX_VARS 50

typedef struct zca_data_t
{
  rtx label; /* This is same as unsigned long ip.  */
  char *string;
  rtx reg_rtx;
  unsigned short dwarf_expr;
  struct zca_data_t *ptr_next;
} zca_data;

/* These are different mask options.  I put 12345 so that we can defferenciate 
   the value during debugging.  */
enum mask_options {
  USE_MASK = 12345,
  USE_NOMASK,
  USE_BOTH
};

/* This data structure will hold all the data from the vector attribute.  */
typedef struct
{
  char *proc_type;
  enum mask_options mask;
  int vectorlength[MAX_VARS];
  int no_vlengths;
  char *uniform_vars[MAX_VARS];
  int no_uvars;
  int uniform_location[MAX_VARS]; /* their location in parm list */
  char *linear_vars[MAX_VARS];
  int linear_steps[MAX_VARS];
  int linear_location[MAX_VARS]; /* their location in parm list */
  int no_lvars;
  int private_location[MAX_VARS]; /* parm not in uniform or linear list */
  int no_pvars;
  char *func_prefix;
  int total_no_args;
} elem_fn_info;

/* This data structure will hold all the arguments in the function.  */
typedef struct 
{
  tree induction_var;
  tree arguments;
  tree return_var;
} fn_vect_elements;

/* Offset of fields in the Cilk frame descriptor.
   Index is same as for cilk_trees.  If the index
   does not correspond to a field of the Cilk frame
   the value is meaningless.  */
extern HOST_WIDE_INT cilk_field_offsets[CILK_TI_MAX];

/* Called once to initialize the Cilk language-independent data structures. */
extern void cilk_init_common (void);

extern void cilk_expand_set_flags (tree, enum tree_code, int);
extern void cilk_expand_get_flags (tree, int);

/* Return the offset of the specified Cilk frame field within
   a block of size 64 x 2^BUCKET.  */
extern HOST_WIDE_INT cilk_field_offset (int bucket, enum cilk_tree_index);

/* Return an address to FIELD in structure FRAME, marking the
   reference volatile if VOLATILE_P.  */
extern tree cilk_frame_ref (tree frame, tree field, bool volatile_p);

extern void cilk_init_frame (tree frame);

/* Call the Cilk runtime to allocate a variable sized object in
   the current frame.  */
extern rtx allocate_cilk_dynamic (rtx target, rtx size);

/* Like build_stack_save_restore, but handles Cilk dynamic frame
   extensions instead of updating the stack pointer.  */
extern void build_stack_save_restore_cilk (tree *save, tree *restore);

/* Return an expression describing a detach operation.  */
extern tree cilk_detach (tree worker);

extern void cilk_init_builtins (void);


extern rtx expand_cilk_frame (tree);

extern rtx expand_builtin_cilk_pop_frame (tree);
extern rtx expand_builtin_cilk_detach (tree);
extern rtx expand_builtin_cilk_stolen (tree);
extern rtx expand_builtin_cilk_synched (tree);
extern rtx expand_builtin_cilk_enter (tree);
extern rtx expand_builtin_cilk_metadata (const char *, tree);
bool cilk_valid_spawn (tree);
void gimplify_cilk_spawn (tree *, gimple_seq *, gimple_seq *);
void gimplify_cilk_sync (tree *, gimple_seq *);
void gimplify_cilk_for (tree *, gimple_seq *, gimple_seq *);
extern tree cilk_call_setjmp (tree);
extern tree make_cilk_frame (tree);
extern tree build_cilk_function_exit (tree, bool, bool);
extern tree build_cilk_sync (void);
extern tree create_detach_expr (tree frame);
extern void cilk_output_metadata (void);
extern void cilk_remove_annotated_functions (rtx first);
extern bool cilk_annotated_function_p (char *); 	    
extern void debug_zca_data (void);
extern zca_data *get_zca_entry (int);
extern void insert_in_zca_table (zca_data);
extern bool is_elem_fn (tree);
extern tree find_elem_fn_name (tree, tree, tree);
extern void elem_fn_create_fn (tree);
extern char *find_processor_code (elem_fn_info *);
extern char *find_vlength_code (elem_fn_info *);
extern tree rename_elem_fn (tree, const char *);
extern char *find_suffix (elem_fn_info *, bool);
extern enum elem_fn_parm_type find_elem_fn_parm_type (gimple, tree, tree *);
extern tree find_elem_fn_name (tree, tree, tree);
elem_fn_info *extract_elem_fn_values (tree);
extern void array_notation_init_builtins (void);
extern bool is_cilk_must_expand_fn (enum built_in_function);
extern bool is_elem_fn_attribute_p (tree);
extern bool is_cilk_function_decl (tree, tree);
extern void clear_pragma_simd_list (void);

#endif /* GCC_CILK_H */
