/* Calculate branch probabilities, and basic block execution counts.
   Copyright (C) 2012. Free Software Foundation, Inc.
   Contributed by Dehao Chen (dehao@google.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* Read and annotate call graph profile from the auto profile data
   file.  */

#include <string.h>

#include "config.h"
#include "system.h"
#include "flags.h"	      /* for auto_profile_file.  */
#include "basic-block.h"      /* for gcov_type.	 */
#include "diagnostic-core.h"  /* for inform().  */
#include "gcov-io.h"	      /* for gcov_read_unsigned().  */
#include "input.h"	      /* for expanded_location.	 */
#include "profile.h"	      /* for profile_info.  */
#include "langhooks.h"	      /* for langhooks.	 */
#include "opts.h"	      /* for in_fnames.	 */
#include "tree-pass.h"	      /* for ipa pass.  */
#include "cfgloop.h"	      /* for loop_optimizer_init.  */
#include "gimple.h"
#include "cgraph.h"
#include "tree-flow.h"
#include "auto-profile.h"

/* The following routines implements AutoFDO optimization.

   This optimization uses sampling profiles to annotate basic block counts
   and uses heuristics to estimate branch probabilities.

   There are three phases in AutoFDO:

   Phase 1: Read profile from the profile data file.
     The following info is read from the profile datafile:
	* Function names and file names.
	* Source level profile, which is a mapping from inline stack to
	  its sample counts. 
	* Module profile: Module to aux-modules mapping
     Phase 1 just reads in data without processing it. It is invoked
     before tree parsing because LIPO needs module profile before tree
     parsing. (read_aux_modules)

   Phase 2: Process profile to build internal data structure (hashmap).
     This is done after tree parsing, because the processing requires the map
     from function name to its debug name (bfd_name). The following hashmaps
     is used to store profile.
	* function_htab: map from function_name to its entry_bb count
	* stack_htab: map from inline stack to its sample count
	* bfd_name_htab: map from function name to its debug name (bfd_name)
	* module_htab: map from module name to its aux-module names

   Phase 3: Annotate control flow graph.
     AutoFDO invokes a separate pass over the control flow graph to:
	* Annotate basic block count
	* Estimate branch probability

   After the above 3 phases, all profile is readily annotated on the GCC IR.
   AutoFDO tries to reuse all FDO infrastructure as much as possible to make
   use of the profile. E.g. it uses existing mechanism to calculate the basic
   block/edge frequency, as well as the cgraph node/edge count.

   However, AutoFDO still differs from FDO in the following aspects:

   * Profile is not accurate, because AutoFDO uses sampling to collect
     profile, and uses debug info to represent the profile. As a result,
     some hot basic blocks may have zero sample count. Because of this,
     some optimization needs to be adjusted (e.g. loop peeling/unrolling).
   * Each cloned context has its own profile, but these contexts may
     not even exist when doing annotation. This provides more context-
     sensitive profiles, but at the same time, adds complexity to the
     implementation. Because of this, additional profile annotation is
     needed for each function after the inline pass, and count scaling
     is tricky in the second annotation.
*/

#define DEFAULT_AUTO_PROFILE_FILE "fbdata.afdo"
#define SP_HTAB_INIT_SIZE 2000

/* GCOV data structures to represent profile stored in the .afdo file.  */

struct gcov_callsite_pos
{
  const char *file;
  const char *func;
  gcov_unsigned_t line;
  gcov_unsigned_t discr;
};

struct gcov_stack
{
  const char *func_name;
  const char *callee_name;
  struct gcov_callsite_pos *stack;
  gcov_unsigned_t size;
  gcov_type num_inst;
  gcov_type count;
  gcov_type max_count;
};

struct gcov_function
{
  const char *name;
  const char *file;
  gcov_type total_count;
  gcov_type entry_count;
  gcov_type max_count;
  /* Number of call stacks in the function.  */
  gcov_unsigned_t stack_num;
  /* All the call stacks in the function.  */
  struct gcov_stack *stacks;
};

struct afdo_bfd_name
{
  const char *assembler_name;
  /* bfd_name is the name that debugger used for function name matching.
     Different assembler names could map to the same bfd_name.  */
  const char *bfd_name;
};

struct afdo_module
{
  char *name;
  int ident;
  unsigned exported;
  unsigned has_asm;
  unsigned num_aux_modules;
  unsigned num_quote_paths;
  unsigned num_bracket_paths;
  unsigned num_cpp_defines;
  unsigned num_cpp_includes;
  unsigned num_cl_args;
  char **strings;
};

/* Store the file name strings read from the profile data file.	 */
static const char **file_names;

/* gcov_ctr_summary structure to store the profile_info.  */
static struct gcov_ctr_summary *afdo_profile_info;

/* Hash table to hold function information.  */
static htab_t function_htab;

/* Hash table to hold stack information.  */
static htab_t stack_htab;

/* Hash table to hold inline scale information.  */
static htab_t stack_scale_htab;

/* Hash table to hold assembler name to bfd name mapping.  */
static htab_t bfd_name_htab;

/* Hash table to hold module informaition.  */
static htab_t module_htab;

/* Store the module hash table contents.  */
static struct afdo_module *modules;

/* File static variables, which is used to pass information between
   init_auto_profile and process_auto_profile.  */
static gcov_unsigned_t function_num;
static gcov_unsigned_t total_module_num;
static struct gcov_function *gcov_functions;

/* Check if PATH_NAME is absolute path, if yes, strip the directory part
   of the PATH_NAME, return the file name.  */

static const char *
afdo_get_filename (const char *path_name)
{
  const char* last;
  return path_name;
  if (path_name == NULL)
    return NULL;
  last = strrchr(path_name, '/');
  return ((last == 0) ? path_name : last + 1);
}

/* Given an assembler function NAME, return its original name. strip the
   suffix at the end of the function name, added by optimizations such as
   constant propagation etc.  */

static gcov_unsigned_t
afdo_get_original_name_size (const char *name)
{
  const char *ret;
  if (!name)
    return 0;
  ret = strchr (name, '.');
  if (!ret)
    return strlen(name);
  else
    return ret - name;
}

/* Given an asssembler function NAME, return its corresponding bfd name.
   If the mapping cannot be found, it means that the assembler function
   name is not used/emitted in the current module(s).  */

static const char *
afdo_get_bfd_name (const char *name)
{
  struct afdo_bfd_name bfd, *bfd_entry;
  gcov_unsigned_t size = afdo_get_original_name_size (name);
  /* If the function name is cloned, we want to find its original name.  */
  char *buf = (char *) alloca (size + 1);
  strncpy (buf, name, size);
  buf[size] = 0;
  bfd.assembler_name = buf;
  bfd_entry = (struct afdo_bfd_name *) htab_find (bfd_name_htab, &bfd);
  if (!bfd_entry)
    return name;
  return bfd_entry->bfd_name;
}

/* Traverse the cgraph, add each function's name to to bfd_name mapping.  */

