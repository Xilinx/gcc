/* File format for coverage information
   Copyright (C) 1996, 1997, 1998, 2000, 2002, 2003, 2004, 2005, 2007,
   2008  Free Software Foundation, Inc.
   Contributed by Bob Manson <manson@cygnus.com>.
   Completely remangled by Nathan Sidwell <nathan@codesourcery.com>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/* Routines declared in gcov-io.h.  This file should be #included by
   another source file, after having #included gcov-io.h.  */

/* Redefine these here, rather than using the ones in system.h since
 * including system.h leads to conflicting definitions of other
 * symbols and macros.  */
#undef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#if !IN_GCOV
static void gcov_write_block (unsigned);
static gcov_unsigned_t *gcov_write_words (unsigned);
#endif
static const gcov_unsigned_t *gcov_read_words (unsigned);
#if !IN_LIBGCOV
static void gcov_allocate (unsigned);
#endif

#ifdef __GCOV_KERNEL__
struct gcov_var gcov_var ATTRIBUTE_HIDDEN;
#endif

static inline gcov_unsigned_t from_file (gcov_unsigned_t value)
{
#if !IN_LIBGCOV
  if (gcov_var.endian)
    {
      value = (value >> 16) | (value << 16);
      value = ((value & 0xff00ff) << 8) | ((value >> 8) & 0xff00ff);
    }
#endif
  return value;
}

/* Open a gcov file. NAME is the name of the file to open and MODE
   indicates whether a new file should be created, or an existing file
   opened. If MODE is >= 0 an existing file will be opened, if
   possible, and if MODE is <= 0, a new file will be created. Use
   MODE=0 to attempt to reopen an existing file and then fall back on
   creating a new one.  If MODE < 0, the file will be opened in
   read-only mode.  Otherwise it will be opened for modification.
   Return zero on failure, >0 on opening an existing file and <0 on
   creating a new one.  */

#ifndef __GCOV_KERNEL__
GCOV_LINKAGE int
#if IN_LIBGCOV
gcov_open (const char *name)
#else
gcov_open (const char *name, int mode)
#endif
{
#if IN_LIBGCOV
  const int mode = 0;
#endif
#if GCOV_LOCKED
  struct flock s_flock;
  int fd;

  s_flock.l_whence = SEEK_SET;
  s_flock.l_start = 0;
  s_flock.l_len = 0; /* Until EOF.  */
  s_flock.l_pid = getpid ();
#endif

  gcc_assert (!gcov_var.file);
  gcov_var.start = 0;
  gcov_var.offset = gcov_var.length = 0;
  gcov_var.overread = -1u;
  gcov_var.error = 0;
#if !IN_LIBGCOV
  gcov_var.endian = 0;
#endif
#if GCOV_LOCKED
  if (mode > 0)
    {
      /* Read-only mode - acquire a read-lock.  */
      s_flock.l_type = F_RDLCK;
      fd = open (name, O_RDONLY);
    }
  else
    {
      /* Write mode - acquire a write-lock.  */
      s_flock.l_type = F_WRLCK;
      fd = open (name, O_RDWR | O_CREAT, 0666);
    }
  if (fd < 0)
    return 0;

  while (fcntl (fd, F_SETLKW, &s_flock) && errno == EINTR)
    continue;

  gcov_var.file = fdopen (fd, (mode > 0) ? "rb" : "r+b");

  if (!gcov_var.file)
    {
      close (fd);
      return 0;
    }

  if (mode > 0)
    gcov_var.mode = 1;
  else if (mode == 0)
    {
      struct stat st;

      if (fstat (fd, &st) < 0)
	{
	  fclose (gcov_var.file);
	  gcov_var.file = 0;
	  return 0;
	}
      if (st.st_size != 0)
	gcov_var.mode = 1;
      else
	gcov_var.mode = mode * 2 + 1;
    }
  else
    gcov_var.mode = mode * 2 + 1;
#else
  if (mode >= 0)
    gcov_var.file = fopen (name, (mode > 0) ? "rb" : "r+b");

  if (gcov_var.file)
    gcov_var.mode = 1;
  else if (mode <= 0)
    {
      gcov_var.file = fopen (name, "w+b");
      if (gcov_var.file)
	gcov_var.mode = mode * 2 + 1;
    }
  if (!gcov_var.file)
    return 0;
#endif

  setbuf (gcov_var.file, (char *)0);

  return 1;
}
#else /* __GCOV_KERNEL__ */

extern _GCOV_FILE *gcov_current_file;

GCOV_LINKAGE int
gcov_open (const char *name)
{
  gcov_var.start = 0;
  gcov_var.offset = gcov_var.length = 0;
  gcov_var.overread = -1u;
  gcov_var.error = 0;
  gcov_var.file = gcov_current_file;
  gcov_var.mode = 1;

  return 1;
}
#endif /* __GCOV_KERNEL__ */

/* Close the current gcov file. Flushes data to disk. Returns nonzero
   on failure or error flag set.  */

GCOV_LINKAGE int
gcov_close (void)
{
  if (gcov_var.file)
    {
#if !IN_GCOV
      if (gcov_var.offset && gcov_var.mode < 0)
	gcov_write_block (gcov_var.offset);
#endif
      _GCOV_fclose (gcov_var.file);
      gcov_var.file = 0;
      gcov_var.length = 0;
    }
#if !IN_LIBGCOV
  free (gcov_var.buffer);
  gcov_var.alloc = 0;
  gcov_var.buffer = 0;
#endif
  gcov_var.mode = 0;
  return gcov_var.error;
}

