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

#if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 10)
/* Use slots in the TCB head rather than __thread lookups.
   GLIBC has reserved words 10 through 15 for TM.  */
#define HAVE_ARCH_GTM_THREAD
#define HAVE_ARCH_GTM_THREAD_TX
#define HAVE_ARCH_GTM_THREAD_DISP

#ifdef __LP64__
# define SEG_READ(OFS)	"movq\t%%fs:(" #OFS "*8),%0"
# define SEG_WRITE(OFS)	"movq\t%0,%%fs:(" #OFS "*8)"
#else
# define SEG_READ(OFS)	"movl\t%%gs:(" #OFS "*4),%0"
# define SEG_WRITE(OFS)	"movl\t%0,%%gs:(" #OFS "*4)"
#endif

static inline struct gtm_thread *gtm_thr(void)
{
  struct gtm_thread *r;
  asm (SEG_READ(10) : "=r"(r));
  return r;
}

static inline void setup_gtm_thr(void)
{
  if (gtm_thr() == NULL)
    asm volatile (SEG_WRITE(10) : : "r"(&_gtm_thr));
}

static inline struct gtm_transaction * gtm_tx(void)
{
  struct gtm_transaction *r;
  asm (SEG_READ(11) : "=r"(r));
  return r;
}

static inline void set_gtm_tx(struct gtm_transaction *x)
{
  asm volatile (SEG_WRITE(11) : : "r"(x));
}

static inline const struct gtm_dispatch *gtm_disp(void)
{
  const struct gtm_dispatch *r;
  asm (SEG_READ(12) : "=r"(r));
  return r;
}

static inline void set_gtm_disp(const struct gtm_dispatch *x)
{
  asm volatile (SEG_WRITE(12) : : "r"(x));
}
#endif /* >= GLIBC 2.10 */