static void
afdo_read_bfd_names (void)
{
  struct cgraph_node *node;

  for (node = cgraph_nodes; node; node = node->next)
    {
      const char *bfd_name;
      if (lang_hooks.dwarf_name (node->decl, 0) == NULL)
	continue;
      bfd_name = xstrdup (lang_hooks.dwarf_name (node->decl, 0));
      afdo_add_bfd_name_mapping
	(IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (node->decl)), bfd_name);
    }
}

/* Hash function for struct afdo_stack.  */

static hashval_t
afdo_stack_hash (const void *stack)
{
  gcov_unsigned_t i;
  /* An arbitrary initial value borrowed from hashtab.c.  */
  hashval_t h = 0x9e3779b9;
  const struct gcov_stack *s = (const struct gcov_stack *) stack;
  if (s->callee_name)
    h = iterative_hash (afdo_get_bfd_name (s->callee_name),
			strlen (afdo_get_bfd_name (s->callee_name)), h);
  if (s->func_name)
    h = iterative_hash (s->func_name,
			afdo_get_original_name_size (s->func_name), h);
  for (i = 0; i < s->size; i++) {
    const struct gcov_callsite_pos *p = s->stack + i;
    const char *file = afdo_get_filename (p->file);
    h = iterative_hash (file, strlen (file), h);
    h = iterative_hash (&p->line, sizeof (p->line), h);
    if (i == 0)
      h = iterative_hash (&p->discr, sizeof (p->discr), h);
  }
  return h;
}

/* Check if two afdo_stack P and Q are identical.  */

static int
afdo_stack_eq (const void *p, const void *q)
{
  const struct gcov_stack *s1 = (const struct gcov_stack *) p;
  const struct gcov_stack *s2 = (const struct gcov_stack *) q;

  gcov_unsigned_t i;
  if (s1->func_name == NULL || s2->func_name == NULL)
    return 0;

  if (s1->callee_name == NULL)
    {
      if (s2->callee_name != NULL)
	return 0;
    }
  else if (s2->callee_name != NULL
	   && strcmp (afdo_get_bfd_name (s1->callee_name),
		      afdo_get_bfd_name (s2->callee_name)))
    return 0;

  i = afdo_get_original_name_size (s1->func_name);
  if (i != afdo_get_original_name_size (s2->func_name))
    return 0;

  if (strncmp (s1->func_name, s2->func_name, i))
    return 0;

  if (s1->size != s2->size)
    return 0;
  for (i = 0; i < s1->size; i++)
    {
      const struct gcov_callsite_pos *p1 = s1->stack + i;
      const struct gcov_callsite_pos *p2 = s2->stack + i;
      if (strcmp (afdo_get_filename(p1->file), afdo_get_filename(p2->file))
	  || p1->line != p2->line || (i== 0 && p1->discr != p2->discr))
	return 0;
    }
  return 1;
}

/* Hash function for struct afdo_function.  */

static hashval_t
afdo_function_hash (const void *func)
{
  /* An arbitrary initial value borrowed from hashtab.c.  */
  hashval_t h = 0x9e3779b9;
  const struct gcov_function *f = (const struct gcov_function *) func;

  if (f->name)
    h = iterative_hash (f->name, afdo_get_original_name_size (f->name), h);
  return h;
}

/* Check if two afdo_function P and Q are identical.  */

static int
afdo_function_eq (const void *p, const void *q)
{
  const struct gcov_function *f1 = (const struct gcov_function *) p;
  const struct gcov_function *f2 = (const struct gcov_function *) q;
  gcov_unsigned_t i;

  if (f1->name == NULL || f2->name == NULL)
    return 0;

  i = afdo_get_original_name_size (f1->name);
  if (i != afdo_get_original_name_size (f2->name))
    return 0;

  return !strncmp (f1->name, f2->name, i);
}

/* Hash function for struct afdo_bfd_name.  */

static hashval_t
afdo_bfd_name_hash (const void *func)
{
  hashval_t h = 0x9e3779b9;
  const struct afdo_bfd_name *f = (const struct afdo_bfd_name *) func;

  if (f->assembler_name)
    h = iterative_hash (f->assembler_name, strlen (f->assembler_name), h);
  return h;
}

/* Check if two struct afdo_bfd_name P and Q are identical.  */

static int
afdo_bfd_name_eq (const void *p, const void *q)
{
  const struct afdo_bfd_name *b1 = (const struct afdo_bfd_name *) p;
  const struct afdo_bfd_name *b2 = (const struct afdo_bfd_name *) q;

  if (b1->assembler_name == NULL || b2->assembler_name == NULL)
    return 0;

  return !strcmp (b1->assembler_name, b2->assembler_name);
}

/* Free the hash table entry P.	 */

static void
afdo_bfd_name_del (void *p)
{
  free (p);
}

/* Hash Function for struct afdo_module.  */

static hashval_t
afdo_module_hash (const void *module)
{
  hashval_t h = 0x9e3779b9;
  const struct afdo_module *m = (const struct afdo_module *)module;

  if (m->name)
    h = iterative_hash (m->name, strlen (m->name), h);

  return h;
}

/* Check if two struct afdo_module P and Q are identical.	 */

static int
afdo_module_eq (const void *p, const void *q)
{
  const struct afdo_module *m1 = (const struct afdo_module *)p;
  const struct afdo_module *m2 = (const struct afdo_module *)q;

  if (m1->name == NULL || m2->name == NULL)
    return 0;

  return !strcmp (m1->name, m2->name);
}

/* Return the total number of emitted string for MODULE.  */

static unsigned long long
afdo_module_num_strings (const struct afdo_module *module)
{
  return module->num_quote_paths +
    module->num_bracket_paths +
    module->num_cpp_defines +
    module->num_cpp_includes +
    module->num_cl_args;
}

/* Add a module (specified in MODULE) into gcov_module_info format in
   MODULE_INFO, which is used by LIPO to import auxiliary modules.
   Set the is_primary flag if IS_PRIMARY is set.  */

static void
afdo_add_module (struct gcov_module_info **module_info,
		 const struct afdo_module *module,
		 gcov_unsigned_t is_primary)
{
  unsigned i;
  size_t info_sz;

  info_sz = sizeof (struct gcov_module_info) +
    sizeof (void *) * afdo_module_num_strings (module);
  *module_info = XCNEWVAR (struct gcov_module_info, info_sz);
  (*module_info)->ident = module->ident;
  (*module_info)->is_primary = is_primary;
  (*module_info)->is_exported = is_primary ? module->exported : 1;
  (*module_info)->source_filename = module->name;
  (*module_info)->num_quote_paths = module->num_quote_paths;
  (*module_info)->num_bracket_paths = module->num_bracket_paths;
  (*module_info)->num_cpp_defines = module->num_cpp_defines;
  (*module_info)->num_cpp_includes = module->num_cpp_includes;
  (*module_info)->num_cl_args = module->num_cl_args;
  for (i = 0; i < afdo_module_num_strings (module); i++)
    (*module_info)->string_array[i] =
	module->strings[module->num_aux_modules + i];
}

