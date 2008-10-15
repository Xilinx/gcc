2008-10-15  Richard Henderson  <rth@redhat.com>

	* builtin-attrs.def (ATTR_RETURNS_TWICE): New.
	(ATTR_RETURNSTWICE_NOTHROW_LIST): New.
	* builtin-types.def: Remove many TM special types.
	* c-common.c (struct c_common_resword): Add __tm_atomic.
	* c-common.h (RID_TM_ATOMIC): New.
	* c-parser.c (c_parser_tm_abort): New.
	(c_parser_omp_construct): Remove PRAGMA_GTM_ATOMIC.
	* c-pragma.c (handle_pragma_optimize): Remove tm atomic pragma.
	* c-pragma.h (PRAGMA_GTM_ATOMIC): Remove.
	(enum pragma_gtm_clause): Remove.
	(c_finish_tm_abort): Remove.
	(prepare_tm_clone): Match Intel's transaction suffix.  Remove	
	code to add an explicit handle.
	* defaults.h (TINYSTM_VERSION_0_9_5, GTM_EXPL_HANDLE,
	GTM_IMPL_HANDLE, TANGER): Remove.
	* gimple-low.c (lower_tm_atomic): Add label after the tm_atomic.
	(lower_stmt): Remove GIMPLE_GTM_RETURN, GIMPLE_GTM_ABORT.
	* gimple.c (gss_for_code): Likewise.
	(gimple_size): Add GSS_TM_ATOMIC.
	(gimple_build_tm_atomic): Add label.
	* gimple-pretty-print.c (dump_gimple_gtm_other): Remove.
	* gimple.def (GIMPLE_GTM_RETURN, GIMPLE_GTM_ABORT): Remove.
	* gimple.h (struct gimple_statement_tm_atomic): New.
	(gimple_tm_atomic_label, gimple_tm_atomic_set_label): New.
	(is_gimple_gtm): Remove.
	* gimplify.c (gimplify_tm_atomic): Wrap the body in EH stuff.
	* gsstruct.def (GSS_TM_ATOMIC): New.
	* gtm-builtins.def: Remove conditional code, simplify interface.
	* gtm-low.c (root_gtm_region, dump_gtm_region, debug_gtm_region,
	debug_all_gtm_regions, new_gtm_region, free_gtm_region_1,
	free_gtm_regions, build_gtm_regions_1, build_gtm_regions,
	remove_gtm_stmts): Remove.
	* tree-cfg.c (make_edges) [GIMPLE_TM_ATOMIC]: Make the edge to
	the out label.
	* tree-flow.h (struct gtm_region): Remove.
	* tree.def (GTM_ABORT): Remove.

	* all files: s/GTM/TM/, s/gtm/tm/.

2008-10-15  Richard Henderson  <rth@redhat.com>

	* Makefile.in (gtm-low.o): Don't depend on gtm-defines.h.
	* builtin-types.def: Don't include it.
	* c-common.c (handle_gtm_unknown_attribute): Stub out
	setting DECL_IS_GTM_UNKNOWN.
	* c-decl.c (merge_decls): Don't propogate it.
	* c-parser.c (c_parser_gtm): Remove decl.
	* c-typeck.c (c_finish_gtm_txn): Don't special-case for OpenMP;
	make that the only case.
	* cgraphbuild.c (prepare_gtm_clone): Use MAIN_NAME_P.  Use concat
	for resetting the assembler name.
	* defaults.h (TINYSTM_VERSION_0_9_5): Set.
	* gimple-low.c (struct lower_data): Add in_transaction.
	(mark_gtm_save_vars): Remove.
	(lower_gtm_directive): Simplify.
	(record_vars_into_tm): Split out from record_vars_into.  When
	inside a transaction, set DECL_IS_GTM_PURE_VAR.
	(lower_gimple_bind): Call it.
	* gimple-pretty-print.c (dump_gimple_gtm_txn): New.
	(dump_gimple_gtm_other): New.
	(dump_gimple_stmt): Call them.
	* gimple.c (gcc_for_code): Add GTM codes.
	(gimple_size): Restructure based on GSS codes.
	(gimple_build_gtm_txn): New.
	(walk_gimple_stmt): Add GTM codes.
	(gimple_rhs_class_table): Make GTM_LOAD/STORE be GIMPLE_SINGLE_RHS.
	* gimple.def (GIMPLE_GTM_TXN, GIMPLE_GTM_RETURN, GIMPLE_GTM_ABORT): New.
	* gimple.h (struct gimple_statement_seq): Rename from 
	gimple_statement_omp.  Update all users.
	(is_gimple_gtm): New.
	* gtm-low.c (new_gtm_region): Remove type code.
	(build_gtm_regions_1): Gimplify.
	(build_gtm_regions): Don't do dominance here.
	(remove_gtm_stmts): Gimpify.
	(collapse_gtm_regions, query_STM_for_flat_nesting): Remove.
	(set_gtm_pure_var, is_gtm_pure_var): Remove.
	(requires_barrier): Merge requires_read_barrier and
	requires_write_barrier; rewrite to match memory variables only.
	(get_real_stm_decl, get_uint_stm_decl, insert_temporary,
	may_repair_rhs, compensate_for_taking_the_address, 
	compose_stm_store_call, compose_stm_load_call, 
	insert_rhs_stm_call, replace_lhs, maybe_replace_rhs_stmt,
	replace_txn_mod_stmt, build_txn_call_expr, replace_txn_stmt,
	setup_recover_bb, setup_begin_bb, expand_gtm_abort, 
	maybe_insert_stm_new, insert_stm_init_thread, 
	maybe_insert_stm_init_thread, insert_stm_exit_thread,
	maybe_insert_stm_exit_thread, expand_gtm_txn_marker,
	expand_gtm_return, insert_stm_init, insert_stm_exit,
	init_label_table, record_bb_into_table, check_and_mark_edges,
	instrument_edges, instrument_return_expr, expand_gtm,
	annotate_gtm_function_body): Remove.
	(maybe_transactify_assign, maybe_transactify_call): New.
	(transactify_stmt): New.
	(checkpoint_live_in_variables, checkpoint_gtm_txn): If 0.
	* omp-low.c (lower_gtm_txn): Remove.
	(diagnose_sb_1): Don't call it.
	* passes.c (pass_checkpoint_gtm): Disable.
	* tree-flow.h (NUM_BB_TXN): Remove.
	(struct gtm_region): Remove txn_bbs, type.
	* tree-pretty-print.c (dump_generic_node): Handle GTM codes.
	* tree.def (GTM_LOAD, GTM_STORE): Add.
	(GTM_RETURN): Remove.
	* tree.h (GTM_DIRECTIVE_P): Remove.
	(DECL_IS_GTM_UNKNOWN): Disable.

2008-10-10  Richard Henderson  <rth@redhat.com>

	Original work from Martin Schindewolf <schindew@ira.uka.de> et al.
	- Imported their 4.3 based patch into this 4.4 branch.
	- Raw conflicts resolved, but not yet converted to gimple tuples.
