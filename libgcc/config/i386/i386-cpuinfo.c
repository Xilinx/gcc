/* Get CPU type and Features for x86 processors.
   Copyright (C) 2011 Free Software Foundation, Inc.
   Contributed by Sriraman Tallam (tmsriram@google.com)

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
<http://www.gnu.org/licenses/>. */

#include "cpuid.h"
#include "tsystem.h"

int __cpu_indicator_init (void) __attribute__ ((constructor (101)));

enum vendor_signatures
{
  SIG_INTEL =	0x756e6547 /* Genu */,
  SIG_AMD =	0x68747541 /* Auth */
};

/* ISA Features supported. */

struct __processor_features
{
  unsigned int __cpu_cmov : 1;
  unsigned int __cpu_mmx : 1;
  unsigned int __cpu_popcnt : 1;
  unsigned int __cpu_sse : 1;
  unsigned int __cpu_sse2 : 1;
  unsigned int __cpu_sse3 : 1;
  unsigned int __cpu_ssse3 : 1;
  unsigned int __cpu_sse4_1 : 1;
  unsigned int __cpu_sse4_2 : 1;
} __cpu_features;

/* Processor Model. */

struct __processor_model
{
  /* Vendor. */
  unsigned int __cpu_is_amd : 1;
  unsigned int __cpu_is_intel : 1;
  /* CPU type. */
  unsigned int __cpu_is_intel_atom : 1;
  unsigned int __cpu_is_intel_core2 : 1;
  unsigned int __cpu_is_intel_corei7 : 1;
  unsigned int __cpu_is_intel_corei7_nehalem : 1;
  unsigned int __cpu_is_intel_corei7_westmere : 1;
  unsigned int __cpu_is_intel_corei7_sandybridge : 1;
  unsigned int __cpu_is_amdfam10h : 1;
  unsigned int __cpu_is_amdfam10h_barcelona : 1;
  unsigned int __cpu_is_amdfam10h_shanghai : 1;
  unsigned int __cpu_is_amdfam10h_istanbul : 1;
  unsigned int __cpu_is_amdfam15h_bdver1 : 1;
  unsigned int __cpu_is_amdfam15h_bdver2 : 1;
} __cpu_model;

/* Get the specific type of AMD CPU.  */

static void
get_amd_cpu (unsigned int family, unsigned int model)
{
  switch (family)
    {
    /* AMD Family 10h.  */
    case 0x10:
      switch (model)
	{
	case 0x2:
	  /* Barcelona.  */
	  __cpu_model.__cpu_is_amdfam10h = 1;
	  __cpu_model.__cpu_is_amdfam10h_barcelona = 1;
	  break;
	case 0x4:
	  /* Shanghai.  */
	  __cpu_model.__cpu_is_amdfam10h = 1;
	  __cpu_model.__cpu_is_amdfam10h_shanghai = 1;
	  break;
	case 0x8:
	  /* Istanbul.  */
	  __cpu_model.__cpu_is_amdfam10h = 1;
	  __cpu_model.__cpu_is_amdfam10h_istanbul = 1;
	  break;
	default:
	  break;
	}
      break;
    /* AMD Family 15h.  */
    case 0x15:
      /* Bulldozer version 1.  */
      if (model >= 0 && model <= 0xf)
        __cpu_model.__cpu_is_amdfam15h_bdver1 = 1;
      /* Bulldozer version 2.  */
      if (model >= 0x10 && model <= 0x1f)
        __cpu_model.__cpu_is_amdfam15h_bdver2 = 1;
      break;
    default:
      break;
    }
}

/* Get the specific type of Intel CPU.  */

static void
get_intel_cpu (unsigned int family, unsigned int model, unsigned int brand_id)
{
  /* Parse family and model only if brand ID is 0. */
  if (brand_id == 0)
    {
      switch (family)
	{
	case 0x5:
	  /* Pentium.  */
	  break;
	case 0x6:
	  switch (model)
	    {
	    case 0x1c:
	    case 0x26:
	      /* Atom.  */
	      __cpu_model.__cpu_is_intel_atom = 1;
	      break;
	    case 0x1a:
	    case 0x1e:
	    case 0x1f:
	    case 0x2e:
	      /* Nehalem.  */
	      __cpu_model.__cpu_is_intel_corei7 = 1;
	      __cpu_model.__cpu_is_intel_corei7_nehalem = 1;
	      break;
	    case 0x25:
	    case 0x2c:
	    case 0x2f:
	      /* Westmere.  */
	      __cpu_model.__cpu_is_intel_corei7 = 1;
	      __cpu_model.__cpu_is_intel_corei7_westmere = 1;
	      break;
	    case 0x2a:
	      /* Sandy Bridge.  */
	      __cpu_model.__cpu_is_intel_corei7 = 1;
	      __cpu_model.__cpu_is_intel_corei7_sandybridge = 1;
	      break;
	    case 0x17:
	    case 0x1d:
	      /* Penryn.  */
	    case 0x0f:
	      /* Merom.  */
	      __cpu_model.__cpu_is_intel_core2 = 1;
	      break;
	    default:
	      break;
	    }
	  break;
	default:
	  /* We have no idea.  */
	  break;
	}
    }
}	             	

