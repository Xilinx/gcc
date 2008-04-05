/* Mudflap: narrow-pointer bounds-checking by tree rewriting.
   Copyright (C) 2002, 2003, 2004, 2005, 2007, 2008 
   Free Software Foundation, Inc.
   Contributed by Frank Ch. Eigler <fche@redhat.com>
   and Graydon Hoare <graydon@redhat.com>
   Splay Tree code originally by Mark Mitchell <mark@markmitchell.com>,
   adapted from libiberty.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   In addition to the permissions in the GNU General Public License, the
   Free Software Foundation gives you unlimited permission to link the
   compiled version of this file into combinations with other programs,
   and to distribute those combinations without any restriction coming
   from the use of this file.  (The General Public License restrictions
   do apply in other respects; for example, they cover modification of
   the file, and distribution when not linked into a combine
   executable.)

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "config.h"

/* These attempt to coax various unix flavours to declare all our
   needed tidbits in the system headers.  */
#if !defined(__FreeBSD__) && !defined(__APPLE__)
#define _POSIX_SOURCE
#endif /* Some BSDs break <sys/socket.h> if this is defined. */
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#define _BSD_TYPES
#define __EXTENSIONS__
#define _ALL_SOURCE
#define _LARGE_FILE_API
#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#include <assert.h>

#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

#include "bounds-runtime.h"
#include "bounds-impl.h"
#include "bounds-splay-tree.h"
#include "bounds-oob.h"

/* ------------------------------------------------------------------------ */
/* Utility macros */

/* Codes to describe the context in which a violation occurs.  */
#define __MF_VIOL_UNKNOWN 0
#define __MF_VIOL_READ 1
#define __MF_VIOL_WRITE 2
#define __MF_VIOL_REGISTER 3
#define __MF_VIOL_UNREGISTER 4
#define __MF_VIOL_ARITH 5

/* Protect against recursive calls.  */

static void
begin_recursion_protect1 (const char *pf)
{
  if (__bounds_get_state () == reentrant)
    {
      write (2, "bounds: erroneous reentrancy detected in `", 38);
      write (2, pf, strlen(pf));
      write (2, "'\n", 2); \
			     abort ();
    }
  __bounds_set_state (reentrant);
}

#define BEGIN_RECURSION_PROTECT() \
  begin_recursion_protect1 (__PRETTY_FUNCTION__)

#define END_RECURSION_PROTECT() \
  __bounds_set_state (active)

/* ------------------------------------------------------------------------ */
/* Globals                                                                  */

int __bounds_starting_p = 1;
enum __bounds_state_enum __bounds_state_1 = reentrant;

const char* __bounds_typename[] = /* for use in tracing */
  {"NOACCESS", "HEAP", "HEAP_I", "STACK", "STATIC", "GUESS"};

/* ------------------------------------------------------------------------ */
/* mode-check-related globals.  */

typedef struct __bounds_object
{
  /* __bounds_register parameters */
  uintptr_t low;              /* Points to base of the object. JK: base */
  uintptr_t high;             /* Points to last byte in object. JK: extent-1 */
   
  const char *name;           /* Name of the object.  */
  char type;                  /* __MF_TYPE_something */
} __bounds_object_t;

/* Cache of __bounds_object's.  */
#define LOOKUP_CACHE_MASK_DFL 1023
#define LOOKUP_CACHE_SIZE_MAX 65536 /* Allows max CACHE_MASK 0xFFFF */
#define LOOKUP_CACHE_SHIFT_DFL 2

__bounds_object_t *__bounds_lookup_cache [LOOKUP_CACHE_SIZE_MAX];
uintptr_t __bounds_lc_mask = LOOKUP_CACHE_MASK_DFL;
unsigned char __bounds_lc_shift = LOOKUP_CACHE_SHIFT_DFL;

