/* ISO_Fortran_binding.h of GCC's GNU Fortran compiler.
   Copyright (C) 2013 Free Software Foundation, Inc.

This file is part of the GNU Fortran runtime library (libgfortran)
Libgfortran is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libgfortran is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libquadmath; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */


/* Definitions as defined by ISO/IEC Technical Specification TS 29113:2012
   on Further Interoperability of Fortran with C.
   Note: The technical specification only mandates the presence of certain
   members; there might be additional compiler-specific fields.  */


#ifndef ISO_FORTRAN_BINDING_H
#define ISO_FORTRAN_BINDING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  /* For size_t and ptrdiff_t.  */


/* Constants, defined as macros.  */

#define CFI_VERSION 1
#define CFI_MAX_RANK 15

#define CFI_attribute_pointer 1
#define CFI_attribute_allocatable 2
#define CFI_attribute_other 3


/* FIXME: Those values have to match the compiler itself.
   "NOTE 8.5: The specifiers for two intrinsic types can have the same value.
    For example, CFI_type_int and CFI_type_int32_t might have the same value."

   "The value for CFI_type_other shall be negative and distinct from all other
    type specifiers. CFI_type_struct specifies a C structure that is
    interoperable with a Fortran derived type; its value shall be positive and
    distinct from all other type specifiers. If a C type is not interoperable
    with a Fortran type and kind supported by the Fortran processor, its macro
    shall evaluate to a negative value.  Otherwise, the value for an intrinsic
    type shall be positive."  */

#define CFI_type_signed_char 1
#define CFI_type_short 2
#define CFI_type_int 3
#define CFI_type_long 4
#define CFI_type_long_long 5
#define CFI_type_size_t 6
#define CFI_type_int8_t 7
#define CFI_type_int16_t 8
#define CFI_type_int32_t 9
#define CFI_type_int64_t 10
#define CFI_type_int_least8_t 11
#define CFI_type_int_least16_t 12
#define CFI_type_int_least32_t 13 
#define CFI_type_int_least64_t 14
#define CFI_type_int_fast8_t 15
#define CFI_type_int_fast16_t 16
#define CFI_type_int_fast32_t 17
#define CFI_type_int_fast64_t 18
#define CFI_type_intmax_t 19
#define CFI_type_intptr_t 20
#define CFI_type_ptrdiff_t 21
#define CFI_type_float 22
#define CFI_type_double 23
#define CFI_type_long_double 24
#define CFI_type_float_Complex 25
#define CFI_type_double_Complex 26
#define CFI_type_long_double_Complex 27
#define CFI_type_Bool 28
#define CFI_type_char 29
#define CFI_type_cptr 30
#define CFI_type_cfunptr 31
#define CFI_type_struct 32
#define CFI_type_other -5

#define CFI_SUCCESS 0
#define CFI_ERROR_BASE_ADDR_NULL 1
#define CFI_ERROR_BASE_ADDR_NOT_NULL 2
#define CFI_INVALID_ELEM_LEN 3
#define CFI_INVALID_RANK 4
#define CFI_INVALID_TYPE 5
#define CFI_INVALID_ATTRIBUTE 6
#define CFI_INVALID_EXTENT 7
#define CFI_INVALID_DESCRIPTOR 8
#define CFI_ERROR_MEM_ALLOCATION 9
#define CFI_ERROR_OUT_OF_BOUNDS 10


/* Types definitions.  */

typedef ptrdiff_t CFI_index_t;
typedef int32_t CFI_attribute_t;
typedef int32_t CFI_type_t;
typedef int32_t CFI_rank_t;

typedef struct CFI_dim_t
{
  CFI_index_t lower_bound;
  CFI_index_t extent;
  CFI_index_t sm;
}
CFI_dim_t;

typedef struct CFI_cdesc_t
{
  void *base_addr;
  size_t elem_len;
  int version;
  CFI_rank_t rank;
  CFI_type_t type;
  CFI_attribute_t attribute;
  /*FIXME: corank? Other information? Padding? Or not needed
    due to "version"?  */
  CFI_dim_t dim[]; /* Must be last field */
}
CFI_cdesc_t;


/* gfortran extension: Type-specific array descriptor.
   FIXME: Shall be the same as CFI_cdesc_t at the end.  */

#define CFI_GFC_CDESC_T(r, type) \
struct {\
  type *base_addr;\
  size_t elem_len;\
  int version; \
  size_t offset;\
  CFI_index_t dtype;\
  CFI_dim_t dim[r];\
}

#define CFI_CDESC_T(r) CFI_GFC_CDESC_T (r, void)


/* Functions. */

void *CFI_address (const CFI_cdesc_t *dv, const CFI_index_t subscripts[]);
int CFI_allocate (CFI_cdesc_t *dv, const CFI_index_t lower_bounds[],
		  const CFI_index_t upper_bounds[], size_t elem_len);
int CFI_deallocate (CFI_cdesc_t *dv);
int CFI_establish (CFI_cdesc_t *dv, void *base_addr, CFI_attribute_t attribute,
		   CFI_type_t type, size_t elem_len, CFI_rank_t rank,
		   const CFI_index_t extents[]);
int CFI_is_contiguous (const CFI_cdesc_t *dv);
int CFI_section (CFI_cdesc_t *result, const CFI_cdesc_t *source,
		 const CFI_index_t lower_bounds[],
		 const CFI_index_t upper_bounds[],
		 const CFI_index_t strides[]);
int CFI_select_part (CFI_cdesc_t *result, const CFI_cdesc_t *source,
		     size_t displacement, size_t elem_len);
int CFI_setpointer (CFI_cdesc_t *result, CFI_cdesc_t *source,
		    const CFI_index_t lower_bounds[]);

#ifdef __cplusplus
}
#endif

#endif  /* ISO_FORTRAN_BINDING_H */
