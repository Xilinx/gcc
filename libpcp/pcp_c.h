/* Copyright (C) 2009 Free Software Foundation, Inc.
   Contributed by Jan Sjodin <jan.sjodin@amd.com>.

   This file is part of the Polyhedral Compilation Package Library (libpcp).

   Libpcp is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libpcp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libpcp; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

#ifndef _PCP_C_INTERFACE_H_
#define _PCP_C_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* PCP Object */
#ifdef __cplusplus
  typedef PcpObject pcp_object;
  typedef PcpAnnot pcp_annot;
  typedef PcpAnnotTerm pcp_annot_term;
  typedef PcpAnnotInt pcp_annot_int;
  typedef PcpAnnotString pcp_annot_string;
  typedef PcpAnnotObject pcp_annot_object;
  typedef PcpAnnotTermBuilder pcp_annot_term_builder;
  typedef PcpArrayType pcp_array_type;
  typedef PcpArrayTypeBuilder pcp_array_type_builder;
  typedef PcpExpr pcp_expr;
  typedef PcpArith pcp_arith;
  typedef PcpArithBuilder pcp_arith_builder;
  typedef PcpConstant pcp_constant;
  typedef PcpIv pcp_iv;
  typedef PcpParameter pcp_parameter;
  typedef PcpBoolArith pcp_bool_arith;
  typedef PcpBoolArithBuilder pcp_bool_arith_builder;
  typedef PcpVariable pcp_variable;
  typedef PcpArrayAccess pcp_array_access;
  typedef PcpArrayAccessBuilder pcp_array_access_builder;
  typedef PcpStmt pcp_stmt;
  typedef PcpCopy pcp_copy;
  typedef PcpUserStmt pcp_user_stmt;
  typedef PcpUserStmtBuilder pcp_user_stmt_builder;
  typedef PcpSequence pcp_sequence;
  typedef PcpSequenceBuilder pcp_sequence_builder;
  typedef PcpGuard pcp_guard;
  typedef PcpLoop pcp_loop;
  typedef PcpScop pcp_scop;
  typedef PcpScopBuilder pcp_scop_builder;
  typedef PcpCompare pcp_compare;
  typedef PcpBoolExpr pcp_bool_expr;
  typedef PcpAnnotSet pcp_annot_set;
#else
  typedef int bool;
  typedef struct pcp_object pcp_object;
  typedef struct pcp_annot pcp_annot;
  typedef struct pcp_annot_term pcp_annot_term;
  typedef struct pcp_annot_int pcp_annot_int;
  typedef struct pcp_annot_string pcp_annot_string;
  typedef struct pcp_annot_object pcp_annot_object;
  typedef struct pcp_annot_term_builder pcp_annot_term_builder;
  typedef struct pcp_array_type pcp_array_type;
  typedef struct pcp_array_type_builder pcp_array_type_builder;
  typedef struct pcp_expr pcp_expr;
  typedef struct pcp_arith pcp_arith;
  typedef struct pcp_arith_builder pcp_arith_builder;
  typedef struct pcp_constant pcp_constant;
  typedef struct pcp_iv pcp_iv;
  typedef struct pcp_parameter pcp_parameter;
  typedef struct pcp_bool_arith pcp_bool_arith;
  typedef struct pcp_bool_arith_builder pcp_bool_arith_builder;
  typedef struct pcp_variable pcp_variable;
  typedef struct pcp_array_access pcp_array_access;
  typedef struct pcp_array_access_builder pcp_array_access_builder;
  typedef struct pcp_stmt pcp_stmt;
  typedef struct pcp_copy pcp_copy;
  typedef struct pcp_user_stmt pcp_user_stmt;
  typedef struct pcp_user_stmt_builder pcp_user_stmt_builder;
  typedef struct pcp_sequence pcp_sequence;
  typedef struct pcp_sequence_builder pcp_sequence_builder;
  typedef struct pcp_guard pcp_guard;
  typedef struct pcp_loop pcp_loop;
  typedef struct pcp_scop pcp_scop;
  typedef struct pcp_scop_builder pcp_scop_builder;
  typedef struct pcp_compare pcp_compare;
  typedef struct pcp_bool_expr pcp_bool_expr;
  typedef struct pcp_annot_set pcp_annot_set;
