/* Read and write coverage files, and associated functionality.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 1996, 1997, 1998, 1999,
   2000, 2001, 2003, 2004, 2005, 2007, 2008, 2009, 2010, 2011, 2012
   Free Software Foundation, Inc.
   Contributed by James E. Wilson, UC Berkeley/Cygnus Support;
   based on some ideas from Dain Samples of UC Berkeley.
   Further mangling by Bob Manson, Cygnus Support.
   Further mangled by Nathan Sidwell, CodeSourcery

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


#define GCOV_LINKAGE

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "flags.h"
#include "output.h"
#include "regs.h"
#include "expr.h"
#include "function.h"
#include "basic-block.h"
#include "toplev.h"
#include "tm_p.h"
#include "ggc.h"
#include "coverage.h"
#include "langhooks.h"
#include "hashtab.h"
#include "tree-iterator.h"
#include "cgraph.h"
#include "tree-pass.h"
#include "opts.h"
#include "gcov-io.h"
#include "tree-flow.h"
#include "cpplib.h"
#include "incpath.h"
#include "diagnostic-core.h"
#include "intl.h"
#include "l-ipo.h"
#include "filenames.h"
#include "dwarf2asm.h"
#include "target.h"

#include "gcov-io.h"
#include "gcov-io.c"
#include "params.h"
#include "dbgcnt.h"
#include "input.h"

struct GTY((chain_next ("%h.next"))) coverage_data
{
  struct coverage_data *next;	 /* next function */
  unsigned ident;		 /* function ident */
  unsigned lineno_checksum;	 /* function lineno checksum */
  unsigned cfg_checksum;	 /* function cfg checksum */
  tree fn_decl;			 /* the function decl */
  tree ctr_vars[GCOV_COUNTERS];	 /* counter variables.  */
};

/* Linked list of -D/-U/-imacro/-include strings for a source module.  */
struct str_list
{
  char *str;
  struct str_list *next;
};

/* Counts information for a function.  */
typedef struct counts_entry
{
  /* We hash by  */
  unsigned HOST_WIDEST_INT ident;
  unsigned ctr;

  /* Store  */
  unsigned lineno_checksum;
  unsigned cfg_checksum;
  gcov_type *counts;
  struct gcov_ctr_summary summary;
} counts_entry_t;

static GTY(()) struct coverage_data *functions_head = 0;
static struct coverage_data **functions_tail = &functions_head;
static unsigned no_coverage = 0;

/* Cumulative counter information for whole program.  */
static unsigned prg_ctr_mask; /* Mask of counter types generated.  */

/* Counter information for current function.  */
static unsigned fn_ctr_mask; /* Mask of counters used.  */
static GTY(()) tree fn_v_ctrs[GCOV_COUNTERS];   /* counter variables.  */
static unsigned fn_n_ctrs[GCOV_COUNTERS]; /* Counters allocated.  */
static unsigned fn_b_ctrs[GCOV_COUNTERS]; /* Allocation base.  */

/* Coverage info VAR_DECL and function info type nodes.  */
static GTY(()) tree gcov_info_var;
static GTY(()) tree gcov_fn_info_type;
static GTY(()) tree gcov_fn_info_ptr_type;

/* Name of the output file for coverage output file.  If this is NULL
   we're not writing to the notes file.  */
static char *bbg_file_name;

/* Name of the count data file.  */
static char *da_file_name;
static char *da_base_file_name;
static char *main_input_file_name;

/* Filename for the global pmu profile */
static char pmu_profile_filename[] = "pmuprofile";

/* Hash table of count data.  */
static htab_t counts_hash = NULL;

/* The names of merge functions for counters.  */
static const char *const ctr_merge_functions[GCOV_COUNTERS] = GCOV_MERGE_FUNCTIONS;
static const char *const ctr_names[GCOV_COUNTERS] = GCOV_COUNTER_NAMES;

/* True during the period that counts_hash is being rebuilt.  */
static bool rebuilding_counts_hash = false;

struct gcov_module_info **module_infos = NULL;

/* List of -D/-U options.  */
static struct str_list *cpp_defines_head = NULL, *cpp_defines_tail = NULL;
static unsigned num_cpp_defines = 0;

/* List of -imcaro/-include options.  */
static struct str_list *cpp_includes_head = NULL, *cpp_includes_tail = NULL;
static unsigned num_cpp_includes = 0;

/* True if the current module has any asm statements.  */
static bool has_asm_statement;

/* extern const char * __gcov_pmu_profile_filename */
static tree gcov_pmu_filename_decl = NULL_TREE;
/* extern const char * __gcov_pmu_profile_options */
static tree gcov_pmu_options_decl = NULL_TREE;
/* extern gcov_unsigned_t  __gcov_pmu_top_n_address */
static tree gcov_pmu_top_n_address_decl = NULL_TREE;

/* To ensure that the above variables are initialized only once.  */
static int pmu_profiling_initialized = 0;

/* Forward declarations.  */
static hashval_t htab_counts_entry_hash (const void *);
static int htab_counts_entry_eq (const void *, const void *);
static void htab_counts_entry_del (void *);
static void read_counts_file (const char *, unsigned);
static tree build_var (tree, tree, int);
static void build_fn_info_type (tree, unsigned, tree);
static void build_info_type (tree, tree);
static tree build_fn_info (const struct coverage_data *, tree, tree);
static tree build_info (tree, tree);
static bool coverage_obj_init (void);
static VEC(constructor_elt,gc) *coverage_obj_fn
(VEC(constructor_elt,gc) *, tree, struct coverage_data const *);
static void coverage_obj_finish (VEC(constructor_elt,gc) *);
static char * get_da_file_name (const char *);
static void init_pmu_profiling (void);
static bool profiling_enabled_p (void);
static tree build_gcov_module_info_type (void);

/* Return the type node for gcov_type.  */

tree
get_gcov_type (void)
{
  return lang_hooks.types.type_for_size (GCOV_TYPE_SIZE, false);
}

/* Return the type node for gcov_unsigned_t.  */

tree
get_gcov_unsigned_t (void)
{
  return lang_hooks.types.type_for_size (32, true);
}

/* Return the type node for const char *.  */

static tree
get_const_string_type (void)
{
  return build_pointer_type
    (build_qualified_type (char_type_node, TYPE_QUAL_CONST));
}

static hashval_t
htab_counts_entry_hash (const void *of)
{
  const counts_entry_t *const entry = (const counts_entry_t *) of;

  return entry->ident * GCOV_COUNTERS + entry->ctr;
}

static int
htab_counts_entry_eq (const void *of1, const void *of2)
{
  const counts_entry_t *const entry1 = (const counts_entry_t *) of1;
  const counts_entry_t *const entry2 = (const counts_entry_t *) of2;

  return entry1->ident == entry2->ident && entry1->ctr == entry2->ctr;
}

static void
htab_counts_entry_del (void *of)
{
  counts_entry_t *const entry = (counts_entry_t *) of;

  /* When rebuilding counts_hash, we will reuse the entry.  */
  if (!rebuilding_counts_hash)
    {
      free (entry->counts);
      free (entry);
    }
}

/* Returns true if MOD_ID is the id of the last source module.  */

int
is_last_module (unsigned mod_id)
{
  return (mod_id == module_infos[num_in_fnames - 1]->ident);
}

/* String hash function  */

static hashval_t
str_hash (const void *p)
{
  const char *s = (const char *)p;
  return htab_hash_string (s);
}

/* String equal function  */

static int
str_eq (const void *p1, const void *p2)
{
  const char *s1 = (const char *)p1;
  const char *s2 = (const char *)p2;

  return !strcmp (s1, s2);
}


/* Returns true if the command-line arguments stored in the given module-infos
   are incompatible.  */
static bool
incompatible_cl_args (struct gcov_module_info* mod_info1,
		      struct gcov_module_info* mod_info2)
{
  char **warning_opts1 = XNEWVEC (char *, mod_info1->num_cl_args);
  char **warning_opts2 = XNEWVEC (char *, mod_info2->num_cl_args);
  char **non_warning_opts1 = XNEWVEC (char *, mod_info1->num_cl_args);
  char **non_warning_opts2 = XNEWVEC (char *, mod_info2->num_cl_args);
  unsigned int i, num_warning_opts1 = 0, num_warning_opts2 = 0;
  unsigned int num_non_warning_opts1 = 0, num_non_warning_opts2 = 0;
  bool warning_mismatch = false;
  bool non_warning_mismatch = false;
  bool with_fexceptions1 = true;
  bool with_fexceptions2 = true;
  htab_t option_tab1, option_tab2;
  unsigned int start_index1 = mod_info1->num_quote_paths +
    mod_info1->num_bracket_paths + mod_info1->num_cpp_defines +
    mod_info1->num_cpp_includes;
  unsigned int start_index2 = mod_info2->num_quote_paths +
    mod_info2->num_bracket_paths + mod_info2->num_cpp_defines +
    mod_info2->num_cpp_includes;

  option_tab1 = htab_create (10, str_hash, str_eq, NULL);
  option_tab2 = htab_create (10, str_hash, str_eq, NULL);

  /* First, separate the warning and non-warning options.  */
  for (i = 0; i < mod_info1->num_cl_args; i++)
    if (mod_info1->string_array[start_index1 + i][1] == 'W')
      warning_opts1[num_warning_opts1++] =
	mod_info1->string_array[start_index1 + i];
    else
      {
        void **slot;
        char* option_string = mod_info1->string_array[start_index1 + i];

        if (!strcmp ("-fexceptions", option_string))
          with_fexceptions1 = true;
        else if (!strcmp ("-fno-exceptions", option_string))
          with_fexceptions1 = false;

        slot = htab_find_slot (option_tab1, option_string, INSERT);
        if (!*slot)
          {
            *slot = option_string;
            non_warning_opts1[num_non_warning_opts1++] = option_string;
          }
      }

  for (i = 0; i < mod_info2->num_cl_args; i++)
    if (mod_info2->string_array[start_index2 + i][1] == 'W')
      warning_opts2[num_warning_opts2++] =
	mod_info2->string_array[start_index2 + i];
    else
      {
        void **slot;
        char* option_string = mod_info2->string_array[start_index2 + i];

        if (!strcmp ("-fexceptions", option_string))
          with_fexceptions2 = true;
        else if (!strcmp ("-fno-exceptions", option_string))
          with_fexceptions2 = false;
        slot = htab_find_slot (option_tab2, option_string, INSERT);
        if (!*slot)
          {
            *slot = option_string;
            non_warning_opts2[num_non_warning_opts2++] = option_string;
          }
      }

