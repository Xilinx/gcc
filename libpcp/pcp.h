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

#ifndef _PCP_H_
#define _PCP_H_

#include "pcp_string_buffer.h"
#include "pcp_dynamic_array.h"
// PCP Object 

class PcpVisitor;

class PcpObject
{
 protected:
  const char* name;
  class PcpAnnotSet* annots;
  
  virtual void initialize();
 public:
  virtual const char* getName();
  virtual void setName(const char* aName);

  virtual PcpAnnotSet* getAnnots();
  virtual void setAnnots(PcpAnnotSet* annots);
  virtual int getNumAnnots();
  virtual class PcpAnnotTerm* getAnnot(int index);
  virtual class PcpAnnotTerm* getAnnotWithTag(const char* tag);

  virtual bool containsAnnotWithTag(const char* tag);
  virtual void addAnnot(class PcpAnnotTerm* annot);

  virtual bool isArrayType();
  virtual bool isExpr();
  virtual bool isBoolExpr();
  virtual bool isVariable();
  virtual bool isArrayAccess();
  virtual bool isStmt();
  virtual bool isScop();

  virtual bool isIv();
  virtual bool isParameter();

  virtual class PcpArrayType* toArrayType();
  virtual class PcpExpr* toExpr();
  virtual class PcpIv* toIv();
  virtual class PcpBoolExpr* toBoolExpr();
  virtual class PcpVariable* toVariable();
  virtual class PcpArrayAccess* toArrayAccess();
  virtual class PcpStmt* toStmt();
  virtual class PcpScop* toScop();
  virtual class PcpParameter* toParameter();

  virtual void accept(PcpVisitor* visitor) = 0;

};

class PcpAnnot
{
 public:
  virtual bool isAnnotInt();
  virtual bool isAnnotString();
  virtual bool isAnnotObject();
  virtual bool isAnnotTerm();

  virtual class PcpAnnotTerm* toAnnotTerm();
  virtual class PcpAnnotInt* toAnnotInt();
  virtual class PcpAnnotObject* toAnnotObject();
  virtual class PcpAnnotString* toAnnotString();

  virtual void accept(PcpVisitor* visitor) = 0;
};

// PCP Annot Set

class PcpAnnotSet
{
  PcpDynamicArray<PcpAnnotTerm*>* annots;

 private:
  virtual void setAnnots(PcpDynamicArray<PcpAnnotTerm*>* annots);
  virtual PcpDynamicArray<PcpAnnotTerm*>* getAnnots();

 public:
  virtual int getNumAnnots();
  virtual PcpAnnotTerm* getAnnot(int index);
  virtual PcpAnnotTerm* getAnnotWithTag(const char* tag);
  virtual void addAnnot(PcpAnnotTerm* annot);

  virtual void accept(PcpVisitor* visitor);

  PcpAnnotSet();
};

// PCP Annot Int 

class PcpAnnotInt : public PcpAnnot
{
  int value;

  virtual void setValue(int value);
 public:
  virtual bool isAnnotInt();
  virtual PcpAnnotInt* toAnnotInt();

  virtual int getValue();
  PcpAnnotInt();
  PcpAnnotInt(int value);

  virtual void accept(PcpVisitor* visitor);
};

class PcpAnnotTerm : public PcpAnnot
{
 protected:
  const char* tag;
  PcpArray<PcpAnnot*>* arguments;

  virtual void setTag(const char* tag);
  virtual void setArguments(PcpArray<PcpAnnot*>* arguments);
  virtual PcpArray<PcpAnnot*>* getArguments();
  virtual void setArgument(int index, PcpAnnot* annot);

 public:
  virtual bool isAnnotTerm();
  virtual PcpAnnotTerm* toAnnotTerm();
  virtual const char* getTag();
  virtual bool tagEquals(const char* tag);
  virtual int getNumArguments();
  virtual PcpAnnot* getArgument(int index);
  PcpAnnotTerm(const char* tag, PcpArray<PcpAnnot*>* arguments);

  virtual void accept(PcpVisitor* visitor) ;
};

// PCP Annot String 

