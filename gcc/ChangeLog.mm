2011-11-02  Richard Henderson  <rth@redhat.com>

	* config/i386/i386.md (UNSPEC_MOVA): New.
	* config/i386/sync.md (ATOMIC): New mode iterator.
	(atomic_load<ATOMIC>, atomic_store<ATOMIC>): New.
	(atomic_loaddi_fpu, atomic_storedi_fpu, movdi_via_fpu): New.

2011-11-02  Richard Henderson  <rth@redhat.com>

	* optabs.c (expand_atomic_load): Don't try compare-and-swap.

2011-11-02  Aldy Hernandez  <aldyh@redhat.com>

	* Merge from trunk at revision 180790.

2011-11-02  Andrew MacLeod  <amacleod@redhat.com>

	* builtins.c (fold_builtin_atomic_always_lock_free): Add object param
	and check alignment if present.
	(expand_builtin_atomic_always_lock_free): Expect 2nd argument.
	(fold_builtin_atomic_is_lock_free): Expect 2nd argument.
	(expand_builtin_atomic_is_lock_free): Expect 2nd argument.
	(fold_builtin_1): Remove LOCK_FREE builtins.
	(fold_builtin_2): Add LOCK_FREE builtins.
	* sync-builtins.def (BUILT_IN_ATOMIC_{IS,ALWAYS}_LOCK_FREE): Add param.
	* builtin-types.def (BT_FN_BOOL_SIZE): Remove.
	(BT_FN_BOOL_SIZE_CONST_VPTR): Add.

2011-11-02  Andrew MacLeod  <amacleod@redhat.com>

	* common.opt (finline-atomics): New. Flag to disable atomic inlining.
	* builtins.c (expand_builtin_atomic_exchange,
	expand_builtin_atomic_compare_exchange, expand_builtin_atomic_load,
	expand_builtin_atomic_store, expand_builtin_atomic_fetch_op,
	fold_builtin_atomic_is_lock_free, expand_builtin_atomic_is_lock_free):
	Don't attempt instruction expansion if inlining atomics is disabled.

2011-10-31  Richard Henderson  <rth@redhat.com>

	* optabs.c (expand_atomic_store): Use create_fixed_operand for
	atomic_store optab.  Don't try to fall back to sync_lock_release.

2011-10-31  Richard Henderson  <rth@redhat.com>

	* omp-low.c (expand_omp_atomic_fetch_op): Don't test individual
	fetch_op optabs, only test can_compare_and_swap_p.  Use __atomic
	builtins instead of __sync builtins.
	* optabs.h (get_atomic_op_for_code): Remove decl.
	(struct atomic_op_functions): Move to...
	* optabs.c: ... here.
	(get_atomic_op_for_code): Make static.

