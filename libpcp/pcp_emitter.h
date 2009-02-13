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

#ifndef _PCP_EMITTER_H_
#define _PCP_EMITTER_H_

#include "pcp.h"
#include "pcp_set.h"

// PCP Emit Buffer 

class PcpEmitter
{
 protected:
  PcpStringBuffer* stringBuffer;
  int indent;
  int column;
  PcpSet<const char*>* tagFilterSet;

  virtual void setStringBuffer(PcpStringBuffer* stringBuffer);
  virtual PcpStringBuffer* getStringBuffer();

  virtual bool tagIsFiltered(const char* tag);
  virtual void setTagFilterSet(PcpSet<const char*>* tagFilterSet);
  virtual PcpSet<const char*>* getTagFilterSet();
  virtual void setIndent(int indent);
  virtual int getIndent();
  virtual void increment();
  virtual void decrement();
  virtual void writeIndent();
  virtual void writeNewline();
  virtual void writeString(const char* string);
  virtual void writeInt(int value);
  virtual void writeStringIndent(const char* string);
  
  virtual const char* stringBufferToString();

 public:
  virtual void addFilteredTag(const char* tag);

  virtual void emitObjectAnnots(PcpObject* object);
  virtual void emitArrayTypeDef(PcpArrayType* type);
  virtual void emitArrayTypeUse(PcpArrayType* type);
  virtual const char* emitGetBoolOperatorString(PcpBoolArithOperator oper);
  virtual void emitObjectNameBinding(PcpObject* object);
  virtual void emitBoolArith(PcpBoolArith* boolArith);
  virtual void emitCompareOperator(PcpCompareOperator oper);
  virtual void emitObjectNameBindingIndent(PcpObject* object);
  virtual void emitConstantDecl(PcpConstant* constant);
  virtual void emitIvDecl(PcpIv* iv);
  virtual void emitVariableDecl(PcpVariable* var);
  virtual void emitVariableUse(PcpVariable* var);
  virtual void emitParameterDecl(PcpParameter* parameter);
  virtual const char* emitGetOperatorString(PcpArithOperator oper);
  virtual void emitCopy(PcpCopy* copy);
  virtual void emitUserStmt(PcpUserStmt* userStmt);
  virtual void emitScopInputs(PcpScop* scop);
  virtual void emitScopOutputs(PcpScop* scop);
  virtual void emitScopParameters(PcpScop* scop);
  virtual void emitScopParametersDecl(PcpScop* scop);
  virtual void emitScopVariables(PcpScop* scop);
  virtual void emitTypeDefs(PcpScop* scop);
  virtual void emitAnnot(PcpAnnot* annot);
  virtual void emitAnnotInt(PcpAnnotInt* annotInt);
  virtual void emitAnnotString(PcpAnnotString* annotString);
  virtual void emitAnnotObject(PcpAnnotObject* annotObject);
  virtual void emitAnnotTerm(PcpAnnotTerm* annotTerm);
  virtual void emitConstantUse(PcpConstant* constant);
  virtual void emitIvUse(PcpIv* iv);
  virtual void emitParameterUse(PcpParameter* parameter);
  virtual void emitExpr(PcpExpr* expr);
  virtual void emitArith(PcpArith* arith);
  virtual void emitBoolExpr(PcpBoolExpr* boolExpr);
  virtual void emitCompare(PcpCompare* compare);
  virtual void emitArrayAccess(PcpArrayAccess* access);
  virtual void emitStmt(PcpStmt* stmt);
  virtual void emitSequence(PcpSequence* sequence);
  virtual void emitGuard(PcpGuard* guard);
  virtual void emitLoop(PcpLoop* loop);
  virtual void emitScop(PcpScop* scop);
  
  virtual const char* bufferToString();

  static const char* pcpAnnotToString(PcpAnnot* annot);
  static const char* pcpAnnotIntToString(PcpAnnotInt* annotInt);
  static const char* pcpAnnotStringToString(PcpAnnotString* annotString);
  static const char* pcpAnnotObjectToString(PcpAnnotObject* annotObject);
  static const char* pcpAnnotTermToString(PcpAnnotTerm* annotTerm);
  static const char* pcpArrayTypeToString(PcpArrayType* type);
  static const char* pcpExprToString(PcpExpr* expr);
  static const char* pcpArithToString(PcpArith* arith);
  static const char* pcpBoolExprToString(PcpBoolExpr* boolExpr);
  static const char* pcpCompareToString(PcpCompare* compare);
  static const char* pcpBoolArithToString(PcpBoolArith* boolArith);
  static const char* pcpArrayAccessToString(PcpArrayAccess* access);
  static const char* pcpIvToUseString(PcpIv* iv);
  static const char* pcpIvToDeclString(PcpIv* iv);
  static const char* pcpParameterToUseString(PcpParameter* parameter);
  static const char* pcpParameterToDeclString(PcpParameter* parameter);
  static const char* pcpConstantToUseString(PcpConstant* constant);
  static const char* pcpConstantToDeclString(PcpConstant* constant);
  static const char* pcpVariableToUseString(PcpVariable* var);
  static const char* pcpVariableToDeclString(PcpVariable* var);
  static const char* pcpStmtToString(PcpStmt* stmt);
  static const char* pcpCopyToString(PcpCopy* copy);
  static const char* pcpUserStmtToString(PcpUserStmt* userStmt);
  static const char* pcpSequenceToString(PcpSequence* sequence);
  static const char* pcpGuardToString(PcpGuard* guard);
  static const char* pcpLoopToString(PcpLoop* loop);
  static const char* pcpScopToString(PcpScop* scop);

  PcpEmitter();
};


#endif // _PCP_EMITTER_H_ <
	