class PcpAnnotString : public PcpAnnot
{
 protected:
  const char* string;
  void setString(const char* string);

 public:
  virtual bool isAnnotString();
  virtual PcpAnnotString* toAnnotString();
  virtual const char* getString();
  PcpAnnotString(const char* string);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Annot Object 

class PcpAnnotObject : public PcpAnnot
{
 protected:
  PcpObject* object;
  
  virtual void setObject(PcpObject* object);

 public:
  virtual bool isAnnotObject();
  virtual PcpAnnotObject* toAnnotObject();

  virtual PcpObject* getObject();
  PcpAnnotObject(PcpObject* object);

  virtual void accept(PcpVisitor* visitor);
};


// PCP Annot Term Builder 

class PcpAnnotTermBuilder
{
 protected:
  const char* tag;
  PcpDynamicArray<PcpAnnot*>* arguments;

  virtual void setArguments(PcpDynamicArray<PcpAnnot*>* arguments);
  virtual PcpDynamicArray<PcpAnnot*>* getArguments();


 public:
  virtual void setTag(const char* tag);
  virtual const char* getTag();
  virtual void addArgument(PcpAnnot* argument);
  virtual PcpAnnotTerm* createAnnot();
  PcpAnnotTermBuilder();
};

// Array Type 

class PcpArrayType : public PcpObject
{
 protected:
  class PcpArray<PcpExpr*>* dimensions;

  virtual void setDimensions(PcpArray<PcpExpr*>* dimensions);
  virtual class PcpArray<PcpExpr*>* getDimensions();

 public:
  virtual bool isArrayType();
  virtual PcpArrayType* toArrayType();
  virtual int getNumDimensions();
  virtual class PcpExpr* getDimension(int index);
  virtual PcpIterator<PcpExpr*>* getDimensionsIterator();
  PcpArrayType(PcpArray<PcpExpr*>* dimensions);
  
  virtual void accept(PcpVisitor* visitor);
};

// Array Type Builder 

class PcpArrayTypeBuilder
{
 protected:
  PcpDynamicArray<PcpExpr*>* array;

  virtual void setArray(PcpDynamicArray<PcpExpr*>* array);
  virtual PcpDynamicArray<PcpExpr*>* getArray();

 public:
  PcpArrayTypeBuilder(); 
  virtual void addDimension(class PcpExpr* dimension);
  virtual void addIntDimension(int dimension);
  virtual PcpArrayType* createType();
};


// PCP Expr 

class PcpExpr : public PcpObject
{
 public:
  virtual bool isExpr();
  virtual PcpExpr* toExpr();

  virtual bool isParameter();
  virtual bool isConstant();
  virtual bool isIv();
  virtual bool isArith();

  virtual class PcpParameter* toParameter();
  virtual class PcpArith* toArith();
  virtual class PcpConstant* toConstant();
  virtual class PcpIv* toIv();
};


// PCP Arith 

class PcpArithOperator
{
 protected:
  enum Opcode
  {
    ARITH_OP_UKNOWN,
    ARITH_OP_ADD,
    ARITH_OP_MULTIPLY,
    ARITH_OP_MIN,
    ARITH_OP_MAX,
    ARITH_OP_SUBTRACT,
    ARITH_OP_FLOOR,
    ARITH_OP_CEILING
  };

  Opcode op;

  inline void setOpcode(Opcode op)
  {
    this->op = op;
  }

  inline Opcode getOpcode()
  {
    return this->op;
  }

  inline PcpArithOperator(Opcode op)
  {
    this->setOpcode(op);
  }

 public:

  inline PcpArithOperator()
  {
    this->setOpcode(ARITH_OP_UKNOWN);
  }


  static inline PcpArithOperator unknown()
  {
    return PcpArithOperator(ARITH_OP_UKNOWN);
  }

  static inline PcpArithOperator add()
  {
    return PcpArithOperator(ARITH_OP_ADD);
  }

  static inline PcpArithOperator multiply()
  {
    return PcpArithOperator(ARITH_OP_MULTIPLY);
  }

