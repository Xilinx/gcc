/* This file is part of GCC.

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
<http://www.gnu.org/licenses/>. */

#include "config.h"
#include "system.h"
#include "ansidecl.h"
#include "coretypes.h"
#include "opts.h"
#include "tree.h"
#include "gimple.h"
#include "toplev.h"
#include "debug.h"
#include "options.h"
#include "flags.h"
#include "convert.h"
#include "diagnostic-core.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "target.h"

#include <gmp.h>
#include <mpfr.h>

#include "gpy.h"

/* Language-dependent contents of a type.  */
struct GTY(()) lang_type {
  char dummy;
} ;

/* Language-dependent contents of a decl.  */
struct GTY(()) lang_decl {
  char dummy;
} ;

/* Language-dependent contents of an identifier.  This must include a
   tree_identifier.
*/
struct GTY(()) lang_identifier {
  struct tree_identifier common;
} ;

/* The resulting tree type.  */
union GTY((desc ("TREE_CODE (&%h.generic) == IDENTIFIER_NODE"),
	   chain_next ("(union lang_tree_node *) TREE_CHAIN (&%h.generic)")))
lang_tree_node
{
  union tree_node GTY((tag ("0"),
		       desc ("tree_node_structure (&%h)"))) generic;
  struct lang_identifier GTY((tag ("1"))) identifier;
};

/* We don't use language_function.  */
struct GTY(()) language_function {
  int dummy;
};

/* Language hooks.  */
static
bool gpy_langhook_init( void )
{
  build_common_tree_nodes( false );

  /* The sizetype may be "unsigned long" or "unsigned long long".  */
  if( TYPE_MODE (long_unsigned_type_node) == ptr_mode )
    {
      size_type_node = long_unsigned_type_node;
    }
  else if( TYPE_MODE (long_long_unsigned_type_node) == ptr_mode )
    {
      size_type_node = long_long_unsigned_type_node;
    }
  else
    {
      size_type_node = long_unsigned_type_node;
    }

  set_sizetype( size_type_node );
  build_common_tree_nodes_2( 0 );

  /* build_common_builtin_nodes( ); */

  void_list_node = build_tree_list( NULL_TREE, void_type_node );

  debug("init!\n");

  return true;
}

/* Initialize before parsing options.  */
static unsigned int
gpy_langhook_init_options( unsigned int argc ATTRIBUTE_UNUSED,
			   const char** argv ATTRIBUTE_UNUSED )
{
  flag_strict_aliasing = 1;
  debug("init options!\n");

  mpfr_set_default_prec (128);

  return 1;
}

/* Handle gpy specific options.  Return 0 if we didn't do anything.  */
static int
gpy_langhook_handle_option( size_t scode,
			    const char *arg ATTRIBUTE_UNUSED,
			    int value ATTRIBUTE_UNUSED,
			    int kind ATTRIBUTE_UNUSED )
{
  enum opt_code code = (enum opt_code) scode;
  int retval = 1;

  debug("inside handle option!\n");

  switch( code )
    {
    default:
      /* Just return 1 to indicate that the option is valid.  */
      break;
    }

  return retval;
}

/* Run after parsing options.  */
static bool
gpy_langhook_post_options( const char **pfilename ATTRIBUTE_UNUSED )
{
  debug("post options!\n");
  gcc_assert( num_in_fnames > 0 );

  if( flag_excess_precision_cmdline == EXCESS_PRECISION_DEFAULT )
    {
      flag_excess_precision_cmdline = EXCESS_PRECISION_STANDARD;
    }

  /* Returning false means that the backend should be used.  */
  return false;
}

static void
gpy_langhook_parse_file( int set_yy_debug ATTRIBUTE_UNUSED )
{
  unsigned int idx = 0;
  debug("parse file!\n");

  for( ; idx<num_in_fnames; ++idx )
    {
      const char * t = in_fnames[idx];
      debug("t = <%s>!\n", t);
      gpy_lex_parse( t );
    }
}

static tree
gpy_langhook_type_for_size( unsigned int bits ATTRIBUTE_UNUSED,
			    int unsignedp ATTRIBUTE_UNUSED )
{
  debug("type for size!\n");
  return NULL;
}

static tree
gpy_langhook_type_for_mode( enum machine_mode mode ATTRIBUTE_UNUSED,
			    int unsignedp ATTRIBUTE_UNUSED )
{
  debug("type for mode!\n");


  return NULL_TREE;
}

/* Record a builtin function.  We just ignore builtin functions.  */
static tree
gpy_langhook_builtin_function( tree decl ATTRIBUTE_UNUSED )
{
  debug("builtin function!\n");

  return decl;
}

static int
gpy_langhook_global_bindings_p( void )
{
  debug("global bindings!\n");
  return 1;
}

static tree
gpy_langhook_pushdecl( tree decl ATTRIBUTE_UNUSED )
{
  debug("pushdecl!\n");
  gcc_unreachable ();
  return NULL;
}

static tree
gpy_langhook_getdecls( void )
{
  debug("get decls!\n");
  return NULL;
}

/* Write out globals.  */
static void
gpy_langhook_write_globals( void )
{
  debug("write globals!\n");
  gpy_write_globals( );
}

static int
gpy_langhook_gimplify_expr( tree *expr_p ATTRIBUTE_UNUSED,
			    gimple_seq *pre_p ATTRIBUTE_UNUSED,
			    gimple_seq *post_p ATTRIBUTE_UNUSED )
{
  debug("gimplify expression!\n");
  return GS_UNHANDLED;
}

/* Functions called directly by the generic backend.  */
tree convert( tree type ATTRIBUTE_UNUSED,
	      tree expr ATTRIBUTE_UNUSED )
{
  debug("tree convert!\n");
  gcc_unreachable( );
  return NULL;
}

static GTY(()) tree gpy_gc_root;

void
gpy_preserve_from_gc( tree t ATTRIBUTE_UNUSED )
{
  gpy_gc_root = tree_cons( NULL_TREE, t, gpy_gc_root );
  debug("preserve from gc!\n");
}

void __gpy_debug__( const char * file, unsigned int lineno,
		    const char * fmt, ... )
{
  va_list args;
  fprintf( stderr, "debug: <%s:%i> -> ",
           file, lineno );
  va_start( args, fmt );
  vfprintf( stderr, fmt, args );
  va_end( args );
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

#include "gt-python-py-lang.h"
#include "gtype-python.h"
#include "symbols.h"
