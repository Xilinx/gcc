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

#ifndef _PCP_PARSER_H_
#define _PCP_PARSER_H_

#include "pcp.h"

class PcpAst;
class PcpParserContext;
class PcpSymtab;
class PcpToken;

class PcpParser
{
 protected:
  PcpSymtab* symtab;
  bool addLineAnnots;
  const char* fileName;

  void setSymtab(PcpSymtab* symtab);
  
  // Get symbol table in CONTEXT.  
  PcpSymtab* getSymtab();
  
  // Set parser CONTEXT to add annotations to ADDLineAnnots.   
  void setAddLineAnnots(bool addLineAnnots);
  
  // Return if parser CONTEXT should insert line annotations.  
  bool getAddLineAnnots();

  
  // Set file name in CONTEXT to FILEName.  
  void setFileName(const char* fileName);
  
  // Get file name in CONTEXT.  
  const char* getFileName();
  
  void parseAnnots(PcpObject* object, PcpAst* objectAst);

  bool pcpTokenIsBoolArithOperator(PcpToken* operatorToken);
  PcpConstant* parseNumeral(PcpAst* numeral);
  void addLineAnnot(PcpObject* object, PcpAst* ast);
  void parseCommonObjectAttributes(PcpAst* ast, PcpObject* object);
  PcpArith* parseMultiply(PcpAst* multiply);
  PcpExpr* parseScalarIdentifier(PcpAst* ast);
  PcpArithOperator parseArithOperator(PcpToken* operatorToken);
  bool pcpTokenIsOperator(PcpToken* operatorToken);
  PcpExpr* parseArith(PcpAst* ast);
  PcpExpr* parseLinearExpr(PcpAst* ast);
  PcpBoolExpr* parseComparison(PcpAst* ast);
  PcpBoolArithOperator parseBoolArithOperator(PcpToken* operatorToken);
  PcpBoolExpr* parseBoolArith(PcpAst* ast);
  PcpBoolExpr* parseBoolExpr(PcpAst* ast);
  PcpObject* parseIdentifier(PcpAst* ast);
  PcpArrayAccess* parseArrayAccess(PcpAst* ast);
  bool parseTokenIsUnknownSymbol(PcpToken* token);
  PcpUserStmt* parseUserStmt(PcpAst* userStmt);
  PcpCopy* parseCopy(PcpAst* copy);
  PcpSequence* parseSequence(PcpAst* sequence);
  PcpGuard* parseGuard(PcpAst* guard);
  PcpIv* parseIv(PcpAst* iv);
  PcpLoop* parseLoop(PcpAst* loop);
  PcpStmt* parseStmt(PcpAst* stmt);
  void parseScopInoutput(PcpAst* inoutput, PcpScopBuilder* builder);
  void parseScopParameters(PcpAst* parameters, PcpScopBuilder* builder);
  PcpScop* parseScop(PcpAst* scop);
  PcpArrayType* parseArrayType(PcpAst* arrayType);
  PcpVariable* parseVariable(PcpAst* array);
  PcpAnnotTerm*pcpParseAnnotTerm(PcpAst* termAst);
  PcpAnnot* pcpParseAnnot(PcpAst* annotAst);
  PcpParameter* parseParameter(PcpAst* parameter);
    
 public:
  PcpScop* pcpAstParseTopLevel(PcpAst* program);
  PcpParser();
  static PcpScop* parse(const char* source);
  static PcpScop* parseFile(const char* filename);
};

#endif // _PCP_PARSER_H_ 
