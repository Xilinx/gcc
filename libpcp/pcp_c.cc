// Copyright (C) 2009 Free Software Foundation, Inc.
// Contributed by Jan Sjodin <jan.sjodin@amd.com>.

// This file is part of the Polyhedral Compilation Package Library (libpcp).

// Libpcp is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.

// Libpcp is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
// more details.

// You should have received a copy of the GNU Lesser General Public License 
// along with libpcp; see the file COPYING.LIB.  If not, write to the
// Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.  

// As a special exception, if you link this library with other files, some
// of which are compiled with GCC, to produce an executable, this library
// does not by itself cause the resulting executable to be covered by the
// GNU General Public License.  This exception does not however invalidate
// any other reasons why the executable file might be covered by the GNU
// General Public License.  

#include "pcp.h"
#include "pcp_c.h"

// PCP Object 

// Set name of OBJECT to NAME.  
void
pcp_object_set_name(pcp_object* object, const char* name)
{
  object->setName(name);
}

// Get name of OBJECT.  
const char* 
pcp_object_get_name(pcp_object* object)
{
  return object->getName();
}

// Get number of annotations in OBJECT.  
int pcp_object_get_num_annots(pcp_object* object)
{
  return object->getAnnots()->getNumAnnots();
}


// Get annotation in OBJECT with given INDEX.  
pcp_annot_term* pcp_object_get_annot(pcp_object* object, int index)
{
  return object->getAnnots()->getAnnot(index);
}


// Get annotation in OBJECT with given TAG.  
pcp_annot_term* pcp_object_get_annot_with_tag(pcp_object* object, 
					      const char* tag)
{
  return object->getAnnotWithTag(tag);
}


// Returns true if an annotation with given TAG exists in OBJECT.  
bool
pcp_object_contains_annot_with_tag(pcp_object* object, const char* tag)
{
  return object->containsAnnotWithTag(tag);
}

// Add ANNOT to OBJECT.  
void pcp_object_add_annot(pcp_object* object, pcp_annot_term* annot)
{
  object->addAnnot(annot);
}


// Get annotation of OBJECT.  
pcp_annot_set* 
pcp_object_get_annots(pcp_object* object)
{
  return object->getAnnots();
}


// Return true if OBJECT is an array type.  
bool
pcp_object_is_array_type(pcp_object* object)
{
  return object->isArrayType();
}

// Return true if OBJECT is an expr.  
bool
pcp_object_is_expr(pcp_object* object)
{
  return object->isExpr();
}

// Return true if OBJECT is an iv.  
bool
pcp_object_is_iv(pcp_object* object)
{
  return object->isIv();
}

// Return true if OBJECT is a parameter.  
bool
pcp_object_is_parameter(pcp_object* object)
{
  
  return object->isParameter();
}

// Return true if OBJECT is a bool_expr.  
bool
pcp_object_is_bool_expr(pcp_object* object)
{
  return object->isBoolExpr();
}

// Return true if OBJECT is a variable.  
bool
pcp_object_is_variable(pcp_object* object)
{
  return object->isVariable();
}

// Return true if OBJECT is an array access.  
bool
pcp_object_is_array_access(pcp_object* object)
{
  return object->isArrayAccess();
}

// Return true if OBJECT is a stmt.  
bool
pcp_object_is_stmt(pcp_object* object)
{
  return object->isStmt();
}

// Return true if OBJECT is a scop.  
bool
pcp_object_is_scop(pcp_object* object)
{
  return object->isScop();
}

// Cast OBJECT to array type.  
pcp_array_type* 
pcp_object_to_array_type(pcp_object* object)
{
  return object->toArrayType();
}

// Cast OBJECT to expr.  
pcp_expr* 
pcp_object_to_expr(pcp_object* object)
{
  return object->toExpr();
}

// Cast OBJECT to parameter.  
pcp_parameter* 
pcp_object_to_parameter(pcp_object* object)
{
  return object->toParameter();
}

// Cast OBJECT to IV.  
pcp_iv* 
pcp_object_to_iv(pcp_object* object)
{
  return object->toIv();
}

// Cast OBJECT to bool expr.  
pcp_bool_expr* 
pcp_object_to_bool_expr(pcp_object* object)
{
  return object->toBoolExpr();
}

// Cast OBJECT to variable.  
pcp_variable* 
pcp_object_to_variable(pcp_object* object)
{
  return object->toVariable();
}

// Cast OBJECT to array access.  
pcp_array_access* 
pcp_object_to_array_access(pcp_object* object)
{
  return object->toArrayAccess();
}

// Cast OBJECT to stmt.  
pcp_stmt* 
pcp_object_to_stmt(pcp_object* object)
{
  return object->toStmt();
}

// Cast OBJECT to scop.  
pcp_scop* 
pcp_object_to_scop(pcp_object* object)
{
  return object->toScop();
}

// PCP Annotation Set 

// Get the annotation in ANNOT_SET with given INDEX.  
pcp_annot_term* pcp_annot_set_get_annot(pcp_annot_set* annot_set, int index)
{
  return annot_set->getAnnot(index);
}