/* Read in the auxiliary modules for the current primary module.  */

static void
read_aux_modules (void)
{
  unsigned i, curr_module = 1;
  struct afdo_module module, *entry;

  module.name = xstrdup (in_fnames[0]);
  entry = (struct afdo_module *) htab_find (module_htab, &module);
  if (!entry)
    {
      inform (0, "primary module %s cannot be found.", in_fnames[0]);
      return;
    }
  module_infos = XCNEWVEC (struct gcov_module_info *,
			   entry->num_aux_modules + 1);
  afdo_add_module (module_infos, entry, true);
  primary_module_id = entry->ident;
  for (i = 0; i < entry->num_aux_modules; i++)
    {
      struct afdo_module *aux_entry;
      module.name = entry->strings[i];
      if (!strcmp (module.name, in_fnames[0]))
	continue;
      aux_entry = (struct afdo_module *) htab_find (module_htab, &module);
      if (!aux_entry)
	{
	  inform (0, "aux module %s cannot be found.", module.name);
	  continue;
	}
      afdo_add_module (&module_infos[curr_module++], aux_entry, false);
      add_input_filename (module.name);
    }
}

/* Return the size of the inline stack of the STMT.  */

static int
get_inline_stack_size_by_stmt (gimple stmt)
{
  tree block;
  int size = 1;

  if (!stmt)
    return 0;
  if (LOCATION_LOCUS (gimple_location (stmt)) == UNKNOWN_LOCATION)
    return 0;
  block = gimple_block (stmt);
  if (!block || TREE_CODE (block) != BLOCK || !gimple_location (stmt))
    return 0;

  for ( block = BLOCK_SUPERCONTEXT (block);
	block && (TREE_CODE (block) == BLOCK);
	block = BLOCK_SUPERCONTEXT (block)) {
    /* Traverse the nesting blocks. If the block contains the source
       location info, save the source location info to the inline stack.  */
    if (LOCATION_LOCUS (BLOCK_SOURCE_LOCATION (block)) == UNKNOWN_LOCATION)
      continue;
    size ++;
  }
  return size;
}

/* Return the size of the inline stack of the EDGE. All inlined callsites
   along he inline chain are recorded.  */

static int
get_inline_stack_size_by_edge (struct cgraph_edge *edge)
{
  struct cgraph_edge *e;
  int size = 0;
  for (e= edge; e; e = e->caller->callers)
    {
      gimple stmt = e->call_stmt;
      if (!stmt)
	break;
      size += get_inline_stack_size_by_stmt (stmt);
      if (!e->caller->global.inlined_to)
	break;
    }
  return size;
}

/* Return the function name of a given lexical BLOCK.  */

static const char *
get_function_name_from_block (tree block)
{
  tree decl;
  for (decl = BLOCK_ABSTRACT_ORIGIN (block);
       decl && (TREE_CODE (decl) == BLOCK);
       decl = BLOCK_ABSTRACT_ORIGIN (decl))
    if (TREE_CODE (decl) == FUNCTION_DECL)
      break;
  return decl ? IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl)) : NULL;
}

/* Store the inline stack of STMT to POS_STACK, return the size of the
   stack. Set the discriminator of the inline stack if DISCR is TRUE.  */

static int
get_inline_stack_by_stmt (gimple stmt, tree decl,
			  struct gcov_callsite_pos *pos_stack, bool discr)
{
  tree block;
  int idx = 0;
  source_location loc;

  if (!stmt)
    return 0;
  block = gimple_block (stmt);
  if (!block || TREE_CODE (block) != BLOCK || !gimple_location (stmt))
    return 0;

  loc = gimple_location (stmt);
  if (LOCATION_LOCUS (loc) == UNKNOWN_LOCATION)
    return 0;
  pos_stack[idx].file = expand_location(loc).file;
  pos_stack[idx].line = expand_location(loc).line;
  if (discr)
    pos_stack[idx].discr = get_discriminator_from_locus (loc);
  else
    pos_stack[idx].discr = 0;
  idx++;
  for (block = BLOCK_SUPERCONTEXT (block);
       block && (TREE_CODE (block) == BLOCK);
       block = BLOCK_SUPERCONTEXT (block))
    {
      if (LOCATION_LOCUS (BLOCK_SOURCE_LOCATION (block)) == UNKNOWN_LOCATION)
	continue;
      loc = BLOCK_SOURCE_LOCATION (block);
      pos_stack[idx].file = expand_location (loc).file;
      pos_stack[idx].line = expand_location (loc).line;
      pos_stack[idx - 1].func = get_function_name_from_block (block);
      pos_stack[idx++].discr = 0;
    }
  if (decl)
    pos_stack[idx - 1].func = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));
  return idx;
}

/* Store the inline stack of EDGE to POS_STACK, return the size of the
   stack. All inlined callsites along the inline stack are recorded.  */

static int
get_inline_stack_by_edge (struct cgraph_edge *edge,
			  struct gcov_callsite_pos *pos_stack)
{
  struct cgraph_edge *e;
  int size = 0;

  for (e = edge; e; e = e->caller->callers)
    {
      gimple stmt = e->call_stmt;
      if (!stmt)
	break;
      size += get_inline_stack_by_stmt (stmt, e->caller->decl,
					pos_stack + size, false);
      if (!e->caller->global.inlined_to)
	break;      
    }
  return size;
}

/* Read sample count info of the function with DECL, and save them
   to ENTRY_COUNT and TOTAL_COUNT respectively.  */

static void
afdo_get_function_count (tree decl,
			 gcov_type *entry_count)
{
  struct gcov_function func;
  const struct gcov_function *func_entry;

  *entry_count = 0;
  func.name =
    IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));
  func.file = DECL_SOURCE_FILE (decl);
  func_entry = (const struct gcov_function *)
    htab_find (function_htab, &func);
  if (func_entry)
    {
      /* We need to use the sum because in the profile collection binary,
	 there are many cloned functions such as isra functions. We want
	 to combine their profiles.  */
      (*entry_count) = func_entry->entry_count;
      return;
    }
  func.name = afdo_get_bfd_name (func.name);
  func_entry = (const struct gcov_function *)
    htab_find (function_htab, &func);
  if (func_entry)
    (*entry_count) = func_entry->entry_count;
}

/* Set the node count of the current function, and update the entry_bb
   count.  */

void
afdo_set_current_function_count (void)
{
  gcov_type entry_count;
  struct cgraph_node *node = cgraph_get_create_node (current_function_decl);

  afdo_get_function_count (current_function_decl, &entry_count);
  node->count = entry_count;
  ENTRY_BLOCK_PTR->count = node->count;
}

/* Add the AS_NAME->BFD_NAME to the assembler_name to bfd_name mapping.  */