  /* Compare warning options. If these mismatch, we emit a warning.  */
  if (num_warning_opts1 != num_warning_opts2)
    warning_mismatch = true;
  else
    for (i = 0; i < num_warning_opts1 && !warning_mismatch; i++)
      warning_mismatch = strcmp (warning_opts1[i], warning_opts2[i]) != 0;

  /* Compare non-warning options. If these mismatch, we emit a warning, and if
     -fripa-disallow-opt-mismatch is supplied, the two modules are also
     incompatible.  */
  if (num_non_warning_opts1 != num_non_warning_opts2)
    non_warning_mismatch = true;
  else
    for (i = 0; i < num_non_warning_opts1 && !non_warning_mismatch; i++)
      non_warning_mismatch =
	strcmp (non_warning_opts1[i], non_warning_opts2[i]) != 0;

  if (warn_ripa_opt_mismatch && (warning_mismatch || non_warning_mismatch))
    warning (OPT_Wripa_opt_mismatch, "command line arguments mismatch for %s "
	     "and %s", mod_info1->source_filename, mod_info2->source_filename);

   if (warn_ripa_opt_mismatch && non_warning_mismatch 
       && (flag_opt_info >= OPT_INFO_MED))
     {
       inform (UNKNOWN_LOCATION, "Options for %s", mod_info1->source_filename);
       for (i = 0; i < num_non_warning_opts1; i++)
         inform (UNKNOWN_LOCATION, non_warning_opts1[i]);
       inform (UNKNOWN_LOCATION, "Options for %s", mod_info2->source_filename);
       for (i = 0; i < num_non_warning_opts2; i++)
         inform (UNKNOWN_LOCATION, non_warning_opts2[i]);
     }

  XDELETEVEC (warning_opts1);
  XDELETEVEC (warning_opts2);
  XDELETEVEC (non_warning_opts1);
  XDELETEVEC (non_warning_opts2);
  htab_delete (option_tab1);
  htab_delete (option_tab2);
  return ((flag_ripa_disallow_opt_mismatch && non_warning_mismatch)
          || (with_fexceptions1 != with_fexceptions2));
}

/* Read in the counts file, if available. DA_FILE_NAME is the
   name of the gcda file, and MODULE_ID is the module id of the
   associated source module.  */

static void
read_counts_file (const char *da_file_name, unsigned module_id)
{
  gcov_unsigned_t fn_ident = 0;
  struct gcov_summary summary;
  unsigned new_summary = 1;
  gcov_unsigned_t tag;
  int is_error = 0;
  unsigned module_infos_read = 0;
  struct pointer_set_t *modset = 0;
  unsigned max_group = PARAM_VALUE (PARAM_MAX_LIPO_GROUP);
  unsigned lineno_checksum = 0;
  unsigned cfg_checksum = 0;

  if (max_group == 0)
    max_group = (unsigned) -1;

  if (!gcov_open (da_file_name, 1))
    {
      if (PARAM_VALUE (PARAM_GCOV_DEBUG))
        {
          /* Try to find .gcda file in the current working dir.  */
          da_file_name = lbasename (da_file_name);
          if (!gcov_open (da_file_name, 1))
            return;
        }
      else
        return;
    }

  if (!gcov_magic (gcov_read_unsigned (), GCOV_DATA_MAGIC))
    {
      warning (0, "%qs is not a gcov data file", da_file_name);
      gcov_close ();
      return;
    }
  else if ((tag = gcov_read_unsigned ()) != GCOV_VERSION)
    {
      char v[4], e[4];

      GCOV_UNSIGNED2STRING (v, tag);
      GCOV_UNSIGNED2STRING (e, GCOV_VERSION);

      warning (0, "%qs is version %q.*s, expected version %q.*s",
 	       da_file_name, 4, v, 4, e);
      gcov_close ();
      return;
    }

  /* Read and discard the stamp.  */
  gcov_read_unsigned ();

  if (!counts_hash)
    counts_hash = htab_create (10,
			       htab_counts_entry_hash, htab_counts_entry_eq,
			       htab_counts_entry_del);
  while ((tag = gcov_read_unsigned ()))
    {
      gcov_unsigned_t length;
      gcov_position_t offset;

      length = gcov_read_unsigned ();
      offset = gcov_position ();
      if (tag == GCOV_TAG_FUNCTION)
	{
	  if (length)
	    {
	      fn_ident = gcov_read_unsigned ();
	      lineno_checksum = gcov_read_unsigned ();
	      cfg_checksum = gcov_read_unsigned ();
	    }
	  else
	    fn_ident = lineno_checksum = cfg_checksum = 0;
	  new_summary = 1;
	}
      else if (tag == GCOV_TAG_PROGRAM_SUMMARY)
	{
	  struct gcov_summary sum;
	  unsigned ix;

	  if (new_summary)
	    memset (&summary, 0, sizeof (summary));

	  gcov_read_summary (&sum);
	  for (ix = 0; ix != GCOV_COUNTERS_SUMMABLE; ix++)
	    {
	      summary.ctrs[ix].num_hot_counters
                  += sum.ctrs[ix].num_hot_counters;
	      summary.ctrs[ix].runs += sum.ctrs[ix].runs;
	      summary.ctrs[ix].sum_all += sum.ctrs[ix].sum_all;
	      if (summary.ctrs[ix].run_max < sum.ctrs[ix].run_max)
		summary.ctrs[ix].run_max = sum.ctrs[ix].run_max;
	      summary.ctrs[ix].sum_max += sum.ctrs[ix].sum_max;
	    }
	  new_summary = 0;
	}
      else if (GCOV_TAG_IS_COUNTER (tag) && fn_ident)
	{
	  counts_entry_t **slot, *entry, elt;
	  unsigned n_counts = GCOV_TAG_COUNTER_NUM (length);
	  unsigned ix;

	  elt.ident = GEN_FUNC_GLOBAL_ID (module_id, fn_ident);
	  elt.ctr = GCOV_COUNTER_FOR_TAG (tag);

	  slot = (counts_entry_t **) htab_find_slot
	    (counts_hash, &elt, INSERT);
	  entry = *slot;
	  if (!entry)
	    {
	      *slot = entry = XCNEW (counts_entry_t);
	      entry->ident = elt.ident;
	      entry->ctr = elt.ctr;
	      entry->lineno_checksum = lineno_checksum;
	      entry->cfg_checksum = cfg_checksum;
	      entry->summary = summary.ctrs[elt.ctr];
	      entry->summary.num = n_counts;
	      entry->counts = XCNEWVEC (gcov_type, n_counts);
	    }
	  else if (entry->lineno_checksum != lineno_checksum
		   || entry->cfg_checksum != cfg_checksum)
	    {
	      error ("Profile data for function %u is corrupted", fn_ident);
	      error ("checksum is (%x,%x) instead of (%x,%x)",
		     entry->lineno_checksum, entry->cfg_checksum,
		     lineno_checksum, cfg_checksum);
	      htab_delete (counts_hash);
	      break;
	    }
	  else if (entry->summary.num != n_counts)
	    {
	      error ("Profile data for function %u is corrupted", fn_ident);
	      error ("number of counters is %d instead of %d", entry->summary.num, n_counts);
	      htab_delete (counts_hash);
	      break;
	    }
	  else if (elt.ctr >= GCOV_COUNTERS_SUMMABLE)
	    {
	      error ("cannot merge separate %s counters for function %u",
		     ctr_names[elt.ctr], fn_ident);
	      goto skip_merge;
	    }
	  else
	    {
	      entry->summary.runs += summary.ctrs[elt.ctr].runs;
	      entry->summary.sum_all += summary.ctrs[elt.ctr].sum_all;
	      if (entry->summary.run_max < summary.ctrs[elt.ctr].run_max)
		entry->summary.run_max = summary.ctrs[elt.ctr].run_max;
	      entry->summary.sum_max += summary.ctrs[elt.ctr].sum_max;
	    }
	  for (ix = 0; ix != n_counts; ix++)
	    entry->counts[ix] += gcov_read_counter ();
	skip_merge:;
	}
      /* Skip the MODULE_INFO records if not in dyn-ipa mode, or when reading
	 auxiliary modules.  */
      else if (tag == GCOV_TAG_MODULE_INFO && flag_dyn_ipa && !module_id)
        {
	  struct gcov_module_info* mod_info;
          size_t info_sz;
          /* each string has at least 8 bytes, so MOD_INFO's
             persistent length >= in core size.  */
          mod_info
              = (struct gcov_module_info *) alloca ((length + 2)
                                                    * sizeof (gcov_unsigned_t));
	  gcov_read_module_info (mod_info, length);
          info_sz = (sizeof (struct gcov_module_info) +
		     sizeof (void *) * (mod_info->num_quote_paths +
					mod_info->num_bracket_paths +
					mod_info->num_cpp_defines +
					mod_info->num_cpp_includes +
					mod_info->num_cl_args));
	  /* The first MODULE_INFO record must be for the primary module.  */
	  if (module_infos_read == 0)
	    {
	      gcc_assert (mod_info->is_primary && !modset);
	      module_infos_read++;
              modset = pointer_set_create ();
              pointer_set_insert (modset, (void *)(size_t)mod_info->ident);
	      primary_module_id = mod_info->ident;
              module_infos = XCNEWVEC (struct gcov_module_info *, 1);
              module_infos[0] = XCNEWVAR (struct gcov_module_info, info_sz);
              memcpy (module_infos[0], mod_info, info_sz);
	    }
	  else
            {
	      int fd;
	      char *aux_da_filename = get_da_file_name (mod_info->da_filename);
              gcc_assert (!mod_info->is_primary);
              if (pointer_set_insert (modset, (void *)(size_t)mod_info->ident))
                {
                  if (flag_opt_info >= OPT_INFO_MAX)
                    inform (input_location, "Not importing %s: already imported",
                            mod_info->source_filename);
                }
              else if ((module_infos[0]->lang & GCOV_MODULE_LANG_MASK) !=
                       (mod_info->lang & GCOV_MODULE_LANG_MASK))
                {
                  if (flag_opt_info >= OPT_INFO_MAX)
                    inform (input_location, "Not importing %s: source language"
                            " different from primary module's source language",
                            mod_info->source_filename);
                }
              else if (module_infos_read == max_group)
                {
                  if (flag_opt_info >= OPT_INFO_MAX)
                    inform (input_location, "Not importing %s: maximum group"
                            " size reached", mod_info->source_filename);
                }
              else if (incompatible_cl_args (module_infos[0], mod_info))
                {
                  if (flag_opt_info >= OPT_INFO_MAX)
                    inform (input_location, "Not importing %s: command-line"
                            " arguments not compatible with primary module",
                            mod_info->source_filename);
                }
              else if ((fd = open (aux_da_filename, O_RDONLY)) < 0)
                {
                  if (flag_opt_info >= OPT_INFO_MAX)
                    inform (input_location, "Not importing %s: couldn't open %s",
                            mod_info->source_filename, aux_da_filename);
                }
              else if ((mod_info->lang & GCOV_MODULE_ASM_STMTS)
                       && flag_ripa_disallow_asm_modules)
                {
                  if (flag_opt_info >= OPT_INFO_MAX)
                    inform (input_location, "Not importing %s: contains "
                            "assembler statements", mod_info->source_filename);
                }
              else
		{
		  close (fd);
		  module_infos_read++;
		  add_input_filename (mod_info->source_filename);
		  module_infos = XRESIZEVEC (struct gcov_module_info *,
					     module_infos, num_in_fnames);
		  gcc_assert (num_in_fnames == module_infos_read);
		  module_infos[module_infos_read - 1]
		    = XCNEWVAR (struct gcov_module_info, info_sz);
		  memcpy (module_infos[module_infos_read - 1], mod_info,
			  info_sz);
		}
            }

          if (flag_opt_info >= OPT_INFO_MAX)
            {
              inform (input_location,
                      "MODULE Id=%d, Is_Primary=%s,"
                      " Is_Exported=%s, Name=%s (%s)",
                      mod_info->ident, mod_info->is_primary?"yes":"no",
                      mod_info->is_exported?"yes":"no", mod_info->source_filename,
                      mod_info->da_filename);
            }
        }
      gcov_sync (offset, length);
      if ((is_error = gcov_is_error ()))
	{
	  error (is_error < 0 ? "%qs has overflowed" : "%qs is corrupted",
		 da_file_name);
	  htab_delete (counts_hash);
	  break;
	}
    }

  /* TODO: profile based multiple module compilation does not work
     together with command line (-combine) based ipo -- add a nice
     warning and bail out instead of asserting.  */

  if (modset)
    pointer_set_destroy (modset);
  gcc_assert (module_infos_read == 0
              || module_infos_read == num_in_fnames);

  if (flag_dyn_ipa)
    gcc_assert (primary_module_id && num_in_fnames >= 1);

  gcov_close ();
}

