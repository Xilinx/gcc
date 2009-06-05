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

#include "pcp_error.h"
#include "pcp_alloc.h"
#include "pcp.h"
#include "pcp_visitor.h"

// PCP Object 

// Initialize 
void 
PcpObject::initialize()
{
  setName(NULL);
  setAnnots(NULL);
}

// Set name to NAME.  
void
PcpObject::setName(const char* name)
{
  this->name = name;
}

// Get name.  
const char* 
PcpObject::getName()
{
  return this->name;
}

// Set annotation to ANNOT.  
void
PcpObject::setAnnots(PcpAnnotSet* annots)
{
  this->annots = annots;
}

// Get number of annotations.  
int PcpObject::getNumAnnots()
{
  return this->getAnnots() == NULL ? 0 : this->getAnnots()->getNumAnnots();
}

// Get annotation with given INDEX.  
PcpAnnotTerm* PcpObject::getAnnot(int index)
{
  pcpAssert(this->getAnnots() != NULL);
  return this->getAnnots()->getAnnot(index);
}

// Get annotation with given TAG.  
PcpAnnotTerm* PcpObject::getAnnotWithTag(const char* tag)
{
  int numAnnots;
  int i;
  
  if(this->getAnnots() == NULL)
    return NULL;

  numAnnots =  this->getNumAnnots();
  for(i = 0; i < numAnnots; i++)
    {
      PcpAnnotTerm* annotTerm = this->getAnnot(i);
      if(annotTerm->tagEquals(tag))
	return annotTerm;
    }
  return NULL;
}

// Returns true if an annotation with given TAG exists.  
bool
PcpObject::containsAnnotWithTag(const char* tag)
{
  return this->getAnnotWithTag(tag) != NULL;
}

// Add ANNOT 
void PcpObject::addAnnot(PcpAnnotTerm* annot)
{
  PcpAnnotSet* annots = this->getAnnots();
  if(annots == NULL)
    {
      annots = new PcpAnnotSet();
      this->setAnnots(annots);
    }
  annots->addAnnot(annot);
}

// Get annotations.  
PcpAnnotSet*
PcpObject::getAnnots()
{
  return this->annots;
}

// Return true if this is an array type.  
bool
PcpObject::isArrayType()
{
  return false;
}

// Return true if this is an expr.  
bool
PcpObject::isExpr()
{
  return false;
}

// Return true if this is an iv.  
bool
PcpObject::isIv()
{
  return false;
}

// Return true if this is a parameter.  
bool
PcpObject::isParameter()
{
  return false;
}


// Return true if this is a bool_expr.  
bool
PcpObject::isBoolExpr()
{
  return false;
}

// Return true if this is a variable.  
bool
PcpObject::isVariable()
{
  return false;
}

// Return true if this is an array access.  
bool
PcpObject::isArrayAccess()
{
  return false;
}

// Return true if this is a stmt.  
bool
PcpObject::isStmt()
{
  return false;
}

// Return true if this is a scop.  
bool
PcpObject::isScop()
{
  return false;
}

// Cast this to array type.  
PcpArrayType* 
PcpObject::toArrayType()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to expr.  
PcpExpr* 
PcpObject::toExpr()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to parameter.  
PcpParameter* 
PcpObject::toParameter()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to IV.  
PcpIv* 
PcpObject::toIv()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to bool expr.  
PcpBoolExpr* 
PcpObject::toBoolExpr()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to variable.  
PcpVariable* 
PcpObject::toVariable()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to array access.  
PcpArrayAccess* 
PcpObject::toArrayAccess()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to stmt.  
PcpStmt* 
PcpObject::toStmt()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to scop.  
PcpScop* 
PcpObject::toScop()
{
  pcpAssert(false);
  return NULL;
}

// Set annotations array in ANNOTSet to ANNOTS.  
void 
PcpAnnotSet::setAnnots(PcpDynamicArray<PcpAnnotTerm*>* annots)
{
  this->annots = annots;
}

// Get annotation array in ANNOTSet.  
PcpDynamicArray<PcpAnnotTerm*>*  
PcpAnnotSet::getAnnots()
{
  return this->annots;
}

// Get the annotation in ANNOTSet with given INDEX.  
PcpAnnotTerm* PcpAnnotSet::getAnnot(int index)
{
  PcpAnnotTerm* result = this->getAnnots()->get(index);
  return result;
}

// Get number of annotations in ANNOTSet.  
int 
PcpAnnotSet::getNumAnnots()
{
  return this->getAnnots()->getSize();
}

// Get the annotation with TAG in ANNOTSet.  
PcpAnnotTerm* PcpAnnotSet::getAnnotWithTag(const char* tag)
{
  int i;
  int numAnnots = this->getNumAnnots();
  for(i = 0; i < numAnnots; i++)
    {
      PcpAnnotTerm* annotTerm = this->getAnnot(i);
      const char* annotTag = annotTerm->getTag();
      if(strcmp(annotTag, tag) == 0)
	return annotTerm;
    }
  return NULL;
}

// Add ANNOT to ANNOTSet. Assert that no previous annotation with the
//   same tag exists.  
void 
PcpAnnotSet::addAnnot(PcpAnnotTerm* annot)
{
  PcpAnnotTerm* existingTerm = this->getAnnotWithTag(annot->getTag());
  if(existingTerm != NULL)
    {
      pcpAssert(false);
      printf("Existing tag: %s\n", annot->getTag());
    }

  this->getAnnots()->add(annot);
}

PcpAnnotSet::PcpAnnotSet()
{
  PcpDynamicArray<PcpAnnotTerm*>* annots = 
    new PcpDynamicArray<PcpAnnotTerm*>(1);
  this->setAnnots(annots);
}

void
PcpAnnotSet::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// Return true if ANNOT is an int.  
bool
PcpAnnot::isAnnotInt()
{
  return false;
}