2011-10-27  Richard Henderson  <rth@redhat.com>

	* config/i386/i386.md (UNSPECV_CMPXCHG): Split into ...
	(UNSPECV_CMPXCHG_1, UNSPECV_CMPXCHG_2,
	UNSPECV_CMPXCHG_3, UNSPECV_CMPXCHG_4): New.
	* config/i386/sync.md (mem_thread_fence): Rename from memory_barrier.
	Handle the added memory model parameter.
	(mfence_nosse): Rename from memory_barrier_nosse.
	(sync_compare_and_swap<CASMODE>): Split into ...
	(atomic_compare_and_swap<SWI124>): this and ...
	(atomic_compare_and_swap<CASMODE>): this.  Handle the new parameters.
	(atomic_compare_and_swap_single<SWI>): Rename from
	sync_compare_and_swap<SWI>; rewrite to use split unspecs.
	(atomic_compare_and_swap_double<DCASMODE>): Rename from
	sync_double_compare_and_swap<DCASMODE>; rewrite to use split unspecs.
	(*atomic_compare_and_swap_doubledi_pic): Rename from
	sync_double_compare_and_swapdi_pic; rewrite to use split unspecs.
	(atomic_fetch_add<SWI>): Rename from sync_old_add<SWI>; add memory
	model parameter.
	(*atomic_fetch_add_cmp<SWI>): Similarly.
	(atomic_add<SWI>, atomic<any_logic><SWI>): Similarly.
	(atomic_sub<SWI>): Similarly.  Use x86_maybe_negate_const_int.
	(sync_lock_test_and_set<SWI>): Merge with ...
	(atomic_exchange<SWI>): ... this.

	* optabs.c (get_atomic_op_for_code): Split out from ...
	(maybe_emit_op): ... here.
	* optabs.h (struct atomic_op_functions): Move from optabs.c and
	rename from struct op_functions.
	(get_atomic_op_for_code): Declare.
	* omp-low.c (expand_omp_atomic_fetch_op): Use get_atomic_op_for_code
	and test both atomic and sync optabs.

	* genopinit.c (optabs): Add atomic_add, atomic_sub, atomic_and,
	atomic_nand, atomic_xor, atomic_or.

	* optabs.c (expand_val_compare_and_swap_1): Remove.
	(expand_val_compare_and_swap, expand_bool_compare_and_swap): Remove.
	(expand_atomic_compare_and_swap): Rename from
	expand_atomic_compare_exchange.  Rewrite to return both success and
	oldval return values; expand via both atomic and sync optabs.
	(expand_compare_and_swap_loop): Use expand_atomic_compare_and_swap.
	(expand_atomic_load): Likewise.
	* builtins.c (expand_builtin_compare_and_swap): Likewise.
	(expand_builtin_atomic_compare_exchange): Likewise.
	* expr.h, optabs.h: Update.

	* optabs.c (can_compare_and_swap_p): New.
	(expand_atomic_exchange, expand_atomic_fetch_op): Use it.
	* builtins.c (fold_builtin_atomic_always_lock_free): Likewise.
	* omp-low.c (expand_omp_atomic_pipeline): Likewise.
	* optabs.h: Update.

	* optabs.c (maybe_gen_insn): Handle 8 operands.

	* builtins.c (expand_builtin_mem_thread_fence): Fixup thinko in
	mem_thread_fence case.

2011-10-26  Andrew MacLeod  <amacleod@redhat.com>

	* builtins.c (expand_builtin_atomic_fetch_op): External calls for
	'op_fetch' builtins need to instead call 'fetch_op' externals and issue 
	correction code.
	(expand_builtin): Provide proper builtin name for external call and
	ignored flag to expand_builtin_atomic_fetch_op.

2011-10-25  Andrew MacLeod  <amacleod@redhat.com>

	* c-typeck.c (build_function_call_vec): Don't reprocess __atomic
	parameters.
	* doc/extend.texi: Document generic __atomic functions.
	* sync-builtin.def (BUILT_IN_ATOMIC_EXCHANGE_N, BUILT_IN_ATOMIC_LOAD_N,
	BUILT_IN_ATOMIC_COMPARE_EXCHANGE_N, BUILT_IN_ATOMIC_STORE_N): Add "_n"
	to the functions real name.
	(BUILT_IN_ATOMIC_EXCHANGE, BUILT_IN_ATOMIC_LOAD,
	BUILT_IN_ATOMIC_COMPARE_EXCHANGE, BUILT_IN_ATOMIC_STORE): New. Add
	generic atomic builtin functions.
	* builtin-types.def (BT_FN_VOID_SIZE_VPTR_PTR_INT,
	BT_FN_VOID_SIZE_CONST_VPTR_PTR_INT, BT_FN_VOID_SIZE_VPTR_PTR_PTR_INT,
	BT_FN_BOOL_SIZE_VPTR_PTR_PTR_INT_INT): New builtin types.

2011-10-20  Aldy Hernandez  <aldyh@redhat.com>

	* optabs.c (expand_atomic_load): Handle a NULL target.

2011-10-20  Andrew MacLeod  <amacleod@redhat.com>

	* optabs.c (add_op): Fix formatting from rename.
	(expand_atomic_fetch_op): Fix formatting from rename.
	* builtins.c (get_builtin_sync_mem): Reverse renamed get_builtin_atomic.
	(expand_builtin_sync_operation): Fix rename issues.
	(expand_builtin_compare_and_swap): Fix rename issues.
	(expand_builtin_sync_lock_test_and_set): Fix rename issues.
	(expand_builtin_sync_lock_release): Fix rename issues.
	(expand_builtin_atomic_exchange): Fix rename issues.
	(expand_builtin_atomic_load): Fix rename issues.
	(expand_builtin_atomic_store): Fix rename issues.
	(expand_builtin_atomic_fetch_op): Fix rename issues.
	(expand_builtin): Fix formatting from rename.