/* Returns the coverage data entry for counter type COUNTER of function
   FUNC. EXPECTED is the number of expected counter entries.  */

static counts_entry_t *
get_coverage_counts_entry (struct function *func, unsigned counter)
{
  counts_entry_t *entry, elt;

  elt.ident = FUNC_DECL_GLOBAL_ID (func);
  elt.ctr = counter;
  entry = (counts_entry_t *) htab_find (counts_hash, &elt);

  return entry;
}

/* Returns the counters for a particular tag.  */

gcov_type *
get_coverage_counts (unsigned counter, unsigned expected,
                     unsigned cfg_checksum, unsigned lineno_checksum,
		     const struct gcov_ctr_summary **summary)
{
  counts_entry_t *entry;

  /* No hash table, no counts.  */
  if (!counts_hash)
    {
      static int warned = 0;

      if ((flag_opt_info >= OPT_INFO_MIN) && !warned++)
	inform (input_location, (flag_guess_branch_prob
		 ? "file %s not found, execution counts estimated"
		 : "file %s not found, execution counts assumed to be zero"),
		da_file_name);
      return NULL;
    }

  entry = get_coverage_counts_entry (cfun, counter);

  if (!entry || !entry->summary.num)
    {
      if ((flag_opt_info >= OPT_INFO_MIN) && !flag_dyn_ipa)
	warning (0, "no coverage for function %qE found",
		 DECL_ASSEMBLER_NAME (current_function_decl));
      return NULL;
    }

  if (entry->cfg_checksum != cfg_checksum
      || entry->summary.num != expected)
    {
      static int warned = 0;
      bool warning_printed = false;
      tree id = DECL_ASSEMBLER_NAME (current_function_decl);

      warning_printed =
	warning_at (input_location, OPT_Wcoverage_mismatch,
		    "the control flow of function %qE does not match "
		    "its profile data (counter %qs)", id, ctr_names[counter]);
      if ((flag_opt_info >= OPT_INFO_MIN) && warning_printed)
	{
	 inform (input_location, "use -Wno-error=coverage-mismatch to tolerate "
	 	 "the mismatch but performance may drop if the function is hot");
	  
	  if (!seen_error ()
	      && !warned++)
	    {
	      inform (input_location, "coverage mismatch ignored");
	      inform (input_location, flag_guess_branch_prob
		      ? G_("execution counts estimated")
		      : G_("execution counts assumed to be zero"));
	      if (!flag_guess_branch_prob)
		inform (input_location,
			"this can result in poorly optimized code");
	    }
	}

      return NULL;
    }
    else if (entry->lineno_checksum != lineno_checksum)
      {
        warning (OPT_Wcoverage_mismatch,
                 "Source location for function %qE have changed,"
                 " the profile data may be out of date",
                 DECL_ASSEMBLER_NAME (current_function_decl));
      }

  if (summary)
    *summary = &entry->summary;

  return entry->counts;
}

/* Returns the coverage data entry for counter type COUNTER of function
   FUNC. On return, *N_COUNTS is set to the number of entries in the counter.  */

gcov_type *
get_coverage_counts_no_warn (struct function *f, unsigned counter, unsigned *n_counts)
{
  counts_entry_t *entry, elt;

  /* No hash table, no counts.  */
  if (!counts_hash || !f)
    return NULL;

  elt.ident = FUNC_DECL_GLOBAL_ID (f);
  elt.ctr = counter;
  entry = (counts_entry_t *) htab_find (counts_hash, &elt);
  if (!entry)
    return NULL;

  *n_counts = entry->summary.num;
  return entry->counts;
}

/* Allocate NUM counters of type COUNTER. Returns nonzero if the
   allocation succeeded.  */

int
coverage_counter_alloc (unsigned counter, unsigned num)
{
  if (no_coverage)
    return 0;

  if (!num)
    return 1;

  if (!fn_v_ctrs[counter])
    {
      tree array_type = build_array_type (get_gcov_type (), NULL_TREE);

      fn_v_ctrs[counter]
	= build_var (current_function_decl, array_type, counter);
    }

  fn_b_ctrs[counter] = fn_n_ctrs[counter];
  fn_n_ctrs[counter] += num;
  
  fn_ctr_mask |= 1 << counter;
  return 1;
}

/* Generate a tree to access COUNTER NO.  */

tree
tree_coverage_counter_ref (unsigned counter, unsigned no)
{
  tree gcov_type_node = get_gcov_type ();

  gcc_assert (no < fn_n_ctrs[counter] - fn_b_ctrs[counter]);

  no += fn_b_ctrs[counter];
  
  /* "no" here is an array index, scaled to bytes later.  */
  return build4 (ARRAY_REF, gcov_type_node, fn_v_ctrs[counter],
		 build_int_cst (integer_type_node, no), NULL, NULL);
}

/* Generate a tree to access the address of COUNTER NO.  */

tree
tree_coverage_counter_addr (unsigned counter, unsigned no)
{
  tree gcov_type_node = get_gcov_type ();

  gcc_assert (no < fn_n_ctrs[counter] - fn_b_ctrs[counter]);
  no += fn_b_ctrs[counter];

  /* "no" here is an array index, scaled to bytes later.  */
  return build_fold_addr_expr (build4 (ARRAY_REF, gcov_type_node,
				       fn_v_ctrs[counter],
				       build_int_cst (integer_type_node, no),
				       NULL, NULL));
}


/* Generate a checksum for a string.  CHKSUM is the current
   checksum.  */

static unsigned
coverage_checksum_string (unsigned chksum, const char *string)
{
  int i;
  char *dup = NULL;

  /* Look for everything that looks if it were produced by
     get_file_function_name and zero out the second part
     that may result from flag_random_seed.  This is not critical
     as the checksums are used only for sanity checking.  */
  for (i = 0; string[i]; i++)
    {
      int offset = 0;
      if (!strncmp (string + i, "_GLOBAL__N_", 11))
      offset = 11;
      if (!strncmp (string + i, "_GLOBAL__", 9))
      offset = 9;

      /* C++ namespaces do have scheme:
         _GLOBAL__N_<filename>_<wrongmagicnumber>_<magicnumber>functionname
       since filename might contain extra underscores there seems
       to be no better chance then walk all possible offsets looking
       for magicnumber.  */
      if (offset)
	{
	  for (i = i + offset; string[i]; i++)
	    if (string[i]=='_')
	      {
		int y;

		for (y = 1; y < 9; y++)
		  if (!(string[i + y] >= '0' && string[i + y] <= '9')
		      && !(string[i + y] >= 'A' && string[i + y] <= 'F'))
		    break;
		if (y != 9 || string[i + 9] != '_')
		  continue;
		for (y = 10; y < 18; y++)
		  if (!(string[i + y] >= '0' && string[i + y] <= '9')
		      && !(string[i + y] >= 'A' && string[i + y] <= 'F'))
		    break;
		if (y != 18)
		  continue;
		if (!dup)
		  string = dup = xstrdup (string);
		for (y = 10; y < 18; y++)
		  dup[i + y] = '0';
	      }
	  break;
	}
    }

  chksum = crc32_string (chksum, string);
  if (dup)
    free (dup);

  return chksum;
}

/* Compute checksum for the current function.  We generate a CRC32.  */

unsigned
coverage_compute_lineno_checksum (void)
{
  tree name;
  expanded_location xloc
    = expand_location (DECL_SOURCE_LOCATION (current_function_decl));
  unsigned chksum = xloc.line;
  const char *pathless_filename = xloc.file;
  int i;
  for (i = strlen (xloc.file); i >= 0; i--)
    if (IS_DIR_SEPARATOR (pathless_filename[i]))
      {
	pathless_filename += i + 1;
	break;
      }

  chksum = coverage_checksum_string (chksum, pathless_filename);

  /* Note: it is a bad design that C++ FE associate the convertion function type
     with the name of the decl. This leads to cross contamination between different
     conversion operators in different modules (If conv_type_names map is cleared
     at the end of parsing of each module).  */
  if (flag_dyn_ipa && lang_hooks.user_conv_function_p (current_function_decl))
    name = DECL_ASSEMBLER_NAME (current_function_decl);
  else
    name = DECL_NAME (current_function_decl);

  chksum = coverage_checksum_string
      (chksum, IDENTIFIER_POINTER (name));

  return chksum;
}