static void
__bounds_violation (void *ptr, size_t sz, uintptr_t pc, /* was __mf_violation */
		    const char *location, int type, const char *msg)
{
  /* Print out a basic warning message.  */
  struct timeval now = { 0, 0 };
#if HAVE_GETTIMEOFDAY
  gettimeofday (& now, NULL);
#endif
  fprintf (stderr,
	   "*******\n"
	   "bounds violation (%s): time=%lu.%06lu "
	   "ptr=%p size=%lu\npc=%p%s%s%s%s%s%s\n",
	   ((type == __MF_VIOL_READ) ? "check/read" :
	    (type == __MF_VIOL_WRITE) ? "check/write" :
	    (type == __MF_VIOL_REGISTER) ? "register" :
	    (type == __MF_VIOL_UNREGISTER) ? "unregister" :
	    (type == __MF_VIOL_ARITH) ? "arithmetic" : "unknown"),
	   now.tv_sec, now.tv_usec,
	   (void *) ptr, (unsigned long)sz, (void *) pc,
	   (location != NULL ? "\nlocation=`" : ""),
	   (location != NULL ? location : ""),
	   (location != NULL ? "'" : ""),
	   (msg != NULL ? "\nerror=`" : ""),
	   (msg != NULL ? msg : ""),
	   (msg != NULL ? "'" : ""));

  abort ();
}


/* Live objects: splay trees, separated by type, ordered on .low (base address).  */
/* Actually stored as static vars within lookup function below.  */

/* ------------------------------------------------------------------------ */
/* Splay tree manipulation functions  */

/* Remove this referent object from the lookup cache.  */

static void
__bounds_uncache_object (__bounds_object_t *old_obj)
{
  uintptr_t low = old_obj->low;
  uintptr_t high = old_obj->high;
  unsigned i;

  if ((high - low) >= (__bounds_lc_mask << __bounds_lc_shift))
    {
      /* For large objects (>= cache size - 1) check the whole cache.  */
      for (i = 0; i <= __bounds_lc_mask; i++)
	if (__bounds_lookup_cache[i] 
	    && __bounds_lookup_cache[i]->low == low)
	  __bounds_lookup_cache[i] = NULL;
    }
  else
    {
      /* Object is now smaller than cache size.  */
      unsigned entry_low_idx = (low >> __bounds_lc_shift) & __bounds_lc_mask;
      unsigned entry_high_idx = (high >> __bounds_lc_shift) & __bounds_lc_mask;
      if (entry_low_idx <= entry_high_idx)
	{
	  for (i = entry_low_idx; i <= entry_high_idx; i++)
	    if (__bounds_lookup_cache[i] 
		&& __bounds_lookup_cache[i]->low == low)
	      __bounds_lookup_cache[i] = NULL;
	}
      else
	{
	  /* Object wrapped around the end of the cache.  First search
	     from low to end of cache and then from 0 to high.  */
	  for (i = entry_low_idx; i <= __bounds_lc_mask; i++)
	    if (__bounds_lookup_cache[i] 
		&& __bounds_lookup_cache[i]->low == low)
	      __bounds_lookup_cache[i] = NULL;

	  for (i = 0; i <= entry_high_idx; i++)
	    if (__bounds_lookup_cache[i] 
		&& __bounds_lookup_cache[i]->low == low)
	      __bounds_lookup_cache[i] = NULL;
	}
    }
}

/* ------------------------------------------------------------------------ */
/* Lookup & manage automatic initialization of the five or so splay trees.  */

static boundssplay_tree
__bounds_object_tree (int type)
{
  static boundssplay_tree trees [__MF_TYPE_MAX+1];
  assert (type >= 0 && type <= __MF_TYPE_MAX);
  if (UNLIKELY (trees[type] == NULL))
    trees[type] = boundssplay_tree_new ();
  return trees[type];
}

static void
__bounds_link_object (__bounds_object_t *node)
{
  boundssplay_tree t = __bounds_object_tree (node->type);
  boundssplay_tree_insert (t, (boundssplay_tree_key) node->low, (boundssplay_tree_value) node);
  __bounds_lookup_cache[(node->low >> __bounds_lc_shift) & __bounds_lc_mask] = node;
}

static void
__bounds_unlink_object (__bounds_object_t *node)
{
  boundssplay_tree t = __bounds_object_tree (node->type);
  boundssplay_tree_remove (t, (boundssplay_tree_key) node->low);
  __bounds_uncache_object (node);
}