// Return true if ANNOT is a string.  
bool
PcpAnnot::isAnnotString()
{
  return false;
}

// Return true if ANNOT is an object.  
bool
PcpAnnot::isAnnotObject()
{
  return false;
}

// Return true if ANNOT is a term.  
bool
PcpAnnot::isAnnotTerm()
{
  return false;
}

// Cast ANNOT to int.  
PcpAnnotInt* 
PcpAnnot::toAnnotInt()
{
  pcpAssert(false);
  return NULL;
}

// Cast ANNOT to string.  
PcpAnnotString* 
PcpAnnot::toAnnotString()
{
  pcpAssert(false);
  return NULL;
}

// Cast ANNOT to object.  
PcpAnnotObject* 
PcpAnnot::toAnnotObject()
{
  pcpAssert(false);
  return NULL;
}

// Cast ANNOT to term.  
PcpAnnotTerm* 
PcpAnnot::toAnnotTerm()
{
  pcpAssert(false);
  return NULL;
}

// Set value of ANNOTInt to VALUE.  
void
PcpAnnotInt::setValue(int value)
{
  this->value = value;
}

// Get value of ANNOTInt.  
int
PcpAnnotInt::getValue()
{
  return this->value;
}

// Create annot int with given VALUE.  
PcpAnnotInt::PcpAnnotInt(int value)
{
  this->setValue(value);
}

bool
PcpAnnotInt::isAnnotInt()
{
  return true;
}

PcpAnnotInt*
PcpAnnotInt::toAnnotInt()
{
  return this;
}

void
PcpAnnotInt::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PcpAnnotString 
bool PcpAnnotString::isAnnotString()
{
  return true;
}
PcpAnnotString* PcpAnnotString::toAnnotString()
{
  return this;
}

// Set string of ANNOTString to STRING.  
void
PcpAnnotString::setString(const char* string)
{
  this->string = string;
}

// Get string of ANNOTString.  
const char* 
PcpAnnotString::getString()
{
  return this->string;
}

// Create annot string with given STRING.  
PcpAnnotString::PcpAnnotString(const char* string)
{
  this->setString(string);
}

void
PcpAnnotString::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Annot Object 

bool PcpAnnotObject::isAnnotObject()
{
  return true;
}

PcpAnnotObject* PcpAnnotObject::toAnnotObject()
{
  return this;
}

// Set object to OBJECT.  
void
PcpAnnotObject::setObject(PcpObject* object)
{
  this->object = object;
}

// Get object.
PcpObject* 
PcpAnnotObject::getObject()
{
  return this->object;
}

// Create annot object with given OBJECT.  
PcpAnnotObject::PcpAnnotObject(PcpObject* object)
{
  this->setObject(object);
}

void
PcpAnnotObject::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// Pcp Annot Term 

// Return true if this is an annot term.
bool PcpAnnotTerm::isAnnotTerm()
{
  return true;
}

// Convert this to an annot term.
PcpAnnotTerm* PcpAnnotTerm::toAnnotTerm()
{
  return this;
}

// Set tag to TAG.  
void
PcpAnnotTerm::setTag(const char* tag)
{
  this->tag = tag;
}

// Get tag.
const char* 
PcpAnnotTerm::getTag()
{
  return this->tag;
}

// Return true if tag equals TAG.  
bool
PcpAnnotTerm::tagEquals(const char* tag)
{
  return strcmp(tag, this->getTag()) == 0;
}

// Get number of arguments.
int
PcpAnnotTerm::getNumArguments()
{
  return this->getArguments()->getSize();
}

// Set arguments to ARGUMENTS.  
void
PcpAnnotTerm::setArguments(PcpArray<PcpAnnot*>* arguments)
{
  this->arguments = arguments;
}

// Get arguments.
PcpArray<PcpAnnot*>*
PcpAnnotTerm::getArguments()
{
  return this->arguments;
}

// Set argument with given INDENT to ANNOT.  
void
PcpAnnotTerm::setArgument(int index, PcpAnnot* annot)
{
  this->getArguments()->set(index, annot);
}

// Get argument with given INDEX.  
PcpAnnot* 
PcpAnnotTerm::getArgument(int index)
{
  return this->getArguments()->get(index);
}

// Create annot term with TAG, and ARGUMENTS.
PcpAnnotTerm::PcpAnnotTerm(const char* tag, PcpArray<PcpAnnot*>* arguments)
{
  this->setTag(tag);
  this->setArguments(arguments);
}

void
PcpAnnotTerm::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Annot Term Builder 

// Set tag to TAG.  
void
PcpAnnotTermBuilder::setTag(const char* tag)
{
  this->tag = tag;
}

// Get tag.
const char* 
PcpAnnotTermBuilder::getTag()
{
  return this->tag;
}

// Set arguments to ARGUMENTS.  
void
PcpAnnotTermBuilder::setArguments(PcpDynamicArray<PcpAnnot*>* arguments)
{
  this->arguments = arguments;
}

// Get arguments.
PcpDynamicArray<PcpAnnot*>*
PcpAnnotTermBuilder::getArguments()
{
  return this->arguments;
}

// Add ARGUMENT.
void
PcpAnnotTermBuilder::addArgument(PcpAnnot* argument)
{
  this->getArguments()->add(argument);
}

// Create new annot term builder.  
PcpAnnotTermBuilder::PcpAnnotTermBuilder()
{
  this->setTag(NULL);
  this->setArguments(new PcpDynamicArray<PcpAnnot*>(1));
}

// Create annot term.
PcpAnnotTerm* 
PcpAnnotTermBuilder::createAnnot()
{
  PcpAnnotTerm* annotTerm;
  annotTerm = new PcpAnnotTerm(this->getTag(), this->getArguments());
  return annotTerm;
}

