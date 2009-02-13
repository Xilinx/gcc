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
#include "pcp_emitter.h"

// PCP Emit Context 

// Set string buffer of CONTEXT to STRINGBuffer.  
void
PcpEmitter::setStringBuffer(PcpStringBuffer* stringBuffer)
{
  this->stringBuffer = stringBuffer;
}

// Get string buffer of CONTEXT.  
PcpStringBuffer* 
PcpEmitter::getStringBuffer()
{
  return this->stringBuffer;
}

void
PcpEmitter::setTagFilterSet(PcpSet<const char*>* tagFilterSet)
{
  this->tagFilterSet = tagFilterSet;
}

PcpSet<const char*>*
PcpEmitter::getTagFilterSet()
{
  return this->tagFilterSet;
}

bool
PcpEmitter::tagIsFiltered(const char* tag)
{
  PcpSet<const char*>* tagSet = this->getTagFilterSet();
  PcpIterator<const char*>* iter = tagSet->getIterator();

  for(;iter->hasNext();iter->next())
    {
      const char* memberTag = iter->get();
      if(strcmp(memberTag, tag) == 0)
	return true;
    }
  delete iter;
  return false;
}
// Set indent level of CONTEXT to INDENT.  
void
PcpEmitter::setIndent(int indent)
{
  this->indent = indent;
}

// Get indent level of CONTEXT.  
int
PcpEmitter::getIndent()
{
  return this->indent;
}

// Increment indent level of CONTEXT.  
void
PcpEmitter::increment()
{
  this->setIndent(this->getIndent() + 1);
}

// Decrement indent level of CONTEXT.  
void
PcpEmitter::decrement()
{
  this->setIndent(this->getIndent() - 1);
}

// Write indent spaces.  
void
PcpEmitter::writeIndent()
{
  int i;
  int indent = this->getIndent();
  for(i = 0; i < indent; i++)
    this->getStringBuffer()->append("  ");
}

// Write newline.  
void
PcpEmitter::writeNewline()
{
  this->getStringBuffer()->newline();
}

// Write STRING.  
void
PcpEmitter::writeString(const char* string)
{
  this->getStringBuffer()->append(string);
}

// Write indented STRING.  
void
PcpEmitter::writeStringIndent(const char* string)
{
  this->writeIndent();
  this->getStringBuffer()->append(string);
}

// Write VALUE.  
void
PcpEmitter::writeInt(int value)
{
  this->getStringBuffer()->appendInt(value);
}

// Convert CONTEXT to string.  
const char* 
PcpEmitter::stringBufferToString()
{
  return
    this->getStringBuffer()->toString();
}


// Write OBJECT annots.  
void
PcpEmitter::emitObjectAnnots(PcpObject* object)
{
  bool first = true;
  PcpAnnotSet* annots = object->getAnnots();
  int numAnnots;
  int i;

  if(annots == NULL)
    return;

  numAnnots = annots->getNumAnnots();


  for(i = 0; i < numAnnots; i++)
    {
      PcpAnnotTerm* annotTerm = annots->getAnnot(i);


      if(this->tagIsFiltered(annotTerm->getTag()))
	continue;

      if(first)
	{
	  this->writeString(" | ");
	  first = false;
	}
      else
	this->writeString(", ");
      this->emitAnnotTerm(annotTerm);
    }
}

// Write ANNOT.  
void
PcpEmitter::emitAnnot(PcpAnnot* annot)
{
  if(annot->isAnnotInt())
    this->emitAnnotInt(annot->toAnnotInt());
  else if(annot->isAnnotString())
    this->emitAnnotString(annot->toAnnotString());
  else if(annot->isAnnotObject())
    this->emitAnnotObject(annot->toAnnotObject());
  else
    this->emitAnnotTerm(annot->toAnnotTerm());
}

void PcpEmitter::addFilteredTag(const char* tag)
{
  this->getTagFilterSet()->insert(tag);
}


const char* PcpEmitter::bufferToString()
{
  return this->stringBufferToString();
}

// Write ANNOTInt.  
void
PcpEmitter::emitAnnotInt(PcpAnnotInt* annotInt)
{
  this->writeInt(annotInt->getValue());
}

