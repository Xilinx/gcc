2011-03-29  Aldy Hernandez  <aldyh@redhat.com>

	* gcc.dg/memmodel/d2.c: Remove.
	* gcc.dg/memmodel/d3.c: Remove.
	* gcc.dg/memmodel/global-hoist.c: Remove.

2011-03-11  Aldy Hernandez  <aldyh@redhat.com>

	* lib/gcc-memmodel-gdb-test.exp: Return if no executable.
	* gcc.dg/memmodel/memmodel.h (memmodel_done): Add noinline
	attribute.
	* g++.dg/memmodel/bitfields.C: New.

2011-03-10  Aldy Hernandez  <aldyh@redhat.com>

	* lib/gcc-memmodel-gdb-test.exp: New.
	* gcc.dg/memmodel/speculative-store.c: New
	* gcc.dg/memmodel/subfields: New
	* gcc.dg/memmodel/guality.h: New
	* gcc.dg/memmodel/memmodel.h: New
	* gcc.dg/memmodel/global-hoist.c: New
	* gcc.dg/memmodel/d2.c: New
	* gcc.dg/memmodel/d3.c: New
	* gcc.dg/memmodel/memmodel.gdb: New.
	* gcc.dg/memmodel/memmodel.exp: New.
	* gcc.dg/memmodel/README: New.
	* g++.dg/memmodel/guality.h: New.
	* g++.dg/memmodel/memmodel.h: New.
	* g++.dg/memmodel/memmodel.gdb: New.
	* g++.dg/memmodel/memmodel.exp: New.
	* g++.dg/memmodel/atomics-1.C: New.

Local Variables:
mode: change-log
change-log-default-name: "ChangeLog.mm"
End:
