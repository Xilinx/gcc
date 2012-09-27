/* Data and functions related to line maps and input files.
   Copyright (C) 2004, 2007, 2008, 2009, 2010, 2011
   Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "intl.h"
#include "input.h"
#include "vec.h"
#include "vecprim.h"

/* Current position in real source file.  */

location_t input_location;

struct line_maps *line_table;

/* Vectors to map a discriminator-enhanced locus to a real locus and
   discriminator value.  */
static VEC(int,heap) *discriminator_location_locations = NULL;
static VEC(int,heap) *discriminator_location_discriminators = NULL;
static location_t min_discriminator_location = UNKNOWN_LOCATION;

/* Expand the source location LOC into a human readable location.  If
   LOC resolves to a builtin location, the file name of the readable
   location is set to the string "<built-in>".  */

expanded_location
expand_location (source_location loc)
{
  expanded_location xloc;
  const struct line_map *map;
  tree block = NULL;

  if (IS_ADHOC_LOC (loc))
    {
      block = LOCATION_BLOCK (loc);
      loc = LOCATION_LOCUS (loc);
    }

  /* If LOC describes a location with a discriminator, extract the
     discriminator and map it to the real location.  */
  if (min_discriminator_location != UNKNOWN_LOCATION
      && loc >= min_discriminator_location)
    loc = map_discriminator_location (loc);

  loc = linemap_resolve_location (line_table, loc,
				  LRK_SPELLING_LOCATION, &map);
  xloc = linemap_expand_location (line_table, map, loc);

  xloc.data = block;
  if (loc <= BUILTINS_LOCATION)
    xloc.file = loc == UNKNOWN_LOCATION ? NULL : _("<built-in>");

  return xloc;
}


/* Associate the DISCRIMINATOR with LOCUS, and return a new locus.
   We associate discriminators with a locus by allocating location_t
   values beyond those assigned by libcpp.  Each new value is mapped
   directly to a real location_t value, and separately to the
   discriminator.  */

location_t
location_with_discriminator (location_t locus, int discriminator)
{
  static int next_discriminator_location = 0;
  tree block = LOCATION_BLOCK (locus);
  location_t ret;
  locus = LOCATION_LOCUS (locus);

  if (min_discriminator_location == UNKNOWN_LOCATION)
    {
      min_discriminator_location = line_table->highest_location + 1;
      next_discriminator_location = min_discriminator_location;
    }

  VEC_safe_push (int, heap, discriminator_location_locations, (int) locus);
  VEC_safe_push (int, heap, discriminator_location_discriminators,
		 discriminator);
  ret = block ?
      COMBINE_LOCATION_DATA (line_table, next_discriminator_location, block)
      : LOCATION_LOCUS (next_discriminator_location);
  next_discriminator_location++;
  return ret;
}

/* Return TRUE if LOCUS represents a location with a discriminator.  */

bool
has_discriminator (location_t locus)
{
  return (min_discriminator_location != UNKNOWN_LOCATION
	  && LOCATION_LOCUS (locus) >= min_discriminator_location);
}

/* Return the real location_t value for LOCUS.  */

location_t
map_discriminator_location (location_t locus)
{
  locus = LOCATION_LOCUS (locus);
  if (! has_discriminator (locus))
    return locus;
  return (location_t) VEC_index (int, discriminator_location_locations,
				 locus - min_discriminator_location);
}

/* Return the discriminator for LOCUS.  */

int
get_discriminator_from_locus (location_t locus)
{
  locus = LOCATION_LOCUS (locus);
  if (! has_discriminator (locus))
    return 0;
  return VEC_index (int, discriminator_location_discriminators,
		    locus - min_discriminator_location);
}

#define ONE_K 1024
#define ONE_M (ONE_K * ONE_K)

/* Display a number as an integer multiple of either:
   - 1024, if said integer is >= to 10 K (in base 2)
   - 1024 * 1024, if said integer is >= 10 M in (base 2)
 */
#define SCALE(x) ((unsigned long) ((x) < 10 * ONE_K \
		  ? (x) \
		  : ((x) < 10 * ONE_M \
		     ? (x) / ONE_K \
		     : (x) / ONE_M)))