void
afdo_add_bfd_name_mapping (const char *as_name, const char *bfd_name)
{
  struct afdo_bfd_name **slot;
  struct afdo_bfd_name *entry = (struct afdo_bfd_name *)
    xmalloc (sizeof (struct afdo_bfd_name));

  entry->assembler_name = as_name;
  entry->bfd_name = bfd_name;
  slot = (struct afdo_bfd_name **)
    htab_find_slot (bfd_name_htab, entry, INSERT);
  if (!*slot)
    *slot = entry;
  else
    free (entry);
}

/* When EDGE is inlined, the callee is cloned recursively. This function
   updates the copy scale recursively along the callee. STACK stores the
   call stack info from the original inlined edge to the caller of EDGE.

   E.g. foo calls bar with call count 100;
	bar calls baz with call count 300;
	bar has an entry count of 400, baz has an entry count of 1000;
   Initial callgraph looks like:
     foo --(100)--> bar(400)
     bar --(300)--> baz(1000)

   Consider baz is first inlined into bar, we will have a call graph like:
     foo --(100)--> bar(400)
     bar --(300)--> baz.clone(300)
     baz(700)
   At this point, a copyscale mapping is added:
     (bar->baz) --> 0.3

   Consider bar is then inlined into foo, we will have a call graph like:
     foo --(100)--> bar.clone(100)
     bar.clone --(75)-->baz.clone_2(75)
     bar --(225)->baz.clone(225)
     baz(700)
   At this point, two copyscale mappings are added:
     (foo->bar) --> 0.25
     (foo->bar->baz)  --> 0.25 * 0.3
*/

static void
afdo_propagate_copy_scale (struct cgraph_edge *edge, struct gcov_stack *stack)
{
  struct gcov_stack *new_stack, *entry, **stack_slot;
  struct cgraph_edge *e;

  if (edge->callee->global.inlined_to == NULL)
    return;
  if (stack->count == 0)
    return;

  new_stack = (struct gcov_stack *) xmalloc (sizeof (struct gcov_stack));
  new_stack->func_name =
      IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (edge->caller->decl));
  new_stack->callee_name =
      IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (edge->callee->decl));
  new_stack->size = get_inline_stack_size_by_stmt (edge->call_stmt);
  new_stack->stack = (struct gcov_callsite_pos *) xmalloc (
      sizeof (struct gcov_callsite_pos) * (new_stack->size + stack->size));
  get_inline_stack_by_stmt (edge->call_stmt, edge->caller->decl,
			    new_stack->stack, false);
  entry = (struct gcov_stack *) htab_find (stack_scale_htab, new_stack);
  if (entry == NULL)
    {
      free (new_stack->stack);
      free (new_stack);
      return;
    }

  new_stack->func_name = stack->func_name;
  new_stack->count = entry->count * stack->count / REG_BR_PROB_BASE;
  memcpy (new_stack->stack + new_stack->size,
	  stack->stack, stack->size * sizeof (struct gcov_callsite_pos));
  new_stack->size += stack->size;
  stack_slot = (struct gcov_stack **)
      htab_find_slot (stack_scale_htab, new_stack, INSERT);
  if (!*stack_slot)
    *stack_slot = new_stack;
  else
    (*stack_slot)->count = MAX ((*stack_slot)->count, new_stack->count);

  for (e = edge->callee->callees; e; e = e->next_callee)
    afdo_propagate_copy_scale (e, new_stack);
}

/* For an inlined EDGE, the scale (i.e. edge->count / edge->callee->count)
   is recorded in a hash map.  */

void
afdo_add_copy_scale (struct cgraph_edge *edge)
{
  struct gcov_stack *stack;
  struct gcov_stack **stack_slot;
  int scale;
  int size = get_inline_stack_size_by_edge (edge);
  struct cgraph_node *n = edge->callee->clone_of;
  struct cgraph_edge *e;
  gcov_type sum_cloned_count;

  if (edge->callee->clone_of)
    {
      n = edge->callee->clone_of->clones;
      sum_cloned_count = edge->callee->clone_of->count;
    }
  else
    {
      n = edge->callee->clones;
      sum_cloned_count = edge->callee->count;
    }

  for (; n; n = n->next_sibling_clone)
    sum_cloned_count += n->count;
  if (sum_cloned_count > 0)
    scale = (double) edge->count * REG_BR_PROB_BASE / sum_cloned_count;
  else if (edge->caller->count == 0 && edge->caller->max_bb_count == 0)
    scale = 0;
  else
    scale = REG_BR_PROB_BASE;
  if (scale > REG_BR_PROB_BASE)
    scale = REG_BR_PROB_BASE;

  if (size == 0)
    return;
  stack = (struct gcov_stack *) xmalloc (sizeof (struct gcov_stack));
  stack->func_name
      = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (
	edge->caller->global.inlined_to ?
	    edge->caller->global.inlined_to->decl : edge->caller->decl));
  stack->callee_name
      = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (edge->callee->decl));
  stack->size = size;
  stack->stack = (struct gcov_callsite_pos *)
      xmalloc (sizeof (struct gcov_callsite_pos) * size);
  stack->count = scale;

  get_inline_stack_by_edge (edge, stack->stack);

  stack_slot = (struct gcov_stack **)
      htab_find_slot (stack_scale_htab, stack, INSERT);
  if (!*stack_slot)
    *stack_slot = stack;
  else
    (*stack_slot)->count = MAX ((*stack_slot)->count, stack->count);

  for (e = edge->callee->callees; e; e = e->next_callee)
    afdo_propagate_copy_scale (e, stack);
}

/* For a given POS_STACK with SIZE, get the COUNT/NUM_INST info for the
   inline stack. If CALLEE_NAME is non-null, the COUNT represents the
   total count in the inline stack. Otherwise, the COUNT represents the
   count of an ordinary statement. Return FALSE if profile is not found
   for the given POS_STACK.  */

static bool
get_stack_count (struct gcov_callsite_pos *pos_stack,
		 const char *callee_name,
		 int size,
		 gcov_type *count, gcov_type *max_count, gcov_type *num_inst)
{
  int i;

  for (i = 0; i < size; i++)
    {
      struct gcov_stack stack, *entry;
      stack.func_name = pos_stack[size - i - 1].func;
      stack.callee_name = callee_name;
      stack.stack = pos_stack;
      stack.size = size - i;
      entry = (struct gcov_stack *) htab_find (stack_htab, &stack);
      if (entry)
	{
	  if (i == 0)
	    {
	      *count = entry->count;
	      *num_inst = entry->num_inst;
	      if (max_count)
		*max_count = entry->max_count;
	      return true;
	    }
	  else
	    {
	      struct gcov_stack scale_stack, *scale_entry;
	      scale_stack.stack = pos_stack + size - i;
	      scale_stack.size = i;
	      scale_stack.func_name = pos_stack[size - 1].func;
	      scale_stack.callee_name = stack.func_name;
	      scale_entry = (struct gcov_stack *)
		  htab_find (stack_scale_htab, &scale_stack);
	      if (scale_entry)
		{
		  *count = entry->count * scale_entry->count
			   / REG_BR_PROB_BASE;
		  *num_inst = entry->num_inst;
		  if (max_count)
		    *max_count = entry->max_count;
		  return true;
		}
	    }
	}
    }
  *count = 0;
  *num_inst = 0;
  if (max_count)
    *max_count = 0;
  return false;
}