#endif

typedef enum pcp_object_kind
{
  pcp_object_kind_array_type,
  pcp_object_kind_expr,
  pcp_object_kind_bool_expr,
  pcp_object_kind_variable,
  pcp_object_kind_array_access,
  pcp_object_kind_stmt,
  pcp_object_kind_scop
} pcp_object_kind;

typedef enum pcp_annot_kind
{
  pcp_annot_kind_int,
  pcp_annot_kind_string,
  pcp_annot_kind_object,
  pcp_annot_kind_term
} pcp_annot_kind;

typedef enum pcp_expr_kind
{
  pcp_expr_kind_parameter,
  pcp_expr_kind_constant,
  pcp_expr_kind_iv,
  pcp_expr_kind_arith
} pcp_expr_kind;

typedef enum pcp_arith_operator
{
  pcp_arith_operator_unknown,
  pcp_arith_operator_add,
  pcp_arith_operator_multiply,
  pcp_arith_operator_min,
  pcp_arith_operator_max,
  pcp_arith_operator_subtract,
  pcp_arith_operator_floor,
  pcp_arith_operator_ceiling
} pcp_arith_operator;

typedef enum pcp_bool_expr_kind
{
  pcp_bool_expr_compare,
  pcp_bool_expr_arith
} pcp_bool_expr_kind;

typedef enum pcp_compare_operator
{
  pcp_compare_operator_unknown,
  pcp_compare_operator_equal,
  pcp_compare_operator_greater_equal
} pcp_compare_operator;

typedef enum pcp_bool_arith_operator
{
  pcp_bool_arith_operator_unknown,
  pcp_bool_arith_operator_and,
  pcp_bool_arith_operator_or
} pcp_bool_arith_operator;

typedef enum pcp_array_operator
{
  pcp_array_operator_unknown,
  pcp_array_operator_use,
  pcp_array_operator_def,
  pcp_array_operator_maydef
} pcp_array_operator;

typedef enum pcp_stmt_kind
{
  pcp_stmt_kind_unknown,
  pcp_stmt_kind_copy,
  pcp_stmt_kind_user,
  pcp_stmt_kind_loop,
  pcp_stmt_kind_guard,
  pcp_stmt_kind_sequence
} pcp_stmt_kind;

void pcp_object_set_name(pcp_object* object, const char* name);
const char* pcp_object_get_name(pcp_object* object);
pcp_annot_set* pcp_object_get_annots(pcp_object* object);
bool pcp_object_contains_annot_with_tag(pcp_object* object, const char* tag);
void pcp_object_add_annot(pcp_object* object, pcp_annot_term* annot);

bool pcp_object_is_array_type(pcp_object* object);
bool pcp_object_is_expr(pcp_object* object);
bool pcp_object_is_bool_expr(pcp_object* object);
bool pcp_object_is_variable(pcp_object* object);
bool pcp_object_is_array_access(pcp_object* object);
bool pcp_object_is_stmt(pcp_object* object);
bool pcp_object_is_scop(pcp_object* object);

bool pcp_object_is_iv(pcp_object* object);
bool pcp_object_is_parameter(pcp_object* object);

/* PCP Annot Set*/

int pcp_annot_set_get_num_annots(pcp_annot_set* annot_set);
pcp_annot_term* pcp_annot_set_get_annot(pcp_annot_set* annot_set, int index);

/* PCP Annot */

pcp_annot_kind pcp_annot_get_kind(pcp_annot* annot);