// Write ANNOTString.  
void
PcpEmitter::emitAnnotString(PcpAnnotString* annotString)
{
  this->writeString(annotString->getString());
}

// Write ANNOTObject.  
void
PcpEmitter::emitAnnotObject(PcpAnnotObject* annotObject)
{
  PcpObject* object = annotObject->getObject();
  pcpAssert(object->getName() != NULL);
  this->writeString(object->getName());
}

// Write ANNOTTerm.  
void
PcpEmitter::emitAnnotTerm(PcpAnnotTerm* annotTerm)
{
  int numArguments = annotTerm->getNumArguments();
  int i;
  bool first = true;

  this->writeString(annotTerm->getTag());
  this->writeString("(");

  for(i = 0; i < numArguments; i++)
    {
      if(first)
	first = false;
      else
	this->writeString(", ");
      this->emitAnnot(annotTerm->getArgument(i));
    }

  this->writeString( ")");
}

// Write TYPE.  
void
PcpEmitter::emitArrayTypeDef(PcpArrayType* type)
{
  bool first = true;
  int numDims = type->getNumDimensions();
  int i;

  this->writeString("array(");
  for(i = 0; i < numDims; i++)
    {
      if(first)
	first = false;
      else
	this->writeString(", ");
      this->emitExpr(type->getDimension(i));
    }
  this->emitObjectAnnots(type);
  this->writeString(")");
}

// Write TYPE.  
void
PcpEmitter::emitArrayTypeUse(PcpArrayType* type)
{
  const char* name = type->getName();
  if(name != NULL)
    this->writeString(name);
  else
    this->emitArrayTypeDef(type);
}

// Write EXPR.  
void
PcpEmitter::emitExpr(PcpExpr* expr)
{
  if(expr->isParameter())
    this->emitParameterUse(expr->toParameter());
  else if(expr->isConstant())
    this->emitConstantUse(expr->toConstant());
  else if(expr->isIv())
    this->emitIvUse(expr->toIv());
  else if(expr->isArith())
    this->emitArith(expr->toArith());
  else
    pcpAssert(false);
}

const char* 
PcpEmitter::emitGetBoolOperatorString(PcpBoolArithOperator oper)
{
  return((oper.isBoolAnd()) ? "and"
	 :(oper.isBoolOr()) ? "or"
	 : "unknown");
}

void
PcpEmitter::emitObjectNameBinding(PcpObject* object)
{
  const char* name = object->getName();

  if(name != NULL)
    {
      this->writeString(name);
      this->writeString(" <- ");
    }
}

void
PcpEmitter::emitBoolArith(PcpBoolArith* boolArith)
{
  const char* operatorString = 
    this->emitGetBoolOperatorString(boolArith->getOperator());
  int numOperands = boolArith->getNumOperands();
  bool first = true;
  int i;

  this->emitObjectNameBinding(boolArith);
  this->writeString(operatorString);
  this->writeString("(");
  for(i = 0; i < numOperands; i++)
    {
      if(first)
	first = false;
      else 
	this->writeString(", ");
      this->emitBoolExpr(boolArith->getOperand(i));
    }
  this->writeString(")");
}

// Write BOOLExpr.  
void
PcpEmitter::emitBoolExpr(PcpBoolExpr* boolExpr)
{
  if(boolExpr->isCompare())
    this->emitCompare(boolExpr->toCompare());
  else if(boolExpr->isBoolArith())
    this->emitBoolArith(boolExpr->toBoolArith());
  else
    pcpAssert(false);
}

void
PcpEmitter::emitCompareOperator(PcpCompareOperator oper)
{
  const char* operatorString = NULL;
  if(oper.isEqual())
    operatorString = "eq";
  else if(oper.isGreaterEqual())
    operatorString = "ge";
  else
    pcpAssert(false);
  
  this->writeString(operatorString);
}

// Write COMPARE 
void
PcpEmitter::emitCompare(PcpCompare* compare)
{
  this->emitCompareOperator(compare->getOperator());
  this->writeString("(");
  this->emitExpr(compare->getLhs());
  this->writeString(", ");
  this->emitExpr(compare->getRhs());
  this->writeString(")");
}