// Pcp Array Type

// Return true if this is an array type.
bool 
PcpArrayType::isArrayType()
{
  return true;
}

// Cast this to array type.
PcpArrayType*
PcpArrayType::toArrayType()
{
  return this;
}

// Return the number of dimensions.
int
PcpArrayType::getNumDimensions()
{
  return this->getDimensions()->getSize();
}

// Set the dimensions to DIMENSIONS.  
void
PcpArrayType::setDimensions(PcpArray<PcpExpr*>* dimensions)
{
  this->dimensions = dimensions;
}

// Get dimensions.
PcpArray<PcpExpr*>* 
PcpArrayType::getDimensions()
{
  return this->dimensions;
}

// Return the size of dimension with given INDEX
PcpExpr*
PcpArrayType::getDimension(int index)
{
  return this->getDimensions()->get(index);
}

// Get iterator over the dimensions.
PcpIterator<PcpExpr*>* 
PcpArrayType::getDimensionsIterator()
{
  return this->getDimensions()->getIterator();
}

// Create new array type.
PcpArrayType::PcpArrayType(PcpArray<PcpExpr*>* dimensions)
{
  initialize();
  this->setDimensions(dimensions);
}

void
PcpArrayType::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// Pcp Array Type Builder

// Set array to ARRAY.  
void
PcpArrayTypeBuilder::setArray(PcpDynamicArray<PcpExpr*>* array)
{
  this->array = array;
}

// Get array.  
PcpDynamicArray<PcpExpr*>* 
PcpArrayTypeBuilder::getArray()
{
  return this->array;
}

// Create array type builder.  
PcpArrayTypeBuilder::PcpArrayTypeBuilder()
{
  this->setArray(new PcpDynamicArray<PcpExpr*>(2));
}

// Add a new dimension with the given SIZE.
void
PcpArrayTypeBuilder::addDimension(PcpExpr* size)
{
  this->getArray()->add(size);
}

// Add a new dimension with given SIZE
void 
PcpArrayTypeBuilder::addIntDimension(int size)
{
  this->addDimension(new PcpConstant(size));
}

// Create new array type from BUIDER.  
PcpArrayType* 
PcpArrayTypeBuilder::createType()
{
  return new PcpArrayType(this->getArray());
}

// PcpExpr

// Return true if this is an expr.
bool PcpExpr::isExpr()
{
  return true;
}

// Convert this to an expr. 
PcpExpr* PcpExpr::toExpr()
{
  return this;
}

// Return true if this is a pcpParameter, otherwise return false.  
bool
PcpExpr::isParameter()
{
  return false;
}

// Return true if this is a pcpArith, otherwise return false.  
bool
PcpExpr::isArith()
{
  return false;
}

// Return true if this is a pcpConstant, otherwise return false.  
bool
PcpExpr::isConstant()
{
  return false;
}

// Return true if this is a pcpIv, otherwise return false.  
bool
PcpExpr::isIv()
{
  return false;
}

// Cast this to pcpParameter.  
PcpParameter* 
PcpExpr::toParameter()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to pcpArith.  
PcpArith* 
PcpExpr::toArith()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to pcpConstant.  
PcpConstant* 
PcpExpr::toConstant()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to pcpIv.  
PcpIv* 
PcpExpr::toIv()
{
  pcpAssert(false);
  return NULL;
}

// PCP Bool Expr 

// Return true if this is a bool expr.
bool
PcpBoolExpr::isBoolExpr()
{
  return true;
}

// Cast this to a bool expr.
PcpBoolExpr* PcpBoolExpr::toBoolExpr()
{
  return this;
}

// Return true if this is a compare.
bool
PcpBoolExpr::isCompare()
{
  return false;
}

// Return true if this is a boolean arithmetic operation.  
bool
PcpBoolExpr::isBoolArith()
{
  return false;
}

// Convert this to a compare.
PcpCompare* 
PcpBoolExpr::toCompare()
{
  pcpAssert(false);
  return NULL;
}

// Cast BOOLExpr to bool arith, if it is not a boolean arithmetic operator return NULL.  
PcpBoolArith*
PcpBoolExpr::toBoolArith()
{
  pcpAssert(false);
  return NULL;
}


// PCP Compare 

// Return true if this is a compare.
bool
PcpCompare::isCompare()
{
  return true;
}

// Convert this to a compare.
PcpCompare*
PcpCompare::toCompare()
{
  return this;
}

// Set operator to OPER.
void
PcpCompare::setOperator(PcpCompareOperator oper)
{
  this->oper = oper;
}

// Get operator.
PcpCompareOperator
PcpCompare::getOperator()
{
  return this->oper;
}

// Set lhs to LHS.
void
PcpCompare::setLhs(PcpExpr* lhs)
{
  this->lhs = lhs;
}

// Get lhs.
PcpExpr* 
PcpCompare::getLhs()
{
  return this->lhs;
}

// Set rhs to RHS.
void
PcpCompare::setRhs(PcpExpr* rhs)
{
  this->rhs = rhs;
}

// Get rhs.
PcpExpr* 
PcpCompare::getRhs()
{
  return this->rhs;
}

// Create new compare with given OPER, LHS and RHS
PcpCompare::PcpCompare(PcpCompareOperator oper,
		       PcpExpr* lhs,
		       PcpExpr* rhs)
{
  initialize();
  this->setOperator(oper);
  this->setLhs(lhs);
  this->setRhs(rhs);
}

void
PcpCompare::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Bool Arith 

// Return true if this is a bool arith
bool PcpBoolArith::isBoolArith()
{
  return true;
}

// Convert this to a bool arith.
PcpBoolArith* PcpBoolArith::toBoolArith()
{
  return this;
}


// Set operator to OPERATOR.  
void
PcpBoolArith::setOperator(PcpBoolArithOperator oper)
{
  this->oper = oper;
}