#if !IN_LIBGCOV
/* Modify FILENAME to a canonical form after stripping known prefixes
   in place.  It removes '/proc/self/cwd' and '/proc/self/cwd/.'.
   Returns the in-place modified filename.  */

GCOV_LINKAGE char *
gcov_canonical_filename (char *filename)
{
  static char cwd_dot_str[] = "/proc/self/cwd/./";
  int cwd_dot_len = strlen (cwd_dot_str);
  int cwd_len = cwd_dot_len - 2; /* without trailing './' */
  int filename_len = strlen (filename);
  /* delete the longer prefix first */
  if (0 == strncmp (filename, cwd_dot_str, cwd_dot_len))
    {
      memmove (filename, filename + cwd_dot_len, filename_len - cwd_dot_len);
      filename[filename_len - cwd_dot_len] = '\0';
      return filename;
    }

  if (0 == strncmp (filename, cwd_dot_str, cwd_len))
    {
      memmove (filename, filename + cwd_len, filename_len - cwd_len);
      filename[filename_len - cwd_len] = '\0';
      return filename;
    }
  return filename;
}

/* Read LEN words and construct load latency info LL_INFO.  */

GCOV_LINKAGE void
gcov_read_pmu_load_latency_info (gcov_pmu_ll_info_t *ll_info,
                                 gcov_unsigned_t len ATTRIBUTE_UNUSED)
{
  ll_info->counts = gcov_read_unsigned ();
  ll_info->self = gcov_read_unsigned ();
  ll_info->cum = gcov_read_unsigned ();
  ll_info->lt_10 = gcov_read_unsigned ();
  ll_info->lt_32 = gcov_read_unsigned ();
  ll_info->lt_64 = gcov_read_unsigned ();
  ll_info->lt_256 = gcov_read_unsigned ();
  ll_info->lt_1024 = gcov_read_unsigned ();
  ll_info->gt_1024 = gcov_read_unsigned ();
  ll_info->wself = gcov_read_unsigned ();
  ll_info->code_addr = gcov_read_counter ();
  ll_info->line = gcov_read_unsigned ();
  ll_info->discriminator = gcov_read_unsigned ();
  ll_info->filetag = gcov_read_unsigned ();
}

/* Read LEN words and construct branch mispredict info BRM_INFO.  */

GCOV_LINKAGE void
gcov_read_pmu_branch_mispredict_info (gcov_pmu_brm_info_t *brm_info,
                                      gcov_unsigned_t len ATTRIBUTE_UNUSED)
{
  brm_info->counts = gcov_read_unsigned ();
  brm_info->self = gcov_read_unsigned ();
  brm_info->cum = gcov_read_unsigned ();
  brm_info->code_addr = gcov_read_counter ();
  brm_info->line = gcov_read_unsigned ();
  brm_info->discriminator = gcov_read_unsigned ();
  brm_info->filetag = gcov_read_unsigned ();
}

/* Read LEN words from an open gcov file and construct data into a
   string table entry */

GCOV_LINKAGE void
gcov_read_pmu_string_table_entry (gcov_pmu_st_entry_t *st_entry,
                                  gcov_unsigned_t len ATTRIBUTE_UNUSED)
{
  const char *str;

  st_entry->index = gcov_read_unsigned ();
  str = gcov_read_string ();
  st_entry->str = str ? gcov_canonical_filename (xstrdup(str)) : 0;
}

/* Read LEN words from an open gcov file and construct data into pmu
   tool header TOOL_HEADER.  */

GCOV_LINKAGE void gcov_read_pmu_tool_header (gcov_pmu_tool_header_t *header,
                                           gcov_unsigned_t len ATTRIBUTE_UNUSED)
{
  const char *str;
  str = gcov_read_string ();
  header->host_cpu = str ? xstrdup (str) : 0;
  str = gcov_read_string ();
  header->hostname = str ? xstrdup (str) : 0;
  str = gcov_read_string ();
  header->kernel_version = str ? xstrdup (str) : 0;
  str = gcov_read_string ();
  header->column_header = str ? xstrdup (str) : 0;
  str = gcov_read_string ();
  header->column_description = str ? xstrdup (str) : 0;
  str = gcov_read_string ();
  header->full_header = str ? xstrdup (str) : 0;
}
#endif

#if !IN_LIBGCOV
/* Check if MAGIC is EXPECTED. Use it to determine endianness of the
   file. Returns +1 for same endian, -1 for other endian and zero for
   not EXPECTED.  */

GCOV_LINKAGE int
gcov_magic (gcov_unsigned_t magic, gcov_unsigned_t expected)
{
  if (magic == expected)
    return 1;
  magic = (magic >> 16) | (magic << 16);
  magic = ((magic & 0xff00ff) << 8) | ((magic >> 8) & 0xff00ff);
  if (magic == expected)
    {
      gcov_var.endian = 1;
      return -1;
    }
  return 0;
}
#endif

#if !IN_LIBGCOV
static void
gcov_allocate (unsigned length)
{
  size_t new_size = gcov_var.alloc;

  if (!new_size)
    new_size = GCOV_BLOCK_SIZE;
  new_size += length;
  new_size *= 2;

  gcov_var.alloc = new_size;
  gcov_var.buffer = XRESIZEVAR (gcov_unsigned_t, gcov_var.buffer, new_size << 2);
}
#endif