static __bounds_object_t *
__bounds_insert_new_object (uintptr_t low, uintptr_t high, int type,
			    const char *name)
{
  DECLARE (void *, malloc, size_t c);

  __bounds_object_t *new_obj;
  new_obj = CALL_REAL (malloc, sizeof(__bounds_object_t));
  new_obj->low = low;
  new_obj->high = high;
  new_obj->type = type;
  new_obj->name = name;

  __bounds_link_object (new_obj);
  return new_obj;
}

static __bounds_object_t *
__bounds_lookup_object (uintptr_t ptr, int type)
{
  boundssplay_tree t;
  boundssplay_tree_key k;
  boundssplay_tree_node n;
  __bounds_object_t *o;

  o = __bounds_lookup_cache[(ptr >> __bounds_lc_shift) & __bounds_lc_mask];
  if (o && ptr >= o->low && ptr <= o->high)
    return o;

  t = __bounds_object_tree (type);
  k = (boundssplay_tree_key) ptr;

  n = boundssplay_tree_lookup (t, k);

  /* An exact match for base address implies a hit.  */
  if (n != NULL)
    return (__bounds_object_t *) n->value;

  n = boundssplay_tree_predecessor (t, k);
  
  if (n == NULL)
    return NULL;

  o = (__bounds_object_t *) n->value;

  if (ptr >= o->low && ptr <= o->high) /* ptr contained in this object */
    {
      __bounds_lookup_cache[(ptr >> __bounds_lc_shift) & __bounds_lc_mask] = o;
      return o;
    }

  return NULL;
}

static __bounds_object_t *
__bounds_find_referent_object (uintptr_t ptr)
{
  __bounds_object_t* obj;
  int type;

  /* For each type of splay tree.  */
  for (type = __MF_TYPE_HEAP; type <= __MF_TYPE_GUESS; type++)
    {
      obj = __bounds_lookup_object (ptr, type);
      if (obj != NULL)
	return obj;
    }

  return NULL;
}

static void
__bounds_oob_violation (__bounds_oob_ptr_t* oob, size_t sz, uintptr_t pc,
			const char *location)
{
  struct timeval now = { 0, 0 };
#if HAVE_GETTIMEOFDAY
  gettimeofday (& now, NULL);
#endif
  
  /* Lookup object that the pointer referred before going out-of-bounds */
  __bounds_object_t* referent = 
    __bounds_find_referent_object (oob->last_valid);

  fprintf (stderr,
	   "*******\n"
	   "bounds violation caused by earlier pointer arithmetic:\n"
	   "\tThe arithmetic took place at %s\n"
           "\tThe violation occured at %s\n"
	   "\tBefore going out-of-bounds the pointer referred to %s\n"
	   "\ttime=%lu.%06lu ptr=%p size=%lu pc=%p\n",
	   oob->arith_location,
	   location,
           (referent != NULL ? referent->name : "<nothing>"),
	   now.tv_sec, now.tv_usec,
	   (void *) oob->ptr, (unsigned long)sz, (void *) pc);

  abort ();
}

/* ------------------------------------------------------------------------ */
/* Public API functions */

void
__bounds_register (void *ptr, size_t sz, int type, const char *name)
{
  LOCKTH ();
  BEGIN_RECURSION_PROTECT ();
  __bounds_ulock_register (ptr, sz, type, name);
  END_RECURSION_PROTECT ();
  UNLOCKTH ();
}

void
__bounds_ulock_register (void *ptr, size_t sz, int type, const char *name)
{
  /* Bounds of object.  */
  uintptr_t low = (uintptr_t) ptr;
  uintptr_t high = CLAMPSZ (ptr, sz);

  /* Add object to splay tree.  */
  __bounds_object_t *obj = __bounds_insert_new_object (low, high, type, name);
  TRACE ("Added object to splay tree: %s object '%s' from %p to %p\n", 
	 __bounds_typename[(int) obj->type], obj->name, (void *)obj->low, (void *)obj->high);
}

void
__bounds_unregister (void *ptr, size_t sz, int type)
{
  LOCKTH ();
  BEGIN_RECURSION_PROTECT ();
  __bounds_ulock_unregister (ptr, sz, type);
  END_RECURSION_PROTECT ();
  UNLOCKTH ();
}

