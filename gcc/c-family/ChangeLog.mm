2011-10-25  Andrew MacLeod  <amacleod@redhat.com>

	* c-common.c (get_atomic_generic_size): New.  Find size of generic
	atomic function parameters and do typechecking.
	(add_atomic_size_parameter): New.  Insert size into parameter list.
	(resolve_overloaded_atomic_exchange): Restructure __atomic_exchange to
	either __atomic_exchange_n or external library call.
	(resolve_overloaded_atomic_compare_exchange): Restructure 
	__atomic_compare_exchange to either _n variant or external library call.
	(resolve_overloaded_atomic_load): Restructure __atomic_load to either 
	__atomic_load_n or an external library call.
	(resolve_overloaded_atomic_store): Restructure __atomic_store to either
	__atomic_store_n or an external library call.
	(resolve_overloaded_builtin): Handle 4 generic __atomic calls.

2011-10-20  Andrew MacLeod  <amacleod@redhat.com>

	* c-common.c (resolve_overloaded_builtin): Don't try to process a 
	return value with an error mark.

2011-10-17  Andrew MacLeod  <amacleod@redhat.com>

	* c-common.c: Rename __sync_mem to __atomic.

2011-09-16  Andrew MacLeod  <amacleod@redhat.com>

	* c-common.c (sync_resolve_params, sync_resolve_return): Only tweak 
	parameters that are the same type size.
	(resolve_overloaded_builtin): Use new param for __sync_mem builtins.
	Support new NAND builtins.

2011-08-13  Andrew MacLeod  <amacleod@redhat.com>

	* c-common.c (resolve_overloaded_builtin): Handle new __SYNC_MEM 
	builtins.
	
2011-07-27  Andrew Macleod  <amacleod@redhat.com>

	* c-common.c (resolve_overloaded_builtin): Handle BUILT_IN_SYNC_MEM_* 
	types.

2011-06-23  Andrew Macleod  <amacleod@redhat.com>

	* c-common.c (resolve_overloaded_builtin): Add BUILT_IN_SYNC_MEM_LOAD_N.

2011-06-21  Andrew Macleod  <amacleod@redhat.com>
	    Aldy Hernandez  <aldyh@redhat.com>

	* c-common.c (BUILT_IN_SYNC_MEM_EXCHANGE_N): Add case.