bool pcp_annot_is_annot_int(pcp_annot* annot);
bool pcp_annot_is_annot_string(pcp_annot* annot);
bool pcp_annot_is_annot_object(pcp_annot* annot);
bool pcp_annot_is_annot_term(pcp_annot* annot);

/* PCP Annot Int */

pcp_annot* pcp_annot_int_to_annot(pcp_annot_int* annot_int);
int pcp_annot_int_get_value(pcp_annot_int* annot_int);
pcp_annot_int* pcp_annot_int_create(int value);

/* PCP Annot String */

pcp_annot* pcp_annot_string_to_annot(pcp_annot_string* annot_string);
const char* pcp_annot_string_get_string(pcp_annot_string* annot_string);
pcp_annot_string* pcp_annot_string_create(const char* string);

/* PCP Annot Object */

pcp_annot* pcp_annot_object_to_annot(pcp_annot_object* annot_object);
pcp_object* pcp_annot_object_get_object(pcp_annot_object* annot_object);
pcp_annot_object* pcp_annot_object_create(pcp_object* object);

/* PCP Annot Term */

pcp_annot* pcp_annot_term_to_annot(pcp_annot_term* annot_term);
const char* pcp_annot_term_get_tag(pcp_annot_term* annot_term);
int pcp_annot_term_get_num_arguments(pcp_annot_term* annot_term);
pcp_annot* pcp_annot_term_get_argument(pcp_annot_term* annot_term,
					int index);

/* PCP Annot Term Builder */

void pcp_annot_term_builder_set_tag(pcp_annot_term_builder* builder,
				     const char* tag);
void pcp_annot_term_builder_add_argument(pcp_annot_term_builder* builder,
					  pcp_annot* argument);
pcp_annot_term_builder* pcp_annot_term_builder_create(void);
pcp_annot_term* pcp_annot_term_builder_create_annot(pcp_annot_term_builder* 
						     builder);

pcp_annot_int* pcp_annot_to_annot_int(pcp_annot* annot);
pcp_annot_string* pcp_annot_to_annot_string(pcp_annot* annot);
pcp_annot_object* pcp_annot_to_annot_object(pcp_annot* annot);
pcp_annot_term* pcp_annot_to_annot_term(pcp_annot* annot);

/* Array Type */

pcp_object* pcp_array_type_to_object(pcp_array_type* );
int pcp_array_type_get_num_dimensions(pcp_array_type* );
pcp_expr* pcp_array_type_get_dimension(pcp_array_type* , int);

/* Array Type Builder */

pcp_array_type_builder* pcp_array_type_builder_create(void);
void pcp_array_type_builder_add_dimension(pcp_array_type_builder* ,
					   pcp_expr* );
void pcp_array_type_builder_add_int_dimension(pcp_array_type_builder*,
					       int);
pcp_array_type* pcp_array_type_builder_create_type(pcp_array_type_builder* );

/* PCP Linear Expr */

pcp_object* pcp_expr_to_object(pcp_expr* expr);
pcp_expr_kind pcp_expr_get_kind(pcp_expr* expr);

bool pcp_expr_is_parameter(pcp_expr* );
bool pcp_expr_is_constant(pcp_expr* );
bool pcp_expr_is_iv(pcp_expr* );
bool pcp_expr_is_subtract(pcp_expr* );
bool pcp_expr_is_arith(pcp_expr* );


/* PCP Arith */

pcp_object* pcp_arith_to_object(pcp_arith* arith);
pcp_expr* pcp_arith_to_expr(pcp_arith* arith);
pcp_arith_operator pcp_arith_get_operator(pcp_arith* arith);
int pcp_arith_get_num_operands(pcp_arith* arith);
pcp_expr* pcp_arith_get_operand(pcp_arith* arith, int index);

pcp_arith* pcp_arith_binary_create(pcp_arith_operator oper,
					    pcp_expr* lhs,
					    pcp_expr* rhs);
