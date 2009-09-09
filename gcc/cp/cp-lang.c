/* Language-dependent hooks for C++.
   Copyright 2001, 2002, 2004, 2007, 2008, 2009 Free Software Foundation, Inc.
   Contributed by Alexandre Oliva  <aoliva@redhat.com>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "cp-tree.h"
#include "c-common.h"
#include "toplev.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "diagnostic.h"
#include "debug.h"
#include "cp-objcp-common.h"
#include "hashtab.h"

enum c_language_kind c_language = clk_cxx;
static void cp_init_ts (void);
static const char * cxx_dwarf_name (tree t, int verbosity);
static enum classify_record cp_classify_record (tree type);
static bool cp_user_conv_function_p (tree);

/* Lang hooks common to C++ and ObjC++ are declared in cp/cp-objcp-common.h;
   consequently, there should be very few hooks below.  */

#undef LANG_HOOKS_NAME
#define LANG_HOOKS_NAME "GNU C++"
#undef LANG_HOOKS_INIT
#define LANG_HOOKS_INIT cxx_init
#undef LANG_HOOKS_CLASSIFY_RECORD
#define LANG_HOOKS_CLASSIFY_RECORD cp_classify_record
#undef LANG_HOOKS_GENERIC_TYPE_P
#define LANG_HOOKS_GENERIC_TYPE_P class_tmpl_impl_spec_p

#undef LANG_HOOKS_GET_INNERMOST_GENERIC_PARMS
#define LANG_HOOKS_GET_INNERMOST_GENERIC_PARMS \
	get_primary_template_innermost_parameters
#undef LANG_HOOKS_GET_INNERMOST_GENERIC_ARGS
#define LANG_HOOKS_GET_INNERMOST_GENERIC_ARGS \
	get_template_innermost_arguments
#undef LANG_HOOKS_GET_ARGUMENT_PACK_ELEMS
#define LANG_HOOKS_GET_ARGUMENT_PACK_ELEMS \
	get_template_argument_pack_elems
#undef LANG_HOOKS_GENERIC_GENERIC_PARAMETER_DECL_P
#define LANG_HOOKS_GENERIC_GENERIC_PARAMETER_DECL_P \
	template_template_parameter_p

#undef LANG_HOOKS_DECL_PRINTABLE_NAME
#define LANG_HOOKS_DECL_PRINTABLE_NAME	cxx_printable_name
#undef LANG_HOOKS_DWARF_NAME
#define LANG_HOOKS_DWARF_NAME cxx_dwarf_name
#undef LANG_HOOKS_FOLD_OBJ_TYPE_REF
#define LANG_HOOKS_FOLD_OBJ_TYPE_REF cp_fold_obj_type_ref
#undef LANG_HOOKS_INIT_TS
#define LANG_HOOKS_INIT_TS cp_init_ts
#undef LANG_HOOKS_USER_CONV_FUNCTION
#define LANG_HOOKS_USER_CONV_FUNCTION cp_user_conv_function_p


/* LIPO support.  */
#undef LANG_HOOKS_ADD_BUILT_IN_DECL
#define LANG_HOOKS_ADD_BUILT_IN_DECL cp_add_built_in_decl
#undef LANG_HOOKS_SAVE_BUILT_IN_PRE
#define LANG_HOOKS_SAVE_BUILT_IN_PRE cp_save_built_in_decl_pre_parsing
#undef LANG_HOOKS_RESTORE_BUILT_IN_PRE
#define LANG_HOOKS_RESTORE_BUILT_IN_PRE cp_restore_built_in_decl_pre_parsing
#undef LANG_HOOKS_SAVE_BUILT_IN_POST
#define LANG_HOOKS_SAVE_BUILT_IN_POST cp_save_built_in_decl_post_parsing
#undef LANG_HOOKS_RESTORE_BUILT_IN_POST
#define LANG_HOOKS_RESTORE_BUILT_IN_POST cp_restore_built_in_decl_post_parsing
#undef LANG_HOOKS_CLEAR_NAME_BINDINGS
#define LANG_HOOKS_CLEAR_NAME_BINDINGS cp_clear_global_name_bindings
#undef LANG_HOOKS_HAS_GLOBAL_NAME
#define LANG_HOOKS_HAS_GLOBAL_NAME cp_is_non_sharable_global_decl
#undef LANG_HOOKS_GET_LANG_DECL_SIZE
#define LANG_HOOKS_GET_LANG_DECL_SIZE cp_get_lang_decl_size
#undef LANG_HOOKS_DUP_LANG_TYPE
#define LANG_HOOKS_DUP_LANG_TYPE cp_lipo_dup_lang_type
#undef LANG_HOOKS_COPY_LANG_TYPE
#define LANG_HOOKS_COPY_LANG_TYPE cp_lipo_copy_lang_type
#undef LANG_HOOKS_PROCESS_PENDING_DECLS
#define LANG_HOOKS_PROCESS_PENDING_DECLS cp_process_pending_declarations
#undef LANG_HOOKS_CLEAR_DEFFERED_FNS
#define LANG_HOOKS_CLEAR_DEFFERED_FNS cp_clear_deferred_fns
#undef LANG_HOOKS_IS_GENERATED_TYPE
#define LANG_HOOKS_IS_GENERATED_TYPE cp_is_compiler_generated_type
#undef LANG_HOOKS_CMP_LANG_TYPE
#define LANG_HOOKS_CMP_LANG_TYPE cp_cmp_lang_type