void
__bounds_ulock_unregister (void *ptr, size_t sz, int type)
{
  DECLARE (void, free, void *p);

  /* Retrieve object from splay tree.  */
  __bounds_object_t *obj = __bounds_find_referent_object ((uintptr_t) ptr);

  TRACE ("Unregistering: %s object '%s' from %p to %p\n", 
	 __bounds_typename[(int) obj->type], obj->name, (void *)obj->low, (void *)obj->high);

  __bounds_unlink_object (obj);
  CALL_REAL (free, obj);
}

void
__bounds_check (void *referent, void *ptr, size_t sz, 
		int access_type, const char *location)
{
  LOCKTH ();
  BEGIN_RECURSION_PROTECT ();
  __bounds_ulock_check (referent, ptr, sz, access_type, location);
  END_RECURSION_PROTECT ();
  UNLOCKTH ();
}

void 
__bounds_ulock_check (void *referent, void *ptr, size_t sz, 
		      int access_type, const char *location)
{
  uintptr_t ptr_low = (uintptr_t) ptr;
  uintptr_t ptr_high = CLAMPSZ (ptr, sz);

  __bounds_object_t* obj;
  __bounds_oob_ptr_t* oob;

  TRACE ("Checking: referent=%p ptr=%p sz=%d location=%s\n",
	 referent, ptr, sz, location); 

  /* Lookup referent object using old pointer value.  */
  obj = __bounds_find_referent_object ((uintptr_t) referent);
  if (UNLIKELY (obj == NULL))
    {
      /* Look for OOB pointer record.  */
      oob = __bounds_get_oob (ptr);
      if (LIKELY (oob))
	__bounds_oob_violation (oob, sz,
				(uintptr_t) __builtin_return_address (0), location);
      else
	TRACE ("Aborting check: foreign pointer (ptr=%p location=%s)\n",
	       ptr, location);

      return;
    }

  /* Check that target base is within referent object.  */
  if (UNLIKELY (ptr_low < obj->low || ptr_low > obj->high))
    __bounds_violation (ptr, sz,
			(uintptr_t) __builtin_return_address (0), location,
			access_type == __MF_CHECK_READ ? __MF_VIOL_READ : __MF_VIOL_WRITE,
			"Start of access is out of bounds");

  /* Check that target limit is within referent object.  */
  if (UNLIKELY ((uintptr_t) ptr_high > obj->high))
    __bounds_violation (ptr, sz,
			(uintptr_t) __builtin_return_address (0), location,
			access_type == __MF_CHECK_READ ? __MF_VIOL_READ : __MF_VIOL_WRITE,
			"End of access is out of bounds");
}

void *
__bounds_arith (void *base, void *addr, const char *location)
{
  void *result;

  LOCKTH ();
  BEGIN_RECURSION_PROTECT ();
  result = __bounds_ulock_arith (base, addr, location);
  END_RECURSION_PROTECT ();
  UNLOCKTH ();

  return result;
}