/* Compute cfg checksum for the current function.
   The checksum is calculated carefully so that
   source code changes that doesn't affect the control flow graph
   won't change the checksum.
   This is to make the profile data useable across source code change.
   The downside of this is that the compiler may use potentially
   wrong profile data - that the source code change has non-trivial impact
   on the validity of profile data (e.g. the reversed condition)
   but the compiler won't detect the change and use the wrong profile data.  */

unsigned
coverage_compute_cfg_checksum (void)
{
  basic_block bb;
  unsigned chksum = n_basic_blocks;

  FOR_EACH_BB (bb)
    {
      edge e;
      edge_iterator ei;
      chksum = crc32_byte (chksum, bb->index);
      FOR_EACH_EDGE (e, ei, bb->succs)
        {
          chksum = crc32_byte (chksum, e->dest->index);
        }
    }

  return chksum;
}

/* Begin output to the graph file for the current function.
   Writes the function header. Returns nonzero if data should be output.  */

int
coverage_begin_function (unsigned lineno_checksum, unsigned cfg_checksum)
{
  expanded_location xloc;
  unsigned long offset;

  /* We don't need to output .gcno file unless we're under -ftest-coverage
     (e.g. -fprofile-arcs/generate/use don't need .gcno to work). */
  if (no_coverage || !bbg_file_name)
    return 0;

  xloc = expand_location (DECL_SOURCE_LOCATION (current_function_decl));

  /* Announce function */
  offset = gcov_write_tag (GCOV_TAG_FUNCTION);
  gcov_write_unsigned (FUNC_DECL_FUNC_ID (cfun));
  gcov_write_unsigned (lineno_checksum);
  gcov_write_unsigned (cfg_checksum);
  gcov_write_string (IDENTIFIER_POINTER
		     (DECL_ASSEMBLER_NAME (current_function_decl)));
  gcov_write_string (xloc.file);
  gcov_write_unsigned (xloc.line);
  gcov_write_length (offset);

  return !gcov_is_error ();
}

/* Finish coverage data for the current function. Verify no output
   error has occurred.  Save function coverage counts.  */

void
coverage_end_function (unsigned lineno_checksum, unsigned cfg_checksum)
{
  unsigned i;

  if (bbg_file_name && gcov_is_error ())
    {
      warning (0, "error writing %qs", bbg_file_name);
      unlink (bbg_file_name);
      bbg_file_name = NULL;
    }

  if (fn_ctr_mask)
    {
      struct coverage_data *item = 0;

      /* If the function is extern (i.e. extern inline), then we won't
	 be outputting it, so don't chain it onto the function
	 list.  */
      if (!DECL_EXTERNAL (current_function_decl))
	{
	  item = ggc_alloc_coverage_data ();
	  
	  item->ident = FUNC_DECL_FUNC_ID (cfun);
	  item->lineno_checksum = lineno_checksum;
	  item->cfg_checksum = cfg_checksum;

	  item->fn_decl = current_function_decl;
	  item->next = 0;
	  *functions_tail = item;
	  functions_tail = &item->next;
	}

      for (i = 0; i != GCOV_COUNTERS; i++)
	{
	  tree var = fn_v_ctrs[i];

	  if (item)
	    item->ctr_vars[i] = var;
	  if (var)
	    {
	      tree array_type = build_index_type (size_int (fn_n_ctrs[i] - 1));
	      array_type = build_array_type (get_gcov_type (), array_type);
	      TREE_TYPE (var) = array_type;
	      DECL_SIZE (var) = TYPE_SIZE (array_type);
	      DECL_SIZE_UNIT (var) = TYPE_SIZE_UNIT (array_type);
	      varpool_finalize_decl (var);
	    }
	  
	  fn_b_ctrs[i] = fn_n_ctrs[i] = 0;
	  fn_v_ctrs[i] = NULL_TREE;
	}
      prg_ctr_mask |= fn_ctr_mask;
      fn_ctr_mask = 0;
    }
}

/* True if a function entry corresponding to the given FN_IDENT
   is present in the coverage internal data structures.  */

bool
coverage_function_present (unsigned fn_ident)
{
  struct coverage_data *item = functions_head;
  while (item && item->ident != fn_ident)
    item = item->next;
  return item != NULL;
}

/* Update function and program direct-call coverage counts.  */

void
coverage_dc_end_function (void)
{
  tree var;

  if (fn_ctr_mask)
    {
      const unsigned idx = GCOV_COUNTER_DIRECT_CALL;
      struct coverage_data *item = functions_head;
      while (item && item->ident != (unsigned) FUNC_DECL_FUNC_ID (cfun))
	item = item->next;

      /* If a matching function entry hasn't been found, either this function
	 has had no coverage counts added in the profile pass, or this
	 is a new function (function versioning, etc). Create a new entry.  */
      if (!item)
	{
          int cnt;

	  item = ggc_alloc_coverage_data ();
	  *functions_tail = item;
	  functions_tail = &item->next;
	  item->next = 0;
	  item->ident = FUNC_DECL_FUNC_ID (cfun);
	  item->fn_decl = current_function_decl;
	  item->lineno_checksum = coverage_compute_lineno_checksum ();
	  item->cfg_checksum = coverage_compute_cfg_checksum ();
          for (cnt = 0; cnt < GCOV_COUNTERS; cnt++)
            item->ctr_vars[cnt] = NULL_TREE;
	}

      var = fn_v_ctrs[idx];
      item->ctr_vars[idx] = var;
      if (var)
        {
          tree array_type = build_index_type (size_int (fn_n_ctrs[idx] - 1));
          array_type = build_array_type (get_gcov_type (), array_type);
          TREE_TYPE (var) = array_type;
          DECL_SIZE (var) = TYPE_SIZE (array_type);
          DECL_SIZE_UNIT (var) = TYPE_SIZE_UNIT (array_type);
          varpool_finalize_decl (var);
        }

      fn_n_ctrs[idx] = fn_b_ctrs[idx] = 0;
      fn_v_ctrs[idx] = NULL_TREE;
      prg_ctr_mask |= fn_ctr_mask;
      fn_ctr_mask = 0;
    }
}

/* Build a coverage variable of TYPE for function FN_DECL.  If COUNTER
   >= 0 it is a counter array, otherwise it is the function structure.  */

static tree
build_var (tree fn_decl, tree type, int counter)
{
  tree var = build_decl (BUILTINS_LOCATION, VAR_DECL, NULL_TREE, type);
  const char *fn_name = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (fn_decl));
  char *buf;
  size_t fn_name_len, len;

  fn_name = targetm.strip_name_encoding (fn_name);
  fn_name_len = strlen (fn_name);
  buf = XALLOCAVEC (char, fn_name_len + 8 + sizeof (int) * 3);

  if (counter < 0)
    strcpy (buf, "__gcov__");
  else
    sprintf (buf, "__gcov%u_", counter);
  len = strlen (buf);
#ifndef NO_DOT_IN_LABEL
  buf[len - 1] = '.';
#elif !defined NO_DOLLAR_IN_LABEL
  buf[len - 1] = '$';
#endif
  memcpy (buf + len, fn_name, fn_name_len + 1);
  DECL_NAME (var) = get_identifier (buf);
  TREE_STATIC (var) = 1;
  TREE_ADDRESSABLE (var) = 1;
  DECL_ALIGN (var) = TYPE_ALIGN (type);

  return var;
}

/* Creates the gcov_fn_info RECORD_TYPE.  */

static void
build_fn_info_type (tree type, unsigned counters, tree gcov_info_type)
{
  tree ctr_info = lang_hooks.types.make_type (RECORD_TYPE);
  tree field, fields;
  tree array_type;

  gcc_assert (counters);

  /* ctr_info::num */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  fields = field;

  /* ctr_info::values */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      build_pointer_type (get_gcov_type ()));
  DECL_CHAIN (field) = fields;
  fields = field;

  finish_builtin_struct (ctr_info, "__gcov_ctr_info", fields, NULL_TREE);

  /* key */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      build_pointer_type (build_qualified_type
					  (gcov_info_type, TYPE_QUAL_CONST)));
  fields = field;

  /* ident */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* lineno_checksum */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* cfg checksum */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  array_type = build_index_type (size_int (counters - 1));
  array_type = build_array_type (ctr_info, array_type);

  /* counters */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE, array_type);
  DECL_CHAIN (field) = fields;
  fields = field;

  finish_builtin_struct (type, "__gcov_fn_info", fields, NULL_TREE);
}

/* Returns a CONSTRUCTOR for a gcov_fn_info.  DATA is
   the coverage data for the function and TYPE is the gcov_fn_info
   RECORD_TYPE.  KEY is the object file key.  */

static tree
build_fn_info (const struct coverage_data *data, tree type, tree key)
{
  tree fields = TYPE_FIELDS (type);
  tree ctr_type;
  unsigned ix;
  VEC(constructor_elt,gc) *v1 = NULL;
  VEC(constructor_elt,gc) *v2 = NULL;

  /* key */
  CONSTRUCTOR_APPEND_ELT (v1, fields,
			  build1 (ADDR_EXPR, TREE_TYPE (fields), key));
  fields = DECL_CHAIN (fields);
  
  /* ident */
  CONSTRUCTOR_APPEND_ELT (v1, fields,
			  build_int_cstu (get_gcov_unsigned_t (),
					  data->ident));
  fields = DECL_CHAIN (fields);

  /* lineno_checksum */
  CONSTRUCTOR_APPEND_ELT (v1, fields,
			  build_int_cstu (get_gcov_unsigned_t (),
					  data->lineno_checksum));
  fields = DECL_CHAIN (fields);

  /* cfg_checksum */
  CONSTRUCTOR_APPEND_ELT (v1, fields,
			  build_int_cstu (get_gcov_unsigned_t (),
					  data->cfg_checksum));
  fields = DECL_CHAIN (fields);

  /* counters */
  ctr_type = TREE_TYPE (TREE_TYPE (fields));
  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    if (prg_ctr_mask & (1 << ix))
      {
	VEC(constructor_elt,gc) *ctr = NULL;
	tree var = data->ctr_vars[ix];
	unsigned count = 0;

	if (var)
	  count
	    = tree_low_cst (TYPE_MAX_VALUE (TYPE_DOMAIN (TREE_TYPE (var))), 0)
	    + 1;

	CONSTRUCTOR_APPEND_ELT (ctr, TYPE_FIELDS (ctr_type),
				build_int_cstu (get_gcov_unsigned_t (),
						count));

	if (var)
	  CONSTRUCTOR_APPEND_ELT (ctr, DECL_CHAIN (TYPE_FIELDS (ctr_type)),
				  build_fold_addr_expr (var));
	
	CONSTRUCTOR_APPEND_ELT (v2, NULL, build_constructor (ctr_type, ctr));

        /* In LIPO mode, coverage_finish is called late when pruning can not be done,
           so we need to force emitting counter variables even for eliminated functions
           to avoid unsat.  */
        if (flag_dyn_ipa && var)
          {
            varpool_mark_needed_node (varpool_node (var));
            varpool_finalize_decl (var);
          }
      }
  
  CONSTRUCTOR_APPEND_ELT (v1, fields,
			  build_constructor (TREE_TYPE (fields), v2));

  return build_constructor (type, v1);
}