// Get number of annotations in ANNOT_SET.  
int pcp_annot_set_get_num_annots(pcp_annot_set* annot_set)
{
  return annot_set->getNumAnnots();
}

// Get the annotation with TAG in ANNOT_SET.  
pcp_annot_term* pcp_annot_set_get_annot_with_tag(pcp_annot_set* annot_set,
						 const char* tag)
{
  return annot_set->getAnnotWithTag(tag);
}

// Add ANNOT to ANNOT_SET. Assert that no previous annotation with the
// same tag exists.  
void pcp_annot_set_add_annot(pcp_annot_set* annot_set, pcp_annot_term* annot)
{
  annot_set->addAnnot(annot);
}

// PCP Annotation 

// Get kind of ANNOT.  
pcp_annot_kind pcp_annot_get_kind(pcp_annot* annot)
{
  return annot->isAnnotInt() ? pcp_annot_kind_int :
    annot->isAnnotString()   ? pcp_annot_kind_string :
    annot->isAnnotObject()   ? pcp_annot_kind_object 
    : pcp_annot_kind_term;
}

// Return true if ANNOT is an int.  
bool
pcp_annot_is_annot_int(pcp_annot* annot)
{
  return annot->isAnnotInt();
}

// Return true if ANNOT is a string.  
bool
pcp_annot_is_annot_string(pcp_annot* annot)
{
  return annot->isAnnotString();
}

// Return true if ANNOT is an object.  
bool
pcp_annot_is_annot_object(pcp_annot* annot)
{
  return annot->isAnnotObject();
}

// Return true if ANNOT is a term.  
bool
pcp_annot_is_annot_term(pcp_annot* annot)
{
  return annot->isAnnotTerm();
}

// Cast ANNOT to int.  
pcp_annot_int* 
pcp_annot_to_annot_int(pcp_annot* annot)
{
  return annot->toAnnotInt();
}

// Cast ANNOT to string.  
pcp_annot_string* 
pcp_annot_to_annot_string(pcp_annot* annot)
{
  return annot->toAnnotString();
}

// Cast ANNOT to object.  
pcp_annot_object* 
pcp_annot_to_annot_object(pcp_annot* annot)
{
  return annot->toAnnotObject();
}

// Cast ANNOT to term.  
pcp_annot_term* 
pcp_annot_to_annot_term(pcp_annot* annot)
{
  return annot->toAnnotTerm();
}

// PCP Annot Int 

// Cast ANNOT_INT to annot.  
pcp_annot* 
pcp_annot_int_to_annot(pcp_annot_int* annot_int)
{
  return annot_int;
}

// Get value of ANNOT_INT.  
int
pcp_annot_int_get_value(pcp_annot_int* annot_int)
{
  return annot_int->getValue();
}

// Create annot int with given VALUE.  
pcp_annot_int* 
pcp_annot_int_create(int value)
{
  return new PcpAnnotInt(value);
}

// PCP Annot String 

// Cast ANNOT_STRING to annot.  
pcp_annot* 
pcp_annot_string_to_annot(pcp_annot_string* annot_string)
{
  return annot_string;
}

// Get string of ANNOT_STRING.  
const char* 
pcp_annot_string_get_string(pcp_annot_string* annot_string)
{
  return annot_string->getString();
}

// Create annot string with given STRING.  
pcp_annot_string* 
pcp_annot_string_create(const char* string)
{
  return new PcpAnnotString(string);
}

// PCP Annot Object 

// Cast ANNOT_OBJECT to annot.  
pcp_annot* 
pcp_annot_object_to_annot(pcp_annot_object* annot_object)
{
  return annot_object;
}

// Get object of ANNOT_OBJECT.  
pcp_object* 
pcp_annot_object_get_object(pcp_annot_object* annot_object)
{
  return annot_object->getObject();
}

// Create annot object with given OBJECT.  
pcp_annot_object* 
pcp_annot_object_create(pcp_object* object)
{
  return new PcpAnnotObject(object);
}

// PCP Annot Term 

// Cast ANNOT_TERM to annot.  
pcp_annot* 
pcp_annot_term_to_annot(pcp_annot_term* annot_term)
{
  return annot_term;
}

// Get tag of ANNOT_TERM.  
const char* 
pcp_annot_term_get_tag(pcp_annot_term* annot_term)
{
  return annot_term->getTag();
}

// Return true if ANNOT_TERM tag equals TAG.  
bool pcp_annot_term_tag_equals(pcp_annot_term* annot_term, const char* tag)
{
  return annot_term->tagEquals(tag);
}

// Get number of arguments of ANNOT_TERM.  
int
pcp_annot_term_get_num_arguments(pcp_annot_term* annot_term)
{
  return annot_term->getNumArguments();
}

// Get ANNOT_TERM argument with given INDEX.  
pcp_annot* 
pcp_annot_term_get_argument(pcp_annot_term* annot_term, int index)
{
  return annot_term->getArgument(index);
}

// PCP Annot Term Builder 