#if !IN_GCOV
/* Write out the current block, if needs be.  */

static void
gcov_write_block (unsigned size)
{
  if (_GCOV_fwrite (gcov_var.buffer, size << 2, 1, gcov_var.file) != 1)
    gcov_var.error = 1;
  gcov_var.start += size;
  gcov_var.offset -= size;
}

#if IN_LIBGCOV
/* Return the number of words STRING would need including the length
   field in the output stream itself.  This should be identical to
   "alloc" calculation in gcov_write_string().  */

GCOV_LINKAGE gcov_unsigned_t
gcov_string_length (const char *string)
{
  gcov_unsigned_t len = (string) ? strlen (string) : 0;
  /* + 1 because of the length field.  */
  gcov_unsigned_t alloc = 1 + ((len + 4) >> 2);

  /* Can not write a bigger than GCOV_BLOCK_SIZE string yet */
  gcc_assert (alloc < GCOV_BLOCK_SIZE);
  return alloc;
}
#endif

/* Allocate space to write BYTES bytes to the gcov file. Return a
   pointer to those bytes, or NULL on failure.  */

static gcov_unsigned_t *
gcov_write_words (unsigned words)
{
  gcov_unsigned_t *result;

  gcc_assert (gcov_var.mode < 0);
#if IN_LIBGCOV
  if (gcov_var.offset + words >= GCOV_BLOCK_SIZE)
    {
      gcov_write_block (MIN (gcov_var.offset, GCOV_BLOCK_SIZE));
      if (gcov_var.offset)
	{
	  gcc_assert (gcov_var.offset < GCOV_BLOCK_SIZE);
	  memcpy (gcov_var.buffer,
                  gcov_var.buffer + GCOV_BLOCK_SIZE,
                  gcov_var.offset << 2);
	}
    }
#else
  if (gcov_var.offset + words > gcov_var.alloc)
    gcov_allocate (gcov_var.offset + words);
#endif
  result = &gcov_var.buffer[gcov_var.offset];
  gcov_var.offset += words;

  return result;
}

/* Write unsigned VALUE to coverage file.  Sets error flag
   appropriately.  */

GCOV_LINKAGE void
gcov_write_unsigned (gcov_unsigned_t value)
{
  gcov_unsigned_t *buffer = gcov_write_words (1);

  buffer[0] = value;
}

/* Write counter VALUE to coverage file.  Sets error flag
   appropriately.  */

#if IN_LIBGCOV
GCOV_LINKAGE void
gcov_write_counter (gcov_type value)
{
  gcov_unsigned_t *buffer = gcov_write_words (2);

  buffer[0] = (gcov_unsigned_t) value;
  if (sizeof (value) > sizeof (gcov_unsigned_t))
    buffer[1] = (gcov_unsigned_t) (value >> 32);
  else
    buffer[1] = 0;
}
#endif /* IN_LIBGCOV */

/* Write STRING to coverage file.  Sets error flag on file
   error, overflow flag on overflow */

GCOV_LINKAGE void
gcov_write_string (const char *string)
{
  unsigned length = 0;
  unsigned alloc = 0;
  gcov_unsigned_t *buffer;

  if (string)
    {
      length = strlen (string);
      alloc = (length + 4) >> 2;
    }

  buffer = gcov_write_words (1 + alloc);

  buffer[0] = alloc;
  buffer[alloc] = 0;
  memcpy (&buffer[1], string, length);
}

#if !IN_LIBGCOV
/* Write a tag TAG and reserve space for the record length. Return a
   value to be used for gcov_write_length.  */

GCOV_LINKAGE gcov_position_t
gcov_write_tag (gcov_unsigned_t tag)
{
  gcov_position_t result = gcov_var.start + gcov_var.offset;
  gcov_unsigned_t *buffer = gcov_write_words (2);

  buffer[0] = tag;
  buffer[1] = 0;

  return result;
}

/* Write a record length using POSITION, which was returned by
   gcov_write_tag.  The current file position is the end of the
   record, and is restored before returning.  Returns nonzero on
   overflow.  */

GCOV_LINKAGE void
gcov_write_length (gcov_position_t position)
{
  unsigned offset;
  gcov_unsigned_t length;
  gcov_unsigned_t *buffer;

  gcc_assert (gcov_var.mode < 0);
  gcc_assert (position + 2 <= gcov_var.start + gcov_var.offset);
  gcc_assert (position >= gcov_var.start);
  offset = position - gcov_var.start;
  length = gcov_var.offset - offset - 2;
  buffer = (gcov_unsigned_t *) &gcov_var.buffer[offset];
  buffer[1] = length;
  if (gcov_var.offset >= GCOV_BLOCK_SIZE)
    gcov_write_block (gcov_var.offset);
}

#else /* IN_LIBGCOV */

/* Write a tag TAG and length LENGTH.  */

GCOV_LINKAGE void
gcov_write_tag_length (gcov_unsigned_t tag, gcov_unsigned_t length)
{
  gcov_unsigned_t *buffer = gcov_write_words (2);

  buffer[0] = tag;
  buffer[1] = length;
}

/* Write a summary structure to the gcov file.  Return nonzero on
   overflow.  */