/* For a given STMT, get the COUNT and NUM_INST from its profile.
   Return FALSE if profile is not found for STMT.  */

static bool
get_stmt_count (gimple stmt, gcov_type *count, gcov_type *num_inst)
{
  struct gcov_callsite_pos *pos_stack;
  int size;

  if (!stmt)
    return false;
  size = get_inline_stack_size_by_stmt (stmt);
  if (size == 0)
    return false;
  if (LOCATION_LOCUS (gimple_location (stmt)) == cfun->function_end_locus)
    return false;

  pos_stack = (struct gcov_callsite_pos *)
      alloca (sizeof (struct gcov_callsite_pos) * size);

  get_inline_stack_by_stmt (stmt, current_function_decl, pos_stack, true);

  return get_stack_count (pos_stack, NULL, size, count, NULL, num_inst);
}

/* For a given EDGE, if IS_TOTAL is true, save EDGE->callee's total count
   to COUNT, otherwise save EDGE's count to COUNT.  */

bool
afdo_get_callsite_count (struct cgraph_edge *edge, gcov_type *count,
			 gcov_type *max_count, bool is_total)
{
  struct gcov_callsite_pos *pos_stack;
  gcov_type num_inst;
  const char *callee_name = is_total ?
      IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (edge->callee->decl)) : NULL;
  int size = get_inline_stack_size_by_edge (edge);

  if (size == 0)
    return 0;
  pos_stack = (struct gcov_callsite_pos *)
      alloca (sizeof (struct gcov_callsite_pos) * size);

  get_inline_stack_by_edge (edge, pos_stack);

  if (!is_total)
    pos_stack[0].discr =
	get_discriminator_from_locus(gimple_location(edge->call_stmt));

  return get_stack_count (pos_stack, callee_name,
			  size, count, max_count, &num_inst);
}

/* For a given BB, return its execution count.  */

gcov_type
afdo_get_bb_count (basic_block bb)
{
  gimple_stmt_iterator gsi;
  gcov_type max_count = 0;
  bool has_annotated = false;

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
    {
      gcov_type count, num_inst;
      gimple stmt = gsi_stmt (gsi);
      if (get_stmt_count (stmt, &count, &num_inst))
	{
	  if (count > max_count)
	    max_count = count;
	  has_annotated = true;
	}
    }
  if (has_annotated)
    {
      bb->flags |= BB_ANNOTATED;
      return max_count;
    }
  else
    return 0;
}

/* Annotate auto profile to the control flow graph.  */

static void
afdo_annotate_cfg (void)
{
  basic_block bb;
  gcov_type max_count = ENTRY_BLOCK_PTR->count;

  FOR_EACH_BB (bb)
    {
      bb->count = afdo_get_bb_count (bb);
      if (bb->count > max_count)
	max_count = bb->count;
    }
  if (ENTRY_BLOCK_PTR->count > ENTRY_BLOCK_PTR->next_bb->count)
    ENTRY_BLOCK_PTR->next_bb->count = ENTRY_BLOCK_PTR->count;
  if (max_count > 0)
    {
      counts_to_freqs ();
      afdo_calculate_branch_prob ();
      profile_status = PROFILE_READ;
    }
}

extern gcov_working_set_t *gcov_working_sets;

/* Read profile from profile data file. Write to the module hashmap.  */

static void
read_profile (void)
{
  gcov_unsigned_t i, j, k, file_name_num;
  gcov_working_set_t set[128];

  if (gcov_open (auto_profile_file, 1) == 0)
    {
      inform (0, "Cannot open profile file %s.", auto_profile_file);
      flag_auto_profile = 0;
      return;
    }

  if (gcov_read_unsigned () != GCOV_DATA_MAGIC)
    {
      inform (0, "Magic number does not mathch.");
      flag_auto_profile = 0;
      return;
    }

  if (gcov_read_unsigned () != GCOV_VERSION)
    {
      inform (0, "Version number does not mathch.");
      flag_auto_profile = 0;
      return;;
    }

  /* Skip the empty integer.  */
  gcov_read_unsigned ();
  gcc_assert (gcov_read_unsigned () == GCOV_TAG_AFDO_FILE_NAMES);

  /* Skip the length of the section.  */
  gcov_read_unsigned ();

  /* Read in the file name table.  */
  file_name_num = gcov_read_unsigned ();
  file_names = (const char **)
    xmalloc (sizeof (const char *) * file_name_num);
  for (i = 0; i < file_name_num; i++)
    file_names[i] = xstrdup (gcov_read_string ());

  if (gcov_read_unsigned () != GCOV_TAG_AFDO_FUNCTION)
    {
      inform (0, "Not expected TAG.");
      return;
    }

  /* Skip the length of the section.  */
  gcov_read_unsigned ();

  /* Read in the function/callsite profile, and store it in local
     data structure.  */
  function_num = gcov_read_unsigned ();
  gcov_functions = (struct gcov_function *)
    xmalloc (function_num * sizeof (struct gcov_function));
  for (i = 0; i < function_num; i++)
    {
      gcov_functions[i].name = xstrdup (gcov_read_string ());
      gcov_functions[i].file = file_names[gcov_read_unsigned ()];
      gcov_functions[i].total_count = gcov_read_counter ();
      gcov_functions[i].entry_count = gcov_read_counter ();
      gcov_functions[i].max_count = 0;
      gcov_functions[i].stack_num = gcov_read_unsigned ();
      gcov_functions[i].stacks = (struct gcov_stack *)
	xmalloc (gcov_functions[i].stack_num * sizeof (struct gcov_stack));
      for (j = 0; j < gcov_functions[i].stack_num; j++)
	{
	  gcov_functions[i].stacks[j].func_name = gcov_functions[i].name;
	  gcov_functions[i].stacks[j].callee_name = NULL;
	  gcov_functions[i].stacks[j].size = gcov_read_unsigned ();
	  gcov_functions[i].stacks[j].stack = (struct gcov_callsite_pos *)
	    xmalloc (gcov_functions[i].stacks[j].size
		     * sizeof (struct gcov_callsite_pos));
	  for (k = 0; k < gcov_functions[i].stacks[j].size; k++)
	    {
	      gcov_functions[i].stacks[j].stack[k].func =
		file_names[gcov_read_unsigned ()];
	      gcov_functions[i].stacks[j].stack[k].file =
		file_names[gcov_read_unsigned ()];
	      gcov_functions[i].stacks[j].stack[k].line =
		gcov_read_unsigned ();
	      gcov_functions[i].stacks[j].stack[k].discr =
		gcov_read_unsigned ();
	    }
	  gcov_functions[i].stacks[j].count = gcov_read_counter ();
	  gcov_functions[i].stacks[j].num_inst = gcov_read_counter ();
	}
    }

  /* Read in the module info.  */
  if (gcov_read_unsigned () != GCOV_TAG_AFDO_MODULE_GROUPING)
    {
      inform (0, "Not expected TAG.");
      return;
    }
  /* Skip the length of the section.  */
  gcov_read_unsigned ();

  /* Read in the file name table.  */
  total_module_num = gcov_read_unsigned ();
  modules = (struct afdo_module *)
    xmalloc (total_module_num * sizeof (struct afdo_module));
  for (i = 0; i < total_module_num; i++)
    {
      unsigned num_strings;
      struct afdo_module **slot;
      modules[i].name = xstrdup (gcov_read_string());
      modules[i].ident = i + 1;
      /* exported flag.	 */
      modules[i].exported = gcov_read_unsigned();
      /* has_asm flag.  */
      modules[i].has_asm = gcov_read_unsigned();
      /* aux_module and 5 options.  */
      modules[i].num_aux_modules = gcov_read_unsigned();
      modules[i].num_quote_paths = gcov_read_unsigned();
      modules[i].num_bracket_paths = gcov_read_unsigned();
      modules[i].num_cpp_defines = gcov_read_unsigned();
      modules[i].num_cpp_includes = gcov_read_unsigned();
      modules[i].num_cl_args = gcov_read_unsigned();
      num_strings = modules[i].num_aux_modules
	+ modules[i].num_quote_paths
	+ modules[i].num_bracket_paths
	+ modules[i].num_cpp_defines
	+ modules[i].num_cpp_includes
	+ modules[i].num_cl_args;
      modules[i].strings = (char **)
	xmalloc (num_strings * sizeof (char *));
      for (j = 0; j < num_strings; j++)
	modules[i].strings[j] = xstrdup (gcov_read_string());
      slot = (struct afdo_module **)
	htab_find_slot (module_htab, &modules[i], INSERT);
      if (!*slot)
	*slot = &modules[i];
      else
	gcc_unreachable ();
    }

  /* Read in the working set.  */
  if (gcov_read_unsigned () != GCOV_TAG_AFDO_WORKING_SET)
    {
      inform (0, "Not expected TAG.");
      return;
    }

  /* Skip the length of the section.  */
  gcov_read_unsigned ();
  for (i = 0; i < 128; i++)
    {
      set[i].num_counters = gcov_read_unsigned ();
      set[i].min_counter = gcov_read_counter ();
    }
  add_working_set (set);
}

