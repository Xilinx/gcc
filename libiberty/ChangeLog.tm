2011-03-10  Richard Henderson  <rth@redhat.com>

	PR 47952
	* cp-demangle.c (cplus_demangle_fill_ctor): Accept
	gnu_v3_object_ctor_group.
	(cplus_demangle_fill_dtor): Accept gnu_v3_object_dtor_group.
	(d_ctor_dtor_name): Recognize gnu_v3_object_ctor_group
	and gnu_v3_object_dtor_group.

2008-11-05  Richard Henderson  <rth@redhat.com>

	* cp-demangle.c (d_dump): Handle DEMANGLE_COMPONENT_TRANSACTION_CLONE
	and DEMANGLE_COMPONENT_NONTRANSACTION_CLONE.
	(d_make_comp, d_print_comp): Likewise.
	(d_special_name): Generate them.