GCOV_LINKAGE void
gcov_write_summary (gcov_unsigned_t tag, const struct gcov_summary *summary)
{
  unsigned ix, h_ix, bv_ix, h_cnt = 0;
  const struct gcov_ctr_summary *csum;
  unsigned histo_bitvector[GCOV_HISTOGRAM_BITVECTOR_SIZE];

  /* Count number of non-zero histogram entries, and fill in a bit vector
     of non-zero indices. The histogram is only currently computed for arc
     counters.  */
  for (bv_ix = 0; bv_ix < GCOV_HISTOGRAM_BITVECTOR_SIZE; bv_ix++)
    histo_bitvector[bv_ix] = 0;
  csum = &summary->ctrs[GCOV_COUNTER_ARCS];
  for (h_ix = 0; h_ix < GCOV_HISTOGRAM_SIZE; h_ix++)
    {
      if (csum->histogram[h_ix].num_counters > 0)
        {
          histo_bitvector[h_ix / 32] |= 1 << (h_ix % 32);
          h_cnt++;
        }
    }
  gcov_write_tag_length (tag, GCOV_TAG_SUMMARY_LENGTH(h_cnt));
  gcov_write_unsigned (summary->checksum);
  for (csum = summary->ctrs, ix = GCOV_COUNTERS_SUMMABLE; ix--; csum++)
    {
      gcov_write_unsigned (csum->num);
      gcov_write_unsigned (csum->runs);
      gcov_write_counter (csum->sum_all);
      gcov_write_counter (csum->run_max);
      gcov_write_counter (csum->sum_max);
      if (ix != GCOV_COUNTER_ARCS)
        {
          for (bv_ix = 0; bv_ix < GCOV_HISTOGRAM_BITVECTOR_SIZE; bv_ix++)
            gcov_write_unsigned (0);
          continue;
        }
      for (bv_ix = 0; bv_ix < GCOV_HISTOGRAM_BITVECTOR_SIZE; bv_ix++)
        gcov_write_unsigned (histo_bitvector[bv_ix]);
      for (h_ix = 0; h_ix < GCOV_HISTOGRAM_SIZE; h_ix++)
        {
          if (!csum->histogram[h_ix].num_counters)
            continue;
          gcov_write_unsigned (csum->histogram[h_ix].num_counters);
          gcov_write_counter (csum->histogram[h_ix].min_value);
          gcov_write_counter (csum->histogram[h_ix].cum_value);
        }
    }
}
#endif /* IN_LIBGCOV */

#endif /*!IN_GCOV */

/* Return a pointer to read BYTES bytes from the gcov file. Returns
   NULL on failure (read past EOF).  */

static const gcov_unsigned_t *
gcov_read_words (unsigned words)
{
  const gcov_unsigned_t *result;
  unsigned excess = gcov_var.length - gcov_var.offset;

  gcc_assert (gcov_var.mode > 0);
  gcc_assert (words < GCOV_BLOCK_SIZE);
  if (excess < words)
    {
      gcov_var.start += gcov_var.offset;
#if IN_LIBGCOV
      if (excess)
	{
	  gcc_assert (excess < GCOV_BLOCK_SIZE);
	  memmove (gcov_var.buffer, gcov_var.buffer + gcov_var.offset, excess * 4);
	}
#else
      memmove (gcov_var.buffer, gcov_var.buffer + gcov_var.offset, excess * 4);
#endif
      gcov_var.offset = 0;
      gcov_var.length = excess;
#if IN_LIBGCOV
      excess = (sizeof (gcov_var.buffer) / sizeof (gcov_var.buffer[0])) - gcov_var.length;
#else
      if (gcov_var.length + words > gcov_var.alloc)
	gcov_allocate (gcov_var.length + words);
      excess = gcov_var.alloc - gcov_var.length;
#endif
      excess = _GCOV_fread (gcov_var.buffer + gcov_var.length,
		      1, excess << 2, gcov_var.file) >> 2;
      gcov_var.length += excess;
      if (gcov_var.length < words)
	{
	  gcov_var.overread += words - gcov_var.length;
	  gcov_var.length = 0;
	  return 0;
	}
    }
  result = &gcov_var.buffer[gcov_var.offset];
  gcov_var.offset += words;
  return result;
}

/* Read unsigned value from a coverage file. Sets error flag on file
   error, overflow flag on overflow */

GCOV_LINKAGE gcov_unsigned_t
gcov_read_unsigned (void)
{
  gcov_unsigned_t value;
  const gcov_unsigned_t *buffer = gcov_read_words (1);

  if (!buffer)
    return 0;
  value = from_file (buffer[0]);
  return value;
}

/* Read counter value from a coverage file. Sets error flag on file
   error, overflow flag on overflow */

GCOV_LINKAGE gcov_type
gcov_read_counter (void)
{
  gcov_type value;
  const gcov_unsigned_t *buffer = gcov_read_words (2);

  if (!buffer)
    return 0;
  value = from_file (buffer[0]);
  if (sizeof (value) > sizeof (gcov_unsigned_t))
    value |= ((gcov_type) from_file (buffer[1])) << 32;
  else if (buffer[1])
    gcov_var.error = -1;

  return value;
}

/* Read string from coverage file. Returns a pointer to a static
   buffer, or NULL on empty string. You must copy the string before
   calling another gcov function.  */

GCOV_LINKAGE const char *
gcov_read_string (void)
{
  unsigned length = gcov_read_unsigned ();

  if (!length)
    return 0;

  return (const char *) gcov_read_words (length);
}

#ifdef __GCOV_KERNEL__
static int
k_popcountll (long long x)
{
  int c = 0;
  while (x)
    {
      c++;
      x &= (x-1);
    }
  return c;
}
#endif

