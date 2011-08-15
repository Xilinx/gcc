/* This file is part of the Intel(R) Cilk(TM) Plus support
   This file contains Cilk Support files.
   
   Copyright (C) 2011  Free Software Foundation, Inc.
   Written by Balaji V. Iyer <balaji.v.iyer@intel.com>,
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

/* Frame status bits known to compiler */
#define CILK_FRAME_STOLEN    0x01
#define CILK_FRAME_UNSYNCHED 0x02
#define CILK_FRAME_DETACHED  0x04
#define CILK_FRAME_EXCEPTING 0x10
#define CILK_FRAME_SLOW      0xff

#define CILK_WORKER_TLS     (-1)
#define CILK_WORKER_PARM    (-2)
#define CILK_WORKER_INVALID (-3)

/* metadata NYI */

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

    
    
    /* Fields of frame (layout must match runtime) */
    CILK_TI_FRAME_EXTENSION,
    CILK_TI_BUCKET,
    CILK_TI_FRAME_FLAGS,
    CILK_TI_FRAME_PARENT,
    CILK_TI_FRAME_WORKER,
    CILK_TI_FRAME_EXCEPTION,
    CILK_TI_FRAME_CONTEXT,
    CILK_TI_STACK_POINTER,

    /* Fields of worker (layout must match runtime) */
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


/* Offset of fields in the Cilk frame descriptor.
   Index is same as for cilk_trees.  If the index
   does not correspond to a field of the Cilk frame
   the value is meaningless. */
extern HOST_WIDE_INT cilk_field_offsets[CILK_TI_MAX];

/* Called once to initialize the Cilk language-independent data structures. */
extern void cilk_init_common (void);

extern void cilk_expand_set_flags (tree, enum tree_code, int);
extern void cilk_expand_get_flags (tree, int);

/* Return the offset of the specified Cilk frame field within
   a block of size 64 x 2^BUCKET. */
extern HOST_WIDE_INT cilk_field_offset (int bucket, enum cilk_tree_index);

/* Return an address to FIELD in structure FRAME, marking the
   reference volatile if VOLATILE_P. */
extern tree cilk_frame_ref (tree frame, tree field, bool volatile_p);

extern void cilk_init_frame (tree frame);


/* #ifdef GCC_RTL_H */ #if 1
/* Call the Cilk runtime to allocate a variable sized object in
   the current frame. */
extern rtx allocate_cilk_dynamic (rtx target, rtx size);
#endif

/* Like build_stack_save_restore, but handles Cilk dynamic frame
   extensions instead of updating the stack pointer. */
extern void build_stack_save_restore_cilk (tree *save, tree *restore);

/* Return an expression describing a detach operation. */
extern tree cilk_detach (tree worker);

extern void cilk_init_builtins (void);

#if /* def GCC_RTL_H */ 1
extern rtx expand_cilk_frame (tree);

extern rtx expand_builtin_cilk_pop_frame (tree);
extern rtx expand_builtin_cilk_detach (tree);
extern rtx expand_builtin_cilk_stolen (tree);
extern rtx expand_builtin_cilk_synched (tree);
extern rtx expand_builtin_cilk_enter (tree);
#endif

extern bool cilk_valid_spawn (tree) __attribute__((weak));
/* bviyer: I commented these out */
extern void gimplify_cilk_spawn (tree *, gimple_seq *, gimple_seq *) __attribute__ ((weak));
extern void gimplify_cilk_sync (tree *, gimple_seq *) __attribute__ ((weak));
extern void gimplify_cilk_for (tree *, gimple_seq *, gimple_seq *) __attribute__ ((weak));


extern tree cilk_call_setjmp (tree);

extern tree make_cilk_frame (tree);
extern tree build_cilk_function_exit (tree, bool, bool);

extern tree build_cilk_sync (void);
extern tree create_detach_expr(tree frame);

#endif /* GCC_CILK_H */