/* Create gcov_info struct.  TYPE is the incomplete RECORD_TYPE to be
   completed, and FN_INFO_PTR_TYPE is a pointer to the function info type.  */

static void
build_info_type (tree type, tree fn_info_ptr_type)
{
  tree field, fields = NULL_TREE;
  tree merge_fn_type, mod_type;

  /* Version ident */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* mod_info */
  mod_type = build_gcov_module_info_type ();
  mod_type = build_pointer_type (mod_type);
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE, mod_type);
  DECL_CHAIN (field) = fields;
  fields = field;

  /* next pointer */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      build_pointer_type (build_qualified_type
					  (type, TYPE_QUAL_CONST)));
  DECL_CHAIN (field) = fields;
  fields = field;

  /* stamp */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Filename */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      build_pointer_type (build_qualified_type
					  (char_type_node, TYPE_QUAL_CONST)));
  DECL_CHAIN (field) = fields;
  fields = field;

  /* eof_pos */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* merge fn array */
  merge_fn_type
    = build_function_type_list (void_type_node,
				build_pointer_type (get_gcov_type ()),
				get_gcov_unsigned_t (), NULL_TREE);
  merge_fn_type
    = build_array_type (build_pointer_type (merge_fn_type),
			build_index_type (size_int (GCOV_COUNTERS - 1)));
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      merge_fn_type);
  DECL_CHAIN (field) = fields;
  fields = field;
  
  /* n_functions */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;
  
  /* function_info pointer pointer */
  fn_info_ptr_type = build_pointer_type
    (build_qualified_type (fn_info_ptr_type, TYPE_QUAL_CONST));
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      fn_info_ptr_type);
  DECL_CHAIN (field) = fields;
  fields = field;

  finish_builtin_struct (type, "__gcov_info", fields, NULL_TREE);
}

/* Compute an array (tree) of include path strings. STRING_TYPE is
   the path string type, INC_PATH_VALUE is the initial value of the
   path array, PATHS gives raw path string values, and NUM is the
   number of paths.  */

static void
build_inc_path_array_value (tree string_type, VEC(constructor_elt, gc) **v,
                            cpp_dir *paths, int num)
{
  int i;
  cpp_dir *pdir;
  for (i = 0, pdir = paths; i < num; pdir = pdir->next)
    {
      const char *path_raw_string;
      int path_string_length;
      tree path_string;
      path_raw_string = pdir->name;
      path_string_length = strlen (path_raw_string);
      path_string = build_string (path_string_length + 1, path_raw_string);
      TREE_TYPE (path_string) = build_array_type
          (char_type_node, build_index_type
           (build_int_cst (NULL_TREE, path_string_length)));
      CONSTRUCTOR_APPEND_ELT (*v, NULL,
                              build1 (ADDR_EXPR, string_type, path_string));
      i++;
    }
}

/* Compute an array (tree) of strings. STR_TYPE is the string type,
   STR_ARRAY_VALUE is the initial value of the string array, and HEAD gives
   the list of raw strings.  */

static void
build_str_array_value (tree str_type, VEC(constructor_elt, gc) **v,
                       struct str_list *head)
{
  const char *raw_str;
  int str_length;
  while (head)
    {
      tree str;
      raw_str = head->str;
      str_length = strlen (raw_str);
      str = build_string (str_length + 1, raw_str);
      TREE_TYPE (str) =
	build_array_type (char_type_node,
			  build_index_type (build_int_cst (NULL_TREE,
							   str_length)));
      CONSTRUCTOR_APPEND_ELT (*v, NULL,
                              build1 (ADDR_EXPR, str_type, str));
      head = head->next;
    }
  return;
}

/* Compute an array (tree) of command-line argument strings. STRING_TYPE is
   the string type, CL_ARGS_VALUE is the initial value of the command-line
   args array. */

static void
build_cl_args_array_value (tree string_type, VEC(constructor_elt, gc) **v)
{
  unsigned int i;

  for (i = 0; i < num_lipo_cl_args; i++)
    {
      int arg_length = strlen (lipo_cl_args[i]);
      tree arg_string = build_string (arg_length + 1, lipo_cl_args[i]);
      TREE_TYPE (arg_string) =
	build_array_type (char_type_node,
			  build_index_type (build_int_cst (NULL_TREE,
							   arg_length)));
      CONSTRUCTOR_APPEND_ELT (*v, NULL,
			       build1 (ADDR_EXPR, string_type, arg_string));
    }
  return;
}

/* Returns the type of the module info associated with the
   current source module being compiled.  */

static tree
build_gcov_module_info_type (void)
{
  tree type, field, fields = NULL_TREE;
  tree string_type, index_type, string_array_type;

  cpp_dir *quote_paths, *bracket_paths, *pdir;
  int num_quote_paths = 0, num_bracket_paths = 0;

  type = lang_hooks.types.make_type (RECORD_TYPE);
  string_type = build_pointer_type (
      build_qualified_type (char_type_node,
                            TYPE_QUAL_CONST));

  /* ident */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* is_primary */
  /* We also overload this field to store a flag that indicates whether this
     module was built in regular FDO or LIPO mode (-fripa). When reading this
     field from a GCDA file, it should be used as the IS_PRIMARY flag. When
     reading this field from the binary's data section, it should be used
     as a FDO/LIPO flag.  */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* is_exported */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* lang field */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* da_filename */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, string_type);
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Source name */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, string_type);
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Num quote paths  */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Num bracket paths  */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Num -D/-U options.  */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Num -imacro/-include options.  */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL, NULL_TREE,
		      get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  /* Num command-line args.  */
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
		      NULL_TREE, get_gcov_unsigned_t ());
  DECL_CHAIN (field) = fields;
  fields = field;

  get_include_chains (&quote_paths, &bracket_paths);
  for (pdir = quote_paths; pdir; pdir = pdir->next)
    {
      if (pdir == bracket_paths)
        break;
      num_quote_paths++;
    }
  for (pdir = bracket_paths; pdir; pdir = pdir->next)
    num_bracket_paths++;

  /* string array  */
  index_type = build_index_type (build_int_cst (NULL_TREE,
						num_quote_paths	+
						num_bracket_paths +
						num_cpp_defines +
						num_cpp_includes +
						num_lipo_cl_args));
  string_array_type = build_array_type (string_type, index_type);
  field = build_decl (BUILTINS_LOCATION, FIELD_DECL,
                      NULL_TREE, string_array_type);
  DECL_CHAIN (field) = fields;
  fields = field;

  finish_builtin_struct (type, "__gcov_module_info", fields, NULL_TREE);

  return type;
}

/* Returns the value of the module info associated with the
   current source module being compiled.  */

static tree
build_gcov_module_info_value (tree mod_type)
{
  tree info_fields, mod_info;
  tree value = NULL_TREE;
  int file_name_len;
  tree filename_string, string_array_type,  string_type;
  cpp_dir *quote_paths, *bracket_paths, *pdir;
  int num_quote_paths = 0, num_bracket_paths = 0;
  unsigned lang;
  char name_buf[50];
  VEC(constructor_elt,gc) *v = NULL, *path_v = NULL;

  info_fields = TYPE_FIELDS (mod_type);

  /* ident */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (), 0));

  info_fields = DECL_CHAIN (info_fields);

  /* is_primary */
  /* We also overload this field to store a flag that indicates whether this
     module was built in regular FDO or LIPO mode (-fripa). When reading this
     field from a GCDA file, it should be used as the IS_PRIMARY flag. When
     reading this field from the binary's data section, it should be used
     as a FDO/LIPO flag.  */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (),
                                          flag_dyn_ipa ? 1 : 0));
  info_fields = DECL_CHAIN (info_fields);

  /* is_exported */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (), 0));
  info_fields = DECL_CHAIN (info_fields);

  /* lang field */
  if (!strcmp (lang_hooks.name, "GNU C"))
    lang = GCOV_MODULE_C_LANG;
  else if (!strcmp (lang_hooks.name, "GNU C++"))
    lang = GCOV_MODULE_CPP_LANG;
  else
    lang = GCOV_MODULE_UNKNOWN_LANG;
  if (has_asm_statement)
    lang |= GCOV_MODULE_ASM_STMTS;

  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (), lang));
  info_fields = DECL_CHAIN (info_fields);

  /* da_filename */

  string_type = TREE_TYPE (info_fields);
  file_name_len = strlen (da_base_file_name);
  filename_string = build_string (file_name_len + 1, da_base_file_name);
  TREE_TYPE (filename_string) = build_array_type
    (char_type_node, build_index_type
     (build_int_cst (NULL_TREE, file_name_len)));
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build1 (ADDR_EXPR, string_type, filename_string));
  info_fields = DECL_CHAIN (info_fields);

  /* Source name */

  file_name_len = strlen (main_input_file_name);
  filename_string = build_string (file_name_len + 1, main_input_file_name);
  TREE_TYPE (filename_string) = build_array_type
    (char_type_node, build_index_type
     (build_int_cst (NULL_TREE, file_name_len)));
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build1 (ADDR_EXPR, string_type, filename_string));
  info_fields = DECL_CHAIN (info_fields);

  get_include_chains (&quote_paths, &bracket_paths);
  for (pdir = quote_paths; pdir; pdir = pdir->next)
    {
      if (pdir == bracket_paths)
        break;
      num_quote_paths++;
    }
  for (pdir = bracket_paths; pdir; pdir = pdir->next)
    num_bracket_paths++;

  /* Num quote paths  */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (),
                                          num_quote_paths));
  info_fields = DECL_CHAIN (info_fields);

  /* Num bracket paths  */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (),
                                          num_bracket_paths));
  info_fields = DECL_CHAIN (info_fields);

  /* Num -D/-U options.  */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (),
                                          num_cpp_defines));
  info_fields = DECL_CHAIN (info_fields);

  /* Num -imacro/-include options.  */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (),
                                          num_cpp_includes));
  info_fields = DECL_CHAIN (info_fields);

  /* Num command-line args.  */
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_int_cstu (get_gcov_unsigned_t (),
                                          num_lipo_cl_args));
  info_fields = DECL_CHAIN (info_fields);

  /* string array  */
  string_array_type = TREE_TYPE (info_fields);
  build_inc_path_array_value (string_type, &path_v,
                              quote_paths, num_quote_paths);
  build_inc_path_array_value (string_type, &path_v,
                              bracket_paths, num_bracket_paths);
  build_str_array_value (string_type, &path_v,
                         cpp_defines_head);
  build_str_array_value (string_type, &path_v,
                         cpp_includes_head);
  build_cl_args_array_value (string_type, &path_v);
  CONSTRUCTOR_APPEND_ELT (v, info_fields,
                          build_constructor (string_array_type, path_v));
  info_fields = DECL_CHAIN (info_fields);

  gcc_assert (!info_fields);
  value = build_constructor (mod_type, v);

  mod_info = build_decl (BUILTINS_LOCATION, VAR_DECL,
                         NULL_TREE, TREE_TYPE (value));
  TREE_STATIC (mod_info) = 1;
  ASM_GENERATE_INTERNAL_LABEL (name_buf, "MODINFO", 0);
  DECL_NAME (mod_info) = get_identifier (name_buf);
  DECL_INITIAL (mod_info) = value;

  /* Build structure.  */
  varpool_finalize_decl (mod_info);

  return mod_info;
}

