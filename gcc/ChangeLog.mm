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