  static inline PcpArithOperator min()
  {
    return PcpArithOperator(ARITH_OP_MIN);
  }

  static inline PcpArithOperator max()
  {
    return PcpArithOperator(ARITH_OP_MAX);
  }

  static inline PcpArithOperator subtract()
  {
    return PcpArithOperator(ARITH_OP_SUBTRACT);
  }

  static inline PcpArithOperator floor()
  {
    return PcpArithOperator(ARITH_OP_FLOOR);
  }

  static inline PcpArithOperator ceiling()
  {
    return PcpArithOperator(ARITH_OP_CEILING);
  }

  inline bool isUnknown()
  {
    return this->getOpcode() == ARITH_OP_UKNOWN;
  }

  inline bool isAdd()
  {
    return this->getOpcode() == ARITH_OP_ADD;
  }

  inline bool isMultiply()
  {
    return this->getOpcode() == ARITH_OP_MULTIPLY;
  }

  inline bool isMin()
  {
    return this->getOpcode() == ARITH_OP_MIN;
  }

  inline bool isMax()
  {
    return this->getOpcode() == ARITH_OP_MAX;
  }

  inline bool isSubtract()
  {
    return this->getOpcode() == ARITH_OP_SUBTRACT;
  }

  inline bool isFloor()
  {
    return this->getOpcode() == ARITH_OP_FLOOR;
  }

  inline bool isCeiling()
  {
    return this->getOpcode() == ARITH_OP_CEILING;
  }

};

class PcpArith : public PcpExpr
{
 protected:
  PcpArithOperator oper;
  PcpArray<PcpExpr*>* operands;

  void setOperator(PcpArithOperator oper);
  void setOperands(PcpArray<PcpExpr*>* operands);
  PcpArray<PcpExpr*>* getOperands();

 public:
  virtual bool isArith();
  virtual PcpArith* toArith();

  virtual PcpArithOperator getOperator();
  virtual int getNumOperands();
  virtual PcpExpr* getOperand(int index);

  virtual PcpIterator<PcpExpr*>* getOperandsIterator();

  PcpArith(PcpArithOperator oper, PcpArray<PcpExpr*>* operands);

  static PcpArith* pcpArithBinaryCreate(PcpArithOperator oper,
					PcpExpr* lhs,
					PcpExpr* rhs);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Arith builder.  

class PcpArithBuilder
{
 protected:
  PcpArithOperator oper;
  PcpDynamicArray<PcpExpr*>* operands;

  void setOperands(PcpDynamicArray<PcpExpr*>* operands);
  PcpDynamicArray<PcpExpr*>* getOperands();


 public:
  virtual void setOperator(PcpArithOperator oper);
  virtual PcpArithOperator getOperator();
  virtual void addOperand(PcpExpr* operand);
  virtual PcpArith* createArith();
  PcpArithBuilder();
};


// PCP Constant 

class PcpConstant : public PcpExpr
{
 protected:
  int value;

  virtual void setValue(int value);

 public:
  virtual bool isConstant();
  virtual PcpConstant* toConstant();

  virtual int getValue();
  PcpConstant(int value);

  virtual void accept(PcpVisitor* visitor);
};


// PCP Induction Variable 

class PcpIv : public PcpExpr
{
 public:
  virtual bool isIv();
  virtual PcpIv* toIv();
  PcpIv(const char* name);

  virtual void accept(PcpVisitor* visitor);
};


// PCP Parameter 

class PcpParameter : public PcpExpr
{
 public:
  virtual bool isParameter();
  virtual PcpParameter* toParameter();

  PcpParameter(const char* name);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Bool Expr 

class PcpBoolExpr : public PcpObject
{
 public:
  virtual bool isBoolExpr();
  virtual PcpBoolExpr* toBoolExpr();

  virtual bool isCompare();
  virtual bool isBoolArith();

  virtual class PcpCompare* toCompare();
  virtual class PcpBoolArith* toBoolArith();
};

// PCP Compare 

class PcpCompareOperator 
{
  enum Opcode
  {
    COMPARE_UNKNOWN,
    EQUAL,
    GREATER_EQUAL,
  };