// Get operator.
PcpBoolArithOperator
PcpBoolArith::getOperator()
{
  return this->oper;
}

// Get number of operands.
int
PcpBoolArith::getNumOperands()
{
  return this->getOperands()->getSize();
}

// Set operands to OPERANDS.  
void
PcpBoolArith::setOperands(PcpArray<PcpBoolExpr*>* operands)
{
  this->operands = operands;
}

// Get operands.
PcpArray<PcpBoolExpr*>*
PcpBoolArith::getOperands()
{
  return this->operands;
}

// Get operand with given INDEX.  
PcpBoolExpr*
PcpBoolArith::getOperand(int index)
{
  return this->getOperands()->get(index);
}

PcpIterator<PcpBoolExpr*>*
PcpBoolArith::getOperandsIterator()
{
  return this->getOperands()->getIterator();
}


// Create new boolean arithmetic operation.  
PcpBoolArith::PcpBoolArith(PcpBoolArithOperator oper,
			   PcpArray<PcpBoolExpr*>* operands)
{
  initialize();
  this->setOperator(oper);
  this->setOperands(operands);
}

void
PcpBoolArith::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Bool Arith Builder.  

// Set operator to OPERATOR.  
void
PcpBoolArithBuilder::setOperator(PcpBoolArithOperator oper)
{
  this->oper = oper;
}

// Get operator.
PcpBoolArithOperator
PcpBoolArithBuilder::getOperator()
{
  return this->oper;
}

// Set operands to OPERANDS.  
void
PcpBoolArithBuilder::setOperands(PcpDynamicArray<PcpBoolExpr*>* operands)
{
  this->operands = operands;
}

// Get operands.
PcpDynamicArray<PcpBoolExpr*>*
PcpBoolArithBuilder::getOperands()
{
  return this->operands;
}

// Add OPERAND.
void
PcpBoolArithBuilder::addOperand(PcpBoolExpr* operand)
{
  this->getOperands()->add(operand);
}

// Create new boolean arithmetic operation builder.  
PcpBoolArithBuilder::PcpBoolArithBuilder()
{
  this->setOperator(PcpBoolArithOperator::unknown());
  this->setOperands(new PcpDynamicArray<PcpBoolExpr*>(2));
}


// Create new boolean arithmetic operation.  
PcpBoolArith*
PcpBoolArithBuilder::createBoolArith()
{
  PcpBoolArith* boolArith;
  boolArith = new PcpBoolArith(this->getOperator(), this->getOperands());
  return boolArith;
}


// Create binary arithmetic operation with OPERATOR, LHS, RHS.  
PcpBoolArith*
PcpBoolArith::pcpBoolArithBinaryCreate(PcpBoolArithOperator oper,
				       PcpBoolExpr* lhs,
				       PcpBoolExpr* rhs)
{
  PcpBoolArith* result;
  PcpBoolArithBuilder* builder = new PcpBoolArithBuilder();
  builder->setOperator(oper);
  builder->addOperand(lhs);
  builder->addOperand(rhs);
  result = builder->createBoolArith();
  delete builder;
  return result;
}

// Set operator of ARITH to OPERATOR.  

// Return true if this is an arithmetic operation.
bool PcpArith::isArith()
{
  return true;
}

// Cast this to an arithmetic operation.
PcpArith* PcpArith::toArith()
{
  return this;
}

// Set operator to OPER.
void
PcpArith::setOperator(PcpArithOperator oper)
{
  this->oper = oper;
}

// Get operator of ARITH.  
PcpArithOperator
PcpArith::getOperator()
{
  return this->oper;
}

// Get number of operands.
int PcpArith::getNumOperands()
{
  return this->getOperands()->getSize();
}

// Set operands to OPERANDS.  
void
PcpArith::setOperands(PcpArray<PcpExpr*>* operands)
{
  this->operands = operands;
}

// Get operands of ARITH.  
PcpArray<PcpExpr*>*
PcpArith::getOperands()
{
  return this->operands;
}

// Get operand with given INDEX.  
PcpExpr* 
PcpArith::getOperand(int index)
{
  return this->getOperands()->get(index);
}

PcpIterator<PcpExpr*>*
PcpArith::getOperandsIterator()
{
  return this->getOperands()->getIterator();
}


// Create new arithmetic operation.  
PcpArith::PcpArith(PcpArithOperator oper, PcpArray<PcpExpr*>* operands)
{
  initialize();
  this->setOperator(oper);
  this->setOperands(operands);
}

// Create binary arithmetic operation with OPERATOR, LHS, RHS.  
PcpArith* 
PcpArith::pcpArithBinaryCreate(PcpArithOperator oper,
			       PcpExpr* lhs,
			       PcpExpr* rhs)
{
  PcpArith* arith;
  PcpArithBuilder* builder = new PcpArithBuilder();
  builder->setOperator(oper);
  builder->addOperand(lhs);
  builder->addOperand(rhs);
  arith = builder->createArith();
  delete builder;
  return arith;
}

void
PcpArith::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}


// PCP Arith Builder.  

// Set operator to OPERATOR.  
void
PcpArithBuilder::setOperator(PcpArithOperator oper)
{
  this->oper = oper;
}

// Get operator.
PcpArithOperator
PcpArithBuilder::getOperator()
{
  return this->oper;
}

// Set operands to OPERANDS.  
void
PcpArithBuilder::setOperands(PcpDynamicArray<PcpExpr*>* operands)
{
  this->operands = operands;
}

// Get operands.
PcpDynamicArray<PcpExpr*>*
PcpArithBuilder::getOperands()
{
  return this->operands;
}

// Add OPERAND.
void
PcpArithBuilder::addOperand(PcpExpr* operand)
{
  this->getOperands()->add(operand);
}