/* Process the profile data and build the function/stack
   hash maps.  */

void
process_auto_profile (void)
{
  unsigned i;

  afdo_read_bfd_names ();
  for (i = 0; i < function_num; i++)
    {
      struct gcov_function **func_slot = (struct gcov_function **)
	  htab_find_slot (function_htab, gcov_functions + i, INSERT);
      if (*func_slot)
	{
	  (*func_slot)->entry_count += gcov_functions[i].entry_count;
	  (*func_slot)->total_count += gcov_functions[i].total_count;
	  if ((*func_slot)->total_count > afdo_profile_info->sum_all)
	    afdo_profile_info->sum_all = (*func_slot)->total_count;
	}
      else
	*func_slot = gcov_functions + i;
    }

  for (i = 0; i < function_num; i++)
    {
      unsigned j;
      struct gcov_function *func = gcov_functions + i;
      for (j = 0; j < func->stack_num; j++)
	{
	  unsigned k;
	  unsigned stack_size = func->stacks[j].size;
	  gcov_type count = func->stacks[j].count;
	  struct gcov_stack **stack_slot = (struct gcov_stack **)
		  htab_find_slot (stack_htab, func->stacks + j, INSERT);
	  if (func->stacks[j].num_inst && count > afdo_profile_info->sum_max)
	    afdo_profile_info->sum_max = count / func->stacks[j].num_inst;
	  if (*stack_slot)
	    {
	      (*stack_slot)->count += count;
	      if ((*stack_slot)->num_inst < func->stacks[j].num_inst)
		(*stack_slot)->num_inst = func->stacks[j].num_inst;
	    }
	  else
	    *stack_slot = func->stacks + j;
	  for (k = 1; k < stack_size; k++)
	    {
	      struct gcov_stack *new_stack = (struct gcov_stack *)
		  xmalloc (sizeof (struct gcov_stack));
	      new_stack->func_name = func->stacks[j].func_name;
	      new_stack->callee_name =
		  func->stacks[j].stack[stack_size - k - 1].func;
	      new_stack->stack = func->stacks[j].stack + stack_size - k;
	      new_stack->size = k;
	      new_stack->num_inst = 0;
	      new_stack->count = 0;
	      new_stack->max_count = 0;
	      stack_slot = (struct gcov_stack **)
		  htab_find_slot (stack_htab, new_stack, INSERT);
	      if (!*stack_slot)
		*stack_slot = new_stack;
	      else
		free (new_stack);
	      (*stack_slot)->count += count;
	      if ((*stack_slot)->max_count < count)
		(*stack_slot)->max_count = count;
	    }
	}
    }
}

/* Create the hash tables, and read the profile from the profile data
   file.  */

void
init_auto_profile (void)
{
  if (auto_profile_file == NULL)
    auto_profile_file = DEFAULT_AUTO_PROFILE_FILE;

  /* Initialize the function hash table.  */
  function_htab = htab_create_alloc ((size_t) SP_HTAB_INIT_SIZE,
				     afdo_function_hash,
				     afdo_function_eq,
				     0,
				     xcalloc,
				     free);
  /* Initialize the stack hash table.  */
  stack_htab = htab_create_alloc ((size_t) SP_HTAB_INIT_SIZE,
				  afdo_stack_hash,
				  afdo_stack_eq,
				  0,
				  xcalloc,
				  free);
  /* Initialize the stack scale hash table.  */
  stack_scale_htab = htab_create_alloc ((size_t) SP_HTAB_INIT_SIZE,
				  afdo_stack_hash,
				  afdo_stack_eq,
				  0,
				  xcalloc,
				  free);
  /* Initialize the bfd name mapping table.  */
  bfd_name_htab = htab_create_alloc ((size_t) SP_HTAB_INIT_SIZE,
				     afdo_bfd_name_hash,
				     afdo_bfd_name_eq,
				     afdo_bfd_name_del,
				     xcalloc,
				     free);
  /* Initialize the module hash table.  */
  module_htab = htab_create_alloc ((size_t) SP_HTAB_INIT_SIZE,
				   afdo_module_hash,
				   afdo_module_eq,
				   0,
				   xcalloc,
				   free);

  afdo_profile_info = (struct gcov_ctr_summary *)
    xcalloc (1, sizeof (struct gcov_ctr_summary));
  afdo_profile_info->runs = 1;
  afdo_profile_info->sum_max = 0;

  /* Read the profile from the profile file.  */
  read_profile ();

  if (flag_dyn_ipa)
    read_aux_modules();
}