/* Each front end provides its own lang hook initializer.  */
struct lang_hooks lang_hooks = LANG_HOOKS_INITIALIZER;

/* Lang hook routines common to C++ and ObjC++ appear in cp/cp-objcp-common.c;
   there should be very few routines below.  */

/* The following function does something real, but only in Objective-C++.  */

tree
objcp_tsubst_copy_and_build (tree t ATTRIBUTE_UNUSED,
			     tree args ATTRIBUTE_UNUSED,
			     tsubst_flags_t complain ATTRIBUTE_UNUSED,
			     tree in_decl ATTRIBUTE_UNUSED,
			     bool function_p ATTRIBUTE_UNUSED)
{
  return NULL_TREE;
}


static void
cp_init_ts (void)
{
  tree_contains_struct[NAMESPACE_DECL][TS_DECL_NON_COMMON] = 1;
  tree_contains_struct[USING_DECL][TS_DECL_NON_COMMON] = 1;
  tree_contains_struct[TEMPLATE_DECL][TS_DECL_NON_COMMON] = 1;

  tree_contains_struct[NAMESPACE_DECL][TS_DECL_WITH_VIS] = 1;
  tree_contains_struct[USING_DECL][TS_DECL_WITH_VIS] = 1;
  tree_contains_struct[TEMPLATE_DECL][TS_DECL_WITH_VIS] = 1;

  tree_contains_struct[NAMESPACE_DECL][TS_DECL_WRTL] = 1;
  tree_contains_struct[USING_DECL][TS_DECL_WRTL] = 1;
  tree_contains_struct[TEMPLATE_DECL][TS_DECL_WRTL] = 1;

  tree_contains_struct[NAMESPACE_DECL][TS_DECL_COMMON] = 1;
  tree_contains_struct[USING_DECL][TS_DECL_COMMON] = 1;
  tree_contains_struct[TEMPLATE_DECL][TS_DECL_COMMON] = 1;

  tree_contains_struct[NAMESPACE_DECL][TS_DECL_MINIMAL] = 1;
  tree_contains_struct[USING_DECL][TS_DECL_MINIMAL] = 1;
  tree_contains_struct[TEMPLATE_DECL][TS_DECL_MINIMAL] = 1;

  init_shadowed_var_for_decl ();

}

static const char *
cxx_dwarf_name (tree t, int verbosity)
{
  gcc_assert (DECL_P (t));

  if (verbosity >= 2)
    return decl_as_string (t,
			   TFF_DECL_SPECIFIERS | TFF_UNQUALIFIED_NAME
			   | TFF_NO_OMIT_DEFAULT_TEMPLATE_ARGUMENTS);

  return cxx_printable_name (t, verbosity);
}

static enum classify_record
cp_classify_record (tree type)
{
  if (CLASSTYPE_DECLARED_CLASS (type))
    return RECORD_IS_CLASS;

  return RECORD_IS_STRUCT;
}

static bool
cp_user_conv_function_p (tree decl)
{
  return DECL_CONV_FN_P (decl);
}


void
finish_file (void)
{
}

#include "gtype-cp.h"
