/* Definitions for GIMPLE parsing and type checking.

   Copyright (C) 2010
   Free Software Foundation, Inc.
   Contributed by Diego Novillo.

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

#ifndef GIMPLE_GIMPLE_TREE_H
#define GIMPLE_GIMPLE_TREE_H

struct GTY(()) lang_identifier
{
  struct tree_identifier base;
};

struct GTY(()) lang_decl
{
  int dummy;  /* Added because ggc does not like empty structs.  */
};

struct GTY(()) lang_type
{
  int dummy;  /* Added because ggc does not like empty structs.  */
};

struct GTY(()) language_function
{
  int dummy;  /* Added because ggc does not like empty structs.  */
};

enum gimple_tree_node_structure_enum {
  TS_GIMPLE_GENERIC
};

union GTY((desc ("gimple_tree_node_structure (&%h)"),
	  chain_next ("(union lang_tree_node *)TREE_CHAIN (&%h.generic)")))
    lang_tree_node
{
  union tree_node GTY ((tag ("TS_GIMPLE_GENERIC"),
			desc ("tree_node_structure (&%h)"))) generic;
};

#endif /* GIMPLE_GIMPLE_TREE_H  */