/* Free the resources.  */

void
end_auto_profile (void)
{
  unsigned i, j;

  for (i = 0; i < function_num; i++)
    {
      for (j = 0; j < gcov_functions[i].stack_num; ++j)
	free (gcov_functions[i].stacks[j].stack);
      free (gcov_functions[i].stacks);
    }
  free (gcov_functions);

  for (i = 0; i < total_module_num; i++)
    free (modules[i].strings);
  free (modules);
  free (afdo_profile_info);
  free (file_names);
  htab_delete (function_htab);
  htab_delete (stack_htab);
  htab_delete (stack_scale_htab);
  htab_delete (bfd_name_htab);
  htab_delete (module_htab);
  profile_info = NULL;
}

/* BB1 and BB2 are in an equivalent class iff:
   1. BB1 dominates BB2.
   2. BB2 post-dominates BB1.
   3. BB1 and BB2 are in the same loop nest.
   This function finds the equivalent class for each basic block, and
   stores a pointer to the first BB in its equivalent class. Meanwhile,
   set bb counts for the same equivalent class to be idenical.  */

static void
afdo_find_equiv_class (void)
{
  basic_block bb;

  FOR_ALL_BB (bb)
    bb->aux = NULL;

  FOR_ALL_BB (bb)
    {
      VEC (basic_block, heap) *dom_bbs;
      basic_block bb1;
      int i;

      if (bb->aux != NULL)
	continue;
      bb->aux = bb;
      dom_bbs = get_dominated_by (CDI_DOMINATORS, bb);
      FOR_EACH_VEC_ELT (basic_block, dom_bbs, i, bb1)
	if (bb1->aux == NULL
	    && dominated_by_p (CDI_POST_DOMINATORS, bb, bb1)
	    && bb1->loop_father == bb->loop_father)
	  {
	    bb1->aux = bb;
	    if (bb1->count > bb->count && (bb1->flags & BB_ANNOTATED) != 0)
	      {
		bb->count = MAX (bb->count, bb1->count);
		bb->flags |= BB_ANNOTATED;
	      }
	  }
      dom_bbs = get_dominated_by (CDI_POST_DOMINATORS, bb);
      FOR_EACH_VEC_ELT (basic_block, dom_bbs, i, bb1)
	if (bb1->aux == NULL
	    && dominated_by_p (CDI_DOMINATORS, bb, bb1)
	    && bb1->loop_father == bb->loop_father)
	  {
	    bb1->aux = bb;
	    if (bb1->count > bb->count && (bb1->flags & BB_ANNOTATED) != 0)
	      {
		bb->count = MAX (bb->count, bb1->count);
		bb->flags |= BB_ANNOTATED;
	      }
	  }
    }
}

/* If a baisk block only has one in/out edge, then the bb count and he
   edge count should be the same.
   IS_SUCC is true if the out edge of the basic block is examined.
   Return TRUE if any basic block/edge count is changed.  */

static bool
afdo_propagate_single_edge (bool is_succ)
{
  basic_block bb;
  bool changed = false;

  FOR_EACH_BB (bb)
    if (is_succ ? single_succ_p (bb) : single_pred_p (bb))
      {
	edge e = is_succ ? single_succ_edge (bb) : single_pred_edge (bb);
	if (((e->flags & EDGE_ANNOTATED) == 0)
	    && ((bb->flags & BB_ANNOTATED) != 0))
	  {
	    e->count = bb->count;
	    e->flags |= EDGE_ANNOTATED;
	    changed = true;
	  }
	else if (((e->flags & EDGE_ANNOTATED) != 0)
	    && ((bb->flags & BB_ANNOTATED) == 0))
	  {
	    bb->count = e->count;
	    bb->flags |= BB_ANNOTATED;
	    changed = true;
	  }
	else if (bb->count != e->count)
	  {
	    e->count = bb->count = MAX (bb->count, e->count);
	    changed = true;
	  }
      }
  return changed;
}

/* If a basic block's count is known, and only one of its in/out edges' count
   is unknown, its count can be calculated.
   Meanwhile, if all of the in/out edges' counts are known, then the basic
   block's unknown count can also be calculated.
   IS_SUCC is true if out edges of a basic blocks are examined.
   Return TRUE if any basic block/edge count is changed.  */

static bool
afdo_propagate_multi_edge (bool is_succ)
{
  basic_block bb;
  bool changed = false;

  FOR_EACH_BB (bb)
    {
      edge e, unknown_edge = NULL, zero_edge = NULL;
      edge_iterator ei;
      int num_unknown_edge = 0;
      gcov_type total_known_count = 0;

      if (is_succ)
	{
	  FOR_EACH_EDGE (e, ei, bb->succs)
	    if ((e->flags & EDGE_ANNOTATED) == 0)
	      num_unknown_edge ++, unknown_edge = e;
	    else if (e->count == 0)
	      zero_edge = e;
	    else
	      total_known_count += e->count;
	}
      else
	{
	  FOR_EACH_EDGE (e, ei, bb->preds)
	    if ((e->flags & EDGE_ANNOTATED) == 0)
	      num_unknown_edge ++, unknown_edge = e;
	    else
	      total_known_count += e->count;
	}

      if (num_unknown_edge == 0)
	{
	  if (total_known_count > bb->count)
	    {
	      bb->count = total_known_count;
	      changed = true;
	    }
	  else if (zero_edge != NULL && total_known_count < bb->count
		   && bb->loop_father && bb->loop_father->header == bb)
	    {
	      zero_edge->count = bb->count - total_known_count;
	      changed = true;
	    }
	  if ((bb->flags & BB_ANNOTATED) == 0)
	    {
	      bb->flags |= BB_ANNOTATED;
	      changed = true;
	    }
	}
      else if (num_unknown_edge == 1
	       && (bb->flags & BB_ANNOTATED) != 0)
	{
	  if (bb->count >= total_known_count)
	    unknown_edge->count = bb->count - total_known_count;
	  else
	    unknown_edge->count = 0;
	  unknown_edge->flags |= EDGE_ANNOTATED;
	  changed = true;
	}
    }
  return changed;
}

/* Special propagation for circuit expressions. Because GCC translates
   control flow into data flow for circuit expressions. E.g.
   BB1:
   if (a && b)
     BB2
   else
     BB3

   will be translated into:

   BB1:
     if (a)
       goto BB.t1
     else
       goto BB.t3
   BB.t1:
     if (b)
       goto BB.t2
     else
       goto BB.t3
   BB.t2:
     goto BB.t3
   BB.t3:
     tmp = PHI (0 (BB1), 0 (BB.t1), 1 (BB.t2)
     if (tmp)
       goto BB2
     else
       goto BB3

   In this case, we need to propagate through PHI to determine the edge
   count of BB1->BB.t1, BB.t1->BB.t2.  */

