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