void *
__bounds_ulock_arith (void *base, void *addr, const char *location)
{
  __bounds_object_t *obj;
  uintptr_t ptr;

  TRACE ("Checking arithmetic(base=%p addr=%p location=%s)\n",
	 base, addr, location); 

  /* 
   * Find referent object for the base pointer.  If found, we check 
   * that the new pointer value (addr) remains within referent object. 
   *
   * If it does, return the calculation result (addr).
   *
   * If not, create an OOB pointer object, store the calculation
   * result in it and return its address as the result.
   *
   * If no referent object found the base pointer either refers to 
   *   (1) an object in unchecked code (foreign)
   *   (2) an out-of-bounds pointer object (OOB)
   *
   *   If from unchecked code, we can never meaningfully check its 
   *   validity so we return the arithmetic result.
   *
   *   If it is an OOB pointer object then this arithmetic will either
   *   bring it back within bounds of its referent object, in which case
   *   we return the calculation result, or it makes no difference so 
   *   we make a copy of the OOB updated to reflect the results of the 
   *   arithmetic and return its address.
   *
   *   Performing the OOB case is harder than it sounds.  We cannot simply
   *   use the new value passed in (addr) as this was calculated using
   *   the incorrect assumption that base was the pointer value.  Therefore,
   *   addr is the results of performing the arithmetic on the OOB object's
   *   address (!) rather than the pointer value.  To solve this, we must
   *   take the difference of base and addr (to find the value of the
   *   arithmetic) and apply this to the stored pointer value.
   */

  /* Lookup referent object using base pointer.  */
  obj = __bounds_find_referent_object ((uintptr_t) base);

  /* XXX: Why is this cast needed? Ask mudflap developer?  */
  ptr = (uintptr_t) addr;

  if (LIKELY (obj))
    {
      VERBOSE_TRACE ("Referent object found for base=%p when "
		     "checking arithmetic at %s\n", base, location);

      if (UNLIKELY (ptr < obj->low || ptr > obj->high))
	{
	  /* Pointer strayed out-of-bounds.  */
	  TRACE ("Arithmetic took pointer outside bounds of referent "
		 "object at %s.  Creating OOB object\n", location);

	  return __bounds_create_oob (addr, base, location);
	}
      else /* Return arithmetic result.  */
	return addr;
    }

  /* Referent object not found.  */
  VERBOSE_TRACE ("Referent object not found for base=%p when "
		 "checking arithmetic at %s\n", base, location);

  /* Look for out-of-bounds pointer record.  */
  __bounds_oob_ptr_t* oob = __bounds_get_oob (base);
  if (LIKELY (oob))
    {
      VERBOSE_TRACE ("OOB found for pointer (base=%p): already out-of-bounds "
		     "when checking arithmetic at %s\n", base, location);
      /* XXX: I can't see why this assertion would ever have been valid:
	 assert (base == (void *) oob->last_valid); */
      assert (base != (void *) oob->last_valid);

      /* Lookup referent object using old pointer value stored in OOB.
	 This may fail if OOB represents uninitialised variable.  */
      obj = __bounds_find_referent_object (oob->last_valid);
      /* XXX: This would be much quicker if we stored the associated
	 referent object in the OOB and used that, however, this
	 assumes that no OOB exists longer than its associated RO
	 (e.g. search OOBs on RO unregister).  */

      /* The new pointer value passed in (addr) can't be used directly
	 as it was calculated using the OOB pointer address rather the
	 real value of the pointer which was stored in the OOB
	 (oob->ptr).  We must redo the arithmetic here in order to get
	 the correct update value.  */
      ptr = oob->ptr + (addr - base);

      if (obj == NULL || ptr < obj->low || ptr > obj->high)
	{
	  /* Pointer remains oob.  */
	  TRACE ("Pointer still oob despite arithmetic at %s. Copying and "
		 "updating OOB (was %p, now %p (last_valid: %p))\n", 
		 location, (void *) oob->ptr, (void *) ptr,
		 (void *) oob->last_valid);

	  /* Create a copy of OOB updated with new pointer value and
	     return its address.  */
	  return __bounds_create_oob ((void *) ptr, 
				      (void *) oob->last_valid, 
				      oob->arith_location);
	}

      /* Pointer within bounds.  */
      TRACE ("Arithmetic brought oob pointer back within bounds at %s\n",
	     location);
    }
  else
    VERBOSE_TRACE ("OOB not found for base=%p when checking arithmetic at %s: "
		   "assuming foreign pointer\n", base, location);

  /* Return arithmetic result.  */
  return (void *) ptr;
}

int
__bounds_comp (void *left, void *right, int op_type, const char *location)
{
  int result;

  LOCKTH ();
  BEGIN_RECURSION_PROTECT ();
  result = __bounds_ulock_comp (left, right, op_type, location);
  END_RECURSION_PROTECT ();
  UNLOCKTH ();

  return result;
}