2011-10-20  Andrew MacLeod  <amacleod@redhat.com>

	* optabs.h (direct_optab_index): Replace DOI_atomic_compare_exchange
	with DOI_atomic_compare_and_swap.
	(direct_op): Add DOI_atomic_compare_and_swap.
	* genopinit.c: Set atomic_compare_and_swap_optab.
	* expr.h (expand_atomic_compare_exchange): Add parameter.
	* builtins.c (builtin_atomic_compare_exchange): Add weak parameter
	and verify it is a compile time constant.
	* optabs.c (expand_atomic_compare_exchange): Use atomic_compare_and_swap
	pattern if present, otherwise use __sync_val_compare_and_swap.
	* builtin-types.def (BT_FN_BOOL_VPTR_PTR_I{1,2,4,8,16}_BOOL_INT_INT):
	Add the bool parameter.
	* sync-builtins.def (BUILT_IN_ATOMIC_COMPARE_EXCHANGE_*): Use new
	prototype.

2011-10-17  Andrew MacLeod  <amacleod@redhat.com>

	* cppbuiltin.c: Rename __sync_mem to __atomic.
	* optabs.c: Rename __sync_mem to __atomic.
	* optabs.h: Rename __sync_mem to __atomic.
	* genopinit.c: Rename __sync_mem to __atomic.
	* builtins.c: Add __atomic as builtin prefix, rename all __sync_mem.
	* sync-builtins.def: Rename __sync_mem to __atomic.
	* expr.h: Rename __sync_mem to __atomic.
	* coretypes.h: Rename __sync_mem to __atomic.
	* doc/extend.texi: Rename __sync_mem to __atomic.
	* config/i387/sync.md: Rename __sync_mem to __atomic.

2011-10-13  Andrew MacLeod  <amacleod@redhat.com>

	* optabs.h (DOI_sync_mem_always_lock_free): New.
	(DOI_sync_mem_is_lock_free): New.
	(sync_mem_always_lock_free_optab, sync_mem_is_lock_free_optab): New.
	* builtins.c (fold_builtin_sync_mem_always_lock_free): New.
	(expand_builtin_sync_mem_always_lock_free): New.
	(fold_builtin_sync_mem_is_lock_free): New.
	(expand_builtin_sync_mem_is_lock_free): New.
	(expand_builtin): Handle BUILT_IN_SYNC_MEM_{is,always}_LOCK_FREE.
	(fold_builtin_1): Handle BUILT_IN_SYNC_MEM_{is,always}_LOCK_FREE.
	* sync-builtins.def: Add BUILT_IN_SYNC_MEM_{is,always}_LOCK_FREE.
	* builtin-types.def: Add BT_FN_BOOL_SIZE type.
	* doc/extend.texi: Add documentation.

2011-10-12  Aldy Hernandez  <aldyh@redhat.com>

	* Merge from trunk at revision 179855.

2011-10-04  Andrew MacLeod  <amacleod@redhat.com>

	* optabs.c (expand_sync_mem_load): Don't expand into a default load if
	the type is larger than a word. Try a compare_and_swap with 0.
	(expand_sync_mem_store): Return const0_rtx if a store is generated. If
	type is larger than a word try an exchange, then fail.
	* builtins.c (expand_builtin_sync_mem_store): Return a value.
	(expand_builtin): If no store generated, leave a function call.
	* expr.h (expand_sync_mem_store): Prototype returns value.

2011-09-16  Andrew MacLeod  <amacleod@redhat.com>

	* expr.h: Remove prototypes.
	* sync-builtins.def (BUILT_IN_SYNC_MEM_FLAG_TEST_AND_SET,
	BUILT_IN_SYNC_MEM_FLAG_CLEAR): Remove.
	(BUILT_IN_SYNC_MEM_NAND_FETCH): New builtin.
	(BUILT_IN_SYNC_MEM_FETCH_NAND): New builtin.
	* optabs.h (enum direct_optab_index): Remove DOI_sync_mem_flag_*.
	Add nand patterns as well as _sync_mem patterns with no result.
	(direct_op): Add new patterns to table.
	* genopinit (optabs[]): Add nand handlers.
	* optabs.c (expand_sync_lock_test_and_set): Remove.
	(expand_sync_mem_exchange): Incorporate sync_lock_test_and_set here.
	(expand_sync_mem_store): If storing const0_rtx, try using
	sync_lock_release.
	(expand_sync_operation, expand_sync_fetch_operation): Remove.
	(struct op_functions): New.  Table of different opcode expanders.
	(add_op, sub_op, xor_op, and_op, nand_op, or_op): New.  Objects
	initialized to expander data for each operation.
	(maybe_emit_op): New. Try to emit a specific op variation.
	(expand_sync_mem_fetch_op): Use maybe_emit_op to try to find a valid
	expansion using expanders in the op_function table.
	* builtins.c (expand_builtin_sync_lock_test_and_set): Expand into
	sync_mem_exchange instead.
	(expand_builtin_sync_lock_release): Expand into sync_mem_store of 0.
	(expand_builtin_sync_mem_flag_test_and_set): Remove.
	(expand_builtin_sync_mem_flag_clear): Remove.
	(expand_builtin_sync_operation): Remove ignore param.
	(expand_builtin_sync_operation): Always call expand_sync_mem_fetch_op
	instead of the old expanders.
	(expand_builtin): Remove cases for __SYNC_MEM_FLAG_*. Remove param 
	'ignore'.  Expand the new NAND builtins.
	* doc/extend.texi: Update documentation to match.

