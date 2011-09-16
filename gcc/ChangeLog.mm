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