/* Returns a CONSTRUCTOR for the gcov_info object.  INFO_TYPE is the
   gcov_info structure type, FN_ARY is the array of pointers to
   function info objects.  */

static tree
build_info (tree info_type, tree fn_ary)
{
  tree info_fields = TYPE_FIELDS (info_type);
  tree merge_fn_type, n_funcs;
  unsigned ix;
  tree mod_value = NULL_TREE;
  tree filename_string;
  int da_file_name_len;
  VEC(constructor_elt,gc) *v1 = NULL;
  VEC(constructor_elt,gc) *v2 = NULL;

  /* Version ident */
  CONSTRUCTOR_APPEND_ELT (v1, info_fields,
			  build_int_cstu (TREE_TYPE (info_fields),
					  GCOV_VERSION));
  info_fields = DECL_CHAIN (info_fields);

  /* mod_info */
  mod_value = build_gcov_module_info_value (TREE_TYPE (TREE_TYPE (info_fields)));
  mod_value = build1 (ADDR_EXPR, TREE_TYPE (mod_value), mod_value);
  CONSTRUCTOR_APPEND_ELT (v1, info_fields, mod_value);
  info_fields = DECL_CHAIN (info_fields);

  /* next -- NULL */
  CONSTRUCTOR_APPEND_ELT (v1, info_fields, null_pointer_node);
  info_fields = DECL_CHAIN (info_fields);
  
  /* stamp */
  CONSTRUCTOR_APPEND_ELT (v1, info_fields,
			  build_int_cstu (TREE_TYPE (info_fields),
					  local_tick));
  info_fields = DECL_CHAIN (info_fields);

  /* Filename */
  da_file_name_len = strlen (da_file_name);
  filename_string = build_string (da_file_name_len + 1, da_file_name);
  TREE_TYPE (filename_string) = build_array_type
    (char_type_node, build_index_type (size_int (da_file_name_len)));
  CONSTRUCTOR_APPEND_ELT (v1, info_fields,
			  build1 (ADDR_EXPR, TREE_TYPE (info_fields),
				  filename_string));
  info_fields = DECL_CHAIN (info_fields);

  /* eof_pos */
  CONSTRUCTOR_APPEND_ELT (v1, info_fields,
                          build_int_cstu (TREE_TYPE (info_fields), 0));
  info_fields = DECL_CHAIN (info_fields);

  /* merge fn array -- NULL slots indicate unmeasured counters */
  merge_fn_type = TREE_TYPE (TREE_TYPE (info_fields));
  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    {
      tree ptr = null_pointer_node;

      if ((1u << ix) & prg_ctr_mask)
	{
	  tree merge_fn = build_decl (BUILTINS_LOCATION,
				      FUNCTION_DECL,
				      get_identifier (ctr_merge_functions[ix]),
				      TREE_TYPE (merge_fn_type));
	  DECL_EXTERNAL (merge_fn) = 1;
	  TREE_PUBLIC (merge_fn) = 1;
	  DECL_ARTIFICIAL (merge_fn) = 1;
	  TREE_NOTHROW (merge_fn) = 1;
	  /* Initialize assembler name so we can stream out. */
	  DECL_ASSEMBLER_NAME (merge_fn);
	  ptr = build1 (ADDR_EXPR, merge_fn_type, merge_fn);
	}
      CONSTRUCTOR_APPEND_ELT (v2, NULL, ptr);
    }
  CONSTRUCTOR_APPEND_ELT (v1, info_fields,
			  build_constructor (TREE_TYPE (info_fields), v2));
  info_fields = DECL_CHAIN (info_fields);

  /* n_functions */
  n_funcs = TYPE_MAX_VALUE (TYPE_DOMAIN (TREE_TYPE (fn_ary)));
  n_funcs = fold_build2 (PLUS_EXPR, TREE_TYPE (info_fields),
			 n_funcs, size_one_node);
  CONSTRUCTOR_APPEND_ELT (v1, info_fields, n_funcs);
  info_fields = DECL_CHAIN (info_fields);

  /* functions */
  CONSTRUCTOR_APPEND_ELT (v1, info_fields,
			  build1 (ADDR_EXPR, TREE_TYPE (info_fields), fn_ary));
  info_fields = DECL_CHAIN (info_fields);

  gcc_assert (!info_fields);
  return build_constructor (info_type, v1);
}

/* Create the gcov_info types and object.  Generate the constructor
   function to call __gcov_init.  Does not generate the initializer
   for the object.  Returns TRUE if coverage data is being emitted.  */

static bool
coverage_obj_init (void)
{
  tree gcov_info_type, ctor, stmt, init_fn;
  unsigned n_counters = 0;
  unsigned ix;
  struct coverage_data *fn;
  struct coverage_data **fn_prev;
  char name_buf[32];

  no_coverage = 1; /* Disable any further coverage.  */

  if (!prg_ctr_mask && !flag_pmu_profile_generate)
    return false;

  if (cgraph_dump_file)
    fprintf (cgraph_dump_file, "Using data file %s\n", da_file_name);

  /* Prune functions.  */
  if (!flag_dyn_ipa)
    /* in lipo mode, coverage_finish is called when function struct is cleared,
       so pruning code here will skip all functions.  */
    for (fn_prev = &functions_head; (fn = *fn_prev);)
      if (DECL_STRUCT_FUNCTION (fn->fn_decl))
        fn_prev = &fn->next;
      else
        /* The function is not being emitted, remove from list.  */
        *fn_prev = fn->next;

  for (ix = 0; ix != GCOV_COUNTERS; ix++)
    if ((1u << ix) & prg_ctr_mask)
      n_counters++;
  
  /* Build the info and fn_info types.  These are mutually recursive.  */
  gcov_info_type = lang_hooks.types.make_type (RECORD_TYPE);
  gcov_fn_info_type = lang_hooks.types.make_type (RECORD_TYPE);
  gcov_fn_info_ptr_type = build_pointer_type
    (build_qualified_type (gcov_fn_info_type, TYPE_QUAL_CONST));
  build_fn_info_type (gcov_fn_info_type, n_counters, gcov_info_type);
  build_info_type (gcov_info_type, gcov_fn_info_ptr_type);
  
  /* Build the gcov info var, this is referred to in its own
     initializer.  */
  gcov_info_var = build_decl (BUILTINS_LOCATION,
			      VAR_DECL, NULL_TREE, gcov_info_type);
  TREE_STATIC (gcov_info_var) = 1;
  ASM_GENERATE_INTERNAL_LABEL (name_buf, "LPBX", 0);
  DECL_NAME (gcov_info_var) = get_identifier (name_buf);

  /* Build a decl for __gcov_init.  */
  init_fn = build_pointer_type (gcov_info_type);
  init_fn = build_function_type_list (void_type_node, init_fn, NULL);
  init_fn = build_decl (BUILTINS_LOCATION, FUNCTION_DECL,
			get_identifier ("__gcov_init"), init_fn);
  TREE_PUBLIC (init_fn) = 1;
  DECL_EXTERNAL (init_fn) = 1;
  DECL_ASSEMBLER_NAME (init_fn);

  /* Generate a call to __gcov_init(&gcov_info).  */
  ctor = NULL;
  stmt = build_fold_addr_expr (gcov_info_var);
  stmt = build_call_expr (init_fn, 1, stmt);
  append_to_statement_list (stmt, &ctor);

  /* Generate a constructor to run it.  */
  cgraph_build_static_cdtor ('I', ctor, DEFAULT_INIT_PRIORITY);

  return true;
}

/* Generate the coverage function info for FN and DATA.  Append a
   pointer to that object to CTOR and return the appended CTOR.  */

static VEC(constructor_elt,gc) *
coverage_obj_fn (VEC(constructor_elt,gc) *ctor, tree fn,
		 struct coverage_data const *data)
{
  tree init = build_fn_info (data, gcov_fn_info_type, gcov_info_var);
  tree var = build_var (fn, gcov_fn_info_type, -1);
  
  DECL_INITIAL (var) = init;
  varpool_finalize_decl (var);
      
  CONSTRUCTOR_APPEND_ELT (ctor, NULL,
			  build1 (ADDR_EXPR, gcov_fn_info_ptr_type, var));
  return ctor;
}

/* Finalize the coverage data.  Generates the array of pointers to
   function objects from CTOR.  Generate the gcov_info initializer.  */

static void
coverage_obj_finish (VEC(constructor_elt,gc) *ctor)
{
  unsigned n_functions = VEC_length(constructor_elt, ctor);
  tree fn_info_ary_type = build_array_type
    (build_qualified_type (gcov_fn_info_ptr_type, TYPE_QUAL_CONST),
     build_index_type (size_int (n_functions - 1)));
  tree fn_info_ary = build_decl (BUILTINS_LOCATION, VAR_DECL, NULL_TREE,
				 fn_info_ary_type);
  char name_buf[32];

  TREE_STATIC (fn_info_ary) = 1;
  ASM_GENERATE_INTERNAL_LABEL (name_buf, "LPBX", 1);
  DECL_NAME (fn_info_ary) = get_identifier (name_buf);
  DECL_INITIAL (fn_info_ary) = build_constructor (fn_info_ary_type, ctor);
  varpool_finalize_decl (fn_info_ary);
  
  DECL_INITIAL (gcov_info_var)
    = build_info (TREE_TYPE (gcov_info_var), fn_info_ary);
  varpool_finalize_decl (gcov_info_var);
}

