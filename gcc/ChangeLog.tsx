2012-08-30  H.J. Lu  <hongjiu.lu@intel.com>

	Backport from mainline
	2012-05-03  Jakub Jelinek  <jakub@redhat.com>

	PR target/53194
	* config/i386/i386-c.c (ix86_target_macros_internal): Don't
	define __ATOMIC_HLE_* macros here.
	(ix86_target_macros): But here, using cpp_define_formatted.

2012-08-30  H.J. Lu  <hongjiu.lu@intel.com>

	Backport from mainline
	2012-05-03  Kirill Yukhin  <kirill.yukhin@intel.com>

	PR target/53201
	* config/i386/driver-i386.c (host_detect_local_cpu): Add space to
	"-mno-hle".

2012-08-30  H.J. Lu  <hongjiu.lu@intel.com>

	Backport from mainline
	2012-05-02  Kirill Yukhin  <kirill.yukhin@intel.com>
		    Andi Kleen <ak@linux.intel.com>

	* coretypes (MEMMODEL_MASK): New.
	* builtins.c (get_memmodel): Add val. Call target.memmodel_check
	and return new variable.
	(expand_builtin_atomic_exchange):  Mask memmodel values.
	(expand_builtin_atomic_compare_exchange): Ditto.
	(expand_builtin_atomic_load): Ditto.
	(expand_builtin_atomic_store): Ditto.
	(expand_builtin_atomic_clear): Ditto.
	* doc/extend.texi: Mention port-dependent memory model flags.
	* config/i386/cpuid.h (bit_HLE): New.
	* config/i386/driver-i386.c (host_detect_local_cpu): Detect
	HLE support.
	* config/i386/i386-protos.h (ix86_generate_hle_prefix): New.
	* config/i386/i386-c.c (ix86_target_macros_internal): Set
	HLE defines.
	(ix86_target_string)<-mhle>: New.
	(ix86_valid_target_attribute_inner_p)<OPT_mhle>: Ditto.
	* config/i386/i386.c (ix86_target_string)<OPTION_MASK_ISA_HLE>:
	New.
	(ix86_valid_target_attribute_inner_p)<OPT_mhle>: Ditto.
	(ix86_option_override_internal)<PTA_HLE>: New switch, set it
	enabled for generic, generic64 and core-avx2.
	(ix86_print_operand): Generate HLE lock prefixes.
	(ix86_memmodel_check): New.
	(TARGET_MEMMODEL_CHECK): Ditto.
	* config/i386/i386.h (OPTION_ISA_HLE): Ditto.
	(IX86_HLE_ACQUIRE): Ditto.
	(IX86_HLE_RELEASE): Ditto.
	* config/i386/i386.h (ix86_generate_hle_prefix): Ditto.
	* config/i386/i386.opt (mhle): Ditto.
	* config/i386/sync.md(atomic_compare_and_swap<mode>): Pass
	success model to instruction emitter.
	(atomic_fetch_add<mode>): Ditto.
	(atomic_exchange<mode>): Ditto.
	(atomic_add<mode>): Ditto.
	(atomic_sub<mode>): Ditto.
	(atomic_<code><mode>): Ditto.
	(*atomic_compare_and_swap_doubledi_pic): Ditto.
	(atomic_compare_and_swap_single<mode>): Define and use argument
	for success model.
	(atomic_compare_and_swap_double<mode>): Ditto.
	* configure.ac: Check if assembler support HLE prefixes.
	* configure: Regenerate.
	* config.in: Ditto.

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