int
__bounds_ulock_comp (void *left, void *right, int op_type,
		     const char *location)
{
  TRACE ("Pointer comparison(left=%p right=%p op_type=%d)\n",
	 left, right, op_type); 

  /* For both left and right operands (assumes both are pointers):

     - try to find referent object.  If found, use the value of the
     pointer as-is for the comparison,

     - otherwise, try to find an OOB for the pointer:
       - if OOB exists, retrieve the value of the pointer from it and
       use this in the comparison,
    
       - if OOB not found, we assume pointer is foreign and proceed 
       with comparison.
    
     Perform the specified comparison and return the result.
   */

  /* Lookup referent objects.  */
  /* XXX: it may be useful to check if the pointer is NULL to avoid
     redundant RO lookup but, then again, the way it is now may make
     the common case fast.  */
  __bounds_object_t *obj_l = __bounds_find_referent_object ((uintptr_t) left);
  __bounds_object_t *obj_r = __bounds_find_referent_object ((uintptr_t) right);

  if (UNLIKELY (!obj_l))
    {
      /* Referent object not found.  */
      __bounds_oob_ptr_t *oob_l;
 
      VERBOSE_TRACE ("Referent object not found for left-hand pointer in "
		     "comparison at %s\n", location);

      /* Look for out-of-bounds pointer record.  */
      oob_l = __bounds_get_oob (left);


      if (LIKELY (oob_l))
	{
	  VERBOSE_TRACE ("OOB found for left-hand pointer in comparison at %s: "
			 "using stored value for comparison\n", location);

	  left = (void *) oob_l->ptr;
	}
      else
	VERBOSE_TRACE ("OOB not found for left-hand pointer in comparison at %s: "
		       "assuming foreign pointer and using as-is\n", location);
    }
 
  if (UNLIKELY (!obj_r))
    {
      /* Referent object not found.  */
      VERBOSE_TRACE ("Referent object not found for right-hand pointer in "
		     "comparison at %s\n", location);

      /* Look for out-of-bounds pointer record.  */
      __bounds_oob_ptr_t *oob_r = __bounds_get_oob (right);
      if (LIKELY (oob_r))
	{
	  VERBOSE_TRACE ("OOB found for right-hand pointer in comparison at %s: "
			 "using stored value for comparison\n", location);

	  right = (void *) oob_r->ptr;
	}
      else
	VERBOSE_TRACE ("OOB not found for right-hand pointer in comparison at %s: "
		       "assuming foreign pointer and using as-is\n", location);
    }

  /* Perform comparison and return results.  */
  switch (op_type)
    {
    case __BOUNDS_COMP_LT:
      return left < right;
    case __BOUNDS_COMP_LE:
      return left <= right;
    case __BOUNDS_COMP_GT:
      return left > right;
    case __BOUNDS_COMP_GE:
      return left >= right;
    case __BOUNDS_COMP_EQ:
      return left == right;
    case __BOUNDS_COMP_NE:
      return left != right;
    default:
      assert(0);
    }

  assert(0);
  return 0;
}

void *
__bounds_cast (void *ptr, const char *location)
{
  void *result;

  LOCKTH ();
  BEGIN_RECURSION_PROTECT ();
  result = __bounds_ulock_cast (ptr, location);
  END_RECURSION_PROTECT ();
  UNLOCKTH ();

  return result;
}

void *
__bounds_ulock_cast (void *ptr, const char *location)
{
  __bounds_object_t *obj;
  __bounds_oob_ptr_t *oob;

  TRACE ("Pointer cast(ptr=%p)\n", ptr); 

  /* Try to find referent object for the pointer being cast (ptr):
     - if found, return the value of the pointer as-is,
     - otherwise, try to find an OOB for the pointer and:
       - if OOB exists, retrieve the value of the pointer from it and
         return that,
       - if OOB not found, assume pointer is foreign and return it
         as-is.
   */

  /* Lookup referent object.  */
  obj = __bounds_find_referent_object ((uintptr_t) ptr);

  if (LIKELY (obj))
    {
      VERBOSE_TRACE ("Referent object found for pointer in cast at %s: "
		     "using as-is\n", location);
      return ptr;
    }

  VERBOSE_TRACE ("Referent object not found for pointer in cast at %s\n",
		 location);

  /* Look for out-of-bounds pointer record.  */
  oob = __bounds_get_oob (ptr);

  if (LIKELY (oob))
    {
      VERBOSE_TRACE ("OOB found for pointer in cast at %s: "
		     "using stored value\n", location);

      return (void *) oob->ptr;
    }

  VERBOSE_TRACE ("OOB not found for pointer in cast at %s: "
		 "assuming foreign pointer and using as-is\n", location);
  return ptr;
}

/* ------------------------------------------------------------------------ */
/* Position independant code handling                                       */