// Set tag of BUILDER to TAG.  
void
pcp_annot_term_builder_set_tag(pcp_annot_term_builder* builder,
			       const char* tag)
{
  builder->setTag(tag);
}

// Add ARGUMENT to BUILDER. 
void
pcp_annot_term_builder_add_argument(pcp_annot_term_builder* builder,
				    pcp_annot* argument)
{
  builder->addArgument(argument);
}

// Create new annot term builder.  
pcp_annot_term_builder* 
pcp_annot_term_builder_create()
{
  return new PcpAnnotTermBuilder();
}

// Create annot term from BUILDER.  
pcp_annot_term* 
pcp_annot_term_builder_create_annot(pcp_annot_term_builder* builder)
{
  return builder->createAnnot();
}

// Array Type 

// Cast ARRAY_TYPE to object.  
pcp_object* 
pcp_array_type_to_object(pcp_array_type* array_type)
{
  return array_type;
}

// Return the number of dimensions of ARRAY_TYPE.  
int
pcp_array_type_get_num_dimensions(pcp_array_type* array_type)
{
  return array_type->getNumDimensions();
}

// Return the size of dimension of INDEX in ARRAY_TYPE.  
pcp_expr*
pcp_array_type_get_dimension(pcp_array_type* array_type, int index)
{
  return array_type->getDimension(index);
}

// Array Type Builder 

// Create array type builder.  
pcp_array_type_builder* 
pcp_array_type_builder_create()
{
  return new PcpArrayTypeBuilder();
}

// Add a new dimension with the given SIZE to BUILDER.  
void
pcp_array_type_builder_add_dimension(pcp_array_type_builder* builder,
				     pcp_expr* size)
{
  builder->addDimension(size);
}

void pcp_array_type_builder_add_int_dimension(pcp_array_type_builder* builder,
					      int size)
{
  builder->addIntDimension(size);
}

// Create new array type from BUIDER.  
pcp_array_type* 
pcp_array_type_builder_create_type(pcp_array_type_builder* builder)
{
  return builder->createType();
}

// PCP Linear Expr 

// Cast EXPR to object.  
pcp_object* 
pcp_expr_to_object(pcp_expr* expr)
{
  return expr;
}

// Get expression kind.  
pcp_expr_kind
pcp_expr_get_kind(pcp_expr* expr)
{
  return expr->isParameter() ? pcp_expr_kind_parameter :
    expr->isConstant() ? pcp_expr_kind_constant :
    expr->isIv() ? pcp_expr_kind_iv :
    pcp_expr_kind_arith;
}

// Return true if EXPR is a pcp_parameter, otherwise return false.  
bool
pcp_expr_is_parameter(pcp_expr* expr)
{
  return expr->isParameter();
}

// Return true if EXPR is a pcp_arith, otherwise return false.  
bool
pcp_expr_is_arith(pcp_expr* expr)
{
  return expr->isArith();
}

// Return true if EXPR is a pcp_constant, otherwise return false.  
bool
pcp_expr_is_constant(pcp_expr* expr)
{
  return expr->isConstant();
}

// Return true if EXPR is a pcp_iv, otherwise return false.  
bool
pcp_expr_is_iv(pcp_expr* expr)
{
  return expr->isIv();
}

// Cast EXPR to pcp_parameter.  
pcp_parameter* 
pcp_expr_to_parameter(pcp_expr* expr)
{
  return expr->toParameter();
}

// Cast EXPR to pcp_arith.  
pcp_arith* 
pcp_expr_to_arith(pcp_expr* expr)
{
  return expr->toArith();
}

// Cast EXPR to pcp_constant.  
pcp_constant* 
pcp_expr_to_constant(pcp_expr* expr)
{
  return expr->toConstant();
}

// Cast EXPR to pcp_iv.  
pcp_iv* 
pcp_expr_to_iv(pcp_expr* expr)
{
  return expr->toIv();
}

// PCP Bool Expr 

// Cast BOOL_EXPR to object.  
pcp_object* 
pcp_bool_expr_to_object(pcp_bool_expr* bool_expr)
{
  return bool_expr;
}

// Get kind of BOOL_EXPR.  
pcp_bool_expr_kind
pcp_bool_expr_get_kind(pcp_bool_expr* bool_expr)
{
  return bool_expr->isCompare() ? pcp_bool_expr_compare
    : pcp_bool_expr_arith;
}

// Return true if BOOL_EXPR is a compare.
bool
pcp_bool_expr_is_compare(pcp_bool_expr* bool_expr)
{
  return bool_expr->isCompare();
}

// Return true if BOOL_EXPR is a boolean arithmetic operation.  
bool
pcp_bool_expr_is_bool_arith(pcp_bool_expr* bool_expr)
{
  return bool_expr->isBoolArith();
}

// Cast BOOL_EXPR to pcp_compare.
pcp_compare* pcp_bool_expr_to_compare(pcp_bool_expr* bool_expr)
{
  return bool_expr->toCompare();
}