/* PCP Arith builder.  */

void pcp_arith_builder_set_operator(pcp_arith_builder* builder,
					 pcp_arith_operator oper);
void pcp_arith_builder_add_operand(pcp_arith_builder* builder,
					pcp_expr* operand);
pcp_arith* 
pcp_arith_builder_create_arith(pcp_arith_builder* builder);
pcp_arith_builder* pcp_arith_builder_create(void);

/* PCP Constant */

pcp_object* pcp_constant_to_object(pcp_constant* constant);
pcp_expr* pcp_constant_to_expr(pcp_constant* constant);
int pcp_constant_get_value(pcp_constant* );
pcp_constant* pcp_constant_create(int);

/* PCP Induction Variable */

pcp_object* pcp_iv_to_object(pcp_iv* iv);
pcp_expr* pcp_iv_to_expr(pcp_iv* iv);
const char* pcp_iv_get_name(pcp_iv* iv);
pcp_iv* pcp_iv_create(const char* name);

/* PCP Parameter */

pcp_object* pcp_parameter_to_object(pcp_parameter* parameter);
pcp_expr* pcp_parameter_to_expr(pcp_parameter* parameter);
const char* pcp_parameter_get_name(pcp_parameter* );
pcp_parameter* pcp_parameter_create(const char* );

pcp_parameter* pcp_expr_to_parameter(pcp_expr* );
pcp_arith* pcp_expr_to_arith(pcp_expr* );
pcp_constant* pcp_expr_to_constant(pcp_expr* );
pcp_iv* pcp_expr_to_iv(pcp_expr* );

/* PCP Bool Expr */


pcp_object* pcp_bool_expr_to_object(pcp_bool_expr* bool_expr);
pcp_bool_expr_kind pcp_bool_expr_get_kind(pcp_bool_expr* bool_expr);
bool pcp_bool_expr_is_compare(pcp_bool_expr* bool_expr);
bool pcp_bool_expr_is_bool_arith(pcp_bool_expr* bool_expr);

/* PCP Compare */

pcp_bool_expr* pcp_compare_to_bool_expr(pcp_compare* compare);
pcp_compare_operator pcp_compare_get_operator(pcp_compare* compare);
pcp_expr* pcp_compare_get_lhs(pcp_compare* compare);
pcp_expr* pcp_compare_get_rhs(pcp_compare* compare);
pcp_compare* pcp_compare_create(pcp_compare_operator oper,
				 pcp_expr* lhs,
				 pcp_expr* rhs);


/* PCP Bool Arith */

pcp_object* pcp_bool_arith_to_object(pcp_bool_arith* bool_arith);
pcp_bool_expr* pcp_bool_arith_to_bool_expr(pcp_bool_arith* bool_arith);
pcp_bool_arith_operator 
pcp_bool_arith_get_operator(pcp_bool_arith* bool_arith);
int pcp_bool_arith_get_num_operands(pcp_bool_arith* bool_arith);
pcp_bool_expr* pcp_bool_arith_get_operand(pcp_bool_arith* bool_arith,
					   int index);
pcp_bool_arith* pcp_bool_arith_binary_create(pcp_bool_arith_operator oper,
					      pcp_bool_expr* lhs,
					      pcp_bool_expr* rhs);


void pcp_bool_arith_builder_set_operator(pcp_bool_arith_builder* builder,
					  pcp_bool_arith_operator oper);
pcp_bool_arith_builder* pcp_bool_arith_builder_create(void);
pcp_bool_arith* pcp_bool_arith_builder_create_bool_arith(pcp_bool_arith_builder* builder);
void pcp_bool_arith_builder_add_operand(pcp_bool_arith_builder* builder,
					 pcp_bool_expr* operand);

pcp_compare* pcp_bool_expr_to_compare(pcp_bool_expr* bool_expr);
pcp_bool_arith* pcp_bool_expr_to_bool_arith(pcp_bool_expr* bool_expr);

