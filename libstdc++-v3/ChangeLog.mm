2011-10-27  Benjamin Kosnik  <bkoz@redhat.com>
	    Andrew MacLeod  <amacleod@redhat.com>

	* include/Makefile.am (bits_headers): Remove atomic_0.h, atomic_2.h.
	* include/Makefile.in: Regenerate.
	* src/Makefile.am (sources): Rename atomic.cc to
	compatibility-atomic-c++0x.cc.
	* src/Makefile.in: Regenerate.
	* include/bits/atomic_0.h: Remove.
	* include/bits/atomic_2.h: Incorporate into...
	* include/bits/atomic_base.h: ...this.
	* include/std/atomic: Add generic atomic calls to basic atomic class.
	* src/atomic.cc: Move...
	* src/compatibility-atomic-c++0x.cc: ...here.
	* src/compatibility-c++0x.cc: Tweak.
	* testsuite/29_atomics/atomic/cons/user_pod.cc: Fix.
	* testsuite/29_atomics/atomic/requirements/explicit_instantiation/1.cc:
	  Same.
	* testsuite/29_atomics/headers/atomic/macros.cc: Same.

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