// Create new arithmetic operation.
PcpArith* 
PcpArithBuilder::createArith()
{
  PcpArith* arith;
  arith = new PcpArith(this->getOperator(), this->getOperands());
  return arith;
}

// Create new arithmetic operation builder.  
PcpArithBuilder::PcpArithBuilder()
{
  this->setOperator(PcpArithOperator::unknown());
  this->setOperands(new PcpDynamicArray<PcpExpr*>(2));
}

// PCP Constant 

// Set value of CONSTANT to VALUE.  
void
PcpConstant::setValue(int value)
{
  this->value = value;
}

// Get value of CONSTANT.  
int
PcpConstant::getValue()
{
  return this->value;
}

// Return true if this is a constant.
bool
PcpConstant::isConstant()
{
  return true;
}

// Cast this to a constant.
PcpConstant*
PcpConstant::toConstant()
{
  return this;
}

// Create constant with VALUE.  
PcpConstant::PcpConstant(int value)
{
  initialize();
  this->setValue(value);
}

void
PcpConstant::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Induction Variable 

// Return true if this is an iv.
bool
PcpIv::isIv()
{
  return true;
}

// Convert this to an iv.
PcpIv*
PcpIv::toIv()
{
  return this;
}

// Create iv with NAME.  
PcpIv::PcpIv(const char* name)
{
  initialize();
  this->setName(name);
}

void
PcpIv::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Variable 

// Return true if this is a variable.
bool PcpVariable::isVariable()
{
  return true;
}

// Cast this to a variable.
PcpVariable* PcpVariable::toVariable()
{
  return this;
}

// Set is input of to ISINPUT.  
void
PcpVariable::setIsInput(bool isInput)
{
  this->isInput = isInput;
}

// Get is input
bool
PcpVariable::getIsInput()
{
  return this->isInput;
}

// Set is output to ISOUTPUT.  
void
PcpVariable::setIsOutput(bool isOutput)
{
  this->isOutput = isOutput;
}

// Get is output.
bool
PcpVariable::getIsOutput()
{
  return this->isOutput;
}

// Set type to TYPE.  
void
PcpVariable::setType(PcpArrayType* type)
{
  this->type = type;
}

// Get tpe.
PcpArrayType* 
PcpVariable::getType()
{
  return this->type;
}

// Create variable given TYPE and NAME.  
PcpVariable::PcpVariable(PcpArrayType* type, const char* name)
{
  initialize();
  this->setType(type);
  this->setName(name);
  this->setIsInput(false);
  this->setIsOutput(false);
}

void
PcpVariable::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Parameter 

// Return true if this is a parameter.
bool PcpParameter::isParameter()
{
  return true;
}

// Cast this to a parameter.
PcpParameter* PcpParameter::toParameter()
{
  return this;
}

// Create parameter with the given NAME.  
PcpParameter::PcpParameter(const char* name)
{
  initialize();
  this->setName(name);
}

void
PcpParameter::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// Array Access.  

// Return true if this is an array access
bool PcpArrayAccess::isArrayAccess()
{
  return true;
}

// Cast this to an array access.
PcpArrayAccess* 
PcpArrayAccess::toArrayAccess()
{
  return this;
}

// Set operator to OPERATOR.  
void
PcpArrayAccess::setOperator(PcpArrayOperator oper)
{
  this->oper = oper;
}

// Get operator.
PcpArrayOperator
PcpArrayAccess::getOperator()
{
  return this->oper;
}

// Set base to BASE.  
void
PcpArrayAccess::setBase(PcpVariable* base)
{
  this->base = base;
}

// Get base.
PcpVariable* 
PcpArrayAccess::getBase()
{
  return this->base;
}

// Set subscripts to SUBSCRIPTS.  
void
PcpArrayAccess::setSubscripts(PcpArray<PcpExpr*>* subscripts)
{
  this->subscripts = subscripts;
}

// Get subscripts.
PcpArray<PcpExpr*>*
PcpArrayAccess::getSubscripts()
{
  return this->subscripts;
}

// Set subscript at INDEX to SUBSCRIPT.  
void
PcpArrayAccess::setSubscript(int index, PcpExpr* subscript)
{
  this->getSubscripts()->set(index, subscript);
}

// Get subscript at INDEX.  
PcpExpr* 
PcpArrayAccess::getSubscript(int index)
{
  return this->getSubscripts()->get(index);
}

// Get number of subscripts.
int
PcpArrayAccess::getNumSubscripts()
{
  PcpVariable* variable = this->getBase();
  PcpArrayType* type = variable->getType();
  int typeNumDims = type->getNumDimensions();
  return typeNumDims;
}

PcpIterator<PcpExpr*>* 
PcpArrayAccess::getSubscriptsIterator()
{
  return this->getSubscripts()->getIterator();
}


// Create array access given OPERATOR, BASE and SUBSCRIPTS.  
PcpArrayAccess::PcpArrayAccess(PcpArrayOperator oper, 
			       PcpVariable* base,
			       PcpArray<PcpExpr*>* subscripts)
{
  initialize();
  this->setOperator(oper);
  this->setBase(base);
  this->setSubscripts(subscripts);
}

void
PcpArrayAccess::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// Return true if this is a use, otherwise return false.  
bool
PcpArrayAccess::isUse()
{
  return this->getOperator().isUse();
}

// Return true if this is a def, otherwise return false.  
bool
PcpArrayAccess::isDef()
{
  return this->getOperator().isDef();
}

// Return true if this is a maydef, otherwise return false.  
bool
PcpArrayAccess::isMaydef()
{
  return this->getOperator().isMaydef();
}

// PCP Array Access Builder 

// Set base to BASE.  
void
PcpArrayAccessBuilder::setBase(PcpVariable* base)
{
  this->base = base;
}