  Opcode op;

  inline void setOpcode(Opcode op)
  {
    this->op = op;
  }

  inline Opcode getOpcode()
  {
    return this->op;
  }

  inline PcpCompareOperator(Opcode op)
  {
    setOpcode(op);
  }
 public:

  inline PcpCompareOperator()
  {
    setOpcode(COMPARE_UNKNOWN);
  }

  static inline PcpCompareOperator unknown()
  {
    return PcpCompareOperator(COMPARE_UNKNOWN);
  }

  static inline PcpCompareOperator equal()
  {
    return PcpCompareOperator(EQUAL);
  }

  static inline PcpCompareOperator greaterEqual()
  {
    return PcpCompareOperator(GREATER_EQUAL);
  }

  inline bool isUnknown()
  {
    return this->getOpcode() == COMPARE_UNKNOWN;
  }

  inline bool isEqual()
  {
    return this->getOpcode() == EQUAL;
  }

  inline bool isGreaterEqual()
  {
    return this->getOpcode() == GREATER_EQUAL;
  }

};

class PcpCompare : public PcpBoolExpr
{
 protected:
  PcpCompareOperator oper;
  PcpExpr* lhs;
  PcpExpr* rhs;

  virtual void setOperator(PcpCompareOperator oper);
  virtual void setLhs(PcpExpr* lhs);
  virtual void setRhs(PcpExpr* rhs);

 public:
  virtual bool isCompare();
  virtual PcpCompare* toCompare();

  virtual PcpCompareOperator getOperator();
  virtual PcpExpr* getLhs();
  virtual PcpExpr* getRhs();
  PcpCompare(PcpCompareOperator oper, PcpExpr* lhs, PcpExpr* rhs);

  virtual void accept(PcpVisitor* visitor);
};


// PCP Bool Arith 
class PcpBoolArithOperator
{
 protected:
  enum Opcode
  {
    BOOL_UNKNOWN,
    BOOL_AND,
    BOOL_OR
  };

  Opcode op;

  inline void setOpcode(Opcode op)
  {
    this->op = op;
  }

  inline Opcode getOpcode()
  {
    return this->op;
  }

  inline PcpBoolArithOperator(Opcode op)
  {
    setOpcode(op);
  }

 public:
  
  inline PcpBoolArithOperator()
  {
    setOpcode(BOOL_UNKNOWN);
  }
  
  static inline PcpBoolArithOperator unknown()
  {
    return PcpBoolArithOperator(BOOL_UNKNOWN);
  }

  static inline PcpBoolArithOperator boolAnd()
  {
    return PcpBoolArithOperator(BOOL_AND);
  }

  static inline PcpBoolArithOperator boolOr()
  {
    return PcpBoolArithOperator(BOOL_OR);
  }

  inline bool isUnknown()
  {
    return this->getOpcode() == BOOL_UNKNOWN;
  }

  inline bool isBoolAnd()
  {
    return this->getOpcode() == BOOL_AND;
  }

  inline bool isBoolOr()
  {
    return this->getOpcode() == BOOL_OR;
  }

};
class PcpBoolArith : public PcpBoolExpr
{
 protected:
  PcpBoolArithOperator oper;
  PcpArray<PcpBoolExpr*>* operands;

  virtual void setOperator(PcpBoolArithOperator oper);
  virtual void setOperands(PcpArray<PcpBoolExpr*>* operands);
  virtual PcpArray<PcpBoolExpr*>* getOperands();

 public:
  virtual bool isBoolArith();
  virtual PcpBoolArith* toBoolArith();
  virtual PcpBoolArithOperator getOperator();
  virtual int getNumOperands();
  virtual PcpBoolExpr* getOperand(int index);

  virtual PcpIterator<PcpBoolExpr*>* getOperandsIterator();

  PcpBoolArith(PcpBoolArithOperator oper,
	       PcpArray<PcpBoolExpr*>* operands);
  static PcpBoolArith* pcpBoolArithBinaryCreate(PcpBoolArithOperator oper,
						PcpBoolExpr* lhs,
						PcpBoolExpr* rhs);

