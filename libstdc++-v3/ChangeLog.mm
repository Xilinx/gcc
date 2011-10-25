2011-10-25  Andrew MacLeod  <amacleod@redhat.com>

	* include/bits/atomic_2.h: Rename __atomic_exchange, __atomic_load,
	__atomic_store, and __atomic_compare_exchange to '_n' variant.

2011-10-20  Andrew MacLeod  <amacleod@redhat.com>

	* include/bits/atomic_2.h: Use __atomic_compare_exchange.

2011-10-17  Andrew MacLeod  <amacleod@redhat.com>

	* include/bits/atomic_2.h: Rename __sync_mem to __atomic.

2011-09-16  Andrew MacLeod  <amacleod@redhat.com>

	* include/bits/atomic_2.h (__atomic2): Use new
	__sync_mem routines.