// Cast BOOL_EXPR to bool arith, if it is not a boolean arithmetic operator return NULL.  
pcp_bool_arith*
pcp_bool_expr_to_bool_arith(pcp_bool_expr* bool_expr)
{
  return bool_expr->toBoolArith();
}

// PCP Compare 
pcp_bool_expr*
pcp_compare_to_bool_expr(pcp_compare* compare)
{
  return compare;
}

// Convert OPER to a PcpCompareOperator.
static PcpCompareOperator
pcp_compare_operator_to_class(pcp_compare_operator oper)
{
  switch(oper)
    {
    case pcp_compare_operator_equal:
      return PcpCompareOperator::equal();
    case pcp_compare_operator_greater_equal:
      return PcpCompareOperator::greaterEqual();
    default:
      return PcpCompareOperator::unknown();
    }
}

// Convert oper to a pcp_compare_operator.
static pcp_compare_operator
pcp_compare_operator_from_class(PcpCompareOperator oper)
{
  if(oper.isEqual())
    return pcp_compare_operator_equal;
  else if(oper.isGreaterEqual())
    return pcp_compare_operator_greater_equal;
  else
    return pcp_compare_operator_unknown;
}

// Get pcp_compare_operator of COMPARE.
pcp_compare_operator
pcp_compare_get_operator(pcp_compare* compare)
{
  return pcp_compare_operator_from_class(compare->getOperator());
}

// Get lhs of COMPARE.
pcp_expr* 
pcp_compare_get_lhs(pcp_compare* compare)
{
  return compare->getLhs();
}

// Get rhs of COMPARE.
pcp_expr* 
pcp_compare_get_rhs(pcp_compare* compare)
{
  return compare->getRhs();
}

// Create compare given OPER, LHS and RHS.
pcp_compare* pcp_compare_create(pcp_compare_operator oper,
				pcp_expr* lhs,
				pcp_expr* rhs)
{
  PcpCompareOperator operClass = pcp_compare_operator_to_class(oper);
  return new PcpCompare(operClass, lhs, rhs);
}

// PCP Bool Arith 
static PcpBoolArithOperator
pcp_bool_arith_operator_to_class(pcp_bool_arith_operator oper)
{
  switch(oper)
    {
    case pcp_bool_arith_operator_and:
      return PcpBoolArithOperator::boolAnd();
    case pcp_bool_arith_operator_or:
      return PcpBoolArithOperator::boolOr();
    default:
      return PcpBoolArithOperator::unknown();
    }
}

// Convert oper to pcp_bool_arith_operator type.
static pcp_bool_arith_operator
pcp_bool_arith_operator_from_class(PcpBoolArithOperator oper)
{
  if(oper.isBoolAnd())
    return pcp_bool_arith_operator_and;
  if(oper.isBoolOr())
    return pcp_bool_arith_operator_or;
  else
    return pcp_bool_arith_operator_unknown;
}

// Get operator of BOOL_ARITH.  
pcp_bool_arith_operator
pcp_bool_arith_get_operator(pcp_bool_arith* bool_arith)
{
  return pcp_bool_arith_operator_from_class(bool_arith->getOperator());
}

// Get number of operands of BOOL_ARITH.  
int
pcp_bool_arith_get_num_operands(pcp_bool_arith* bool_arith)
{
  return bool_arith->getNumOperands();
}

// Get operand of BOOL_ARITH with given INDEX.  
pcp_bool_expr*
pcp_bool_arith_get_operand(pcp_bool_arith* bool_arith, int index)
{
  return bool_arith->getOperand(index);
}

// Cast BOOL_ARITH to object.  
pcp_object*
pcp_bool_arith_to_object(pcp_bool_arith* bool_arith)
{
  return bool_arith;
}

// Cast BOOL_ARITH to bool_expr.  
pcp_bool_expr*
pcp_bool_arith_to_bool_expr(pcp_bool_arith* bool_arith)
{
  return bool_arith;
}

// PCP Bool Arith Builder.  
// Set operator in BUILDER to OPERATOR.  
void
pcp_bool_arith_builder_set_operator(pcp_bool_arith_builder* builder,
				    pcp_bool_arith_operator oper)
{
  builder->setOperator(pcp_bool_arith_operator_to_class(oper));
}

// Add OPERAND to BUILDER.  
void
pcp_bool_arith_builder_add_operand(pcp_bool_arith_builder* builder,
				   pcp_bool_expr* operand)
{
  builder->addOperand(operand);
}

// Create new boolean arithmetic operation builder.  
pcp_bool_arith_builder*
pcp_bool_arith_builder_create()
{
  return new PcpBoolArithBuilder();
}

// Create new boolean arithmetic operation from BUILDER.  
pcp_bool_arith*
pcp_bool_arith_builder_create_bool_arith(pcp_bool_arith_builder* builder)
{
  return builder->createBoolArith();
}

// Create binary arithmetic operation with OPERATOR, LHS, RHS.  
pcp_bool_arith*
pcp_bool_arith_binary_create(pcp_bool_arith_operator oper,
			     pcp_bool_expr* lhs,
			     pcp_bool_expr* rhs)
{
  
  return PcpBoolArith::pcpBoolArithBinaryCreate(pcp_bool_arith_operator_to_class(oper), lhs, rhs);
}