GCOV_LINKAGE void
gcov_read_summary (struct gcov_summary *summary)
{
  unsigned ix, h_ix, bv_ix, h_cnt = 0;
  struct gcov_ctr_summary *csum;
  unsigned histo_bitvector[GCOV_HISTOGRAM_BITVECTOR_SIZE];
  unsigned cur_bitvector;

  summary->checksum = gcov_read_unsigned ();
  for (csum = summary->ctrs, ix = GCOV_COUNTERS_SUMMABLE; ix--; csum++)
    {
      csum->num = gcov_read_unsigned ();
      csum->runs = gcov_read_unsigned ();
      csum->sum_all = gcov_read_counter ();
      csum->run_max = gcov_read_counter ();
      csum->sum_max = gcov_read_counter ();
      memset (csum->histogram, 0,
              sizeof (gcov_bucket_type) * GCOV_HISTOGRAM_SIZE);
      for (bv_ix = 0; bv_ix < GCOV_HISTOGRAM_BITVECTOR_SIZE; bv_ix++)
        {
          histo_bitvector[bv_ix] = gcov_read_unsigned ();
#ifndef __GCOV_KERNEL__
          h_cnt += __builtin_popcountll (histo_bitvector[bv_ix]);
#else
          h_cnt += k_popcountll (histo_bitvector[bv_ix]);
#endif
        }
      bv_ix = 0;
      h_ix = 0;
      cur_bitvector = 0;
      while (h_cnt--)
        {
          /* Find the index corresponding to the next entry we will read in.
             First find the next non-zero bitvector and re-initialize
             the histogram index accordingly, then right shift and increment
             the index until we find a set bit.  */
          while (!cur_bitvector)
            {
              h_ix = bv_ix * 32;
              cur_bitvector = histo_bitvector[bv_ix++];
              gcc_assert(bv_ix <= GCOV_HISTOGRAM_BITVECTOR_SIZE);
            }
          while (!(cur_bitvector & 0x1))
            {
              h_ix++;
              cur_bitvector >>= 1;
            }
          gcc_assert(h_ix < GCOV_HISTOGRAM_SIZE);

          csum->histogram[h_ix].num_counters = gcov_read_unsigned ();
          csum->histogram[h_ix].min_value = gcov_read_counter ();
          csum->histogram[h_ix].cum_value = gcov_read_counter ();
          /* Shift off the index we are done with and increment to the
             corresponding next histogram entry.  */
          cur_bitvector >>= 1;
          h_ix++;
        }
    }
}

#if !IN_LIBGCOV && IN_GCOV != 1
/* Read LEN words (unsigned type) and construct MOD_INFO.  */

GCOV_LINKAGE void
gcov_read_module_info (struct gcov_module_info *mod_info,
                       gcov_unsigned_t len)
{
  gcov_unsigned_t src_filename_len, filename_len, i, j, num_strings;
  mod_info->ident = gcov_read_unsigned ();
  mod_info->is_primary = gcov_read_unsigned ();
  mod_info->is_exported = gcov_read_unsigned ();
  mod_info->lang  = gcov_read_unsigned ();
  mod_info->num_quote_paths = gcov_read_unsigned ();
  mod_info->num_bracket_paths = gcov_read_unsigned ();
  mod_info->num_cpp_defines = gcov_read_unsigned ();
  mod_info->num_cpp_includes = gcov_read_unsigned ();
  mod_info->num_cl_args = gcov_read_unsigned ();
  len -= 9;

  filename_len = gcov_read_unsigned ();
  mod_info->da_filename = (char *) xmalloc (filename_len *
                                            sizeof (gcov_unsigned_t));
  for (i = 0; i < filename_len; i++)
    ((gcov_unsigned_t *) mod_info->da_filename)[i] = gcov_read_unsigned ();
  len -= (filename_len + 1);

  src_filename_len = gcov_read_unsigned ();
  mod_info->source_filename = (char *) xmalloc (src_filename_len *
						sizeof (gcov_unsigned_t));
  for (i = 0; i < src_filename_len; i++)
    ((gcov_unsigned_t *) mod_info->source_filename)[i] = gcov_read_unsigned ();
  len -= (src_filename_len + 1);

  num_strings = mod_info->num_quote_paths + mod_info->num_bracket_paths +
    mod_info->num_cpp_defines + mod_info->num_cpp_includes +
    mod_info->num_cl_args;
  for (j = 0; j < num_strings; j++)
   {
     gcov_unsigned_t string_len = gcov_read_unsigned ();
     mod_info->string_array[j] =
       (char *) xmalloc (string_len * sizeof (gcov_unsigned_t));
     for (i = 0; i < string_len; i++)
       ((gcov_unsigned_t *) mod_info->string_array[j])[i] =
	 gcov_read_unsigned ();
     len -= (string_len + 1);
   }
  gcc_assert (!len);
}
#endif

#if !IN_LIBGCOV
/* Reset to a known position.  BASE should have been obtained from
   gcov_position, LENGTH should be a record length.  */

GCOV_LINKAGE void
gcov_sync (gcov_position_t base, gcov_unsigned_t length)
{
#ifdef __GCOV_KERNEL__
  /* should not reach this point */
  gcc_assert (0);
#else /* __GCOV_KERNEL__ */
  gcc_assert (gcov_var.mode > 0);
  base += length;
  if (base - gcov_var.start <= gcov_var.length)
    gcov_var.offset = base - gcov_var.start;
  else
    {
      gcov_var.offset = gcov_var.length = 0;
      _GCOV_fseek (gcov_var.file, base << 2, SEEK_SET);
      gcov_var.start = _GCOV_ftell (gcov_var.file) >> 2;
    }
#endif /* __GCOV_KERNEL__ */
}
#endif

