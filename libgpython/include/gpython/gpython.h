/* This file is part of GCC.

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

#ifndef __GCC_GPYTHON__
#define __GCC_GPYTHON__

typedef struct gpy_rr_object_state_t {
  char * obj_t_ident;
  signed long ref_count;
  void * self;
} gpy_rr_object_state_t ;

typedef crl_symbol_obj * (*binary_op)( crl_symbol_obj * , crl_symbol_obj *,
				       crl_context_table * )

typedef struct crl_number_prot_t
{
  bool init;

  binary_op n_add;
  binary_op n_sub;
  binary_op n_div;
  binary_op n_mul;
  binary_op n_pow;

  binary_op n_let;
  binary_op n_lee;
  binary_op n_get;
  binary_op n_gee;
  binary_op n_eee;
  binary_op n_nee;
  binary_op n_orr;
  binary_op n_and;

} crl_num_prot_t ;

typedef struct crl_type_obj_def_t {
  char * identifier;
  size_t builtin_type_size;
  void * (*init_hook)( crl_symbol_obj * ,
		       struct crl_type_obj_def_t **  );
  void (*destroy_hook)( void * );
  bool (*print_hook)( void * , FILE * , bool );
  const struct crl_number_prot_t * binary_protocol;
  const struct crl_builtin_member_def_t * member_fields;
  const struct crl_builtin_function_def_t * member_functions;
  bool builtin;
} crl_type_obj_def_t ;

#endif //__GCC_GPYTHON__