// PCP Arith 

// Convert oper enum to PcpArithOperator class
static PcpArithOperator
pcp_arith_operator_to_class(pcp_arith_operator oper)
{
  switch(oper)
    {
    case pcp_arith_operator_add:
      return PcpArithOperator::add();
    case pcp_arith_operator_multiply:
      return PcpArithOperator::multiply();
    case pcp_arith_operator_subtract:
      return PcpArithOperator::subtract();
    case pcp_arith_operator_min:
      return PcpArithOperator::min();
    case pcp_arith_operator_max:
      return PcpArithOperator::max();
    case pcp_arith_operator_floor:
      return PcpArithOperator::floor();
    case pcp_arith_operator_ceiling:
      return PcpArithOperator::ceiling();
    default:
      return PcpArithOperator::unknown();
    }
}

// Convert oper class to pcp_arith_operator enum.
static pcp_arith_operator
pcp_arith_operator_from_class(PcpArithOperator oper)
{
  return oper.isAdd() ? pcp_arith_operator_add :
    oper.isMultiply() ? pcp_arith_operator_multiply :
    oper.isSubtract() ? pcp_arith_operator_subtract :
    oper.isMin() ? pcp_arith_operator_min :
    oper.isMax() ? pcp_arith_operator_max :
    oper.isFloor() ? pcp_arith_operator_floor :
    oper.isCeiling() ? pcp_arith_operator_ceiling :
    pcp_arith_operator_unknown;
}

// Cast ARITH to object.
pcp_object* pcp_arith_to_object(pcp_arith* arith)
{
  return arith;
}

// Cast ARITH to expr.  
pcp_expr* pcp_arith_to_expr(pcp_arith* arith)
{
  return arith;
}

// Get operator of ARITH.  
pcp_arith_operator pcp_arith_get_operator(pcp_arith* arith)
{
  return pcp_arith_operator_from_class(arith->getOperator());
}

// Get number of operands of ARITH.  
int pcp_arith_get_num_operands(pcp_arith* arith)
{
  return arith->getNumOperands();
}

// Get operand of ARITH with given INDEX.  
pcp_expr* 
pcp_arith_get_operand(pcp_arith* arith, int index)
{
  return arith->getOperand(index);
}

// PCP Arith Builder.  

// Set operator in BUILDER to OPERATOR.  
void
pcp_arith_builder_set_operator(pcp_arith_builder* builder,
			       pcp_arith_operator oper)
{
  builder->setOperator(pcp_arith_operator_to_class(oper));
}

// Add OPERAND to BUILDER.  
void
pcp_arith_builder_add_operand(pcp_arith_builder* builder,
			      pcp_expr* operand)
{
  builder->addOperand(operand);
}

// Create new arithmetic operation from BUILDER.  
pcp_arith* 
pcp_arith_builder_create_arith(pcp_arith_builder* builder)
{
  return builder->createArith();
}

// Create new arithmetic operation builder.  
pcp_arith_builder*
pcp_arith_builder_create()
{
  return new PcpArithBuilder();
}

// PCP Constant 

// Cast CONSTANT to object.  
pcp_object* 
pcp_constant_to_object(pcp_constant* constant)
{
  return constant;
}

// Cast CONSTANT to pcp_expr.  
pcp_expr* 
pcp_constant_to_expr(pcp_constant* constant)
{
  return constant;
}

// Get value of CONSTANT.  
int
pcp_constant_get_value(pcp_constant* constant)
{
  return constant->getValue();
}

// Create constant with VALUE.  
pcp_constant* 
pcp_constant_create(int value)
{
  return new PcpConstant(value);
}

// PCP Induction Variable 

// Cast IV to object.  
pcp_object* 
pcp_iv_to_object(pcp_iv* iv)
{
  return iv;
}

// Cast IV to pcp_expr.  
pcp_expr* 
pcp_iv_to_expr(pcp_iv* iv)
{
  return iv;
}

// Get name of IV.  
const char* 
pcp_iv_get_name(pcp_iv* iv)
{
  return iv->getName();
}

// Create iv with NAME.  
pcp_iv* 
pcp_iv_create(const char* name)
{
  return new PcpIv(name);
}

// PCP Variable 

// Cast VAR to object.  
pcp_object* 
pcp_variable_to_object(pcp_variable* var)
{
  return var;
}

// Set is input of VAR to IS_INPUT.  
void
pcp_variable_set_is_input(pcp_variable* var, bool is_input)
{
  var->setIsInput(is_input);
}

// Get is input of VAR.  
bool
pcp_variable_get_is_input(pcp_variable* var)
{
  return var->getIsInput();
}

// Set is output of VAR to IS_OUTPUT.  
void
pcp_variable_set_is_output(pcp_variable* var, bool is_output)
{
  var->setIsOutput(is_output);
}

