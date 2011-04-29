/* Data and functions related to line maps and input files.
   Copyright (C) 2004, 2007, 2008, 2009, 2010
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

expanded_location
expand_location (source_location loc)
{
  expanded_location xloc;

  /* If LOC describes a location with a discriminator, extract the
     discriminator and map it to the real location.  */
  if (min_discriminator_location != UNKNOWN_LOCATION
      && loc >= min_discriminator_location)
    loc = map_discriminator_location (loc);

  if (loc <= BUILTINS_LOCATION)
    {
      xloc.file = loc == UNKNOWN_LOCATION ? NULL : _("<built-in>");
      xloc.line = 0;
      xloc.column = 0;
      xloc.sysp = 0;
    }
  else
    {
      const struct line_map *map = linemap_lookup (line_table, loc);
      xloc.file = map->to_file;
      xloc.line = SOURCE_LINE (map, loc);
      xloc.column = SOURCE_COLUMN (map, loc);
      xloc.sysp = map->sysp != 0;
    };
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

  if (min_discriminator_location == UNKNOWN_LOCATION)
    {
      min_discriminator_location = line_table->highest_location + 1;
      next_discriminator_location = min_discriminator_location;
    }

  VEC_safe_push (int, heap, discriminator_location_locations, (int) locus);
  VEC_safe_push (int, heap, discriminator_location_discriminators,
		 discriminator);
  return next_discriminator_location++;
}

/* Return TRUE if LOCUS represents a location with a discriminator.  */

bool
has_discriminator (location_t locus)
{
  return (min_discriminator_location != UNKNOWN_LOCATION
	  && locus >= min_discriminator_location);
}

/* Return the real location_t value for LOCUS.  */

location_t
map_discriminator_location (location_t locus)
{
  if (! has_discriminator (locus))
    return locus;
  return (location_t) VEC_index (int, discriminator_location_locations,
				 locus - min_discriminator_location);
}

/* Return the discriminator for LOCUS.  */

int
get_discriminator_from_locus (location_t locus)
{
  if (! has_discriminator (locus))
    return 0;
  return VEC_index (int, discriminator_location_discriminators,
		    locus - min_discriminator_location);
}