// Get base.
PcpVariable* 
PcpArrayAccessBuilder::getBase()
{
  return this->base;
}

// Set operator to OPERATOR.  
void
PcpArrayAccessBuilder::setOperator(PcpArrayOperator oper)
{
  this->oper = oper;
}

// Get operator.
PcpArrayOperator
PcpArrayAccessBuilder::getOperator()
{
  return this->oper;
}

// Set subscript index to SUBSCRIPTINDEX.  
void
PcpArrayAccessBuilder::setSubscriptIndex(int subscriptIndex)
{
  this->subscriptIndex = subscriptIndex;
}

// Get subscript index.  
int
PcpArrayAccessBuilder::getSubscriptIndex()
{
  return this->subscriptIndex;
}

// Get subscripts.
PcpArray<PcpExpr*>*
PcpArrayAccessBuilder::getSubscripts()
{
  return this->subscripts;
}

// Set subscript with given INDEX to SUBSCRIPT.  
void
PcpArrayAccessBuilder::setSubscript(int index, PcpExpr* subscript)
{
  this->getSubscripts()->set(index, subscript);
}


// Set subscripts to SUBSCRIPTS.  
void
PcpArrayAccessBuilder::setSubscripts(PcpArray<PcpExpr*>* subscripts)
{
  this->subscripts = subscripts;
}

// Get subscript with given INDEX.  
PcpExpr* 
PcpArrayAccessBuilder::getSubscript(int index)
{
  return this->getSubscripts()->get(index);
}

// Get number of dimensions of base.
int
PcpArrayAccessBuilder::getBaseNumDimensions()
{
  PcpVariable* variable = this->getBase();
  PcpArrayType* type = variable->getType();
  int typeNumDims = type->getNumDimensions();
  return typeNumDims;
}

// Add SUBSCRIPT.  
void
PcpArrayAccessBuilder::addSubscript(PcpExpr* subscript)
{
  int currentIndex = this->getSubscriptIndex();
  int typeNumDims = this->getBaseNumDimensions();

  pcpAssert(currentIndex < typeNumDims);
  this->setSubscript(currentIndex, subscript);
  this->setSubscriptIndex(currentIndex + 1);
}


// Create new access builder with given BASE.  
PcpArrayAccessBuilder::PcpArrayAccessBuilder(PcpVariable* base)
{
  PcpArrayType* type = base->getType();
  PcpArray<PcpExpr*>* subscripts = new PcpArray<PcpExpr*>(type->getNumDimensions());

  this->setBase(base);
  this->setOperator(PcpArrayOperator::unknown());
  this->setSubscriptIndex(0);
  this->setSubscripts(subscripts);
}

// Create array access.  
PcpArrayAccess* 
PcpArrayAccessBuilder::createAccess()
{
  pcpAssert(!this->getOperator().isUnknown());
  pcpAssert(this->getSubscriptIndex() == this->getBaseNumDimensions());
  return new PcpArrayAccess(this->getOperator(),
			    this->getBase(),
			    this->getSubscripts());
}

// PCP Stmt 

// Return true if this is a statement
bool
PcpStmt::isStmt()
{
  return true;
}
 
// Cast this to a statement.
PcpStmt* PcpStmt::toStmt()
{
  return this;
}

// Returns true if this is a copy stmt.  
bool
PcpStmt::isCopy()
{
  return false;
}

// Returns true if this is a user stmt.  
bool
PcpStmt::isUserStmt()
{
  return false;
}

// Returns true if this is a loop stmt.  
bool
PcpStmt::isLoop()
{
  return false;
}

// Returns true if this is a guard stmt.  
bool
PcpStmt::isGuard()
{
  return false;
}

// Returns true if this is a sequence stmt.  
bool
PcpStmt::isSequence()
{
  return false;
}

// Cast this to copy stmt
PcpCopy* 
PcpStmt::toCopy()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to user stmt
PcpUserStmt* 
PcpStmt::toUserStmt()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to guard stmt
PcpGuard* 
PcpStmt::toGuard()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to loop stmt
PcpLoop* 
PcpStmt::toLoop()
{
  pcpAssert(false);
  return NULL;
}

// Cast this to sequence stmt
PcpSequence* 
PcpStmt::toSequence()
{
  pcpAssert(false);
  return NULL;
}

// PCP Copy Stmt 

// Return true if this is a copy.
bool PcpCopy::isCopy()
{
  return true;
}

// Cast this to a copy.
PcpCopy* PcpCopy::toCopy()
{
  return this;
}

// Set source to SRC.  
void
PcpCopy::setSrc(PcpArrayAccess* src)
{
  this->src = src;
}

// Get source.
PcpArrayAccess* 
PcpCopy::getSrc()
{
  return this->src;
}

// Set destination to DEST.  
void
PcpCopy::setDest(PcpArrayAccess* dest)
{
  this->dest = dest;
}

// Get destination.
PcpArrayAccess* 
PcpCopy::getDest()
{
  return this->dest;
}

// Create new copy statement given DEST and SRC.  
PcpCopy::PcpCopy(PcpArrayAccess* dest, PcpArrayAccess* src)
{
  initialize();
  this->setDest(dest);
  this->setSrc(src);
}

void
PcpCopy::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP User Stmt 

// Return true if this is a user stmt.
bool PcpUserStmt::isUserStmt()
{
  return true;
}

// Cast this to a user stmt.
PcpUserStmt* PcpUserStmt::toUserStmt()
{
  return this;
}

// Get numer of accessses in USERStmt.  
int
PcpUserStmt::getNumAccesses()
{
  return this->getArrayAccesses()->getSize();
}

// Set array accesses of USERStmt to ACCESSSES.  
void
PcpUserStmt::setArrayAccesses(PcpArray<PcpArrayAccess*>* accesses)
{
  this->accesses = accesses;
}