// Get is output of VAR.  
bool
pcp_variable_get_is_output(pcp_variable* var)
{
  return var->getIsOutput();
}

// Get tpe of VAR.  
pcp_array_type* 
pcp_variable_get_type(pcp_variable* var)
{
  return var->getType();
}

// Get name of VAR.  
const char* 
pcp_variable_get_name(pcp_variable* var)
{
  return var->getName();
}

// Create variable given TYPE and NAME.  
pcp_variable* 
pcp_variable_create(pcp_array_type* type, const char* name)
{
  return new PcpVariable(type, name);
}

// PCP Parameter 

// Cast PARAMETER to pcp_object.  
pcp_object* 
pcp_parameter_to_object(pcp_parameter* parameter)
{
  return parameter;
}

// Cast PARAMETER to pcp_expr.  
pcp_expr* 
pcp_parameter_to_expr(pcp_parameter* parameter)
{
  return parameter;
}

// Get name of PARAMETER. 
const char* 
pcp_parameter_get_name(pcp_parameter* parameter)
{
  return parameter->getName();
}

// Create parameter with the given NAME.  
pcp_parameter* 
pcp_parameter_create(const char* name)
{
  return new PcpParameter(name);
}

// Create binary arithmetic operation with OPERATOR, LHS, RHS.  
pcp_arith* 
pcp_arith_binary_create(pcp_arith_operator oper,
			pcp_expr* lhs,
			pcp_expr* rhs)
{
  return PcpArith::pcpArithBinaryCreate(pcp_arith_operator_to_class(oper),
					lhs, rhs);
}

// PCP Array Access 

// Convert oper enum to PcpArrayOperator object
static PcpArrayOperator
pcp_array_operator_to_class(pcp_array_operator oper)
{
  switch(oper)
    {
    case pcp_array_operator_use:
      return PcpArrayOperator::use();
    case pcp_array_operator_def:
      return PcpArrayOperator::def();
    case pcp_array_operator_maydef:
      return PcpArrayOperator::maydef();
    default:
      return PcpArrayOperator::unknown();
    }
}

// Convert oper object to pcp_array_operator enum.
static pcp_array_operator
pcp_array_operator_from_class(PcpArrayOperator oper)
{
  return oper.isUse() ? pcp_array_operator_use :
    oper.isDef() ? pcp_array_operator_def :
    oper.isMaydef() ? pcp_array_operator_maydef :
    pcp_array_operator_unknown;
}

// Cast ACCESS to object.  
pcp_object* 
pcp_array_access_to_object(pcp_array_access* access)
{
  return access;
}

// Get operator enum from ACCESS.
pcp_array_operator
pcp_array_access_get_operator(pcp_array_access* access)
{
  return pcp_array_operator_from_class(access->getOperator());
}

// Get base of ACCESS.  
pcp_variable* 
pcp_array_access_get_base(pcp_array_access* access)
{
  return access->getBase();
}

// Get ACCESS subscript at INDEX.  
pcp_expr* 
pcp_array_access_get_subscript(pcp_array_access* access, int index)
{
  return access->getSubscript(index);
}

// Get number of subscripts in ACCESS.  
int
pcp_array_access_get_num_subscripts(pcp_array_access* access)
{
  return access->getNumSubscripts();
}

// Return true if ACCESS is use, otherwise return false.  
bool
pcp_array_access_is_use(pcp_array_access* access)
{
  return access->getOperator().isUse();
}

// Return true if ACCESS is def, otherwise return false.  
bool
pcp_array_access_is_def(pcp_array_access* access)
{
  return access->getOperator().isDef();
}

// Return true if ACCESS is maydef, otherwise return false.  
bool
pcp_array_access_is_maydef(pcp_array_access* access)
{
  return access->getOperator().isMaydef();
}

// PCP Array Access Builder 

// Set operator of BUIDLER to OPERATOR.  
void
pcp_array_access_builder_set_operator(pcp_array_access_builder* builder,
				      pcp_array_operator oper)
{
  builder->setOperator(pcp_array_operator_to_class(oper));
}

// Add SUBSCRIPT to BUILDER.  
void
pcp_array_access_builder_add_subscript(pcp_array_access_builder* builder,
				       pcp_expr* subscript)
{
  builder->addSubscript(subscript);
}

// Create new access builder with given BASE.  
pcp_array_access_builder* 
pcp_array_access_builder_create(pcp_variable* base)
{
  return new PcpArrayAccessBuilder(base);
}

// Create array access from BUILDER.  
pcp_array_access* 
pcp_array_access_builder_create_access(pcp_array_access_builder* builder)
{
  return builder->createAccess();
}

// PCP Stmt 
static pcp_stmt_kind
pcp_stmt_kind_from_class(pcp_stmt* stmt)
{
  return stmt->isCopy() ? pcp_stmt_kind_copy :
    stmt->isUserStmt() ? pcp_stmt_kind_user :
    stmt->isSequence() ? pcp_stmt_kind_sequence :
    stmt->isGuard() ? pcp_stmt_kind_guard :
    stmt->isLoop() ? pcp_stmt_kind_loop :
    pcp_stmt_kind_unknown;
}