void
PcpEmitter::emitObjectNameBindingIndent(PcpObject* object)
{
  this->writeIndent();
  this->emitObjectNameBinding(object);
}

// Write CONSTANT declaration.  
void
PcpEmitter::emitConstantDecl(PcpConstant* constant)
{
  const char* name = constant->getName();
  if(name != NULL)
    {
      this->writeString(name);
      this->writeString(" <- constant(");
      this->writeInt(constant->getValue());
      this->emitObjectAnnots(constant);
      this->writeString(")");
    }
}

// Write CONSTANT use string.  
void
PcpEmitter::emitConstantUse(PcpConstant* constant)
{
  const char* name = constant->getName();
  if(name != NULL)
    this->writeString(name);
  else
    this->writeInt(constant->getValue());
}

// Write IV declaration.  
void
PcpEmitter::emitIvDecl(PcpIv* iv)
{
  this->writeString(iv->getName());
  this->writeString(" <- iv(");
  this->emitObjectAnnots(iv);
  this->writeString(")");
}

// Write IV use.  
void
PcpEmitter::emitIvUse(PcpIv* iv)
{
  this->writeString(iv->getName());
}

// Write VAR declaration.  
void
PcpEmitter::emitVariableDecl(PcpVariable* var)
{
  this->writeString(var->getName());
  this->writeString(" <- variable(");
  this->emitArrayTypeUse(var->getType());
  this->emitObjectAnnots(var);
  this->writeString(")");
}

// Write VAR use.  
void
PcpEmitter::emitVariableUse(PcpVariable* var)
{
  this->writeString(var->getName());
}

// Write PARAMETER declaration.  
void
PcpEmitter::emitParameterDecl(PcpParameter* parameter)
{
  this->writeString(parameter->getName());
  this->writeString(" <- parameter(");
  this->emitObjectAnnots(parameter);
  this->writeString(")");
}


// Write PARAMETER use.  
void
PcpEmitter::emitParameterUse(PcpParameter* parameter)
{
  this->writeString(parameter->getName());
}


// Write ARITH.  
const char* 
PcpEmitter::emitGetOperatorString(PcpArithOperator oper)
{
  const char* result = NULL;
  if(oper.isAdd())  result = "+";
  else if(oper.isMultiply()) result = "*";
  else if(oper.isMin())      result = "min";
  else if(oper.isMax())      result = "max";
  else if(oper.isSubtract()) result = "-";
  else if(oper.isFloor())    result = "floor";
  else if(oper.isCeiling())  result = "ceil";
  else if(oper.isUnknown())  result = "UNKNOWN";
  else pcpAssert(false);

  return result;
}

void
PcpEmitter::emitArith(PcpArith* arith)
{
  const char* operatorString = 
    this->emitGetOperatorString(arith->getOperator());
  int numOperands = arith->getNumOperands();
  bool first = true;
  int i;

  this->emitObjectNameBinding(arith);
  this->writeString(operatorString);
  this->writeString("(");
  for(i = 0; i < numOperands; i++)
    {
      if(first)
	first = false;
      else 
	this->writeString(", ");
      this->emitExpr(arith->getOperand(i));
    }
  this->writeString(")");
}



// Write ACCESS.  
void
PcpEmitter::emitArrayAccess(PcpArrayAccess* access)
{
  int i;
  int numSubscripts = access->getNumSubscripts();
  const char* oper = access->isUse() ? "use" :
    access->isDef() ? "def" :
    access->isMaydef() ? "maydef" : "undefinedAccess";

  this->emitObjectNameBinding(access);

  this->writeString(oper);
  this->writeString("(");
  this->emitVariableUse(access->getBase());

  for(i = 0; i < numSubscripts; i++)
    {
      this->writeString(", ");
      this->emitExpr(access->getSubscript(i));
    }
  this->emitObjectAnnots(access);
  this->writeString(")");
}