static void
get_available_features (unsigned int ecx, unsigned int edx)
{
  __cpu_features.__cpu_cmov = (edx & bit_CMOV) ? 1 : 0;
  __cpu_features.__cpu_mmx = (edx & bit_MMX) ? 1 : 0;
  __cpu_features.__cpu_sse = (edx & bit_SSE) ? 1 : 0;
  __cpu_features.__cpu_sse2 = (edx & bit_SSE2) ? 1 : 0;
  __cpu_features.__cpu_popcnt = (ecx & bit_POPCNT) ? 1 : 0;
  __cpu_features.__cpu_sse3 = (ecx & bit_SSE3) ? 1 : 0;
  __cpu_features.__cpu_ssse3 = (ecx & bit_SSSE3) ? 1 : 0;
  __cpu_features.__cpu_sse4_1 = (ecx & bit_SSE4_1) ? 1 : 0;
  __cpu_features.__cpu_sse4_2 = (ecx & bit_SSE4_2) ? 1 : 0;
}


/* Sanity check for the vendor and cpu type flags.  */

static int
sanity_check (void)
{
  unsigned int one_type = 0;

  /* Vendor cannot be Intel and AMD. */
  gcc_assert((__cpu_model.__cpu_is_intel == 0)
             || (__cpu_model.__cpu_is_amd == 0));
 
  /* Only one CPU type can be set. */
  one_type = (__cpu_model.__cpu_is_intel_atom
	      + __cpu_model.__cpu_is_intel_core2
	      + __cpu_model.__cpu_is_intel_corei7_nehalem
	      + __cpu_model.__cpu_is_intel_corei7_westmere
	      + __cpu_model.__cpu_is_intel_corei7_sandybridge
	      + __cpu_model.__cpu_is_amdfam10h_barcelona
	      + __cpu_model.__cpu_is_amdfam10h_shanghai
	      + __cpu_model.__cpu_is_amdfam10h_istanbul
	      + __cpu_model.__cpu_is_amdfam15h_bdver1
	      + __cpu_model.__cpu_is_amdfam15h_bdver2);

  gcc_assert (one_type <= 1);
  return 0;
}

/* A noinline function calling __get_cpuid. Having many calls to
   cpuid in one function in 32-bit mode causes GCC to complain:
   "can’t find a register in class ‘CLOBBERED_REGS’".  This is
   related to PR rtl-optimization 44174. */

static int __attribute__ ((noinline))
__get_cpuid_output (unsigned int __level,
		    unsigned int *__eax, unsigned int *__ebx,
		    unsigned int *__ecx, unsigned int *__edx)
{
  return __get_cpuid (__level, __eax, __ebx, __ecx, __edx);
}


/* A constructor function that is sets __cpu_model and __cpu_features with
   the right values.  This needs to run only once.  This constructor is
   given the highest priority and it should run before constructors without
   the priority set.  However, it still runs after ifunc initializers and
   needs to be called explicitly there.  */

int __attribute__ ((constructor (101)))
__cpu_indicator_init (void)
{
  unsigned int eax, ebx, ecx, edx;

  int max_level = 5;
  unsigned int vendor;
  unsigned int model, family, brand_id;
  unsigned int extended_model, extended_family;
  static int called = 0;

  /* This function needs to run just once.  */
  if (called)
    return 0;
  else
    called = 1;

  /* Assume cpuid insn present. Run in level 0 to get vendor id. */
  if (!__get_cpuid_output (0, &eax, &ebx, &ecx, &edx))
    return -1;

  vendor = ebx;
  max_level = eax;

  if (max_level < 1)
    return -1;

  if (!__get_cpuid_output (1, &eax, &ebx, &ecx, &edx))
    return -1;

  model = (eax >> 4) & 0x0f;
  family = (eax >> 8) & 0x0f;
  brand_id = ebx & 0xff;
  extended_model = (eax >> 12) & 0xf0;
  extended_family = (eax >> 20) & 0xff;

  if (vendor == SIG_INTEL)
    {
      /* Adjust model and family for Intel CPUS. */
      if (family == 0x0f)
	{
	  family += extended_family;
	  model += extended_model;
	}
      else if (family == 0x06)
	model += extended_model;

      /* Get CPU type.  */
      __cpu_model.__cpu_is_intel = 1;
      get_intel_cpu (family, model, brand_id);
    }

  if (vendor == SIG_AMD)
    {
      /* Adjust model and family for AMD CPUS. */
      if (family == 0x0f)
	{
	  family += extended_family;
	  model += (extended_model << 4);
	}

      /* Get CPU type.  */
      __cpu_model.__cpu_is_amd = 1;
      get_amd_cpu (family, model);
    }

  /* Find available features. */
  get_available_features (ecx, edx);

  sanity_check ();

  return 0;
}