#if IN_LIBGCOV
/* Move to a given position in a gcov file.  */

GCOV_LINKAGE void
gcov_seek (gcov_position_t base)
{
  gcc_assert (gcov_var.mode < 0);
  if (gcov_var.offset)
    gcov_write_block (gcov_var.offset);
  _GCOV_fseek (gcov_var.file, base << 2, SEEK_SET);
  gcov_var.start = _GCOV_ftell (gcov_var.file) >> 2;
}

/* Truncate the gcov file at the current position.  */

GCOV_LINKAGE void
gcov_truncate (void)
{
#ifdef __GCOV_KERNEL__
  /* should not reach this point */
  gcc_assert (0);
#else /* __GCOV_KERNEL__ */
  long offs;
  int filenum;
  gcc_assert (gcov_var.mode < 0);
  if (gcov_var.offset)
    gcov_write_block (gcov_var.offset);
  offs = ftell (gcov_var.file);
  filenum = fileno (gcov_var.file);
  if (offs == -1 || filenum == -1 || ftruncate (filenum, offs))
    gcov_var.error = 1;
#endif /* __GCOV_KERNEL__ */
}
#endif

#ifndef __GCOV_KERNEL__
/* Convert an unsigned NUMBER to a percentage after dividing by
   100.  */

GCOV_LINKAGE float
convert_unsigned_to_pct (const unsigned number)
{
  return (float)number / 100.0f;
}
#endif

#if !IN_LIBGCOV && IN_GCOV != 1
/* Print load latency information given by LL_INFO in a human readable
   format into an open output file pointed by FP. NEWLINE specifies
   whether or not to print a trailing newline.  */

GCOV_LINKAGE void
print_load_latency_line (FILE *fp, const gcov_pmu_ll_info_t *ll_info,
                         const enum print_newline newline)
{
  if (!ll_info)
    return;
  fprintf (fp, " %u %.2f%% %.2f%% %.2f%% %.2f%% %.2f%% %.2f%% %.2f%% "
           "%.2f%% %.2f%% " HOST_WIDEST_INT_PRINT_HEX " %d %d %d",
           ll_info->counts,
           convert_unsigned_to_pct (ll_info->self),
           convert_unsigned_to_pct (ll_info->cum),
           convert_unsigned_to_pct (ll_info->lt_10),
           convert_unsigned_to_pct (ll_info->lt_32),
           convert_unsigned_to_pct (ll_info->lt_64),
           convert_unsigned_to_pct (ll_info->lt_256),
           convert_unsigned_to_pct (ll_info->lt_1024),
           convert_unsigned_to_pct (ll_info->gt_1024),
           convert_unsigned_to_pct (ll_info->wself),
           ll_info->code_addr,
           ll_info->filetag,
           ll_info->line,
           ll_info->discriminator);
  if (newline == add_newline)
    fprintf (fp, "\n");
}

/* Print BRM_INFO into the file pointed by FP.  NEWLINE specifies
   whether or not to print a trailing newline.  */

GCOV_LINKAGE void
print_branch_mispredict_line (FILE *fp, const gcov_pmu_brm_info_t *brm_info,
                              const enum print_newline newline)
{
  if (!brm_info)
    return;
  fprintf (fp, " %u %.2f%% %.2f%% " HOST_WIDEST_INT_PRINT_HEX " %d %d %d",
           brm_info->counts,
           convert_unsigned_to_pct (brm_info->self),
           convert_unsigned_to_pct (brm_info->cum),
           brm_info->code_addr,
           brm_info->filetag,
           brm_info->line,
           brm_info->discriminator);
  if (newline == add_newline)
    fprintf (fp, "\n");
}

/* Print STRING_TABLE_ENTRY into the file pointed by FP. NEWLINE specifies
   whether or not to print a trailing newline. */

GCOV_LINKAGE void
print_pmu_string_table_entry (FILE *fp, const gcov_pmu_st_entry_t *st_entry,
                              const enum print_newline newline)
{
  if (!st_entry)
      return;
  fprintf (fp, " %d %s", st_entry->index, st_entry->str);
  if (newline == add_newline)
    fprintf (fp, "\n");
}

/* Print TOOL_HEADER into the file pointed by FP.  NEWLINE specifies
   whether or not to print a trailing newline.  */

GCOV_LINKAGE void
print_pmu_tool_header (FILE *fp, gcov_pmu_tool_header_t *tool_header,
                       const enum print_newline newline)
{
  if (!tool_header)
    return;
  fprintf (fp, "\nhost_cpu: %s\n", tool_header->host_cpu);
  fprintf (fp, "hostname: %s\n", tool_header->hostname);
  fprintf (fp, "kernel_version: %s\n", tool_header->kernel_version);
  fprintf (fp, "column_header: %s\n", tool_header->column_header);
  fprintf (fp, "column_description: %s\n", tool_header->column_description);
  fprintf (fp, "full_header: %s\n", tool_header->full_header);
  if (newline == add_newline)
    fprintf (fp, "\n");
}
#endif

#if IN_GCOV > 0
/* Return the modification time of the current gcov file.  */