// Write COPY.  
void
PcpEmitter::emitCopy(PcpCopy* copy)
{
  this->emitObjectNameBindingIndent(copy);
  this->writeString("copy(");
  this->emitArrayAccess(copy->getDest());
  this->writeString(", ");
  this->emitArrayAccess(copy->getSrc());
  this->emitObjectAnnots(copy);
  this->writeString(")");
}

// Write USERSTMT.  
void
PcpEmitter::emitUserStmt(PcpUserStmt* userStmt)
{
  bool first = true;
  int i;
  int numAccesses = userStmt->getNumAccesses();
  PcpArrayAccess* access;

  this->writeStringIndent(userStmt->getName());
  this->writeString("(");

  for(i = 0; i < numAccesses; i++)
    {
      if(first)
	first = false;
      else
	this->writeString(", ");

      access = userStmt->getArrayAccess(i);
      this->emitArrayAccess(access);
    }
  this->emitObjectAnnots(userStmt);
  this->writeString(")");
}

// Write SEQUENCE.  
void
PcpEmitter::emitSequence(PcpSequence* sequence)
{
  int i;
  int numStmts = sequence->getNumStmts();
  bool first = true;
  for(i = 0; i < numStmts; i++)
    {
      if(first)
	first = false;
      else
	this->writeString("\n");
      this->emitStmt(sequence->getStmt(i));
    }
}

// Write STMT.  
void
PcpEmitter::emitStmt(PcpStmt* stmt)
{
  if(stmt->isCopy())
    this->emitCopy(stmt->toCopy());
  else if(stmt->isUserStmt())
    this->emitUserStmt(stmt->toUserStmt());
  else if(stmt->isSequence())
    this->emitSequence(stmt->toSequence());
  else if(stmt->isGuard())
    this->emitGuard(stmt->toGuard());
  else if(stmt->isLoop())
    this->emitLoop(stmt->toLoop());
  else
    pcpAssert(false);
}

// Write GUARD.  
void
PcpEmitter::emitGuard(PcpGuard* guard)
{
  this->emitObjectNameBindingIndent(guard);
  this->writeString("guard(");
  this->emitBoolExpr(guard->getCondition());
  this->emitObjectAnnots(guard);
  this->writeString(")");
  this->writeNewline();
  this->writeStringIndent("{");
  this->writeNewline();

  this->increment();
  this->emitStmt(guard->getBody());
  this->decrement();

  this->writeNewline();
  this->writeStringIndent("}");
}

// Write LOOP.  
void
PcpEmitter::emitLoop(PcpLoop* loop)
{
  this->emitObjectNameBindingIndent(loop);
  this->writeString("loop(");
  this->emitIvDecl(loop->getIv());
  this->writeString(", ");
  this->emitExpr(loop->getStart());
  this->writeString(", ");
  this->emitBoolExpr(loop->getCondition());
  this->writeString(", ");
  this->emitConstantUse(loop->getStride());
  this->emitObjectAnnots(loop);
  this->writeString(")");
  this->writeNewline();
  this->writeStringIndent("{");
  this->writeNewline();

  this->increment();
  this->emitStmt(loop->getBody());
  this->decrement();

  this->writeNewline();
  this->writeStringIndent("}");
}

// Write SCOP inputs.  
void
PcpEmitter::emitScopInputs(PcpScop* scop)
{
  int numVariables = scop->getNumVariables();
  bool first = true;
  int i;

  this->writeString("inputs(");
  for(i = 0; i < numVariables; i++)
    {
      PcpVariable* var = scop->getVariable(i);

      if(var->getIsInput())
	{
	  if(first)
	    first = false;
	  else
	    this->writeString(", ");

	  this->emitVariableUse(var);
	}
    }
  this->writeString(")");
}

// Write SCOP outputs into context.  
void
PcpEmitter::emitScopOutputs(PcpScop* scop)
{
  int numVariables = scop->getNumVariables();
  bool first = true;
  int i;

  this->writeString("outputs(");

  for(i = 0; i < numVariables; i++)
    {
      PcpVariable* var = scop->getVariable(i);

      if(var->getIsOutput())
	{
	  if(first)
	    first = false;
	  else
	    this->writeString(", ");


	  this->emitVariableUse(var);
	}
    }

  this->writeString(")");

}