  virtual void accept(PcpVisitor* visitor);
};


class PcpBoolArithBuilder
{
 protected:
  PcpBoolArithOperator oper;
  PcpDynamicArray<PcpBoolExpr*>* operands;

  virtual void setOperands(PcpDynamicArray<PcpBoolExpr*>* operands);

 public:
  virtual void setOperator(PcpBoolArithOperator oper);
  virtual PcpBoolArithOperator getOperator();
  virtual PcpDynamicArray<PcpBoolExpr*>* getOperands();
  PcpBoolArithBuilder();
  virtual PcpBoolArith* createBoolArith();
  virtual void addOperand(PcpBoolExpr* operand);
};


// PCP Variable 

class PcpVariable : public PcpObject
{
 protected:
  bool isInput;
  bool isOutput;
  PcpArrayType* type;

  virtual void setType(PcpArrayType* type);
 public:
  virtual bool isVariable();
  virtual PcpVariable* toVariable();

  virtual void setIsInput(bool isInput);
  virtual bool getIsInput();
  virtual void setIsOutput(bool isOutput);
  virtual bool getIsOutput();
  virtual PcpArrayType* getType();
  PcpVariable(PcpArrayType* type, const char* name);

  virtual void accept(PcpVisitor* visitor);
};


// PCP Array Access 

// Array operator 
class PcpArrayOperator
{
 protected:
  enum Opcode
  {
    ARRAY_UNKNOWN,
    USE,
    DEF,
    MAYDEF
  };

  Opcode op;

  inline void setOpcode(Opcode op)
  {
    this->op = op;
  }

  inline Opcode getOpcode()
  {
    return this->op;
  }

  inline PcpArrayOperator(Opcode op)
  {
    setOpcode(op);
  }

 public:
  inline PcpArrayOperator()
  {
    setOpcode(ARRAY_UNKNOWN);
  }

  static inline PcpArrayOperator unknown()
  {
    return PcpArrayOperator(ARRAY_UNKNOWN);
  }

  static inline PcpArrayOperator use()
  {
    return PcpArrayOperator(USE);
  }

  static inline PcpArrayOperator def()
  {
    return PcpArrayOperator(DEF);
  }

  static inline PcpArrayOperator maydef()
  {
    return PcpArrayOperator(MAYDEF);
  }

  inline bool isUnknown()
  {
    return this->getOpcode() == ARRAY_UNKNOWN;
  }

  inline bool isUse()
  {
    return this->getOpcode() == USE;
  }

  inline bool isDef()
  {
    return this->getOpcode() == DEF;
  }

  inline bool isMaydef()
  {
    return this->getOpcode() == MAYDEF;
  }
  
};

class PcpArrayAccess : public PcpObject
{
  friend class PcpExprCanonicalizer;

 protected:
  PcpArrayOperator oper;
  PcpVariable* base;
  PcpArray<PcpExpr*>* subscripts;

  virtual void setOperator(PcpArrayOperator oper);
  virtual void setBase(PcpVariable* base);
  virtual void setSubscripts(PcpArray<PcpExpr*>* subscripts);
  virtual PcpArray<PcpExpr*>* getSubscripts();
  virtual void setSubscript(int index, PcpExpr* subscript);

 public:
  virtual bool isArrayAccess();
  virtual PcpArrayAccess* toArrayAccess();
  virtual PcpArrayOperator getOperator();
  virtual PcpVariable* getBase();
  virtual int getNumSubscripts();
  virtual PcpExpr* getSubscript(int index);
  virtual bool isUse();
  virtual bool isDef();
  virtual bool isMaydef();

  virtual PcpIterator<PcpExpr*>* getSubscriptsIterator();

  PcpArrayAccess(PcpArrayOperator oper,
		 PcpVariable* base,
		 PcpArray<PcpExpr*>* subscripts);

  virtual void accept(PcpVisitor* visitor);
};


// PCP Array Access Builder 

class PcpArrayAccessBuilder
{
 protected:
  PcpVariable* base;
  PcpArrayOperator oper;
  int subscriptIndex;
  PcpArray<PcpExpr*>* subscripts;

