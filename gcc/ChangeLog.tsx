2012-08-30  H.J. Lu  <hongjiu.lu@intel.com>

	Backport from mainline
	2012-02-12  Kirill Yukhin  <kirill.yukhin@intel.com>

	* doc/invoke.texi: Document -mrtm option.
	* common/config/i386/i386-common.c (OPTION_MASK_ISA_RTM_SET):
	New.
	(OPTION_MASK_ISA_RTM_UNSET): Ditto.
	(ix86_handle_option): Handle OPT_mrtm.
	* config.gcc (i[34567]86-*-*): Add rtmintrin.h and
	xtestintrin.h.
	(x86_64-*-*): Ditto.
	* i386-builtin-types.def (INT_FTYPE_VOID): New.
	* config/i386/i386-c.c (ix86_target_macros_internal): Define
	__RTM__ if needed.
	(ix86_target_string): Define -mrtm option.
	(PTA_RTM): New.
	(ix86_option_override_internal): Extend "corei7-avx" with
	RTM option. Handle new option.
	(ix86_valid_target_attribute_inner_p): Add OPT_mrtm.
	(ix86_builtins): Add IX86_BUILTIN_XBEGIN, IX86_BUILTIN_XEND,
	IX86_BUILTIN_XTEST.
	(bdesc_special_args): Ditto.
	(ix86_init_mmx_sse_builtins): Add IX86_BUILTIN_XABORT.
	(ix86_expand_special_args_builtin): Handle new built-in type.
	(ix86_expand_builtin): Handle XABORT instruction.
	* config/i386/i386.h (TARGET_RTM): New.
	* config/i386/i386.md (UNSPECV_XBEGIN): New.
	(UNSPECV_XEND): Ditto.
	(UNSPECV_XABORT): Ditto.
	(UNSPECV_XTEST): Ditto.
	(xbegin): Ditto.
	(xbegin_1): Ditto.
	(xend): Ditto.
	(xabort): Ditto
	(xtest): Ditto.
	(xtest_1): Ditto.
	* config/i386/i386.opt (mrtm): New.
	* config/i386/immintrin.h: Include rtmintrin.h and
	xtestintrin.h.
	* config/i386/rtmintrin.h: New header.
	* config/i386/xtestintrin.h: Ditto.
