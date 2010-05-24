#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "coretypes.h"
#include "opts.h"
#include "tree.h"
#include "gimple.h"
#include "ggc.h"
#include "toplev.h"
#include "debug.h"
#include "options.h"
#include "flags.h"
#include "convert.h"
#include "diagnostic.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "target.h"

struct GTY(()) lang_type
{
  char dummy;
};

struct GTY(()) lang_decl
{
  char dummy;
};

struct GTY(()) lang_identifier
{
  struct tree_identifier common;
};

union GTY((desc ("TREE_CODE (&%h.generic) == IDENTIFIER_NODE"),
	   chain_next ("(union lang_tree_node *) TREE_CHAIN (&%h.generic)")))
lang_tree_node
{
  union tree_node GTY((tag ("0"),
		       desc ("tree_node_structure (&%h)"))) generic;
  struct lang_identifier GTY((tag ("1"))) identifier;
};

/* We don't use language_function.  */

struct GTY(()) language_function
{
  int dummy;
};

/* Language hooks.  */

static
bool gpy_langhook_init( void )
{
  return true;
}

/* Initialize before parsing options.  */

static unsigned int
gpy_langhook_init_options (unsigned int argc ATTRIBUTE_UNUSED,
			  const char** argv ATTRIBUTE_UNUSED)
{
  return 0;
}

/* Handle Go specific options.  Return 0 if we didn't do anything.  */

static int
gpy_langhook_handle_option (size_t scode, const char *arg,
			   int value ATTRIBUTE_UNUSED)
{
  return 1;
}

/* Run after parsing options.  */

static bool
gpy_langhook_post_options( const char **pfilename ATTRIBUTE_UNUSED )
{
  return true;
}

static void
gpy_langhook_parse_file( int set_yy_debug ATTRIBUTE_UNUSED )
{
  return;
}

static tree
gpy_langhook_type_for_size (unsigned int bits, int unsignedp)
{
  return 0;
}

static tree
gpy_langhook_type_for_mode (enum machine_mode mode, int unsignedp)
{
  return 0;
}

/* Record a builtin function.  We just ignore builtin functions.  */

static tree
gpy_langhook_builtin_function( tree decl )
{
  return decl;
}

static int
gpy_langhook_global_bindings_p( void )
{
  return 0;
}

static tree
gpy_langhook_pushdecl( tree decl ATTRIBUTE_UNUSED )
{
  return NULL;
}

static
tree gpy_langhook_getdecls( void )
{
  return NULL;
}

/* Write out globals.  */

static void
gpy_langhook_write_globals( void )
{
  return;
}

static int
gpy_langhook_gimplify_expr( tree *expr_p, gimple_seq *pre_p, gimple_seq *post_p )
{
  return 0;
}

static GTY(()) tree gpy_gc_root;

void gpy_preserve_from_gc( tree t )
{
  return;
}

#undef LANG_HOOKS_NAME
#undef LANG_HOOKS_INIT
#undef LANG_HOOKS_INIT_OPTIONS
#undef LANG_HOOKS_HANDLE_OPTION
#undef LANG_HOOKS_POST_OPTIONS
#undef LANG_HOOKS_PARSE_FILE
#undef LANG_HOOKS_TYPE_FOR_MODE
#undef LANG_HOOKS_TYPE_FOR_SIZE
#undef LANG_HOOKS_BUILTIN_FUNCTION
#undef LANG_HOOKS_GLOBAL_BINDINGS_P
#undef LANG_HOOKS_PUSHDECL
#undef LANG_HOOKS_GETDECLS
#undef LANG_HOOKS_WRITE_GLOBALS
#undef LANG_HOOKS_GIMPLIFY_EXPR

#define LANG_HOOKS_NAME			"GNU Python"
#define LANG_HOOKS_INIT			gpy_langhook_init
#define LANG_HOOKS_INIT_OPTIONS		gpy_langhook_init_options
#define LANG_HOOKS_HANDLE_OPTION	gpy_langhook_handle_option
#define LANG_HOOKS_POST_OPTIONS		gpy_langhook_post_options
#define LANG_HOOKS_PARSE_FILE		gpy_langhook_parse_file
#define LANG_HOOKS_TYPE_FOR_MODE	gpy_langhook_type_for_mode
#define LANG_HOOKS_TYPE_FOR_SIZE	gpy_langhook_type_for_size
#define LANG_HOOKS_BUILTIN_FUNCTION	gpy_langhook_builtin_function
#define LANG_HOOKS_GLOBAL_BINDINGS_P	gpy_langhook_global_bindings_p
#define LANG_HOOKS_PUSHDECL		gpy_langhook_pushdecl
#define LANG_HOOKS_GETDECLS		gpy_langhook_getdecls
#define LANG_HOOKS_WRITE_GLOBALS	gpy_langhook_write_globals
#define LANG_HOOKS_GIMPLIFY_EXPR	gpy_langhook_gimplify_expr

struct lang_hooks lang_hooks = LANG_HOOKS_INITIALIZER;

#include "gtype-gpy.h"
