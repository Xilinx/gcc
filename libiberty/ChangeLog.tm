2008-11-05  Richard Henderson  <rth@redhat.com>

	* cp-demangle.c (d_dump): Handle DEMANGLE_COMPONENT_TRANSACTION_CLONE
	and DEMANGLE_COMPONENT_NONTRANSACTION_CLONE.
	(d_make_comp, d_print_comp): Likewise.
	(d_special_name): Generate them.