// Write SCOP parameters.  
void
PcpEmitter::emitScopParameters(PcpScop* scop)
{
  int i;
  int numParameters = scop->getNumParameters();
  bool first = true;

  this->writeString("parameters(");

  // Write parameters.  
  for(i = 0; i < numParameters; i++)
    {
      if(first)
	first = false;
      else
	this->writeString(", ");

      this->emitParameterUse(scop->getParameter(i));
    }

  this->writeString(")");

}

// Write SCOP parameter declarations.  
void
PcpEmitter::emitScopParametersDecl(PcpScop* scop)
{
  int i;
  int numParameters = scop->getNumParameters();

  // Write parameter declarations.  
  for(i = 0; i < numParameters; i++)
    {
      this->emitParameterDecl(scop->getParameter(i));
      this->writeNewline();
    }
}

// Write SCOP variable declarations.  
void
PcpEmitter::emitScopVariables(PcpScop* scop)
{
  int i;
  int numVariables = scop->getNumVariables();

  // Write variable declarations.  
  for(i = 0; i < numVariables; i++)
    {
      this->emitVariableDecl(scop->getVariable(i));
      this->writeNewline();
    }
}

void
PcpEmitter::emitTypeDefs(PcpScop* scop)
{
  int i;
  int numVariables = scop->getNumVariables();
  PcpSet<PcpArrayType*>* emittedTypes = new PcpSet<PcpArrayType*>();

  for(i = 0; i < numVariables; i++)
    {
      PcpVariable* variable = scop->getVariable(i);
      PcpArrayType* type = variable->getType();
      if(!emittedTypes->contains(type) &&
	 type->getName() != NULL)
	{
	  emittedTypes->insert(type);
	  this->emitObjectNameBindingIndent(type);
	  this->emitArrayTypeDef(type);
	  this->writeNewline();
	}
    }
}

// Write SCOP.  
void
PcpEmitter::emitScop(PcpScop* scop)
{
  this->emitScopParametersDecl(scop);
  this->emitTypeDefs(scop);
  this->emitScopVariables(scop);
  this->emitObjectNameBindingIndent(scop);
  this->writeStringIndent("scop(");
  this->emitScopInputs(scop);
  this->writeString(", ");
  this->emitScopOutputs(scop);
  this->writeString(", ");
  this->emitScopParameters(scop);
  this->emitObjectAnnots(scop);
  this->writeString(")");
  this->writeNewline();
  this->writeStringIndent("{");

  this->increment();
  this->writeNewline();
  this->emitStmt(scop->getBody());
  this->writeNewline();
  this->decrement();

  this->writeStringIndent("}");
  this->writeNewline();
}

// Convert ANNOT to string.  

const char* 
PcpEmitter::pcpAnnotToString(PcpAnnot* annot)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitAnnot(annot);
  return emitter->bufferToString();
}

// Convert ANNOTInt to string.  
const char* 
PcpEmitter::pcpAnnotIntToString(PcpAnnotInt* annotInt)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitAnnotInt(annotInt);
  return emitter->bufferToString();
}

// Convert ANNOTString to string.  
const char* 
PcpEmitter::pcpAnnotStringToString(PcpAnnotString* annotString)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitAnnotString(annotString);
  return emitter->bufferToString();
}

// Convert ANNOTObject to string.  
const char* 
PcpEmitter::pcpAnnotObjectToString(PcpAnnotObject* annotObject)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitAnnotObject(annotObject);
  return emitter->bufferToString();
}

// Convert ANNOTTerm to string.  
const char* 
PcpEmitter::pcpAnnotTermToString(PcpAnnotTerm* annotTerm)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitAnnotTerm(annotTerm);
  return emitter->bufferToString();
}

// Convert TYPE to string.  
const char* 
PcpEmitter::pcpArrayTypeToString(PcpArrayType* type)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitArrayTypeDef(type);
  return emitter->bufferToString();
}

// Convert EXPR to string.  
const char* 
PcpEmitter::pcpExprToString(PcpExpr* expr)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitExpr(expr);
  return emitter->bufferToString();
}