2011-09-08  Andrew MacLeod  <amacleod@redhat.com>

	* builtins.c (get_memmodel): If the memory model is not a compile time
	constant, default to SEQ_CST mode.
	* doc/extend.texi: Document behaviour.

2011-09-08  Aldy Hernandez  <aldyh@redhat.com>

	* Merge from trunk at revision 178608.

2011-08-23  Andrew MacLeod  <amacleod@redhat.com>

	* builtin-types.def (BT_CONST_VOLATILE_PTR): New primitive type.
	(BT_FN_I{1,2,4,8,16}_VPTR_INT): Change prototype to be const.
	* sync-builtins.def (BUILT_IN_SYNC_MEM_LOAD_*): Change to be const.

2011-08-13  Andrew MacLeod  <amacleod@redhat.com>

	* sync-builtins.def (BUILT_IN_SYNC_MEM_{ADD,SUB,AND,XOR,OR}_FETCH): New.
	* optabs.h (enum direct_optab_index): Add defines and access macros
	for DOI_sync_mem_{add,sub,and,xor,or}_fetch.
	* genopinit.c (optabs): Set handler for
	sync_mem_{add,sub,and,xor,or}_fetch.
	* expr.h (expand_sync_mem_fetch_op): Add parameter to prototype.
	* optabs.c (expand_sync_mem_fetch_op): Add bool parameter to handle 
	fetch_ops which return the result of the calculation.
	* builtins.c (expand_builtin_sync_mem_fetch_op): Add bool parameter
	to handle fetch_ops which return the result of the calculation.
	(expand_builtin): Add new _SYNC_MEM cases and add bool parameter to
	calls to expand_builtin_sync_mem_fetch_op.
	* doc/extend.texi: Document __sync_mem_add_fetch, __sync_mem_sub_fetch,
	__sync_mem_and_fetch, __sync_mem_xor_fetch, and __sync_mem_or_fetch.
	Fix some formatting.
	