/* Get the da file name, given base file name.  */

static char *
get_da_file_name (const char *base_file_name)
{
  char *da_file_name;
  int len = strlen (base_file_name);
  const char *prefix = profile_data_prefix;
  int prefix_len = 0;

  if (profile_data_prefix == 0 && !IS_ABSOLUTE_PATH(&base_file_name[0]))
    {
      profile_data_prefix = getpwd ();
      prefix = profile_data_prefix;
    }

  prefix_len = (prefix) ? strlen (prefix) + 1 : 0;

  /* Name of da file.  */
  da_file_name = XNEWVEC (char, len + strlen (GCOV_DATA_SUFFIX)
			  + prefix_len + 2);

  if (prefix)
    {
      strcpy (da_file_name, prefix);
      da_file_name[prefix_len - 1] = '/';
      da_file_name[prefix_len] = 0;
    }
  else
    da_file_name[0] = 0;
  strcat (da_file_name, base_file_name);
  if (profile_base_name_suffix_to_strip)
    {
      int base_name_len = strlen (da_file_name);
      int suffix_to_strip_len = strlen (profile_base_name_suffix_to_strip);

      if (base_name_len > suffix_to_strip_len
          && !strcmp (da_file_name + (base_name_len - suffix_to_strip_len),
                      profile_base_name_suffix_to_strip))
        da_file_name[base_name_len - suffix_to_strip_len] = '\0';
    }

  strcat (da_file_name, GCOV_DATA_SUFFIX);
  return da_file_name;
}

/* Rebuild counts_hash already built the primary module. This hashtable
   was built with a module-id of zero. It needs to be rebuilt taking the
   correct primary module-id into account.  */

static int
rebuild_counts_hash_entry (void **x, void *y)
{
  counts_entry_t *entry = (counts_entry_t *) *x;
  htab_t *new_counts_hash = (htab_t *) y;
  counts_entry_t **slot;
  entry->ident = GEN_FUNC_GLOBAL_ID (primary_module_id, entry->ident);
  slot = (counts_entry_t **) htab_find_slot (*new_counts_hash, entry, INSERT);
  *slot = entry;
  return 1;
}

/* Rebuild counts_hash already built the primary module. This hashtable
   was built with a module-id of zero. It needs to be rebuilt taking the
   correct primary module-id into account.  */

static void
rebuild_counts_hash (void)
{
  htab_t new_counts_hash =
    htab_create (10, htab_counts_entry_hash, htab_counts_entry_eq,
		 htab_counts_entry_del);
  gcc_assert (primary_module_id);
  rebuilding_counts_hash = true;
  htab_traverse_noresize (counts_hash, rebuild_counts_hash_entry, &new_counts_hash);
  htab_delete (counts_hash);
  rebuilding_counts_hash = false;
  counts_hash = new_counts_hash;
}

/* Add the module information record for the module with id
   MODULE_ID. IS_PRIMARY is true if the module is the primary module.
   INDEX is the index of the new record in the module info array.  */

void
add_module_info (unsigned module_id, bool is_primary, int index)
{
  struct gcov_module_info *cur_info;
  module_infos = XRESIZEVEC (struct gcov_module_info *,
                             module_infos, index + 1);
  module_infos[index] = XNEW (struct gcov_module_info);
  cur_info = module_infos[index];
  cur_info->ident = module_id;
  cur_info->is_exported = true;
  cur_info->num_quote_paths = 0;
  cur_info->num_bracket_paths = 0;
  cur_info->da_filename = NULL;
  cur_info->source_filename = NULL;
  if (is_primary)
    primary_module_id = module_id;
}

/* Process the include paths needed for parsing the aux modules.
   The sub_pattern is in the form SUB_PATH:NEW_SUB_PATH. If it is
   defined, the SUB_PATH in ORIG_INC_PATH will be replaced with
   NEW_SUB_PATH.  */

static void
process_include (char **orig_inc_path, char* old_sub, char *new_sub)
{
  char *inc_path, *orig_sub;

  if (strlen (*orig_inc_path) < strlen (old_sub))
    return;

  inc_path = (char*) xmalloc (strlen (*orig_inc_path) + strlen (new_sub)
                              - strlen (old_sub) + 1);
  orig_sub = strstr (*orig_inc_path, old_sub);
  if (!orig_sub)
    {
      inform (UNKNOWN_LOCATION, "subpath %s not found in path %s",
              old_sub, *orig_inc_path);
      free (inc_path);
      return;
    }

  strncpy (inc_path, *orig_inc_path, orig_sub - *orig_inc_path);
  inc_path[orig_sub - *orig_inc_path] = '\0';
  strcat (inc_path, new_sub);
  strcat (inc_path, orig_sub + strlen (old_sub));

  free (*orig_inc_path);
  *orig_inc_path = inc_path;
}

/* Process include paths for MOD_INFO according to option
   -fripa-inc-path-sub=OLD_SUB:NEW_SUB   */

static void
process_include_paths_1 (struct gcov_module_info *mod_info,
                         char* old_sub, char *new_sub)
{
  unsigned i, j;

  for (i = 0; i < mod_info->num_quote_paths; i++)
    process_include (&mod_info->string_array[i], old_sub, new_sub);

  for (i = 0, j = mod_info->num_quote_paths;
       i < mod_info->num_bracket_paths; i++, j++)
    process_include (&mod_info->string_array[j], old_sub, new_sub);

  for (i = 0, j = mod_info->num_quote_paths + mod_info->num_bracket_paths +
       mod_info->num_cpp_defines; i < mod_info->num_cpp_includes; i++, j++)
    process_include (&mod_info->string_array[j], old_sub, new_sub);

}

/* Process include paths for MOD_INFO according to option
   -fripa-inc-path-sub=old_sub1:new_sub1[,old_sub2:new_sub2]  */

static void
process_include_paths (struct gcov_module_info *mod_info)
{
  char *sub_pattern, *cur, *next,  *new_sub;

  if (!lipo_inc_path_pattern)
    return;

  sub_pattern = xstrdup (lipo_inc_path_pattern);
  cur = sub_pattern;

  do
    {
      next = strchr (cur, ',');
      if (next)
        *next++ = '\0';
      new_sub = strchr (cur, ':');
      if (!new_sub)
        {
          error ("Invalid path substibution pattern %s", sub_pattern);
          free (sub_pattern);
          return;
        }
      *new_sub++ = '\0';
      process_include_paths_1 (mod_info, cur, new_sub);
      cur = next;
    } while (cur);
  free (sub_pattern);
}

/* Set the prepreprocessing context (include search paths, -D/-U).
   PARSE_IN is the preprocessor reader, I is the index of the module,
   and VERBOSE is the verbose flag.  */

void
set_lipo_c_parsing_context (struct cpp_reader *parse_in, int i, bool verbose)
{
  struct gcov_module_info *mod_info;
  if (!L_IPO_COMP_MODE)
    return;

  mod_info = module_infos[i];

  gcc_assert (flag_dyn_ipa);
  current_module_id = mod_info->ident;
  reset_funcdef_no ();

  if (current_module_id != primary_module_id)
    {
      unsigned i, j;

      process_include_paths (mod_info);
      /* Setup include paths.  */
      clear_include_chains ();
      for (i = 0; i < mod_info->num_quote_paths; i++)
        add_path (xstrdup (mod_info->string_array[i]),
                  QUOTE, 0, 1);
      for (i = 0, j = mod_info->num_quote_paths;
	   i < mod_info->num_bracket_paths; i++, j++)
        add_path (xstrdup (mod_info->string_array[j]),
                  BRACKET, 0, 1);
      register_include_chains (parse_in, NULL, NULL, NULL,
                               0, 0, verbose);

      /* Setup defines/undefs.  */
      for (i = 0, j = mod_info->num_quote_paths + mod_info->num_bracket_paths;
	   i < mod_info->num_cpp_defines; i++, j++)
	if (mod_info->string_array[j][0] == 'D')
	  cpp_define (parse_in, mod_info->string_array[j] + 1);
	else
	  cpp_undef (parse_in, mod_info->string_array[j] + 1);

      /* Setup -imacro/-include.  */
      for (i = 0, j = mod_info->num_quote_paths + mod_info->num_bracket_paths +
	     mod_info->num_cpp_defines; i < mod_info->num_cpp_includes;
	   i++, j++)
	cpp_push_include (parse_in, mod_info->string_array[j]);
    }
}

/* Perform file-level initialization. Read in data file, generate name
   of graph file.  */

void
coverage_init (const char *filename, const char* source_name)
{
  char* src_name_prefix = 0;
  int src_name_prefix_len = 0;
  int len = strlen (filename);

  has_asm_statement = false;
  da_file_name = get_da_file_name (filename);
  da_base_file_name = XNEWVEC (char, strlen (filename) + 1);
  strcpy (da_base_file_name, filename);

  /* Name of bbg file.  */
  if (flag_test_coverage && !flag_compare_debug)
    {
      bbg_file_name = XNEWVEC (char, len + strlen (GCOV_NOTE_SUFFIX) + 1);
      memcpy (bbg_file_name, filename, len);
      strcpy (bbg_file_name + len, GCOV_NOTE_SUFFIX);
      if (!gcov_open (bbg_file_name, -1))
	{
	  error ("cannot open %s", bbg_file_name);
	  bbg_file_name = NULL;
	}
      else
	{
	  gcov_write_unsigned (GCOV_NOTE_MAGIC);
	  gcov_write_unsigned (GCOV_VERSION);
	  gcov_write_unsigned (local_tick);
	}
    }

  if (profile_data_prefix == 0 && !IS_ABSOLUTE_PATH (source_name))
    {
      src_name_prefix = getpwd ();
      src_name_prefix_len = strlen (src_name_prefix) + 1;
    }
  main_input_file_name = XNEWVEC (char, strlen (source_name) + 1 
                                  + src_name_prefix_len);
  if (!src_name_prefix)
    strcpy (main_input_file_name, source_name);
  else
    {
      strcpy (main_input_file_name, src_name_prefix);
      strcat (main_input_file_name, "/");
      strcat (main_input_file_name, source_name);
    }

  if (flag_branch_probabilities)
    read_counts_file (da_file_name, 0);

  /* Rebuild counts_hash and read the auxiliary GCDA files.  */
  if (flag_profile_use && L_IPO_COMP_MODE)
    {
      unsigned i;
      gcc_assert (flag_dyn_ipa);
      rebuild_counts_hash ();
      for (i = 1; i < num_in_fnames; i++)
	read_counts_file (get_da_file_name (module_infos[i]->da_filename),
			  module_infos[i]->ident);
    }

  /* Define variables which are referenced at runtime by libgcov.  */
  if (profiling_enabled_p ())
    {
      tree_init_dyn_ipa_parameters ();
      init_pmu_profiling ();
      tree_init_instrumentation_sampling ();
    }
}

