2011-08-13  Andrew MacLeod  <amacleod@redhat.com>

	* c-family/c-common.c (resolve_overloaded_builtin): Handle new
	__SYNC_MEM builtins.
	
2011-07-27  Andrew Macleod  <amacleod@redhat.com>

	* c-common.c (resolve_overloaded_builtin): Handle BUILT_IN_SYNC_MEM_* 
	types.

2011-06-23  Andrew Macleod  <amacleod@redhat.com>

	* c-common.c (resolve_overloaded_builtin): Add BUILT_IN_SYNC_MEM_LOAD_N.

2011-06-21  Andrew Macleod  <amacleod@redhat.com>
	    Aldy Hernandez  <aldyh@redhat.com>

	* c-common.c (BUILT_IN_SYNC_MEM_EXCHANGE_N): Add case.