// Cast STMT to object.  
pcp_object* 
pcp_stmt_to_object(pcp_stmt* stmt)
{
  return stmt;
}

// Get kind of STMT.  
pcp_stmt_kind
pcp_stmt_get_kind(pcp_stmt* stmt)
{
  return pcp_stmt_kind_from_class(stmt);
}

// Returns true if STMT is a copy stmt.  
bool
pcp_stmt_is_copy(pcp_stmt* stmt)
{
  return stmt->isCopy();
}

// Returns true if STMT is a user stmt.  
bool
pcp_stmt_is_user_stmt(pcp_stmt* stmt)
{
  return stmt->isUserStmt();
}

// Returns true if STMT is a loop stmt.  
bool
pcp_stmt_is_loop(pcp_stmt* stmt)
{
  return stmt->isLoop();
}

// Returns true if STMT is a guard stmt.  
bool
pcp_stmt_is_guard(pcp_stmt* stmt)
{
  return stmt->isGuard();
}

// Returns true if STMT is a sequence stmt.  
bool
pcp_stmt_is_sequence(pcp_stmt* stmt)
{
  return stmt->isSequence();
}

// Cast STMT to copy stmt, if STMT is not a copy return NULL.  
pcp_copy* 
pcp_stmt_to_copy(pcp_stmt* stmt)
{
  return stmt->toCopy();
}

// Cast SfTMT to user stmt, if STMT is not a user stmt return NULL.  
pcp_user_stmt* 
pcp_stmt_to_user_stmt(pcp_stmt* stmt)
{
  return stmt->toUserStmt();
}

// Cast STMT to guard stmt, if STMT is not a guard return NULL.  
pcp_guard* 
pcp_stmt_to_guard(pcp_stmt* stmt)
{
  return stmt->toGuard();
}

// Cast STMT to loop stmt, if STMT is not a loop return NULL.  
pcp_loop* 
pcp_stmt_to_loop(pcp_stmt* stmt)
{
  return stmt->toLoop();
}

// Cast STMT to sequence stmt, if STMT is not a sequence return NULL.  
pcp_sequence* 
pcp_stmt_to_sequence(pcp_stmt* stmt)
{
  return stmt->toSequence();
}

// PCP Copy Stmt 

// Cast COPY to object.  
pcp_object* 
pcp_copy_to_object(pcp_copy* copy)
{
  return copy;
}

// Cast COPU to stmt.  
pcp_stmt* 
pcp_copy_to_stmt(pcp_copy* copy)
{
  return copy;
}

// Get source of COPY.  
pcp_array_access* 
pcp_copy_get_src(pcp_copy* copy)
{
  return copy->getSrc();
}

// Get destination of COPY.  
pcp_array_access* 
pcp_copy_get_dest(pcp_copy* copy)
{
  return copy->getDest();
}

// Create new copy statement given DEST and SRC.  
pcp_copy* 
pcp_copy_create(pcp_array_access* dest, pcp_array_access* src)
{
  return new PcpCopy(dest, src);
}

// PCP User Stmt 

// Cast USER_STMT to object.  
pcp_object* 
pcp_user_stmt_to_object(pcp_user_stmt* user_stmt)
{
  return user_stmt;
}

// Cast USER_STMT to stmt.  
pcp_stmt* 
pcp_user_stmt_to_stmt(pcp_user_stmt* user_stmt)
{
  return user_stmt;
}

// Get name of USER_STMT.  
const char* 
pcp_user_stmt_get_name(pcp_user_stmt* user_stmt)
{
  return user_stmt->getName();
}

// Get numer of accessses in USER_STMT.  
int
pcp_user_stmt_get_num_accesses(pcp_user_stmt* user_stmt)
{
  return user_stmt->getNumAccesses();
}

// Get array access in USER_STMT with given INDEX.  
pcp_array_access* 
pcp_user_stmt_get_array_access(pcp_user_stmt* user_stmt, int index)
{
  return user_stmt->getArrayAccess(index);
}

// PCP User Stmt Builder 

// Set name in BUILDER to NAME.  
void
pcp_user_stmt_builder_set_name(pcp_user_stmt_builder* builder,
			       const char* name)
{
  builder->setName(name);
}

// Add ACCESS to BUILDER.  
void
pcp_user_stmt_builder_add_access(pcp_user_stmt_builder* builder,
				 pcp_array_access* access)
{
  builder->addAccess(access);
}

// Create stmt builder.  
pcp_user_stmt_builder* 
pcp_user_stmt_builder_create()
{
  return new PcpUserStmtBuilder();
}

// Create user stmt from BUILDER.  
pcp_user_stmt* 
pcp_user_stmt_builder_create_user_stmt(pcp_user_stmt_builder* builder)
{
  return builder->createUserStmt();
}

// PCP Sequence 

// Cast SEQUENCE to stmt.  
pcp_stmt* 
pcp_sequence_to_stmt(pcp_sequence* sequence)
{
  return sequence;
}