2011-07-27  Andrew Macleod  <amacleod@redhat.com>

	* expr.h (expand_sync_mem_exchange): Change parameter order.
	(expand_sync_mem_*): New prototypes.
	(expand_builtin_sync_synchronize): Remove prototype.
	(expand_builtin_mem_thread_fence): Add prototype.
	* optabs.h (DOI_sync_mem_*): Add new optable enums.
	(sync_mem_*_optab): Add new #defines for table entries.
	* genopinit.c (const optabs[]): Add direct optab handlers.
	* optabs.c (expand_sync_mem_exchange): Change parameter order, and use
	builtin_mem_thread_fence.
	(expand_sync_mem_compare_exchange, expand_sync_mem_load,
	expand_sync_mem_store, expand_sync_mem_fetch_op): New. Expand
	__sync_mem functions which handle multiple integral types.
	* builtins.c (expand_expr_force_mode): New. Factor out common code for
	ensuring an integer argument is in the proper mode.
	(expand_builtin_sync_operation, expand_builtin_compare_and_swap,
	expand_builtin_sync_lock_test_and_set): Use maybe_convert_modes.
	(expand_builtin_sync_lock_release): Relocate higher in the file.
	(get_memmodel): Don't assume the memmodel is the 3rd argument.
	(expand_builtin_sync_mem_exchange): Change error check and use
	maybe_convert_modes.
	(expand_builtin_sync_mem_compare_exchange): New.
	(expand_builtin_sync_mem_load, expand_builtin_sync_mem_store): New.
	(expand_builtin_sync_mem_fetch_op): New.
	(expand_builtin_sync_mem_flag_test_and_set): New.
	(expand_builtin_sync_mem_flag_clear): New.
	(expand_builtin_mem_thread_fence): New.
	(expand_builtin_sync_mem_thread_fence): New.
	(expand_builtin_mem_signal_fence): New.
	(expand_builtin_sync_mem_signal_fence): New.
	(expand_builtin): Handle BUILT_IN_SYNC_MEM_* types.
	* builtin-types.def (BT_FN_I{1,2,4,8,16}_VPTR_INT): New builtin type.
	(BT_FN_VOID_VPTR_INT, BT_FN_BOOL_VPTR_INT): New builtin types.
	(BT_FN_VOID_VPTR_I{1,2,4,8,16}_INT: New builtin type.
	(BT_FN_BOOL_VPTR_PTR_I{1,2,4,8,16}_INT_INT): New builtin type.
	* sync-builtins.def (BUILT_IN_SYNC_MEM_*): New sync builtins.

2011-07-27  Andrew Macleod  <amacleod@redhat.com>

	* doc/extend.texi (__sync_mem_*) : Document all the atomic builtin
	functions which deal with memory models.

2011-06-23  Andrew Macleod  <amacleod@redhat.com>

	* doc/extend.texi (__sync_mem_load): Document.
	* optabs.c (expand_sync_mem_load): New.
	* optabs.h (enum direct_optab_index): Add DOI_sync_mem_load.
	(sync_mem_load_optab): Define.
	* genopinit.c: Add entry for sync_mem_load.
	* builtins.c (expand_builtin_sync_mem_load): New.
	(expand_builtin): Handle BUILT_IN_SYNC_MEM_LOAD_*
	* sync-bultins.def: Add entries for BUILT_IN_SYNC_MEM_LOAD_*.
	* builtin-types.def (BT_FN_I{1,2,4,8,16}_VPTR_INT): New.
	* expr.h (expand_sync_mem_load): Declare.
	* config/i386/sync.md (sync_mem_load<mode>): New pattern.

2011-06-21  Andrew Macleod  <amacleod@redhat.com>
	    Aldy Hernandez  <aldyh@redhat.com>

	* doc/extend.texi (__sync_mem_exchange): Document.
	* cppbuiltin.c (define__GNUC__): Define __SYNC_MEM*.
	* optabs.c (expand_sync_mem_exchange): New.
	* optabs.h (enum direct_optab_index): Add DOI_sync_mem_exchange entry.
	(sync_mem_exchange_optab): Define.
	* genopinit.c: Add entry for sync_mem_exchange.
	* builtins.c (get_memmodel): New.
	(expand_builtin_sync_mem_exchange): New.
	(expand_builtin_sync_synchronize): Remove static.
	(expand_builtin): Add cases for BUILT_IN_SYNC_MEM_EXCHANGE_*.
	* sync-builtins.def: Add entries for BUILT_IN_SYNC_MEM_EXCHANGE_*.
	* builtin-types.def (BT_FN_I{1,2,4,8,16}_VPTR_I{1,2,4,8,16}_INT): New.
	* expr.h (expand_sync_mem_exchange): Declare.
	(expand_builtin_sync_synchronize): Declare.
	* coretypes.h (enum memmodel): New.
	* Makefile.in (cppbuiltin.o) Add missing dependency on $(TREE_H)
	* config/i386/sync.md (sync_mem_exchange<mode>): New pattern.

2011-03-14  Aldy Hernandez  <aldyh@redhat.com>

	* params.h (ALLOW_LOAD_DATA_RACES): New.
	(ALLOW_STORE_DATA_RACES): New.
	(ALLOW_PACKED_LOAD_DATA_RACES): New.
	(ALLOW_PACKED_STORE_DATA_RACES): New.
	* params.def (PARAM_ALLOW_LOAD_DATA_RACES): New.
	(PARAM_ALLOW_STORE_DATA_RACES): New.
	(PARAM_ALLOW_PACKED_LOAD_DATA_RACES): New.
	(PARAM_ALLOW_PACKED_STORE_DATA_RACES): New.
	* doc/invoke.texi (Optimize Options): Document above parameters.
