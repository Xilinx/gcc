/* Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "libitm_i.h"

#if defined(HAVE_MMAP_ANON) || defined(HAVE_MMAP_DEV_ZERO)
#include <sys/mman.h>
#include <fcntl.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif


static gtm_cacheline_page *free_pages;

#if defined(HAVE_MMAP_ANON)
# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#  define MAP_ANONYMOUS MAP_ANON
# endif
# define dev_zero -1
#elif defined(HAVE_MMAP_DEV_ZERO)
# ifndef MAP_ANONYMOUS
#  define MAP_ANONYMOUS 0
# endif
static int dev_zero = -1;
#endif

#if defined(HAVE_MMAP_ANON) || defined(HAVE_MMAP_DEV_ZERO)
/* If we get here, we've already opened /dev/zero and verified that
   PAGE_SIZE is valid for the system.  */
static gtm_cacheline_page * UNUSED
alloc_mmap (void)
{
  gtm_cacheline_page *r;
  r = (gtm_cacheline_page *) mmap (NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, dev_zero, 0);
  if (r == (gtm_cacheline_page *) MAP_FAILED)
    abort ();
  return r;
}
#endif /* MMAP_ANON | MMAP_DEV_ZERO */

#ifdef HAVE_MEMALIGN
static gtm_cacheline_page * alloc_memalign (void) UNUSED;
static gtm_cacheline_page *
alloc_memalign (void)
{
  gtm_cacheline_page *r;
  r = (gtm_cacheline_page *) memalign (PAGE_SIZE, PAGE_SIZE);
  if (r == NULL)
    abort ();
  return r;
}
#endif /* MEMALIGN */

#ifdef HAVE_POSIX_MEMALIGN
static gtm_cacheline_page *alloc_posix_memalign (void) UNUSED;
static gtm_cacheline_page *
alloc_posix_memalign (void)
{
  void *r;
  if (posix_memalign (&r, PAGE_SIZE, PAGE_SIZE))
    abort ();
  return (gtm_cacheline_page *) r;
}
#endif /* POSIX_MEMALIGN */

#if defined(HAVE_MMAP_ANON) && defined(FIXED_PAGE_SIZE)
# define alloc_page  alloc_mmap
#elif defined(HAVE_MMAP_DEV_ZERO) && defined(FIXED_PAGE_SIZE)
static gtm_cacheline_page *
alloc_page (void)
{
  if (dev_zero < 0)
    {
      dev_zero = open ("/dev/zero", O_RDWR);
      assert (dev_zero >= 0);
    }
  return alloc_mmap ();
}
#elif defined(HAVE_MMAP_ANON) || defined(HAVE_MMAP_DEV_ZERO)
static gtm_cacheline_page * (*alloc_page) (void);
static void __attribute__((constructor))
init_alloc_page (void)
{
  size_t page_size = getpagesize ();
  if (page_size <= PAGE_SIZE && PAGE_SIZE % page_size == 0)
    {
# ifndef HAVE_MMAP_ANON
      dev_zero = open ("/dev/zero", O_RDWR);
      assert (dev_zero >= 0);
# endif
      alloc_page = alloc_mmap;
      return;
    }
# ifdef HAVE_MEMALIGN
  alloc_page = alloc_memalign;
# elif defined(HAVE_POSIX_MEMALIGN)
  alloc_page = alloc_posix_memalign;
# else
#  error "No fallback aligned memory allocation method"
# endif
}
#elif defined(HAVE_MEMALIGN)
# define alloc_page  alloc_memalign
#elif defined(HAVE_POSIX_MEMALIGN)
# define alloc_page  alloc_posix_memalign
#else
# error "No aligned memory allocation method"
#endif


gtm_cacheline_page *
GTM_page_alloc (void)
{
  gtm_cacheline_page *r = free_pages;

 restart:
  if (r)
    {
      gtm_cacheline_page *n, *p = r->prev;
      n = __sync_val_compare_and_swap (&free_pages, r, p);
      if (n != r)
	{
	  r = n;
	  goto restart;
	}
    }
  else
    r = alloc_page ();

  return r;
}

void
GTM_page_release (gtm_cacheline_page *head, gtm_cacheline_page *tail)
{
  gtm_cacheline_page *n, *p = free_pages;

  /* ??? We should eventually free some of these.  */

 restart:
  tail->prev = p;
  n = __sync_val_compare_and_swap (&free_pages, p, head);
  if (n != p)
    {
      p = n;
      goto restart;
    }
}