// Get number of statements in SEQUENCE.  
int
pcp_sequence_get_num_stmts(pcp_sequence* sequence)
{
  return sequence->getNumStmts();
}

// Get statment in SEQUENCE with given INDEX.  
pcp_stmt* 
pcp_sequence_get_stmt(pcp_sequence* sequence, int index)
{
  return sequence->getStmt(index);
}

// PCP Sequence Builder 

// Add STMT to the end of the sequence in BUILDER.  
void
pcp_sequence_builder_add(pcp_sequence_builder* builder, pcp_stmt* stmt)
{
  builder->add(stmt);
}

// Create sequence builder.  
pcp_sequence_builder* 
pcp_sequence_builder_create()
{
  return new PcpSequenceBuilder();
}

// Create sequence from BUILDER.  
pcp_sequence* 
pcp_sequence_builder_create_sequence(pcp_sequence_builder* builder)
{
  return builder->createSequence();
}

// PCP Guard 

// Cast GUARD to objec.  
pcp_object* 
pcp_guard_to_object(pcp_guard* guard)
{
  return guard;
}

// Cast GUARD to stmt.  
pcp_stmt* 
pcp_guard_to_stmt(pcp_guard* guard)
{
  return guard;
}

// Get condition of GUARD.  
pcp_bool_expr* 
pcp_guard_get_condition(pcp_guard* guard)
{
  return guard->getCondition();
}

// Get body of GUARD.  
pcp_stmt* 
pcp_guard_get_body(pcp_guard* guard)
{
  return guard->getBody();
}

// Create guard with given CONDITION and BODY.  
pcp_guard* 
pcp_guard_create(pcp_bool_expr* condition, pcp_stmt* body)
{
  return new PcpGuard(condition, body);
}


// PCP Loop 

// Cast LOOP to object.  
pcp_object* 
pcp_loop_to_object(pcp_loop* loop)
{
  return loop;
}

// Cast LOOP to stmt.  
pcp_stmt* 
pcp_loop_to_stmt(pcp_loop* loop)
{
  return loop;
}

// Get iv of LOOP.  
pcp_iv* 
pcp_loop_get_iv(pcp_loop* loop)
{
  return loop->getIv();
}

// Get start of LOOP.  
pcp_expr* 
pcp_loop_get_start(pcp_loop* loop)
{
  return loop->getStart();
}

// Get condition of LOOP.  
pcp_bool_expr* 
pcp_loop_get_condition(pcp_loop* loop)
{
  return loop->getCondition();
}

// Get stride of LOOP.  
pcp_constant* 
pcp_loop_get_stride(pcp_loop* loop)
{
  return loop->getStride();
}

// Get body of LOOP.  
pcp_stmt* 
pcp_loop_get_body(pcp_loop* loop)
{
  return loop->getBody();
}

// Create loop as loop(IV, START, CONDITION, STRIDE) { BODY }.  
pcp_loop* 
pcp_loop_create(pcp_iv* iv, pcp_expr* start, pcp_bool_expr* condition,
		pcp_constant* stride, pcp_stmt* body)
{
  return new PcpLoop(iv, start, condition, stride, body);
}

// PCP Scop 

// Cast SCOP to object.  
pcp_object* 
pcp_scop_to_object(pcp_scop* scop)
{
  return scop;
}

// Get numer of variables of SCOP.  
int
pcp_scop_get_num_variables(pcp_scop* scop)
{
  return scop->getNumVariables();
}

// Get variable of SCOP with given INDEX.  
pcp_variable* 
pcp_scop_get_variable(pcp_scop* scop, int index)
{
  return scop->getVariable(index);
}

// Get number of paramters of SCOP.  
int
pcp_scop_get_num_parameters(pcp_scop* scop)
{
  return scop->getNumParameters();
}

// Get parameter of SCOP with given INDEX.  
pcp_parameter* 
pcp_scop_get_parameter(pcp_scop* scop, int index)
{
  return scop->getParameter(index);
}

// Get body of SCOP.  
pcp_stmt* 
pcp_scop_get_body(pcp_scop* scop)
{
  return scop->getBody();
}

// PCP Scop Builder 

// Add VARIABLE to BUILDER.  
void
pcp_scop_builder_add_variable(pcp_scop_builder* builder,
			      pcp_variable* variable)
{
  builder->addVariable(variable);
}

// Add PARAMETER to BUILDER.  
void
pcp_scop_builder_add_parameter(pcp_scop_builder* builder,
			       pcp_parameter* parameter)
{
  builder->addParameter(parameter);
}

// Set body in BUILDER to BODY.  
void
pcp_scop_builder_set_body(pcp_scop_builder* builder, pcp_stmt* body)
{
  builder->setBody(body);
}

// Create new scop builder.  
pcp_scop_builder* 
pcp_scop_builder_create()
{
  return new PcpScopBuilder();
}

// Create new scop from BUILDER.  
pcp_scop* 
pcp_scop_builder_create_scop(pcp_scop_builder* builder)
{
  return builder->createScop();
}