GCOV_LINKAGE time_t
gcov_time (void)
{
  struct stat status;

  if (fstat (fileno (gcov_var.file), &status))
    return 0;
  else
    return status.st_mtime;
}
#endif /* IN_GCOV */

#if IN_LIBGCOV || !IN_GCOV
/* Determine the index into histogram for VALUE. */

static unsigned
gcov_histo_index(gcov_type value)
{
  gcov_type_unsigned v = (gcov_type_unsigned)value;
  unsigned r = 0;
  unsigned prev2bits = 0;

  /* Find index into log2 scale histogram, where each of the log2
     sized buckets is divided into 4 linear sub-buckets for better
     focus in the higher buckets.  */

  /* Find the place of the most-significant bit set.  */
  if (v > 0)
    r = 63 - __builtin_clzll (v);

  /* If at most the 2 least significant bits are set (value is
     0 - 3) then that value is our index into the lowest set of
     four buckets.  */
  if (r < 2)
    return (unsigned)value;

  gcc_assert (r < 64);

  /* Find the two next most significant bits to determine which
     of the four linear sub-buckets to select.  */
  prev2bits = (v >> (r - 2)) & 0x3;
  /* Finally, compose the final bucket index from the log2 index and
     the next 2 bits. The minimum r value at this point is 2 since we
     returned above if r was 2 or more, so the minimum bucket at this
     point is 4.  */
  return (r - 1) * 4 + prev2bits;
}

/* Merge SRC_HISTO into TGT_HISTO. The counters are assumed to be in
   the same relative order in both histograms, and are matched up
   and merged in reverse order. Each counter is assigned an equal portion of
   its entry's original cumulative counter value when computing the
   new merged cum_value.  */

static void gcov_histogram_merge(gcov_bucket_type *tgt_histo,
                                 gcov_bucket_type *src_histo)
{
  int src_i, tgt_i, tmp_i = 0;
  unsigned src_num, tgt_num, merge_num;
  gcov_type src_cum, tgt_cum, merge_src_cum, merge_tgt_cum, merge_cum;
  gcov_type merge_min;
  gcov_bucket_type tmp_histo[GCOV_HISTOGRAM_SIZE];
  int src_done = 0;

  memset(tmp_histo, 0, sizeof (gcov_bucket_type) * GCOV_HISTOGRAM_SIZE);

  /* Assume that the counters are in the same relative order in both
     histograms. Walk the histograms from largest to smallest entry,
     matching up and combining counters in order.  */
  src_num = 0;
  src_cum = 0;
  src_i = GCOV_HISTOGRAM_SIZE - 1;
  for (tgt_i = GCOV_HISTOGRAM_SIZE - 1; tgt_i >= 0 && !src_done; tgt_i--)
    {
      tgt_num = tgt_histo[tgt_i].num_counters;
      tgt_cum = tgt_histo[tgt_i].cum_value;
      /* Keep going until all of the target histogram's counters at this
         position have been matched and merged with counters from the
         source histogram.  */
      while (tgt_num > 0 && !src_done)
        {
          /* If this is either the first time through this loop or we just
             exhausted the previous non-zero source histogram entry, look
             for the next non-zero source histogram entry.  */
          if (!src_num)
            {
              /* Locate the next non-zero entry.  */
              while (src_i >= 0 && !src_histo[src_i].num_counters)
                src_i--;
              /* If source histogram has fewer counters, then just copy over the
                 remaining target counters and quit.  */
              if (src_i < 0)
                {
                  tmp_histo[tgt_i].num_counters += tgt_num;
                  tmp_histo[tgt_i].cum_value += tgt_cum;
                  if (!tmp_histo[tgt_i].min_value ||
                      tgt_histo[tgt_i].min_value < tmp_histo[tgt_i].min_value)
                    tmp_histo[tgt_i].min_value = tgt_histo[tgt_i].min_value;
                  while (--tgt_i >= 0)
                    {
                      tmp_histo[tgt_i].num_counters
                          += tgt_histo[tgt_i].num_counters;
                      tmp_histo[tgt_i].cum_value += tgt_histo[tgt_i].cum_value;
                      if (!tmp_histo[tgt_i].min_value ||
                          tgt_histo[tgt_i].min_value
                          < tmp_histo[tgt_i].min_value)
                        tmp_histo[tgt_i].min_value = tgt_histo[tgt_i].min_value;
                    }

                  src_done = 1;
                  break;
                }

              src_num = src_histo[src_i].num_counters;
              src_cum = src_histo[src_i].cum_value;
            }

          /* The number of counters to merge on this pass is the minimum
             of the remaining counters from the current target and source
             histogram entries.  */
          merge_num = tgt_num;
          if (src_num < merge_num)
            merge_num = src_num;

          /* The merged min_value is the sum of the min_values from target
             and source.  */
          merge_min = tgt_histo[tgt_i].min_value + src_histo[src_i].min_value;

          /* Compute the portion of source and target entries' cum_value
             that will be apportioned to the counters being merged.
             The total remaining cum_value from each entry is divided
             equally among the counters from that histogram entry if we
             are not merging all of them.  */
          merge_src_cum = src_cum;
          if (merge_num < src_num)
            merge_src_cum = merge_num * src_cum / src_num;
          merge_tgt_cum = tgt_cum;
          if (merge_num < tgt_num)
            merge_tgt_cum = merge_num * tgt_cum / tgt_num;
          /* The merged cum_value is the sum of the source and target
             components.  */
          merge_cum = merge_src_cum + merge_tgt_cum;

          /* Update the remaining number of counters and cum_value left
             to be merged from this source and target entry.  */
          src_cum -= merge_src_cum;
          tgt_cum -= merge_tgt_cum;
          src_num -= merge_num;
          tgt_num -= merge_num;

          /* The merged counters get placed in the new merged histogram
             at the entry for the merged min_value.  */
          tmp_i = gcov_histo_index(merge_min);
          gcc_assert (tmp_i < GCOV_HISTOGRAM_SIZE);
          tmp_histo[tmp_i].num_counters += merge_num;
          tmp_histo[tmp_i].cum_value += merge_cum;
          if (!tmp_histo[tmp_i].min_value ||
              merge_min < tmp_histo[tmp_i].min_value)
            tmp_histo[tmp_i].min_value = merge_min;

          /* Ensure the search for the next non-zero src_histo entry starts
             at the next smallest histogram bucket.  */
          if (!src_num)
            src_i--;
        }
    }

  gcc_assert (tgt_i < 0);

  /* In the case where there were more counters in the source histogram,
     accumulate the remaining unmerged cumulative counter values. Add
     those to the smallest non-zero target histogram entry. Otherwise,
     the total cumulative counter values in the histogram will be smaller
     than the sum_all stored in the summary, which will complicate
     computing the working set information from the histogram later on.  */
  if (src_num)
    src_i--;
  while (src_i >= 0)
    {
      src_cum += src_histo[src_i].cum_value;
      src_i--;
    }
  /* At this point, tmp_i should be the smallest non-zero entry in the
     tmp_histo.  */
  gcc_assert(tmp_i >= 0 && tmp_i < GCOV_HISTOGRAM_SIZE
             && tmp_histo[tmp_i].num_counters > 0);
  tmp_histo[tmp_i].cum_value += src_cum;

  /* Finally, copy the merged histogram into tgt_histo.  */
  memcpy(tgt_histo, tmp_histo, sizeof (gcov_bucket_type) * GCOV_HISTOGRAM_SIZE);
}
#endif /* IN_LIBGCOV || !IN_GCOV */