/* PCP Variable */

pcp_object* pcp_variable_to_object(pcp_variable* var);
void pcp_variable_set_is_input(pcp_variable* var, bool is_input);
bool pcp_variable_get_is_input(pcp_variable* var);
void pcp_variable_set_is_output(pcp_variable* var, bool is_output);
bool pcp_variable_get_is_output(pcp_variable* var);
pcp_array_type* pcp_variable_get_type(pcp_variable* );
const char* pcp_variable_get_name(pcp_variable* );
pcp_variable* pcp_variable_create(pcp_array_type* type, const char* name);

/* PCP Array Access */

pcp_object* pcp_array_access_to_object(pcp_array_access* access);
pcp_array_operator pcp_array_access_get_operator(pcp_array_access* access);
pcp_variable* pcp_array_access_get_base(pcp_array_access* access);
int pcp_array_access_get_num_subscripts(pcp_array_access* access);
pcp_expr* pcp_array_access_get_subscript(pcp_array_access* access,
					  int index);

bool pcp_array_access_is_use(pcp_array_access* );
bool pcp_array_access_is_def(pcp_array_access* );
bool pcp_array_access_is_maydef(pcp_array_access* );

/* PCP Array Access Builder */

void pcp_array_access_builder_set_operator(pcp_array_access_builder* 
					    builder,
					    pcp_array_operator oper);
pcp_array_access_builder* pcp_array_access_builder_create(pcp_variable* 
							   base);
pcp_array_access
 * pcp_array_access_builder_create_access(pcp_array_access_builder* 
					    builder);
void pcp_array_access_builder_add_subscript(pcp_array_access_builder* 
					     builder, pcp_expr* subscript);

/* PCP Stmt */

pcp_object* pcp_stmt_to_object(pcp_stmt* stmt);
pcp_stmt_kind pcp_stmt_get_kind(pcp_stmt* );

bool pcp_stmt_is_copy(pcp_stmt* );
bool pcp_stmt_is_user_stmt(pcp_stmt* );
bool pcp_stmt_is_guard(pcp_stmt* );
bool pcp_stmt_is_loop(pcp_stmt* );
bool pcp_stmt_is_sequence(pcp_stmt* );

/* PCP Copy Stmt */

pcp_object* pcp_copy_to_object(pcp_copy* copy);
pcp_stmt* pcp_copy_to_stmt(pcp_copy* copy);
pcp_array_access* pcp_copy_get_src(pcp_copy* copy);
pcp_array_access* pcp_copy_get_dest(pcp_copy* copy);
pcp_copy* pcp_copy_create(pcp_array_access* dest, pcp_array_access* src);

/* PCP User Stmt */

pcp_object* pcp_user_stmt_to_object(pcp_user_stmt* user_stmt);
pcp_stmt* pcp_user_stmt_to_stmt(pcp_user_stmt* user_stmt);
const char* pcp_user_stmt_get_name(pcp_user_stmt* user_stmt);
int pcp_user_stmt_get_num_accesses(pcp_user_stmt* user_stmt);
pcp_array_access* pcp_user_stmt_get_array_access(pcp_user_stmt* user_stmt,
						  int index);

/* PCP User Stmt Builder */

void pcp_user_stmt_builder_set_name(pcp_user_stmt_builder* builder,
				     const char* name);
void pcp_user_stmt_builder_add_access(pcp_user_stmt_builder* builder,
				       pcp_array_access* access);
pcp_user_stmt_builder* pcp_user_stmt_builder_create(void);
pcp_user_stmt* pcp_user_stmt_builder_create_user_stmt(pcp_user_stmt_builder* 
						       builder);

/* PCP Sequence */

pcp_stmt* pcp_sequence_to_stmt(pcp_sequence* sequence);
int pcp_sequence_get_num_stmts(pcp_sequence* sequence);
pcp_stmt* pcp_sequence_get_stmt(pcp_sequence* sequence, int index);

