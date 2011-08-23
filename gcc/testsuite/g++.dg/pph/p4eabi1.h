// { dg-options "-w -fpermissive" }
// c1eabi1.h	c1eabi1.pph

#ifndef C4EABI1_H
#define C4EABI1_H

/* This file tests most of the non-C++ run-time helper functions
   described in Section 4 of the "Run-Time ABI for the ARM
   Architecture".  These are basic tests; they do not try to validate
   all of the corner cases in these routines.  

   The functions not tested here are:

     __aeabi_cdcmpeq
     __aeabi_cdcmple
     __aeabi_cdrcmple
     __aeabi_cfcmpeq
     __aeabi_cfcmple
     __aeabi_cfrcmple
     __aeabi_ldivmod
     __aeabi_uldivmod
     __aeabi_idivmod
     __aeabi_uidivmod

   These functions have non-standard calling conventions that would
   require the use of inline assembly to test.  It would be good to
   add such tests, but they have not yet been implemented.  

   There are also no tests for the "division by zero", "memory copying,
   clearing, and setting" functions.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* All these functions are defined to use the base ABI, so use the
   attribute to ensure the tests use the base ABI to call them even
   when the VFP ABI is otherwise in effect.  */
#define PCS __attribute__((pcs("aapcs")))

#define decl_float(code, type)						\
  extern type __aeabi_ ## code ## add (type, type) PCS;			\
  extern type __aeabi_ ## code ## div (type, type) PCS;			\
  extern type __aeabi_ ## code ## mul (type, type) PCS;			\
  extern type __aeabi_ ## code ## neg (type) PCS;			\
  extern type __aeabi_ ## code ## rsub (type, type) PCS;		\
  extern type __aeabi_ ## code ## sub (type, type) PCS;			\
  extern int __aeabi_ ## code ## cmpeq (type, type) PCS;		\
  extern int __aeabi_ ## code ## cmplt (type, type) PCS;		\
  extern int __aeabi_ ## code ## cmple (type, type) PCS;		\
  extern int __aeabi_ ## code ## cmpge (type, type) PCS;		\
  extern int __aeabi_ ## code ## cmpgt (type, type) PCS;		\
  extern int __aeabi_ ## code ## cmpun (type, type) PCS;		\
  extern int __aeabi_ ## code ## 2iz (type) PCS;			\
  extern unsigned int __aeabi_ ## code ## 2uiz (type) PCS;		\
  extern long long __aeabi_ ## code ## 2lz (type) PCS;			\
  extern unsigned long long __aeabi_ ## code ## 2ulz (type) PCS;	\
  extern type __aeabi_i2 ## code (int) PCS;				\
  extern type __aeabi_ui2 ## code (int) PCS;				\
  extern type __aeabi_l2 ## code (long long) PCS;			\
  extern type __aeabi_ul2 ## code (unsigned long long) PCS;		\
									\
  type code ## zero = 0.0;						\
  type code ## one = 1.0;						\
  type code ## two = 2.0;						\
  type code ## four = 4.0;						\
  type code ## minus_one = -1.0;					\
  type code ## minus_two = -2.0;					\
  type code ## minus_four = -4.0;					\
  type code ## epsilon = 1E-32;						\
  type code ## NaN = 0.0 / 0.0;

decl_float (d, double)
decl_float (f, float)

extern float __aeabi_d2f (double) PCS;
extern double __aeabi_f2d (float) PCS;
extern long long __aeabi_lmul (long long, long long);
extern long long __aeabi_llsl (long long, int);
extern long long __aeabi_llsr (long long, int);
extern long long __aeabi_lasr (long long, int);
extern int __aeabi_lcmp (long long, long long);
extern int __aeabi_ulcmp (unsigned long long, unsigned long long);
extern int __aeabi_idiv (int, int);
extern unsigned int __aeabi_uidiv (unsigned int, unsigned int);
extern int __aeabi_uread4 (void *);
extern int __aeabi_uwrite4 (int, void *);
extern long long __aeabi_uread8 (void *);
extern long long __aeabi_uwrite8 (long long, void *);

#define eq(a, b, type, abs, epsilon, format)			\
  {								\
    type a1;							\
    type b1;							\
								\
    a1 = a;							\
    b1 = b;							\
    if (abs (a1 - b1) > epsilon)				\
    {								\
      fprintf (stderr, "%d: Test %s == %s\n", __LINE__, #a, #b);	\
      fprintf (stderr, "%d: " format " != " format "\n",	\
	       __LINE__, a1, b1);				\
      abort ();							\
    }								\
  }

#define ieq(a, b) eq (a, b, int, abs, 0, "%d")
#define ueq(a, b) eq (a, b, unsigned int, abs, 0, "%u")
#define leq(a, b) eq (a, b, long long, abs, 0, "%lld")
#define uleq(a, b) eq (a, b, unsigned long long, abs, 0, "%llu")
#define feq(a, b) eq (a, b, float, fabs, fepsilon, "%f")
#define deq(a, b) eq (a, b, double, fabs, depsilon, "%g")

#define NUM_CMP_VALUES 6

/* Values picked to cover a range of small, large, positive and negative.  */
static unsigned int cmp_val[NUM_CMP_VALUES] = 
{
  0,
  1,
  0x40000000,
  0x80000000,
  0xc0000000,
  0xffffffff
};

/* All combinations for each of the above values. */
#define ulcmp(l, s, m) \
    s, l, l, l, l, l,  m, s, l, l, l, l, \
    m, m, s, l, l, l,  m, m, m, s, l, l, \
    m, m, m, m, s, l,  m, m, m, m, m, s

#define lcmp(l, s, m) \
    s, l, l, m, m, m,  m, s, l, m, m, m, \
    m, m, s, m, m, m,  l, l, l, s, l, l, \
    l, l, l, m, s, l,  l, l, l, m, m, s

/* All combinations of the above for high/low words.  */
static int lcmp_results[] =
{
  lcmp(ulcmp(-1, -1, -1), ulcmp(-1, 0, 1), ulcmp(1, 1, 1))
};

static int ulcmp_results[] =
{
  ulcmp(ulcmp(-1, -1, -1), ulcmp(-1, 0, 1), ulcmp(1, 1, 1))
};

static int signof(int i)
{
  if (i < 0)
    return -1;

  if (i == 0)
    return 0;

  return 1;
}

#endif