// Get array accesses of USERStmt.  
PcpArray<PcpArrayAccess*>*
PcpUserStmt::getArrayAccesses()
{
  return this->accesses;
}

// Set array access in USERStmt with given INDEX to ACCESS.  
void
PcpUserStmt::setArrayAccess(int index, PcpArrayAccess* access)
{
  this->getArrayAccesses()->set(index, access);
}

// Get array access in USERStmt with given INDEX.  
PcpArrayAccess* 
PcpUserStmt::getArrayAccess(int index)
{
  return this->getArrayAccesses()->get(index);
}

PcpIterator<PcpArrayAccess*>* 
PcpUserStmt::getArrayAccessesIterator()
{
  return this->getArrayAccesses()->getIterator();
}

// Create new user stmt with given NAME and ACCESSES.
PcpUserStmt::PcpUserStmt(const char* name, 
			 PcpArray<PcpArrayAccess*>* accesses)
{
  this->initialize();
  this->setName(name);
  this->setArrayAccesses(accesses);
}

void
PcpUserStmt::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP User Stmt Builder 

// Set name to NAME.  
void
PcpUserStmtBuilder::setName(const char* name)
{
  this->name = name;
}

// Get name.
const char* 
PcpUserStmtBuilder::getName()
{
  return this->name;
}

// Set accesses to ACCESSES.  
void
PcpUserStmtBuilder::setAccesses(PcpDynamicArray<PcpArrayAccess*>* accesses)
{
  this->accesses = accesses;
}

// Get accesses.
PcpDynamicArray<PcpArrayAccess*>*
PcpUserStmtBuilder::getAccesses()
{
  return this->accesses;
}

// Add ACCESS.
void
PcpUserStmtBuilder::addAccess(PcpArrayAccess* access)
{
  this->getAccesses()->add(access);
}

// Create stmt builder.  
PcpUserStmtBuilder::PcpUserStmtBuilder()
{
  this->setName(NULL);
  this->setAccesses(new PcpDynamicArray<PcpArrayAccess*>(2));
}


// Create user stmt.
PcpUserStmt* 
PcpUserStmtBuilder::createUserStmt()
{
  PcpUserStmt* userStmt;
  pcpAssert(this->getName() != NULL);
  userStmt = new PcpUserStmt(this->getName(),this->getAccesses());
  return userStmt;
}

// PCP Sequence 

// Return true if this is a sequence.
bool PcpSequence::isSequence()
{
  return true;
}

// Cast this to a sequence.
PcpSequence* PcpSequence::toSequence()
{
  return this;
}

// Get number of statements.
int
PcpSequence::getNumStmts()
{
  return this->getStmts()->getSize();
}

// Set stmts to STMTS.  
void
PcpSequence::setStmts(PcpArray<PcpStmt*>* stmts)
{
  this->stmts = stmts;
}

// Get stmts.
PcpArray<PcpStmt*>*
PcpSequence::getStmts()
{
  return this->stmts;
}

// Set statement with given INDEX to STMT.  
void
PcpSequence::setStmt(int index, PcpStmt* stmt)
{
  this->getStmts()->set(index, stmt);
}

// Get statment with given INDEX.  
PcpStmt* 
PcpSequence::getStmt(int index)
{
  return this->getStmts()->get(index);
}

PcpIterator<PcpStmt*>*
PcpSequence::getStmtsIterator()
{
  return this->getStmts()->getIterator();
}


// Create Sequence with STMTS 
PcpSequence::PcpSequence(PcpArray<PcpStmt*>* stmts)
{
  initialize();
  this->setStmts(stmts);
}

void
PcpSequence::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}
// PCP Sequence Builder 

// Set stmts to STMTS.  
void
PcpSequenceBuilder::setStmts(PcpDynamicArray<PcpStmt*>* stmts)
{
  this->stmts = stmts;
}

// Get stmts.
PcpDynamicArray<PcpStmt*>*
PcpSequenceBuilder::getStmts()
{
  return this->stmts;
}

// Add STMT to the end of the sequence.
void
PcpSequenceBuilder::add(PcpStmt* stmt)
{
  this->getStmts()->add(stmt);
}

// Create sequence builder.  
PcpSequenceBuilder::PcpSequenceBuilder()
{
  this->setStmts(new PcpDynamicArray<PcpStmt*>(3));
}

// Create sequence.  
PcpSequence* 
PcpSequenceBuilder::createSequence()
{
  PcpSequence* sequence;
  sequence = new PcpSequence(this->getStmts());
  return sequence;
}

// PCP Guard 

// Return true if this is a guard.
bool PcpGuard::isGuard()
{
  return true;
}

// Cast this to a guard.
PcpGuard* PcpGuard::toGuard()
{
  return this;
}

// Set condition to CONDITION.  
void
PcpGuard::setCondition(PcpBoolExpr* condition)
{
  this->condition = condition;
}

// Get condition.  
PcpBoolExpr* 
PcpGuard::getCondition()
{
  return this->condition;
}

// Set body to BODY.  
void
PcpGuard::setBody(PcpStmt* body)
{
  this->body = body;
}

// Get body.  
PcpStmt* 
PcpGuard::getBody()
{
  return this->body;
}

// Create guard with given CONDITION and BODY.  
PcpGuard::PcpGuard(PcpBoolExpr* condition, PcpStmt* body)
{
  initialize();
  this->setCondition(condition);
  this->setBody(body);
}

void
PcpGuard::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Loop 

// Return true if this is a loop.
bool PcpLoop::isLoop()
{
  return true;
}

// Cast this to a loop.
PcpLoop* PcpLoop::toLoop()
{
  return this;
}

// Set iv to IV.  
void
PcpLoop::setIv(PcpIv* iv)
{
  this->iv = iv;
}