/* PCP Sequence Builder */

void pcp_sequence_builder_add(pcp_sequence_builder* builder,
			       pcp_stmt* stmt);
pcp_sequence_builder* pcp_sequence_builder_create(void);
pcp_sequence* pcp_sequence_builder_create_sequence(pcp_sequence_builder* 
						    builder);

/* PCP Guard */

pcp_object* pcp_guard_to_object(pcp_guard* guard);
pcp_stmt* pcp_guard_to_stmt(pcp_guard* guard);
pcp_bool_expr* pcp_guard_get_condition(pcp_guard* guard);
pcp_stmt* pcp_guard_get_body(pcp_guard* guard);
pcp_guard* pcp_guard_create(pcp_bool_expr* condition, pcp_stmt* body);


/* PCP Loop */

pcp_object* pcp_loop_to_object(pcp_loop* loop);
pcp_stmt* pcp_loop_to_stmt(pcp_loop* loop);
pcp_iv* pcp_loop_get_iv(pcp_loop* loop);
pcp_expr* pcp_loop_get_start(pcp_loop* loop);
pcp_bool_expr* pcp_loop_get_condition(pcp_loop* loop);
pcp_constant* pcp_loop_get_stride(pcp_loop* loop);
pcp_stmt* pcp_loop_get_body(pcp_loop* loop);
pcp_loop* pcp_loop_create(pcp_iv* iv, pcp_expr* start, pcp_bool_expr* condition,
			   pcp_constant* stride, pcp_stmt* body);

pcp_copy* pcp_stmt_to_copy(pcp_stmt* stmt);
pcp_user_stmt* pcp_stmt_to_user_stmt(pcp_stmt* stmt);
pcp_guard* pcp_stmt_to_guard(pcp_stmt* stmt);
pcp_loop* pcp_stmt_to_loop(pcp_stmt* stmt);
pcp_sequence* pcp_stmt_to_sequence(pcp_stmt* stmt);


/* PCP Scop */

pcp_object* pcp_scop_to_object(pcp_scop* scop);
int pcp_scop_get_num_variables(pcp_scop* scop);
int pcp_scop_get_num_parameters(pcp_scop* scop);
pcp_variable* pcp_scop_get_variable(pcp_scop* scop, int index);
pcp_parameter* pcp_scop_get_parameter(pcp_scop* scop, int index);
pcp_stmt* pcp_scop_get_body(pcp_scop* scop);


/* PCP Scop Builder */

void pcp_scop_builder_add_variable(pcp_scop_builder* builder,
				    pcp_variable* variable);
void pcp_scop_builder_add_parameter(pcp_scop_builder* builder,
				     pcp_parameter* parameter);
void pcp_scop_builder_set_body(pcp_scop_builder* builder, pcp_stmt* body);

pcp_scop_builder* pcp_scop_builder_create(void);
pcp_scop* pcp_scop_builder_create_scop(pcp_scop_builder* builder);

/* PCP Object casts */
pcp_annot* pcp_object_to_annot(pcp_object* object);
pcp_array_type* pcp_object_to_array_type(pcp_object* object);
pcp_expr* pcp_object_to_expr(pcp_object* object);
pcp_iv* pcp_object_to_iv(pcp_object* object);
pcp_bool_expr* pcp_object_to_bool_expr(pcp_object* object);
pcp_variable* pcp_object_to_variable(pcp_object* object);
pcp_array_access* pcp_object_to_array_access(pcp_object* object);
pcp_stmt* pcp_object_to_stmt(pcp_object* object);
pcp_scop* pcp_object_to_scop(pcp_object* object);
pcp_parameter* pcp_object_to_parameter(pcp_object* object);
#ifdef __cplusplus
}
#endif
#endif /* __PCP_C_INTERFACE_H_ */