#ifdef PIC

void
__bounds_resolve_single_dynamic (struct __bounds_dynamic_entry *e)
{
  char *err;

  assert (e);
  if (e->pointer) return;

#if HAVE_DLVSYM
  if (e->version != NULL && e->version[0] != '\0') /* non-null/empty */
    e->pointer = dlvsym (RTLD_NEXT, e->name, e->version);
  else
#endif
    e->pointer = dlsym (RTLD_NEXT, e->name);

  err = dlerror ();

  if (err)
    {
      fprintf (stderr, "bounds: error in dlsym(\"%s\"): %s\n",
               e->name, err);
      abort ();
    }
  if (! e->pointer)
    {
      fprintf (stderr, "bounds: dlsym(\"%s\") = NULL\n", e->name);
      abort ();
    }
}

static void
__bounds_resolve_dynamics ()
{
  int i;
  for (i = 0; i < dyn_INITRESOLVE; i++)
    __bounds_resolve_single_dynamic (& __bounds_dynamic[i]);
}


/* NB: order must match enums in bounds-impl.h */
struct __bounds_dynamic_entry __bounds_dynamic [] =
  {
    {NULL, "calloc", NULL},
    {NULL, "free", NULL},
    {NULL, "malloc", NULL},
    {NULL, "mmap", NULL},
    {NULL, "munmap", NULL},
    {NULL, "realloc", NULL},
    {NULL, "DUMMY", NULL}, /* dyn_INITRESOLVE */
  };

#endif /* PIC */

/* ------------------------------------------------------------------------ */
/* Bounds-checking library constructor/destructor functions                 */

void __bounds_init () __attribute__ ((constructor));

/* not static */ void
__bounds_init ()
{
  TRACE ("__bounds_init\n");

  /* Return if initialization has already been done. */
  if (LIKELY (__bounds_starting_p == 0))
    return;

  /* This initial bootstrap phase requires that __bounds_starting_p = 1. */
#ifdef PIC
  __bounds_resolve_dynamics ();
#endif
  __bounds_starting_p = 0;

  __bounds_set_state (active);

  memset (__bounds_lookup_cache, 0, sizeof (__bounds_lookup_cache));
}

extern void __bounds_fini () __attribute__ ((destructor));
void __bounds_fini ()
{
  TRACE ("__bounds_fini\n");
}

/* ------------------------------------------------------------------------ */
/* Wrap main function */
int
__wrap_main (int argc, char* argv[])
{
  extern char **environ;
  extern int main ();
  extern int __real_main ();
  static int been_here = 0;

  if (!been_here)
    {
      unsigned i;

      been_here = 1;
      __bounds_register (argv, sizeof(char *)*(argc+1), __MF_TYPE_STATIC, "argv[]");
      for (i=0; i<argc; i++)
        {
          unsigned j = strlen (argv[i]);
          __bounds_register (argv[i], j+1, __MF_TYPE_STATIC, "argv element");
        }

      for (i=0; ; i++)
        {
          char *e = environ[i];
          unsigned j;
          if (e == NULL) break;
          j = strlen (environ[i]);
          __bounds_register (environ[i], j+1, __MF_TYPE_STATIC, "environ element");
        }
      __bounds_register (environ, sizeof(char *)*(i+1), __MF_TYPE_STATIC, "environ[]");

      __bounds_register (& errno, sizeof (errno), __MF_TYPE_STATIC, "errno area");

      __bounds_register (stdin,  sizeof (*stdin),  __MF_TYPE_STATIC, "stdin");
      __bounds_register (stdout, sizeof (*stdout), __MF_TYPE_STATIC, "stdout");
      __bounds_register (stderr, sizeof (*stderr), __MF_TYPE_STATIC, "stderr");

      /* Make some effort to register ctype.h static arrays.  */
      /* XXX: e.g., on Solaris, may need to register __ctype, _ctype, __ctype_mask, __toupper, etc. */
      /* On modern Linux GLIBC, these are thread-specific and changeable, and are dealt
         with in mf-hooks2.c.  */
    }

#ifdef PIC
  return main (argc, argv, environ);
#else
  return __real_main (argc, argv, environ);
#endif
}