  virtual void setBase(PcpVariable* base);
  virtual PcpVariable* getBase();
  virtual void setSubscriptIndex(int subscriptIndex);
  virtual int getSubscriptIndex();
  virtual PcpArray<PcpExpr*>* getSubscripts();
  virtual void setSubscript(int index, PcpExpr* subscript);
  virtual PcpExpr* getSubscript(int index);
  virtual int getBaseNumDimensions();
  virtual void setSubscripts(PcpArray<PcpExpr*>* subscripts);


 public:
  virtual void setOperator(PcpArrayOperator oper);
  virtual PcpArrayOperator getOperator();
  PcpArrayAccessBuilder(PcpVariable* base);
  virtual PcpArrayAccess* createAccess();
  virtual void addSubscript(PcpExpr* subscript);
};


// PCP Stmt 

class PcpStmt : public PcpObject
{
 public:
  virtual bool isStmt();
  virtual PcpStmt* toStmt();
  
  virtual bool isCopy();
  virtual bool isUserStmt();
  virtual bool isGuard();
  virtual bool isLoop();
  virtual bool isSequence();

  virtual class PcpCopy* toCopy();
  virtual class PcpUserStmt* toUserStmt();
  virtual class PcpGuard* toGuard();
  virtual class PcpLoop* toLoop();
  virtual class PcpSequence* toSequence();
};

// PCP Copy Stmt 
class PcpCopy : public PcpStmt
{
 protected:
  PcpArrayAccess* src;
  PcpArrayAccess* dest;

  virtual void setSrc(PcpArrayAccess* src);
  virtual void setDest(PcpArrayAccess* dest);


 public:
  virtual bool isCopy();
  virtual PcpCopy* toCopy();

  virtual PcpArrayAccess* getSrc();
  virtual PcpArrayAccess* getDest();
  PcpCopy(PcpArrayAccess* dest, PcpArrayAccess* src);

  virtual void accept(PcpVisitor* visitor);
};

// PCP User Stmt 
class PcpUserStmt : public PcpStmt
{
 protected:
  PcpArray<PcpArrayAccess*>* accesses;

  virtual void setArrayAccesses(PcpArray<PcpArrayAccess*>* accesses);
  virtual PcpArray<PcpArrayAccess*>* getArrayAccesses();
  virtual void setArrayAccess(int index, PcpArrayAccess* access);

 public:
  
  virtual bool isUserStmt();
  virtual PcpUserStmt* toUserStmt();
  virtual int getNumAccesses();
  virtual PcpArrayAccess* getArrayAccess(int index);

  virtual PcpIterator<PcpArrayAccess*>* getArrayAccessesIterator();
  
  PcpUserStmt(const char* name, PcpArray<PcpArrayAccess*>* accesses);

  virtual void accept(PcpVisitor* visitor);
};

// PCP User Stmt Builder 
class PcpUserStmtBuilder
{
 protected:
  const char* name;
  PcpDynamicArray<PcpArrayAccess*>* accesses;

  virtual void setAccesses(PcpDynamicArray<PcpArrayAccess*>* accesses);
  virtual PcpDynamicArray<PcpArrayAccess*>* getAccesses();

 public:
  virtual const char* getName();

  virtual void setName(const char* name);
  virtual void addAccess(PcpArrayAccess* access);
  virtual PcpUserStmt* createUserStmt();
  PcpUserStmtBuilder();
};

// PCP Sequence 
class PcpSequence : public PcpStmt
{
 protected:
  PcpArray<PcpStmt*>* stmts;

  virtual void setStmts(PcpArray<PcpStmt*>* stmts);
  virtual PcpArray<PcpStmt*>* getStmts();
  virtual void setStmt(int index, PcpStmt* stmt);

 public:
  virtual bool isSequence();
  virtual PcpSequence* toSequence();
  virtual int getNumStmts();
  virtual PcpStmt* getStmt(int index);

  virtual PcpIterator<PcpStmt*>* getStmtsIterator();