static void
afdo_propagate_circuit (void)
{
  basic_block bb;
  FOR_ALL_BB (bb)
    {
      gimple phi_stmt;
      tree cmp_rhs, cmp_lhs;
      gimple cmp_stmt = last_stmt (bb);
      edge e;
      edge_iterator ei;

      if (!cmp_stmt || gimple_code (cmp_stmt) != GIMPLE_COND)
	continue;
      cmp_rhs = gimple_cond_rhs (cmp_stmt);
      cmp_lhs = gimple_cond_lhs (cmp_stmt);
      if (!TREE_CONSTANT (cmp_rhs)
	  || !(integer_zerop (cmp_rhs) || integer_onep (cmp_rhs)))
	continue;
      if (TREE_CODE (cmp_lhs) != SSA_NAME)
	continue;
      if ((bb->flags & BB_ANNOTATED) == 0)
	continue;
      phi_stmt = SSA_NAME_DEF_STMT (cmp_lhs);
      while (phi_stmt && gimple_code (phi_stmt) == GIMPLE_ASSIGN
	     && gimple_assign_single_p (phi_stmt)
	     && TREE_CODE(gimple_assign_rhs1 (phi_stmt)) == SSA_NAME)
	phi_stmt = SSA_NAME_DEF_STMT (gimple_assign_rhs1 (phi_stmt));
      if (!phi_stmt || gimple_code (phi_stmt) != GIMPLE_PHI)
	continue;
      FOR_EACH_EDGE (e, ei, bb->succs)
	{
	  unsigned i, total = 0;
	  edge only_one;
	  bool check_value_one = (((integer_onep (cmp_rhs))
		    ^ (gimple_cond_code (cmp_stmt) == EQ_EXPR))
		    ^ ((e->flags & EDGE_TRUE_VALUE) != 0));
	  if ((e->flags & EDGE_ANNOTATED) == 0)
	    continue;
	  for (i = 0; i < gimple_phi_num_args (phi_stmt); i++)
	    {
	      tree val = gimple_phi_arg_def (phi_stmt, i);
	      edge ep = gimple_phi_arg_edge (phi_stmt, i);

	      if (!TREE_CONSTANT (val) || !(integer_zerop (val)
		  || integer_onep (val)))
		continue;
	      if (check_value_one ^ integer_onep (val))
		continue;
	      total++;
	      only_one = ep;
	      if (e->probability == 0 && (e->flags & EDGE_ANNOTATED) == 0)
		{
		  ep->probability = 0;
		  ep->count = 0;
		  ep->flags |= EDGE_ANNOTATED;
		}
	    }
	  if (total == 1 && (only_one->flags & EDGE_ANNOTATED) == 0)
	    {
	      only_one->probability = e->probability;
	      only_one->count = e->count;
	      only_one->flags |= EDGE_ANNOTATED;
	    }
	}
    }
}

/* Propagate the basic block count and edge count on the control flow
   graph. We do the propagation iteratively until stablize.  */

static void
afdo_propagate (void)
{
  basic_block bb;
  bool changed = true;

  FOR_ALL_BB (bb)
    {
      bb->count = ((basic_block) bb->aux)->count;
      if ((((basic_block) bb->aux)->flags & BB_ANNOTATED) != 0)
	bb->flags |= BB_ANNOTATED;
    }

  while (changed)
    {
      changed = false;

      if (afdo_propagate_single_edge (true))
	changed = true;
      if (afdo_propagate_single_edge (false))
	changed = true;
      if (afdo_propagate_multi_edge (true))
	changed = true;
      if (afdo_propagate_multi_edge (false))
	changed = true;
      afdo_propagate_circuit ();
    }
}

/* Propagate counts on control flow graph and calculate branch
   probabilities.  */

void
afdo_calculate_branch_prob (void)
{
  basic_block bb;
  bool has_sample = false;

  FOR_EACH_BB (bb)
    if (bb->count > 0)
      has_sample = true;

  if (!has_sample)
    return;

  calculate_dominance_info (CDI_POST_DOMINATORS);
  calculate_dominance_info (CDI_DOMINATORS);
  loop_optimizer_init (0);

  afdo_find_equiv_class ();
  afdo_propagate ();

  FOR_EACH_BB (bb)
    {
      edge e;
      edge_iterator ei;
      int num_unknown_succ = 0;
      gcov_type total_count = 0;

      FOR_EACH_EDGE (e, ei, bb->succs)
	{
	  if ((e->flags & EDGE_ANNOTATED) == 0)
	    num_unknown_succ ++;
	  else
	    total_count += e->count;
	}
      if (num_unknown_succ == 0 && total_count > 0)
	{
	  FOR_EACH_EDGE (e, ei, bb->succs)
	    e->probability =
		(double) e->count * REG_BR_PROB_BASE / total_count;
	}
    }
  FOR_ALL_BB (bb)
    {
      edge e;
      edge_iterator ei;

      FOR_EACH_EDGE (e, ei, bb->succs)
	e->count =
		(double) bb->count * e->probability / REG_BR_PROB_BASE;
      bb->aux = NULL;
    }

  loop_optimizer_finalize ();
  free_dominance_info (CDI_DOMINATORS);
  free_dominance_info (CDI_POST_DOMINATORS);
}

/* Use AutoFDO profile to annoate the control flow graph.
   Return the todo flag.  */

static unsigned int
auto_profile (void)
{
  struct cgraph_node *node;

  if (cgraph_state == CGRAPH_STATE_FINISHED)
    return 0;

  init_node_map();
  profile_info = afdo_profile_info;

  for (node = cgraph_nodes; node; node = node->next)
    {
      if (!node->analyzed
	  || !gimple_has_body_p (node->decl)
	  || !(!node->clone_of || node->decl != node->clone_of->decl))
	continue;

      /* Don't profile functions produced for builtin stuff.  */
      if (DECL_SOURCE_LOCATION (node->decl) == BUILTINS_LOCATION
	  || DECL_STRUCT_FUNCTION (node->decl)->after_tree_profile)
	continue;

      push_cfun (DECL_STRUCT_FUNCTION (node->decl));
      current_function_decl = node->decl;

      afdo_annotate_cfg ();
      compute_function_frequency ();

      current_function_decl = NULL;
      pop_cfun ();
    }

  return TODO_rebuild_cgraph_edges;
}

static bool
gate_auto_profile_ipa (void)
{
  return flag_auto_profile;
}

struct simple_ipa_opt_pass pass_ipa_auto_profile =
{
 {
  SIMPLE_IPA_PASS,
  "afdo",                              /* name */
  gate_auto_profile_ipa,               /* gate */
  auto_profile,                        /* execute */
  NULL,                                /* sub */
  NULL,                                /* next */
  0,                                   /* static_pass_number */
  TV_IPA_AUTOFDO,                      /* tv_id */
  0,                                   /* properties_required */
  0,                                   /* properties_provided */
  0,                                   /* properties_destroyed */
  0,                                   /* todo_flags_start */
  TODO_dump_func                       /* todo_flags_finish */
 }
};