#ifdef __GCOV_KERNEL__

/* File fclose operation in kernel mode.  */

int
kernel_file_fclose (gcov_kernel_vfile *fp)
{
  return 0;
}

/* File ftell operation in kernel mode. It currently should not
   be called.  */

long
kernel_file_ftell (gcov_kernel_vfile *fp)
{
  return 0;
}

/* File fseek operation in kernel mode. It should only be called
   with OFFSET==0 and WHENCE==0 to a freshly opened file.  */

int
kernel_file_fseek (gcov_kernel_vfile *fp, long offset, int whence)
{
  gcc_assert (offset == 0 && whence == 0 && fp->count == 0);
  return 0;
}

/* File ftruncate operation in kernel mode. It currently should not
   be called.  */

int
kernel_file_ftruncate (gcov_kernel_vfile *fp, off_t value)
{
  gcc_assert (0);  /* should not reach here */
  return 0;
}

/* File fread operation in kernel mode. It currently should not
   be called.  */

int
kernel_file_fread (void *ptr, size_t size, size_t nitems,
                  gcov_kernel_vfile *fp)
{
  gcc_assert (0);  /* should not reach here */
  return 0;
}

/* File fwrite operation in kernel mode. It outputs the data
   to a buffer in the virual file.  */

int
kernel_file_fwrite (const void *ptr, size_t size,
                   size_t nitems, gcov_kernel_vfile *fp)
{
  char *vbuf;
  unsigned vsize, vpos;
  unsigned len;

  if (!fp) return 0;

  vbuf = fp->buf;
  vsize = fp->size;
  vpos = fp->count;

  if (vsize <= vpos)
    {
      printk (KERN_ERR
         "GCOV_KERNEL: something wrong in file %s: vbuf=%p vsize=%u"
         " vpos=%u\n",
          fp->info->filename, vbuf, vsize, vpos);
      return 0;
    }
  len = vsize - vpos;
  len /= size;

  if (len > nitems)
    len = nitems;

  memcpy (vbuf+vpos, ptr, size*len);
  fp->count += len*size;

  if (len != nitems)
    printk (KERN_ERR
        "GCOV_KERNEL: something wrong in file %s: size=%lu nitems=%lu"
        " ret=%d, vsize=%u vpos=%u \n",
        fp->info->filename, size, nitems, len, vsize, vpos);
  return len;
}

/* File fileno operation in kernel mode. It currently should not
   be called.  */

int
kernel_file_fileno (gcov_kernel_vfile *fp)
{
  gcc_assert (0);  /* should not reach here */
  return 0;
}
#else /* __GCOV_KERNEL__ */

#if IN_GCOV != 1
/* Delete pmu tool header TOOL_HEADER.  */

GCOV_LINKAGE void
destroy_pmu_tool_header (gcov_pmu_tool_header_t *tool_header)
{
  if (!tool_header)
    return;
  if (tool_header->host_cpu)
    free (tool_header->host_cpu);
  if (tool_header->hostname)
    free (tool_header->hostname);
  if (tool_header->kernel_version)
    free (tool_header->kernel_version);
  if (tool_header->column_header)
    free (tool_header->column_header);
  if (tool_header->column_description)
    free (tool_header->column_description);
  if (tool_header->full_header)
    free (tool_header->full_header);
}
#endif

#endif /* GCOV_KERNEL */