  PcpSequence(PcpArray<PcpStmt*>* stmts);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Sequence Builder 
class PcpSequenceBuilder
{
 protected:
  PcpDynamicArray<PcpStmt*>* stmts;

  virtual void setStmts(PcpDynamicArray<PcpStmt*>* stmts);
  virtual PcpDynamicArray<PcpStmt*>* getStmts();

 public:
  virtual void add(PcpStmt* stmt);
  virtual PcpSequence* createSequence();
  PcpSequenceBuilder();
};

// PCP Guard 
class PcpGuard : public PcpStmt
{

  friend class PcpExprCanonicalizer;

 protected:
  PcpBoolExpr* condition;
  PcpStmt* body;

  virtual void setCondition(PcpBoolExpr* condition);
  virtual void setBody(PcpStmt* body);


 public:
  virtual bool isGuard();
  virtual PcpGuard* toGuard();
  virtual PcpBoolExpr* getCondition();
  virtual PcpStmt* getBody();
  PcpGuard(PcpBoolExpr* condition, PcpStmt* body);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Loop 
class PcpLoop : public PcpStmt
{

  friend class PcpExprCanonicalizer;

 protected:
  PcpIv* iv;
  PcpExpr* start;
  PcpBoolExpr* condition;
  PcpConstant* stride;
  PcpStmt* body;

  virtual void setIv(PcpIv* iv);
  virtual void setStart(PcpExpr* start);
  virtual void setCondition(PcpBoolExpr* condition);
  virtual void setStride(PcpConstant* stride);
  virtual void setBody(PcpStmt* body);

 public:
  virtual bool isLoop();
  virtual PcpLoop* toLoop();
  virtual PcpIv* getIv();
  virtual PcpExpr* getStart();
  virtual PcpBoolExpr* getCondition();
  virtual PcpConstant* getStride();
  virtual PcpStmt* getBody();
  PcpLoop(PcpIv* iv, PcpExpr* start, PcpBoolExpr* condition,
	  PcpConstant* stride, PcpStmt* body);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Scop 

class PcpScop : public PcpObject
{
 protected:
  PcpArray<PcpVariable*>* variables;
  PcpArray<PcpParameter*>* parameters;
  PcpStmt* body;

  virtual void setVariables(PcpArray<PcpVariable*>* variables);
  virtual PcpArray<PcpVariable*>* getVariables();
  virtual void setParameters(PcpArray<PcpParameter*>* parameters);
  virtual PcpArray<PcpParameter*>* getParameters();
  virtual void setBody(PcpStmt* body);

 public:
  virtual bool isScop();
  virtual PcpScop* toScop();
  virtual int getNumVariables();
  virtual int getNumParameters();
  virtual PcpVariable* getVariable(int index);
  virtual PcpParameter* getParameter(int index);
  virtual PcpStmt* getBody();

  virtual PcpIterator<PcpVariable*>* getVariablesIterator();
  virtual PcpIterator<PcpParameter*>* getParametersIterator();

  PcpScop(PcpArray<PcpVariable*>* variables,
	  PcpArray<PcpParameter*>* parameters,
	  PcpStmt* body);

  virtual void accept(PcpVisitor* visitor);
};

// PCP Scop Builder 

class PcpScopBuilder
{
 protected:
  PcpDynamicArray<PcpVariable*>* variables;
  PcpDynamicArray<PcpParameter*>* parameters;
  PcpStmt* body;

  virtual void setVariables(PcpDynamicArray<PcpVariable*>* variables);
  virtual PcpDynamicArray<PcpVariable*>* getVariables();
  virtual void setParameters(PcpDynamicArray<PcpParameter*>* parameters);
  virtual PcpDynamicArray<PcpParameter*>* getParameters();
  virtual bool containsVariable(PcpVariable* variable);

 public:
  virtual void addVariable(PcpVariable* variable);
  virtual void addParameter(PcpParameter* parameter);
  virtual void setBody(PcpStmt* body);
  virtual PcpStmt* getBody();
  PcpScopBuilder();
  PcpScop* createScop();

};

#endif // _PCP_H_ 