/* For a given integer, display either:
   - the character 'k', if the number is higher than 10 K (in base 2)
     but strictly lower than 10 M (in base 2)
   - the character 'M' if the number is higher than 10 M (in base2)
   - the charcter ' ' if the number is strictly lower  than 10 K  */
#define STAT_LABEL(x) ((x) < 10 * ONE_K ? ' ' : ((x) < 10 * ONE_M ? 'k' : 'M'))

/* Display an integer amount as multiple of 1K or 1M (in base 2).
   Display the correct unit (either k, M, or ' ') after the amout, as
   well.  */
#define FORMAT_AMOUNT(size) SCALE (size), STAT_LABEL (size)

/* Dump statistics to stderr about the memory usage of the line_table
   set of line maps.  This also displays some statistics about macro
   expansion.  */

void
dump_line_table_statistics (void)
{
  struct linemap_stats s;
  long total_used_map_size,
    macro_maps_size,
    total_allocated_map_size;

  memset (&s, 0, sizeof (s));

  linemap_get_statistics (line_table, &s);

  macro_maps_size = s.macro_maps_used_size
    + s.macro_maps_locations_size;

  total_allocated_map_size = s.ordinary_maps_allocated_size
    + s.macro_maps_allocated_size
    + s.macro_maps_locations_size;

  total_used_map_size = s.ordinary_maps_used_size
    + s.macro_maps_used_size
    + s.macro_maps_locations_size;

  fprintf (stderr, "Number of expanded macros:                     %5ld\n",
           s.num_expanded_macros);
  if (s.num_expanded_macros != 0)
    fprintf (stderr, "Average number of tokens per macro expansion:  %5ld\n",
             s.num_macro_tokens / s.num_expanded_macros);
  fprintf (stderr,
           "\nLine Table allocations during the "
           "compilation process\n");
  fprintf (stderr, "Number of ordinary maps used:        %5ld%c\n",
           SCALE (s.num_ordinary_maps_used),
           STAT_LABEL (s.num_ordinary_maps_used));
  fprintf (stderr, "Ordinary map used size:              %5ld%c\n",
           SCALE (s.ordinary_maps_used_size),
           STAT_LABEL (s.ordinary_maps_used_size));
  fprintf (stderr, "Number of ordinary maps allocated:   %5ld%c\n",
           SCALE (s.num_ordinary_maps_allocated),
           STAT_LABEL (s.num_ordinary_maps_allocated));
  fprintf (stderr, "Ordinary maps allocated size:        %5ld%c\n",
           SCALE (s.ordinary_maps_allocated_size),
           STAT_LABEL (s.ordinary_maps_allocated_size));
  fprintf (stderr, "Number of macro maps used:           %5ld%c\n",
           SCALE (s.num_macro_maps_used),
           STAT_LABEL (s.num_macro_maps_used));
  fprintf (stderr, "Macro maps used size:                %5ld%c\n",
           SCALE (s.macro_maps_used_size),
           STAT_LABEL (s.macro_maps_used_size));
  fprintf (stderr, "Macro maps locations size:           %5ld%c\n",
           SCALE (s.macro_maps_locations_size),
           STAT_LABEL (s.macro_maps_locations_size));
  fprintf (stderr, "Macro maps size:                     %5ld%c\n",
           SCALE (macro_maps_size),
           STAT_LABEL (macro_maps_size));
  fprintf (stderr, "Duplicated maps locations size:      %5ld%c\n",
           SCALE (s.duplicated_macro_maps_locations_size),
           STAT_LABEL (s.duplicated_macro_maps_locations_size));
  fprintf (stderr, "Total allocated maps size:           %5ld%c\n",
           SCALE (total_allocated_map_size),
           STAT_LABEL (total_allocated_map_size));
  fprintf (stderr, "Total used maps size:                %5ld%c\n",
           SCALE (total_used_map_size),
           STAT_LABEL (total_used_map_size));
  fprintf (stderr, "\n");
}