// Get iv.  
PcpIv* 
PcpLoop::getIv()
{
  return this->iv;
}

// Set start to START.  
void
PcpLoop::setStart(PcpExpr* start)
{
  this->start = start;
}

// Get start.  
PcpExpr* 
PcpLoop::getStart()
{
  return this->start;
}

// Set condition to CONDITION.  
void
PcpLoop::setCondition(PcpBoolExpr* condition)
{
  this->condition = condition;
}

// Get condition.  
PcpBoolExpr* 
PcpLoop::getCondition()
{
  return this->condition;
}

// Set stride to STRIDE.  
void
PcpLoop::setStride(PcpConstant* stride)
{
  this->stride = stride;
}

// Get stride.  
PcpConstant* 
PcpLoop::getStride()
{
  return this->stride;
}

// Set body to BODY.  
void
PcpLoop::setBody(PcpStmt* body)
{
  this->body = body;
}

// Get body.  
PcpStmt* 
PcpLoop::getBody()
{
  return this->body;
}

// Create loop as loop(IV, START, CONDITION, STRIDE) { BODY }.  
PcpLoop::PcpLoop(PcpIv* iv, PcpExpr* start, PcpBoolExpr* condition,
		 PcpConstant* stride, PcpStmt* body)
{
  initialize();
  this->setIv(iv);
  this->setStart(start);
  this->setCondition(condition);
  this->setStride(stride);
  this->setBody(body);
}

void
PcpLoop::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// PCP Scop 

// Return true if this is a scop.
bool PcpScop::isScop()
{
  return true;
}

// Cast this to a scop.
PcpScop* PcpScop::toScop()
{
  return this;
}

// Get numer of variables.  
int
PcpScop::getNumVariables()
{
  return this->getVariables()->getSize();
}

// Set variables to VARIABLES.  
void
PcpScop::setVariables(PcpArray<PcpVariable*>* variables)
{
  this->variables = variables;
}

// Get variables.  
PcpArray<PcpVariable*>*
PcpScop::getVariables()
{
  return this->variables;
}

// Get variable with given INDEX.  
PcpVariable* 
PcpScop::getVariable(int index)
{
  return this->getVariables()->get(index);
}

// Get number of paramters.  
int
PcpScop::getNumParameters()
{
  return this->getParameters()->getSize();
}

// Set paramters to PARAMTERS.  
void
PcpScop::setParameters(PcpArray<PcpParameter*>* parameters)
{
  this->parameters = parameters;
}

// Get parameters.  
PcpArray<PcpParameter*>*
PcpScop::getParameters()
{
  return this->parameters;
}

// Get parameter with given INDEX.  
PcpParameter* 
PcpScop::getParameter(int index)
{
  return this->getParameters()->get(index);
}

// Set body to BODY.  
void
PcpScop::setBody(PcpStmt* body)
{
  this->body = body;
}

// Get body.  
PcpStmt* 
PcpScop::getBody()
{
  return this->body;
}

PcpIterator<PcpVariable*>*
PcpScop::getVariablesIterator()
{
  return this->getVariables()->getIterator();
}

PcpIterator<PcpParameter*>*
PcpScop::getParametersIterator()
{
  return this->getParameters()->getIterator();
}


// Create new scop given VARIABLES, PARAMETERS and BODY.
PcpScop::PcpScop(PcpArray<PcpVariable*>* variables,
		 PcpArray<PcpParameter*>* parameters,
		 PcpStmt* body)
{
  initialize();
  this->setVariables(variables);
  this->setParameters(parameters);
  this->setBody(body);
}

void
PcpScop::accept(PcpVisitor* visitor)
{
  visitor->visit(this);
}

// Set varaibles to VARAIBLES.  
void
PcpScopBuilder::setVariables(PcpDynamicArray<PcpVariable*>* variables)
{
  this->variables = variables;
}

// Get variables.  
PcpDynamicArray<PcpVariable*>*
PcpScopBuilder::getVariables()
{
  return this->variables;
}

// Set parameters to PARAMETERS.  
void
PcpScopBuilder::setParameters(PcpDynamicArray<PcpParameter*>* parameters)
{
  this->parameters = parameters;
}

// Get parameters.  
PcpDynamicArray<PcpParameter*>*
PcpScopBuilder::getParameters()
{
  return this->parameters;
}

// Return true if VARIABLE already exists in the variables array.
bool
PcpScopBuilder::containsVariable(PcpVariable* variable)
{
  PcpDynamicArray<PcpVariable*>* variables = this->getVariables();
  PcpIterator<PcpVariable*>* iter;
  bool result = false;
  for(iter = variables->getIterator(); iter->hasNext(); iter->next())
    {
      if(variable == iter->get())
	result = true;
    }
  delete iter;
  return result;
}

// Add VARIABLE.  
void
PcpScopBuilder::addVariable(PcpVariable* variable)
{
  if(!this->containsVariable(variable))
    this->getVariables()->add(variable);
}

// Add PARAMETER.  
void
PcpScopBuilder::addParameter(PcpParameter* parameter)
{
  this->getParameters()->add(parameter);
}

// Set body to BODY.  
void
PcpScopBuilder::setBody(PcpStmt* body)
{
  this->body = body;
}

// Get body.  
PcpStmt* 
PcpScopBuilder::getBody()
{
  return this->body;
}

// Create new scop builder.  
PcpScopBuilder::PcpScopBuilder()
{
  this->setVariables(new PcpDynamicArray<PcpVariable*>(5));
  this->setParameters(new PcpDynamicArray<PcpParameter*>(5));
  this->setBody(NULL);
}

// Create new scop.  
PcpScop* 
PcpScopBuilder::createScop()
{
  PcpScop* scop = new PcpScop(this->getVariables(),
			      this->getParameters(),
			      this->getBody());
  return scop;
}