/* Return True if any type of profiling is enabled which requires linking
   in libgcov otherwise return False.  */

static bool
profiling_enabled_p (void)
{
  return flag_pmu_profile_generate
   || profile_arc_flag
   || flag_profile_generate_sampling
   || flag_profile_reusedist;
}

/* Construct variables for PMU profiling.
   1) __gcov_pmu_profile_filename,
   2) __gcov_pmu_profile_options,
   3) __gcov_pmu_top_n_address.  */

static void
init_pmu_profiling (void)
{
  if (!pmu_profiling_initialized)
    {
      unsigned top_n_addr = PARAM_VALUE (PARAM_PMU_PROFILE_N_ADDRESS);
      tree filename_ptr, options_ptr;

      /* Construct an initializer for __gcov_pmu_profile_filename.  */
      gcov_pmu_filename_decl =
        build_decl (UNKNOWN_LOCATION, VAR_DECL,
                    get_identifier ("__gcov_pmu_profile_filename"),
                    get_const_string_type ());
      TREE_PUBLIC (gcov_pmu_filename_decl) = 1;
      DECL_ARTIFICIAL (gcov_pmu_filename_decl) = 1;
      make_decl_one_only (gcov_pmu_filename_decl,
                          DECL_ASSEMBLER_NAME (gcov_pmu_filename_decl));
      TREE_STATIC (gcov_pmu_filename_decl) = 1;

      if (flag_pmu_profile_generate)
        {
          const char *filename = get_da_file_name (pmu_profile_filename);
          int file_name_len;
          tree filename_string;
          file_name_len = strlen (filename);
          filename_string = build_string (file_name_len + 1, filename);
          TREE_TYPE (filename_string) = build_array_type
            (char_type_node, build_index_type
             (build_int_cst (NULL_TREE, file_name_len)));
          filename_ptr = build1 (ADDR_EXPR, get_const_string_type (),
                                 filename_string);
        }
      else
        filename_ptr = null_pointer_node;

      DECL_INITIAL (gcov_pmu_filename_decl) = filename_ptr;
      varpool_finalize_decl (gcov_pmu_filename_decl);

      /* Construct an initializer for __gcov_pmu_profile_options.  */
      gcov_pmu_options_decl =
        build_decl (UNKNOWN_LOCATION, VAR_DECL,
                    get_identifier ("__gcov_pmu_profile_options"),
                    get_const_string_type ());
      TREE_PUBLIC (gcov_pmu_options_decl) = 1;
      DECL_ARTIFICIAL (gcov_pmu_options_decl) = 1;
      make_decl_one_only (gcov_pmu_options_decl,
                          DECL_ASSEMBLER_NAME (gcov_pmu_options_decl));
      TREE_STATIC (gcov_pmu_options_decl) = 1;

      /* If the flag is false we generate a null pointer to indicate
         that we are not doing the pmu profiling.  */
      if (flag_pmu_profile_generate)
        {
          const char *pmu_options = flag_pmu_profile_generate;
          int pmu_options_len;
          tree pmu_options_string;

          pmu_options_len = strlen (pmu_options);
          pmu_options_string = build_string (pmu_options_len + 1, pmu_options);
          TREE_TYPE (pmu_options_string) = build_array_type
            (char_type_node, build_index_type (build_int_cst
                                               (NULL_TREE, pmu_options_len)));
          options_ptr = build1 (ADDR_EXPR, get_const_string_type (),
                                pmu_options_string);
        }
      else
        options_ptr = null_pointer_node;

      DECL_INITIAL (gcov_pmu_options_decl) = options_ptr;
      varpool_finalize_decl (gcov_pmu_options_decl);

      /* Construct an initializer for __gcov_pmu_top_n_address.  We
         don't need to guard this with the flag_pmu_profile generate
         because the value of __gcov_pmu_top_n_address is ignored when
         not doing profiling.  */
      gcov_pmu_top_n_address_decl =
        build_decl (UNKNOWN_LOCATION, VAR_DECL,
                    get_identifier ("__gcov_pmu_top_n_address"),
                    get_gcov_unsigned_t ());
      TREE_PUBLIC (gcov_pmu_top_n_address_decl) = 1;
      DECL_ARTIFICIAL (gcov_pmu_top_n_address_decl) = 1;
      make_decl_one_only (gcov_pmu_top_n_address_decl,
                          DECL_ASSEMBLER_NAME (gcov_pmu_top_n_address_decl));
      TREE_STATIC (gcov_pmu_top_n_address_decl) = 1;
      DECL_INITIAL (gcov_pmu_top_n_address_decl) =
        build_int_cstu (get_gcov_unsigned_t (), top_n_addr);
      varpool_finalize_decl (gcov_pmu_top_n_address_decl);
    }
  pmu_profiling_initialized = 1;
}

/* Performs file-level cleanup.  Close graph file, generate coverage
   variables and constructor.  */

void
coverage_finish (void)
{
  if (bbg_file_name && gcov_close ())
    unlink (bbg_file_name);
  
  if (!local_tick)
    /* Only remove the da file, if we cannot stamp it.  If we can
       stamp it, libgcov will DTRT.  */
    unlink (da_file_name);

  if (coverage_obj_init ())
    {
      VEC(constructor_elt,gc) *fn_ctor = NULL;
      struct coverage_data *fn;
      
      for (fn = functions_head; fn; fn = fn->next)
	fn_ctor = coverage_obj_fn (fn_ctor, fn->fn_decl, fn);
      coverage_obj_finish (fn_ctor);
    }
}

/* Add S to the end of the string-list, the head and tail of which are
   pointed-to by HEAD and TAIL, respectively.  */

static void
str_list_append (struct str_list **head, struct str_list **tail, const char *s)
{
  struct str_list *e = XNEW (struct str_list);
  e->str = XNEWVEC (char, strlen (s) + 1);
  strcpy (e->str, s);
  e->next = NULL;
  if (*tail)
    (*tail)->next = e;
  else
    *head = e;
  *tail = e;
}

extern bool is_kernel_build;

#define KERNEL_BUILD_PREDEF_STRING "__KERNEL__"

/* Copies the macro def or undef CPP_DEF and saves the copy
   in a list. IS_DEF is a flag indicating if CPP_DEF represents
   a -D or -U.  */

void
coverage_note_define (const char *cpp_def, bool is_def)
{
  char *s = XNEWVEC (char, strlen (cpp_def) + 2);
  s[0] = is_def ? 'D' : 'U';
  strcpy (s + 1, cpp_def);
  str_list_append (&cpp_defines_head, &cpp_defines_tail, s);
  num_cpp_defines++;

  /* When -D__KERNEL__ is in the option list, we assume this is
     compilation for Linux Kernel.  */
  if (!strcmp(cpp_def, KERNEL_BUILD_PREDEF_STRING))
    is_kernel_build = is_def;
}

/* Copies the -imacro/-include FILENAME and saves the copy in a list.  */

void
coverage_note_include (const char *filename)
{
  str_list_append (&cpp_includes_head, &cpp_includes_tail, filename);
  num_cpp_includes++;
}

/* Mark this module as containing asm statements.  */

void
coverage_has_asm_stmt (void)
{
  has_asm_statement = flag_ripa_disallow_asm_modules;
}

/* Write command line options to the .note section.  */

void
write_opts_to_asm (void)
{
  size_t i;
  cpp_dir *quote_paths, *bracket_paths, *pdir;
  struct str_list *pdef, *pinc;
  int num_quote_paths = 0;
  int num_bracket_paths = 0;

  get_include_chains (&quote_paths, &bracket_paths);

  /* Write quote_paths to ASM section.  */
  switch_to_section (get_section (".gnu.switches.text.quote_paths",
				  SECTION_DEBUG, NULL));
  for (pdir = quote_paths; pdir; pdir = pdir->next)
    {
      if (pdir == bracket_paths)
	break;
      num_quote_paths++;
    }
  dw2_asm_output_nstring (in_fnames[0], (size_t)-1, NULL);
  dw2_asm_output_data_uleb128 (num_quote_paths, NULL);
  for (pdir = quote_paths; pdir; pdir = pdir->next)
    {
      if (pdir == bracket_paths)
	break;
      dw2_asm_output_nstring (pdir->name, (size_t)-1, NULL);
    }

  /* Write bracket_paths to ASM section.  */
  switch_to_section (get_section (".gnu.switches.text.bracket_paths",
				  SECTION_DEBUG, NULL));
  for (pdir = bracket_paths; pdir; pdir = pdir->next)
    num_bracket_paths++;
  dw2_asm_output_nstring (in_fnames[0], (size_t)-1, NULL);
  dw2_asm_output_data_uleb128 (num_bracket_paths, NULL);
  for (pdir = bracket_paths; pdir; pdir = pdir->next)
    dw2_asm_output_nstring (pdir->name, (size_t)-1, NULL);

  /* Write cpp_defines to ASM section.  */
  switch_to_section (get_section (".gnu.switches.text.cpp_defines",
				  SECTION_DEBUG, NULL));
  dw2_asm_output_nstring (in_fnames[0], (size_t)-1, NULL);
  dw2_asm_output_data_uleb128 (num_cpp_defines, NULL);
  for (pdef = cpp_defines_head; pdef; pdef = pdef->next)
    dw2_asm_output_nstring (pdef->str, (size_t)-1, NULL);

  /* Write cpp_includes to ASM section.  */
  switch_to_section (get_section (".gnu.switches.text.cpp_includes",
				  SECTION_DEBUG, NULL));
  dw2_asm_output_nstring (in_fnames[0], (size_t)-1, NULL);
  dw2_asm_output_data_uleb128 (num_cpp_includes, NULL);
  for (pinc = cpp_includes_head; pinc; pinc = pinc->next)
    dw2_asm_output_nstring (pinc->str, (size_t)-1, NULL);

  /* Write cl_args to ASM section.  */
  switch_to_section (get_section (".gnu.switches.text.cl_args",
				  SECTION_DEBUG, NULL));
  dw2_asm_output_nstring (in_fnames[0], (size_t)-1, NULL);
  dw2_asm_output_data_uleb128 (num_lipo_cl_args, NULL);
  for (i = 0; i < num_lipo_cl_args; i++)
    dw2_asm_output_nstring (lipo_cl_args[i], (size_t)-1, NULL);
}
#include "gt-coverage.h"