// Convert BOOLEXPR to string.  
const char* 
PcpEmitter::pcpBoolExprToString(PcpBoolExpr* boolExpr)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitBoolExpr(boolExpr);
  return emitter->bufferToString();
}

const char* 
PcpEmitter::pcpCompareToString(PcpCompare* compare)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitCompare(compare);
  return emitter->bufferToString();
}


const char* 
PcpEmitter::pcpBoolArithToString(PcpBoolArith* boolArith)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitBoolArith(boolArith);
  return emitter->bufferToString();
}

// Convert CONSTANT to declaration string.  
const char* 
PcpEmitter::pcpConstantToDeclString(PcpConstant* constant)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitConstantDecl(constant);
  return emitter->bufferToString();
}

// Convert CONSTANT to use string.  
const char* 
PcpEmitter::pcpConstantToUseString(PcpConstant* constant)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitConstantUse(constant);
  return emitter->bufferToString();
}

// Convert IV to declaration string. 
const char* 
PcpEmitter::pcpIvToDeclString(PcpIv* iv)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitIvDecl(iv);
  return emitter->bufferToString();
}

// Convert IV to use string.  
const char* 
PcpEmitter::pcpIvToUseString(PcpIv* iv)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitIvUse(iv);
  return emitter->bufferToString();
}

// Convert VAR to declaration string.  
const char* 
PcpEmitter::pcpVariableToDeclString(PcpVariable* var)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitVariableDecl(var);
  return emitter->bufferToString();
}

// Convert VAR to use string.  
const char* 
PcpEmitter::pcpVariableToUseString(PcpVariable* var)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitVariableUse(var);
  return emitter->bufferToString();
}

// Convert PARAMETER to declaration string.  
const char* 
PcpEmitter::pcpParameterToDeclString(PcpParameter* parameter)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitParameterDecl(parameter);
  return emitter->bufferToString();
}

// Convert PARAMETER to use string.  
const char* 
PcpEmitter::pcpParameterToUseString(PcpParameter* parameter)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitParameterUse(parameter);
  return emitter->bufferToString();
}

const char* 
PcpEmitter::pcpArithToString(PcpArith* arith)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitArith(arith);
  return emitter->bufferToString();
}


// Convert ACCESS to string.  
const char* 
PcpEmitter::pcpArrayAccessToString(PcpArrayAccess* access)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitArrayAccess(access);
  return emitter->bufferToString();
}

// Convert COPY to string.  
const char* 
PcpEmitter::pcpCopyToString(PcpCopy* copy)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitCopy(copy);
  return emitter->bufferToString();
}

// Convert USERStmt to string.  
const char* 
PcpEmitter::pcpUserStmtToString(PcpUserStmt* userStmt)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitUserStmt(userStmt);
  return emitter->bufferToString();
}

// Convert SEQUENCE to string.  
const char* 
PcpEmitter::pcpSequenceToString(PcpSequence* sequence)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitSequence(sequence);
  return emitter->bufferToString();

}

// Convert STMT to string.  
const char* 
PcpEmitter::pcpStmtToString(PcpStmt* stmt)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitStmt(stmt);
  return emitter->bufferToString();
}

// Convert GUARD to string.  
const char* 
PcpEmitter::pcpGuardToString(PcpGuard* guard)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitGuard(guard);
  return emitter->bufferToString();
}

// Convert LOOP to string.  
const char* 
PcpEmitter::pcpLoopToString(PcpLoop* loop)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->emitLoop(loop);
  return emitter->bufferToString();
}

// Convert SCOP into string.  
const char* 
PcpEmitter::pcpScopToString(PcpScop* scop)
{
  PcpEmitter* emitter = new PcpEmitter();
  emitter->getTagFilterSet()->insert("lineinfo");
  emitter->emitScop(scop);
  return emitter->bufferToString();
}

PcpEmitter::PcpEmitter()
{
  this->setIndent(0);
  this->setStringBuffer(new PcpStringBuffer());
  this->setTagFilterSet(new PcpSet<const char*>());
}
