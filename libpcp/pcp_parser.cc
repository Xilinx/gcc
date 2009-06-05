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
#include "pcp_string_buffer.h"
#include "pcp_dynamic_array.h"
#include "pcp_emitter.h"
#include "pcp_parser.h"
#include <string.h>


// Parser Token 
class PcpToken
{
public:
  enum Kind
    {
      PCP_TOKEN_NUMERAL,
      PCP_TOKEN_IDENTIFIER,
      PCP_TOKEN_GUARD,
      PCP_TOKEN_LOOP,
      PCP_TOKEN_COPY,
      PCP_TOKEN_EQ,
      PCP_TOKEN_GE,
      PCP_TOKEN_AND,
      PCP_TOKEN_OR,
      PCP_TOKEN_DEF,
      PCP_TOKEN_USE,
      PCP_TOKEN_MAYDEF,
      PCP_TOKEN_ARRAY,
      PCP_TOKEN_IV,
      PCP_TOKEN_PARAMETER,
      PCP_TOKEN_VARIABLE,
      PCP_TOKEN_PARAMETERS,
      PCP_TOKEN_MULTIPLY,
      PCP_TOKEN_ADD,
      PCP_TOKEN_SUBTRACT,
      PCP_TOKEN_ARROW,
      PCP_TOKEN_OPEN_PAREN,
      PCP_TOKEN_CLOSE_PAREN,
      PCP_TOKEN_COMMA,
      PCP_TOKEN_OPEN_BRACE,
      PCP_TOKEN_CLOSE_BRACE,
      PCP_TOKEN_VERTICAL_BAR,
      PCP_TOKEN_SCOP,
      PCP_TOKEN_INPUT,
      PCP_TOKEN_OUTPUT,
      PCP_TOKEN_FLOOR,
      PCP_TOKEN_CEIL,
      PCP_TOKEN_MIN,
      PCP_TOKEN_MAX,
      PCP_TOKEN_ERROR,
      PCP_TOKEN_SEQUENCE
    };

protected:
  static PcpToken* sequenceToken;
  static PcpArray<PcpToken*>* delimiters;
  static PcpArray<PcpToken*>* keywords;
  Kind kind;
  const char* string;
  int lineNumber;

  // Set sequence token to SEQUENCETOKEN.
  static void setSequenceToken(PcpToken* sequenceToken)
  {
    PcpToken::sequenceToken = sequenceToken;
  }

  // Set delimiters to DELIMITERS.
  static void setDelimiters(PcpArray<PcpToken*>* delimiters)
  {
    PcpToken::delimiters = delimiters;
  }

  // Get Delimiters
  static PcpArray<PcpToken*>* getDelimiters()
  {
    return PcpToken::delimiters;
  }

  // Set keywords to KEYWORDS.
  static void setKeywords(PcpArray<PcpToken*>* keywords)
  {
    PcpToken::keywords = keywords;
  }

  // Get keywords.
  static PcpArray<PcpToken*>* getKeywords()
  {
    return PcpToken::keywords;
  }

  // Set kind to KIND.  
  void
  setKind(Kind kind)
  {
    this->kind = kind;
  }

  // Set string to STRING.  
  void
  setString(const char* string)
  {
    this->string = string;
  }

  // Set line number to LINENUMBER.  
  void
  setLineNumber(int lineNumber)
  {
    this->lineNumber = lineNumber;
  }

  // Initialize delimiters to PCP delimiters.
  static void initializeDelimiters()
  {
    PcpDynamicArray<PcpToken*>* delimiters = new PcpDynamicArray<PcpToken*>(7);
    delimiters->add(new PcpToken(PCP_TOKEN_ARROW, "<-", -1));
    delimiters->add(new PcpToken(PCP_TOKEN_OPEN_PAREN, "(", -1));
    delimiters->add(new PcpToken(PCP_TOKEN_CLOSE_PAREN, ")", -1));
    delimiters->add(new PcpToken(PCP_TOKEN_COMMA, ",", -1));
    delimiters->add(new PcpToken(PCP_TOKEN_OPEN_BRACE, "{", -1));
    delimiters->add(new PcpToken(PCP_TOKEN_CLOSE_BRACE, "}", -1));
    delimiters->add(new PcpToken(PCP_TOKEN_VERTICAL_BAR, "|", -1));
    PcpToken::setDelimiters(delimiters);
  }

  // Initialize keywords to PCP keywords and operators.
  static void initializeKeywords()
  {
    PcpDynamicArray<PcpToken*>* keywords = new PcpDynamicArray<PcpToken*>(25);
    keywords->add(new PcpToken(PCP_TOKEN_GUARD, "guard", -1));
    keywords->add(new PcpToken(PCP_TOKEN_LOOP, "loop", -1));
    keywords->add(new PcpToken(PCP_TOKEN_COPY, "copy", -1));
    keywords->add(new PcpToken(PCP_TOKEN_EQ, "eq", -1));
    keywords->add(new PcpToken(PCP_TOKEN_GE, "ge", -1));
    keywords->add(new PcpToken(PCP_TOKEN_AND, "and", -1));
    keywords->add(new PcpToken(PCP_TOKEN_OR, "or", -1));
    keywords->add(new PcpToken(PCP_TOKEN_DEF, "def", -1));
    keywords->add(new PcpToken(PCP_TOKEN_USE, "use", -1));
    keywords->add(new PcpToken(PCP_TOKEN_MAYDEF, "maydef", -1));
    keywords->add(new PcpToken(PCP_TOKEN_ARRAY, "array", -1));
    keywords->add(new PcpToken(PCP_TOKEN_IV, "iv", -1));
    keywords->add(new PcpToken(PCP_TOKEN_PARAMETER, "parameter", -1));
    keywords->add(new PcpToken(PCP_TOKEN_VARIABLE, "variable", -1));
    keywords->add(new PcpToken(PCP_TOKEN_MULTIPLY, "*", -1));
    keywords->add(new PcpToken(PCP_TOKEN_ADD, "+", -1));
    keywords->add(new PcpToken(PCP_TOKEN_SUBTRACT, "-", -1));
    keywords->add(new PcpToken(PCP_TOKEN_SCOP, "scop", -1));
    keywords->add(new PcpToken(PCP_TOKEN_INPUT, "inputs", -1));
    keywords->add(new PcpToken(PCP_TOKEN_OUTPUT, "outputs", -1));
    keywords->add(new PcpToken(PCP_TOKEN_FLOOR, "floor", -1));
    keywords->add(new PcpToken(PCP_TOKEN_CEIL, "ceil", -1));
    keywords->add(new PcpToken(PCP_TOKEN_MIN, "min", -1));
    keywords->add(new PcpToken(PCP_TOKEN_MAX, "max", -1));
    keywords->add(new PcpToken(PCP_TOKEN_PARAMETERS, "parameters", -1));
    PcpToken::setKeywords(keywords);
  }

public:

  // Get sequence token.
  static PcpToken* getSequenceToken()
  {
    if(PcpToken::sequenceToken == NULL)
      PcpToken::setSequenceToken(new PcpToken(PCP_TOKEN_SEQUENCE,
					      "SEQUENCE",
					      -1));
    return PcpToken::sequenceToken;
  }

  // Get kind.  
  Kind
  getKind()
  {
    return this->kind;
  }

  // Get an iterator over the delimiters.
  static PcpIterator<PcpToken*>* getDelimitersIterator()
  {
    PcpArray<PcpToken*>* delimiters = PcpToken::getDelimiters();
    if(delimiters == NULL)
      PcpToken::initializeDelimiters();
    return PcpToken::getDelimiters()->getIterator();
  }

  // Get an iterator over the keywords.
  static PcpIterator<PcpToken*>* getKeywordsIterator()
  {
    PcpArray<PcpToken*>* keywords = PcpToken::getKeywords();
    if(keywords == NULL)
      PcpToken::initializeKeywords();
    return PcpToken::getKeywords()->getIterator();
  }

  // Get string.
  const char* 
  getString()
  {
    return this->string;
  }

  // Get line number.
  int
  getLineNumber()
  {
    return this->lineNumber;
  }

  // Get name. This function is for debug purposes.  
  const char* 
  getTokenKindName()
  {
    switch(getKind())
      {
      case PCP_TOKEN_NUMERAL:
	return "Numeral";
      case PCP_TOKEN_IDENTIFIER:
	return "Identifier";
      case PCP_TOKEN_GUARD:
	return "Guard";
      case PCP_TOKEN_LOOP:
	return "Loop";
      case PCP_TOKEN_COPY:
	return "Copy";
      case PCP_TOKEN_EQ:
	return "Eq";
      case PCP_TOKEN_GE:
	return "Ge";
      case PCP_TOKEN_AND:
	return "And";
      case PCP_TOKEN_OR:
	return "Or";
      case PCP_TOKEN_DEF:
	return "Def";
      case PCP_TOKEN_USE:
	return "Use";
      case PCP_TOKEN_MAYDEF:
	return "MayDef";
      case PCP_TOKEN_ARRAY:
	return "array";
      case PCP_TOKEN_IV:
	return "InductionVariable";
      case PCP_TOKEN_PARAMETER:
	return "Parameter";
      case PCP_TOKEN_VARIABLE:
	return "Variable";
      case PCP_TOKEN_PARAMETERS:
	return "Parameters";
      case PCP_TOKEN_MULTIPLY:
	return "Multiply";
      case PCP_TOKEN_ADD:
	return "Add";
      case PCP_TOKEN_SUBTRACT:
	return "Subtract";
      case PCP_TOKEN_ARROW:
	return "Arrow";
      case PCP_TOKEN_OPEN_PAREN:
	return "OpenParen";
      case PCP_TOKEN_CLOSE_PAREN:
	return "CloseParen";
      case PCP_TOKEN_COMMA:
	return "Comma";
      case PCP_TOKEN_OPEN_BRACE:
	return "OpenBrace";
      case PCP_TOKEN_CLOSE_BRACE:
	return "CloseBrace";
      case PCP_TOKEN_VERTICAL_BAR:
	return "VerticalBar";
      case PCP_TOKEN_SCOP:
	return "Scop";
      case PCP_TOKEN_INPUT:
	return "Input";
      case PCP_TOKEN_FLOOR:
	return "Floor";
      case PCP_TOKEN_CEIL:
	return "Ceil";
      case PCP_TOKEN_MIN:
	return "Min";
      case PCP_TOKEN_MAX:
	return "Max";
      case PCP_TOKEN_OUTPUT:
	return "Output";
      case PCP_TOKEN_ERROR:
	return "ERROR";
      case PCP_TOKEN_SEQUENCE:
	return "SEQUENCE";
      default:
	return "Unknown Token";
      }
  }
  
  // Return true if this is a numeral.  
  bool
  isNumeral()
  {
    return this->getKind() == PCP_TOKEN_NUMERAL;
  }

  // Return true if this is an identifier.  
  bool
  isIdentifier()
  {
    return this->getKind() == PCP_TOKEN_IDENTIFIER;
  }
  
  // Return true if this is a guard.  
  bool
  isGuard()
  {
    return this->getKind() == PCP_TOKEN_GUARD;
  }
  
  // Return true if this is a loop.  
  bool
  isLoop()
  {
    return this->getKind() == PCP_TOKEN_LOOP;
  }

  // Return true if this is a copy.  
  bool
  isCopy()
  {
    return this->getKind() == PCP_TOKEN_COPY;
  }

  // Return true if this is an equality.  
  bool
  isEq()
  {
    return this->getKind() == PCP_TOKEN_EQ;
  }

  // Return true if this is a greater or equals  
  bool
  isGe()
  {
    return this->getKind() == PCP_TOKEN_GE;
  }

  // Return true if this is an and.  
  bool
  isAnd()
  {
    return this->getKind() == PCP_TOKEN_AND;
  }

  // Return true if this is an or.  
  bool
  isOr()
  {
    return this->getKind() == PCP_TOKEN_OR;
  }

  // Return true if this is a def.  
  bool
  isDef()
  {
    return this->getKind() == PCP_TOKEN_DEF;
  }

  // Return true if this is a use. 
  bool
  isUse()
  {
    return this->getKind() == PCP_TOKEN_USE;
  }

  // Return true if this is a maydef.  
  bool
  isMaydef()
  {
    return this->getKind() == PCP_TOKEN_MAYDEF;
  }

  // Return true if this is an array.  
  bool
  isArray()
  {
    return this->getKind() == PCP_TOKEN_ARRAY;
  }

  // Return true if this is an iv.  
  bool
  isIv()
  {
    return this->getKind() == PCP_TOKEN_IV;
  }

  // Return true if this is a parameter.  
  bool
  isParameter()
  {
    return this->getKind() == PCP_TOKEN_PARAMETER;
  }

  // Return true if this is a variable.  
  bool
  isVariable()
  {
    return this->getKind() == PCP_TOKEN_VARIABLE;
  }

  // Return true if this is parameters.  
  bool
  isParameters()
  {
    return this->getKind() == PCP_TOKEN_PARAMETERS;
  }

  // Return true if this is an arrow.  
  bool
  isArrow()
  {
    return this->getKind() == PCP_TOKEN_ARROW;
  }

  // Return true if this is a multiply.  
  bool
  isMultiply()
  {
    return this->getKind() == PCP_TOKEN_MULTIPLY;
  }

  // Return true if this is an add.  
  bool
  isAdd()
  {
    return this->getKind() == PCP_TOKEN_ADD;
  }

  // Return true if this is a subtract.  
  bool
  isSubtract()
  {
    return this->getKind() == PCP_TOKEN_SUBTRACT;
  }

  // Return true if this is an open paren.  
  bool
  isOpenParen()
  {
    return this->getKind() == PCP_TOKEN_OPEN_PAREN;
  }

  // Return true if this is a close paren.  
  bool
  isCloseParen()
  {
    return this->getKind() == PCP_TOKEN_CLOSE_PAREN;
  }

  // Return true if this is a comma.  
  bool
  isComma()
  {
    return this->getKind() == PCP_TOKEN_COMMA;
  }

  // Return true if this is an open brace.  
  bool
  isOpenBrace()
  {
    return this->getKind() == PCP_TOKEN_OPEN_BRACE;
  }

  // Return true if this is a close brace.  
  bool
  isCloseBrace()
  {
    return this->getKind() == PCP_TOKEN_CLOSE_BRACE;
  }

  // Return true if this is a verticalBar.  
  bool
  isVerticalBar()
  {
    return this->getKind() == PCP_TOKEN_VERTICAL_BAR;
  }

  // Return true if this is a scop.  
  bool
  isScop()
  {
    return this->getKind() == PCP_TOKEN_SCOP;
  }

  // Return true if this is an input.  
  bool
  isInput()
  {
    return this->getKind() == PCP_TOKEN_INPUT;
  }

  // Return true if this is an floor.  
  bool
  isFloor()
  {
    return this->getKind() == PCP_TOKEN_FLOOR;
  }
  // Return true if this is an ceil.  
  bool
  isCeil()
  {
    return this->getKind() == PCP_TOKEN_CEIL;
  }
  // Return true if this is an min.  
  bool
  isMin()
  {
    return this->getKind() == PCP_TOKEN_MIN;
  }
  // Return true if this is an max.  
  bool
  isMax()
  {
    return this->getKind() == PCP_TOKEN_MAX;
  }

  // Return true if this is an output.  
  bool
  isOutput()
  {
    return this->getKind() == PCP_TOKEN_OUTPUT;
  }

  // Return true if this is an error.  
  bool
  isError()
  {
    return this->getKind() == PCP_TOKEN_ERROR;
  }

  // Return true if this is a sequence.  
  bool
  isSequence()
  {
    return this->getKind() == PCP_TOKEN_SEQUENCE;
  }

  // Return true if this is a functor (supposed to take arguments).
  bool
  isFunctor()
  {
    bool result;
    switch(this->getKind())
      {
      case PcpToken::PCP_TOKEN_GUARD:
      case PcpToken::PCP_TOKEN_LOOP:
      case PcpToken::PCP_TOKEN_COPY:
      case PcpToken::PCP_TOKEN_EQ:
      case PcpToken::PCP_TOKEN_GE:
      case PcpToken::PCP_TOKEN_AND:
      case PcpToken::PCP_TOKEN_OR:
      case PcpToken::PCP_TOKEN_DEF:
      case PcpToken::PCP_TOKEN_USE:
      case PcpToken::PCP_TOKEN_MAYDEF:
      case PcpToken::PCP_TOKEN_ARRAY:
      case PcpToken::PCP_TOKEN_IV:
      case PcpToken::PCP_TOKEN_PARAMETER:
      case PcpToken::PCP_TOKEN_VARIABLE:
      case PcpToken::PCP_TOKEN_PARAMETERS:
      case PcpToken::PCP_TOKEN_MULTIPLY:
      case PcpToken::PCP_TOKEN_ADD:
      case PcpToken::PCP_TOKEN_SUBTRACT:
      case PcpToken::PCP_TOKEN_SCOP:
      case PcpToken::PCP_TOKEN_INPUT:
      case PcpToken::PCP_TOKEN_FLOOR:
      case PcpToken::PCP_TOKEN_CEIL:
      case PcpToken::PCP_TOKEN_MIN:
      case PcpToken::PCP_TOKEN_MAX:
      case PcpToken::PCP_TOKEN_OUTPUT:
	result = true;
	break;
      default:
	result = false;
	break;
      }
    return result;
  }

  // Create new token of given KIND and STRING.  
  PcpToken(Kind kind, const char* string, int lineNumber)
  {
    setKind(kind);
    setString(string);
    setLineNumber(lineNumber);
  }
   
};

PcpToken* PcpToken::sequenceToken;
template <> PcpArray<PcpToken*>* PcpToken::delimiters;
template <> PcpArray<PcpToken*>* PcpToken::keywords;

// PCP Tokenizer 

class PcpTokenizer
{
protected:
  int row;
  int index;
  const char* source;
  int sourceLength;

  // Set row in TOKENIZER to ROW.  
  void
  setRow(int row)
  {
    this->row = row;
  }
  
  // Get row in TOKENIZER.  
  int
  getRow()
  {
    return this->row;
  }
  
  // Increment row in TOKENIZER by 1.  
  void
  incrementRow()
  {
    this->setRow(this->getRow() + 1);
  }
  
  // Set index of TOKENIZER to INDEX.  
  void
  setIndex(int index)
  {
    this->index = index;
  }
  
  // Get index of TOKENIZER.  
  int
  getIndex()
  {
    return this->index;
  }
  
  // Set source length of TOKENIZER to SOURCE_LENGTH.  
  void
  setSourceLength(int sourceLength)
  {
    this->sourceLength = sourceLength;
  }
  
  // Get source length of TOKENIZER.  
  int
  getSourceLength()
  {
    return this->sourceLength;
  }
    
  // Set source of TOKENIZER to SOURCE.  
  void
  setSource(const char* source)
  {
    this->source = source;
  }
  
  // Get source of TOKENIZER.  
  const char* 
  getSource()
  {
    return this->source;
  }
    
  // Get next char in TOKENIZER with OFFSET from the last consumed
  //   charater.  
  char
  peekChar(int offset)
  {
    
    int index = this->getIndex() + offset;
    char result;
    if(index >= this->getSourceLength())
      return '\0';
    result = this->getSource()[index];
    return result;
  }
  
  // Get the next char in TOKENIZER that has not been consumed.  
  char
  peekNextChar()
  {
    return this->peekChar(0);
  }
  
  // Consume the next charachter in TOKENIZER.  
  void
  consumeChar()
  {
    this->setIndex(this->getIndex() + 1);
  }
  
  // Return true if CHARACHTER is a newline.  
  bool
  charIsNewline(char character)
  {
    return character == '\n';
  }
  
  // Return true if CHARACTER is a whitespace.  
  bool
  charIsWhitespace(char character)
  {
    return(character == '\t' 
	   || character == ' '
	   || this->charIsNewline(character));
  }
  
  // Return true if CHARACTER is end of source.  
  bool
  charIsEndOfSource(char character)
  {
    return character == '\0';
  }
  
  // Return true if TOKENIZER has reached end of source.  
  bool
  endOfSource()
  {
    return this->charIsEndOfSource(this->peekNextChar());
  }
  
  // Return true if CHARACTER will end a token.  
  bool
  charIsTokenBreak(char character)
  {
    return this->charIsWhitespace(character)
      || this->charIsEndOfSource(character);
  }
  
  // Return true if CHARACTER is a digit.  
  bool
  charIsDigit(char character)
  {
    return(character >= '0' && character <= '9');
  }
  
  // Return true if CHARACTER is a letter.  
  bool
  charIsLetter(char character)
  {
    return(character >= 'A' && character <= 'Z')
      || (character >= 'a' && character <= 'z');
  }
  
  // Return true if CHARACTER is a symbol.  
  bool
  charIsSymbol(char character)
  {
    return character == '!'
      || character == '#'
      || character == '$'
      || character == '%'
      || character == '&'
      || character == '*'
      || character == '+'
      || character == '-'
      || character == '/'
      || character == '<'
      || character == '='
      || character == '>'
      || character == '?'
      || character == '@' 
      || character == '\\' 
      || character == '_';
  }
  
  // Return true if CHARACTER is illegal.  
  bool
  charIsIllegal(char character)
  {
    return !this->charIsWhitespace(character)
      && !this->charIsDigit(character)
      && !this->charIsLetter(character)
      && !this->charIsSymbol(character)
      && !this->charIsEndOfSource(character);
  }
  
  // Consume all white space character in TOKENIZER until a
  //   non-whitespace chararacter is encountered.  
  void
  skipWhitespaces()
  {
    char character;
    
    for(character = this->peekNextChar();
	this->charIsWhitespace(character);)
      {
	if(this->charIsNewline(character))
	  this->incrementRow();
	this->consumeChar();
	character = this->peekNextChar();
      }
  }
  
  // Match STRING in TOKENIZER with at OFFSET.  
  bool
  matchStringAtOffset(int offset, const char* string)
  {
    int length = strlen(string);
    int index = 0;
    bool mismatch = false;
    
    for(index = 0; !mismatch && index < length; index++)
      {
	char peekChar = this->peekChar(index + offset);
	char stringChar = string[index];
	if(stringChar != peekChar)
	  mismatch = true;
      }
    return mismatch;
  }
  
  // Match STRING in TOKENIZER at curent location.  
  bool
  matchString(const char* string)
  {
    return this->matchStringAtOffset(0, string);
  }
  
  // Match any delimiter in TOKENIZER at given OFFSET.  
  PcpToken* 
  matchDelimiterAtOffset(int offset)
  {
    PcpIterator<PcpToken*>* delimIter = PcpToken::getDelimitersIterator();
    for(;delimIter->hasNext(); delimIter->next())
      {
	PcpToken* delimiter = delimIter->get();
	bool mismatch;
	mismatch = this->matchStringAtOffset(
					     offset,
					     delimiter->getString());
	if(!mismatch)
	  return delimiter;
      }
    return NULL;
  }
  
  // Match any delimiter in TOKENIZER.  
  PcpToken* 
  matchDelimiter()
  {
    return this->matchDelimiterAtOffset(0);
  }
  
  // Return true if a token break exists in TOKEN at given OFFSET.  
  bool
  hasTokenBreakAtOffset(int offset)
  {
    return
      this->charIsTokenBreak(this->peekChar
			     (offset))
      || this->matchDelimiterAtOffset(offset);
    
  }
  
  // Match any keyword in TOKENIZER.  
  PcpToken* 
  matchKeyword()
  {
    
    PcpIterator<PcpToken*>* delimIter = PcpToken::getKeywordsIterator();
    for(;delimIter->hasNext(); delimIter->next())
      {
	PcpToken* keyword = delimIter->get();
	const char* keywordString = keyword->getString();
	bool mismatch = this->matchString(keywordString);
	if(!mismatch && this->hasTokenBreakAtOffset(strlen(keywordString)))
	  return new PcpToken(keyword->getKind(),
			      keyword->getString(), 
			      this->getRow());
      }
    return NULL;
  }
  
  // Write all characters in TOKENIZER from OFFSET into BUFFER until a token 
  // break is found.  
  void
  fillBufferUntilTokenBreak(int offset,
			    PcpStringBuffer* buffer)
  {
    int i;
    for(i = offset; !this->hasTokenBreakAtOffset(i);
	i++)
      {
	buffer->appendChar(this->peekChar(i));
      }
  }
  
  // Match a numeral from TOKENIZER.  
  PcpToken* 
  matchNumeral()
  {
    int i = 0;
    PcpStringBuffer* buffer = new PcpStringBuffer();
    char peekChar = this->peekChar(i);
    while(this->charIsDigit(peekChar))
      {
	buffer->appendChar(peekChar);
	i = i + 1;
	if(this->hasTokenBreakAtOffset(i))
	  return new PcpToken(PcpToken::PCP_TOKEN_NUMERAL,
			      buffer->toString(),
			      this->getRow());
	peekChar = this->peekChar(i);
      }
    if(buffer->getLength() != 0)
      {
	this->fillBufferUntilTokenBreak(i, buffer);
	return new PcpToken(PcpToken::PCP_TOKEN_ERROR,
			    buffer->toString(),
			    this->getRow());
      }
    return NULL;
  }
  
  // Match an identifier from TOKENIZER. 
  PcpToken* 
  matchIdentifier()
  {
    int i = 0;
    PcpStringBuffer* buffer = new PcpStringBuffer();
    char peekChar = this->peekChar(i);
    if(this->charIsLetter(peekChar))
      {
	buffer->appendChar(peekChar);
	i = i + 1;
	peekChar = this->peekChar(i);
	while(this->charIsLetter(peekChar)
	      || this->charIsDigit(peekChar)
	      || this->charIsSymbol(peekChar))
	  {
	    buffer->appendChar(peekChar);
	    i = i + 1;
	    if(this->hasTokenBreakAtOffset(i))
	      return new PcpToken(PcpToken::PCP_TOKEN_IDENTIFIER,
				  buffer->toString(),
				  this->getRow());
	    peekChar = this->peekChar(i);
	  }
	if(buffer->getLength() != 0)
	  {
	    return new PcpToken(PcpToken::PCP_TOKEN_IDENTIFIER,
				buffer->toString(),
				this->getRow());
	  }
	return NULL;
      }
    return NULL;
  }
  
public:  

  // Create new tokenizer with given SOURCE.  
  PcpTokenizer(const char* source)
  {
    setRow(1);
    setIndex(0);
    setSource(source);
    setSourceLength(strlen(source));
  }

  // Consume TOKEN in TOKENIZER. This increments the index with the
  //   token string length.  
  void
  consumeToken(PcpToken* token)
  {
    int length = strlen(token->getString());
    this->setIndex(getIndex() + length);
  }


  // Get next token from TOKENIZER(without consuming it).  
  PcpToken* 
  peekNextToken()
  {
    PcpToken* token;
    if(this->endOfSource())
      return NULL;
    
    this->skipWhitespaces();
    
    token = this->matchDelimiter();
    if(token != NULL)
      return token;
    
    token = this->matchKeyword();
    if(token != NULL)
      return token;
    
    token = this->matchNumeral();
    if(token != NULL)
      return token;
    
    token = this->matchIdentifier();
    return token;
  }
  
  // Get and consume the next token from TOKENIZER.  
  PcpToken* 
  getNextToken()
  {
    PcpToken* token = this->peekNextToken();
    this->consumeToken(token);
    return token;
  }
  

  
};


// Report parse error.  
static void
pcpParseError(const char* error)
{
  printf("Error: %s\n", error);
}

class PcpAst
{
protected:
  PcpToken* name;
  PcpToken* token;
  bool hasChildren;
  PcpDynamicArray<PcpAst*>* children;
  PcpDynamicArray<PcpAst*>* annots;
  
  // Set children of AST to CHILDREN.  
  void
  setChildren(PcpDynamicArray<PcpAst*>* children)
  {
    this->children = children;
  }
  
  // Get children of AST.  
  PcpDynamicArray<PcpAst*>*
  getChildren()
  {
    return this->children;
  }
    
  void
  setAnnots(PcpDynamicArray<PcpAst*>* annots)
  {
    this->annots = annots;
  }
  
  PcpDynamicArray<PcpAst*>*
  getAnnots()
  {
    return this->annots;
  }
  
    
  // Write AST into BUFFER.  
  void
  toStringInBuffer(PcpStringBuffer* buffer)
  {
    PcpToken* token = this->getToken();
    PcpToken* name = this->getName();
    int numChildren = this->getNumChildren();
    int i;
    
    if(name != NULL)
      {
	buffer->append(name->getString());
	buffer->append(" <- ");
      }

    if(token == NULL)
      buffer->append("NULL");
    else
      buffer->append(token->getString());
    
    if(this->getHasChildren())
      {
	bool first = true;
	buffer->append("(");
	for(i = 0; i < numChildren; i++)
	  {
	    if(first)
	      first = false;
	    else
	      buffer->append(", ");
	    this->getChild(i)->toStringInBuffer(buffer);
	  }
	buffer->append(")");
      }
  }
  
  // Parse functor list surrounded by '{' '}'.  
  static PcpAst* 
  parseStmtList(PcpTokenizer* tokenizer)
  {
    PcpToken* token = tokenizer->getNextToken();
    PcpAst* result = NULL;
    if(!token->isOpenBrace())
      {
	pcpParseError("Expected argument list");
      }
    result = PcpAst::parseStmtListHelper(tokenizer);
    token = tokenizer->peekNextToken();
    if(token != NULL && token->isCloseBrace())
      tokenizer->consumeToken(token);
    else
      pcpParseError("Expected close brace");
    return result;
  }
  
  static void
  parseObjectList1(PcpTokenizer* tokenizer, PcpAst* root,
		   bool isAnnot)
  {
    PcpToken* peek = tokenizer->peekNextToken();
    PcpAst* object = NULL;
    
    object = PcpAst::parseObject(tokenizer);
    if(isAnnot)
      root->addAnnot(object);
    else
      root->addChild(object);
    peek = tokenizer->peekNextToken();
    if(peek == NULL)
      {
	pcpParseError("Unexpected end of stream");
	return;
      }
    
    while(peek->isComma())
      {
	tokenizer->consumeToken(peek);
	object = PcpAst::parseObject(tokenizer);
	if(isAnnot)
	  root->addAnnot(object);
	else
	  root->addChild(object);

	peek = tokenizer->peekNextToken();
	if(peek == NULL)
	  {
	    pcpParseError("Unexpected end of stream");
	    return;
	  }
      }
  }
  
  // Parse comma separated object list.  
  static PcpAst* 
  parseObjectList(PcpTokenizer* tokenizer)
  {
    PcpToken* token = tokenizer->getNextToken();
    PcpToken* peek;
    PcpAst* result = new PcpAst(PcpToken::getSequenceToken());
    result->setHasChildren(true);
    
    if(!token->isOpenParen())
      {
	pcpParseError("Expected argument list");
      }
    
    peek = tokenizer->peekNextToken();
    if(peek->isCloseParen())
      {
	tokenizer->consumeToken(token);
	return result;
      }
    
    if(peek->isVerticalBar())
      {
	tokenizer->consumeToken(peek);
	peek = tokenizer->peekNextToken();
	if(!peek->isCloseParen())
	  PcpAst::parseObjectList1(tokenizer, result, true);
      }
    else
      {
	PcpAst::parseObjectList1(tokenizer, result, false);
	peek = tokenizer->peekNextToken();
	if(peek->isVerticalBar())
	  {
	    tokenizer->consumeToken(peek);
	    PcpAst::parseObjectList1(tokenizer, result, true);
	  }
      }

    peek = tokenizer->peekNextToken();
    if(peek->isCloseParen())
      tokenizer->consumeToken(token);
    else
      pcpParseError("Error, expected close paren");
    
    
    return result;
  }
  
  // Parse object. 
  static PcpAst* 
  parseObject(PcpTokenizer* tokenizer)
  {
    PcpToken* token = tokenizer->getNextToken();
    if(token->isFunctor())
      {
	PcpAst* result = PcpAst::parseObjectList(tokenizer);
	PcpToken* peek = tokenizer->peekNextToken();
	result->setToken(token);
	// We may have a body to parse as well.  
	if(peek != NULL && peek->isOpenBrace())
	  {
	    // Stmt list is a non comma separated object list.  
	    PcpAst* body = PcpAst::parseStmtList(tokenizer);
	    result->addChild(body);
	  }
	return result;
      }
    else if(token->isIdentifier())
      {
	PcpToken* peek = tokenizer->peekNextToken();
	if(peek->isOpenParen())
	  {
	    PcpAst* result = PcpAst::parseObjectList(tokenizer);
	    result->setToken(token);
	    return result;
	  }
	else if(peek->isArrow())
	  {
	    PcpAst* result = PcpAst::parseBinding(tokenizer, token);
	    return result;
	    
	  }
	return new PcpAst(token);
      }
    else if(token->isNumeral())
      return new PcpAst(token);
    else
      {
	pcpParseError("Unexpected token");
	return NULL;
      }
  }
  
  // Parse "identifier <- object" sequence. 
  static PcpAst* 
  parseBinding(PcpTokenizer* tokenizer, PcpToken* name)
  {
    PcpToken* token = tokenizer->getNextToken();
    
    pcpAssert(name != NULL);
    if(token->isArrow())
      {
	PcpAst* result = PcpAst::parseObject(tokenizer);
	result->setName(name);
	return result;
      }
    else
      pcpParseError("Expected '<-' after identifier");
    return NULL;
  }

  // Parse list of functor strings.  
  static PcpAst* 
  parseStmtListHelper(PcpTokenizer* tokenizer)
  {
    PcpToken* peek = tokenizer->peekNextToken();
    PcpAst* result = new PcpAst(PcpToken::getSequenceToken());
    PcpAst* object = NULL;

    result->setHasChildren(true);
    
    if(peek->isFunctor() || peek->isIdentifier())
      {
	PcpToken* token;
	object = PcpAst::parseObject(tokenizer);
	result->addChild(object);
	token = tokenizer->peekNextToken();
	while(token != NULL && (token->isFunctor() || token->isIdentifier()))
	  {
	    object = PcpAst::parseObject(tokenizer);
	    result->addChild(object);
	    token = tokenizer->peekNextToken();
	  }
      }
    
    return result;
  }
  
public:

  // Convert AST to string.  
  const char* 
  toString()
  {
    PcpStringBuffer* buffer = new PcpStringBuffer();
    this->toStringInBuffer(buffer);
    return buffer->toString();
  }

  int
  getNumAnnots()
  {
    return this->getAnnots()->getSize();
  }
  
  PcpAst* 
  getAnnot(int index)
  {
    return this->getAnnots()->get(index);
  }


  // Get number of childern in AST.  
  int
  getNumChildren()
  {
    return this->getChildren()->getSize();
  }

  // Get child with CHILDIndex from AST.  
  PcpAst* 
  getChild(int childIndex)
  {
    return this->getChildren()->get(childIndex);
  }
  
  // Set name of AST to NAME.  
  void
  setName(PcpToken* name)
  {
    this->name = name;
  }
  
  // Get name of AST.  
  PcpToken* 
  getName()
  {
    return this->name;
  }
  
  // Set token of AST to TOKEN.  
  void
  setToken(PcpToken* token)
  {
    this->token = token;
  }
  
  // Get token of AST.  
  PcpToken* 
  getToken()
  {
    return this->token;
  }

  // Set if AST HASChildren.  
  void
  setHasChildren(bool hasChildren)
  {
    this->hasChildren = hasChildren;
  }
  
  // Return true if AST has children.  
  bool
  getHasChildren()
  {
    return this->hasChildren;
  }
  
  void
  addAnnot(PcpAst* annot)
  {
    this->getAnnots()->add(annot);
  }

  // Add CHILD to AST.  
  void
  addChild(PcpAst* child)
  {
    this->getChildren()->add(child);
  }
  
  // Create a new token ast from TOKEN.  
  PcpAst(PcpToken* token)
  {
    PcpDynamicArray<PcpAst*>* children = new PcpDynamicArray<PcpAst*>(1);
    PcpDynamicArray<PcpAst*>* annots = new PcpDynamicArray<PcpAst*>(1);
    this->setName(NULL);
    this->setToken(token);
    this->setChildren(children);
    this->setHasChildren(false);
    this->setAnnots(annots);
  }  

  // Parse list of functor strings.  
  static PcpAst* 
  parse(PcpTokenizer* tokenizer)
  {
    return PcpAst::parseStmtListHelper(tokenizer);
  }
  
};

// Pcp Parser (Token stream to AST)

// Return true if token is a functor(token that must take
//   arguments).  

class PcpSymtab
{
protected:
  PcpDynamicArray<PcpObject*>* entries;
  
  // Set entries of SYMTAB to ENTRIES.  
  void
  setEntries(PcpDynamicArray<PcpObject*>* entries)
  {
    this->entries = entries;
  }
  
  // Get entries of SYMTAB.  
  PcpDynamicArray<PcpObject*>*
  getEntries()
  {
    return this->entries;
  }
  
public:
  
  // Return object with given NAME in SYMTAB, if it does not exist
  //   return NULL.  
  PcpObject* 
  lookupName(const char* name)
  {
    PcpIterator<PcpObject*>* symtabIter = this->getEntries()->getIterator();
    for(;symtabIter->hasNext(); symtabIter->next())
      {
	PcpObject* entry = symtabIter->get();
	const char* entryName = entry->getName();
	if(strcmp(name, entryName) == 0)
	  return entry;
      }
    return NULL;
  }
  
  // Lookup TOKEN's string in SYMTAB.  
  PcpObject* 
  lookup(PcpToken* token)
  {
    const char* name = token->getString();
    return this->lookupName(name);
  }
  
  
  // Insert OBJECT in SYMTAB.  
  void
  insert(PcpObject* object)
  {
    PcpObject* existingObject = 
      this->lookupName(object->getName());
    pcpAssert(object->getName() != NULL);
    pcpAssert(existingObject == NULL || existingObject == object);
    this->getEntries()->add(object);
  }
  
  // Create new symbol table. 
  PcpSymtab()
  {
    this->setEntries(new PcpDynamicArray<PcpObject*>(5));
  }
};


// PCP Ast Parser 

void
PcpParser::setSymtab(PcpSymtab* symtab)
{
  this->symtab = symtab;
}
  
// Get symbol table in CONTEXT.  
PcpSymtab*
PcpParser::getSymtab()
{
  return this->symtab;
}
  
// Set parser CONTEXT to add annotations to ADDLineAnnots.   
void
PcpParser::setAddLineAnnots(bool addLineAnnots)
{
  this->addLineAnnots = addLineAnnots;
}
  
// Return if parser CONTEXT should insert line annotations.  
bool
PcpParser::getAddLineAnnots()
{
  return this->addLineAnnots;
}

// Set file name in CONTEXT to FILEName.  
void
PcpParser::setFileName(const char* fileName)
{
  this->fileName = fileName;
}

// Get file name in CONTEXT.  
const char* 
PcpParser::getFileName()
{
  return this->fileName;
}


// Parse NUMERAL and return created constant.  
PcpConstant* 
PcpParser::parseNumeral(PcpAst* numeral)
{
  int value;
  PcpConstant* result;
  PcpToken* token = numeral->getToken();

  if(!token->isNumeral())
    pcpParseError("Expected numeral");
  sscanf(token->getString(), "%d", &value);
  result = new PcpConstant(value);
  
  return result;
}

void
PcpParser::addLineAnnot(PcpObject* object, PcpAst* ast)
{
  if(this->getAddLineAnnots() && !object->containsAnnotWithTag("lineinfo"))
    {
      PcpAnnotTermBuilder* termBuilder = new PcpAnnotTermBuilder();
      const char* filename = this->getFileName();
      PcpAnnotString* filenameAnnot = new PcpAnnotString(filename);
      PcpAnnotInt* lineNumberAnnot = 
	new PcpAnnotInt(ast->getToken()->getLineNumber());
      PcpAnnotTerm* lineinfoAnnot = NULL;
      
      termBuilder->setTag("lineinfo");
      termBuilder->addArgument(filenameAnnot);
      termBuilder->addArgument(lineNumberAnnot);
      lineinfoAnnot = termBuilder->createAnnot();
      
      object->addAnnot(lineinfoAnnot);
    }
}

// Register OBJECT in SYMTAB with TOKENAst's token string. If
//   TOKENAst is NULL or the string is NULL do nothing. 
void
PcpParser::parseCommonObjectAttributes(PcpAst* ast,
				       PcpObject* object)
{
  PcpSymtab* symtab = this->getSymtab();
  PcpToken* tokenName = ast->getName();
  if(tokenName != NULL && tokenName->getString() != NULL)
    {
      const char* name = tokenName->getString();
      object->setName(name);
      symtab->insert(object);
    }
  this->parseAnnots(object, ast);
  this->addLineAnnot(object, ast);
}

// Parse MULTIPLY and return the created pcp multiply object.  
PcpArith* 
PcpParser::parseMultiply(PcpAst* multiply)
{
  int numChildren = multiply->getNumChildren();
  if(numChildren != 2)
    pcpParseError("Expected two arguments for multiply");
  else
    {
      PcpSymtab* symtab = this->getSymtab();
      PcpAst* lhsAst = multiply->getChild(0);
      PcpToken* lhsToken = lhsAst->getToken();
      PcpAst* rhsAst = multiply->getChild(1);
      PcpToken* rhsToken = rhsAst->getToken();
      PcpObject* rhsObject;

      if(!lhsToken->isNumeral())
	pcpParseError("Expected constant as lhs of multiply");

      if(!rhsToken->isIdentifier())
	pcpParseError("Expected iv as rhs of multiply");

      rhsObject = symtab->lookup(rhsToken);

      if(rhsObject == NULL || !rhsObject->isIv())
	pcpParseError("Expected iv as rhs of multiply");
      else
	{
	  PcpArith* multiplyObject =
	    PcpArith::pcpArithBinaryCreate(PcpArithOperator::multiply(),
					   this->parseNumeral(lhsAst),
					   rhsObject->toIv());
	  return multiplyObject;
	}
    }
  return NULL;
}

// Parse AST and return the created pcp expression.  
PcpExpr* 
PcpParser::parseScalarIdentifier(PcpAst* ast)
{
  PcpSymtab* symtab = this->getSymtab();
  PcpToken* token = ast->getToken();
  PcpObject* result = symtab->lookup(token);
  if(ast->getHasChildren()
     || result == NULL
     || (!result->isParameter() && !result->isIv()))
    pcpParseError("Expected scalar value");
  return result->toExpr();
}

PcpArithOperator
PcpParser::parseArithOperator(PcpToken* operatorToken)
{
  if(operatorToken->isAdd())
    return PcpArithOperator::add();
  else if(operatorToken->isSubtract())
    return PcpArithOperator::subtract();
  else if(operatorToken->isFloor())
    return PcpArithOperator::floor();
  else if(operatorToken->isCeil())
    return PcpArithOperator::ceiling();
  else if(operatorToken->isMin())
    return PcpArithOperator::min();
  else if(operatorToken->isMax())
    return PcpArithOperator::max();
  else
    return PcpArithOperator::unknown();
}

bool PcpParser::pcpTokenIsOperator(PcpToken* operatorToken)
{
  return !this->parseArithOperator(operatorToken).isUnknown();
}

// Parse AST(add or subtract ast) and return the created pcp expression.  
PcpExpr* 
PcpParser::parseArith(PcpAst* ast)
{
  int numChildren = ast->getNumChildren();
  PcpArithOperator oper = 
    this->parseArithOperator(ast->getToken());
  PcpArith* arith;
  int i;
  PcpArithBuilder* arithBuilder = new PcpArithBuilder();

  if(oper.isUnknown())
    pcpParseError("Not a valid operator");
  if(oper.isSubtract() && (numChildren > 2 || numChildren < 1))
    pcpParseError("Wrong number of arguments to subtract");
  if(numChildren < 2)
    pcpParseError("Too few arguments to arith");

  arithBuilder->setOperator(oper);

  for(i = 0; i < numChildren; i++)
    {
      PcpExpr* operand = this->parseLinearExpr(ast->getChild(i));
      arithBuilder->addOperand(operand);
    }
  arith = arithBuilder->createArith();
  return arith;
}

// Parse AST and return the created pcp expression.  
PcpExpr* 
PcpParser::parseLinearExpr(PcpAst* ast)
{
  PcpExpr* result;
  PcpToken* token = ast->getToken();
  if(token->isNumeral())
    result = this->parseNumeral(ast);
  else if(token->isIdentifier())
    result = this->parseScalarIdentifier(ast);
  else if(pcpTokenIsOperator(token))
    result = this->parseArith(ast);
  else if(token->isMultiply())
    result = this->parseMultiply(ast);
  else
    {
      pcpParseError("Not a linear expr");
      return NULL;
    }

  if(!token->isNumeral() && !token->isIdentifier())
    this->parseCommonObjectAttributes(ast, result);
  return result;
}

// Parse AST and return the created comparison(eq or ge).  
PcpBoolExpr* 
PcpParser::parseComparison(PcpAst* ast)
{
  int numChildren = ast->getNumChildren();
  PcpToken* token = ast->getToken();
  PcpExpr* lhs;
  PcpExpr* rhs;

  if(numChildren != 2)
    pcpParseError("Wrong number of arguments to eq/ge");

  lhs = this->parseLinearExpr(ast->getChild(0));
  rhs = this->parseLinearExpr(ast->getChild(1));
  if(token->isEq())
    return new PcpCompare(PcpCompareOperator::equal(), lhs, rhs);
  else if(token->isGe())
    return new PcpCompare(PcpCompareOperator::greaterEqual(), lhs, rhs);
  else
    pcpParseError("Not eq/ge expr");
  return NULL;
}

PcpBoolArithOperator
PcpParser::parseBoolArithOperator(PcpToken* operatorToken)
{
  if(operatorToken->isAnd())
    return PcpBoolArithOperator::boolAnd();
  else if(operatorToken->isOr())
    return PcpBoolArithOperator::boolOr();
  else
    return PcpBoolArithOperator::unknown();
}

bool PcpParser::pcpTokenIsBoolArithOperator(PcpToken* operatorToken)
{
  return !this->parseBoolArithOperator(operatorToken).isUnknown();
}

PcpBoolExpr* 
PcpParser::parseBoolArith(PcpAst* ast)
{
  int numChildren = ast->getNumChildren();
  PcpBoolArithOperator oper = 
    this->parseBoolArithOperator(ast->getToken());
  PcpBoolArith* boolArith;
  int i;
  PcpBoolArithBuilder* boolArithBuilder = new PcpBoolArithBuilder();

  if(oper.isUnknown())
    pcpParseError("Not a valid operator");
  if(numChildren < 2)
    pcpParseError("Too few arguments to bool arith");

  boolArithBuilder->setOperator(oper);

  for(i = 0; i < numChildren; i++)
    {
      PcpBoolExpr* operand = 
	this->parseBoolExpr(ast->getChild(i));
      boolArithBuilder->addOperand(operand);
    }
  boolArith = boolArithBuilder->createBoolArith();
  return boolArith;
}

// Parse ast and return the created pcp boolean expression.  
PcpBoolExpr* 
PcpParser::parseBoolExpr(PcpAst* ast)
{
  PcpToken* token = ast->getToken();
  PcpBoolExpr* result = NULL;
  if(token->isEq() || token->isGe())
    result = this->parseComparison(ast);
  if(token->isAnd() || token->isOr())
    result = this->parseBoolArith(ast);

  if(result == NULL)
    {
      pcpParseError("Not a boolean expression");
      return result;
    }
  this->parseCommonObjectAttributes(ast, result);
  return result;
}

// Parse AST(identifier) and return the corresponding object from
//   SYMTAB. If the identifier does not exist in SYMTAB
//   report as error and return NULL. 
PcpObject* 
PcpParser::parseIdentifier(PcpAst* ast)
{
  PcpObject* result = NULL;
  PcpToken* token = ast->getToken();

  if(!ast->getHasChildren())
    {
      PcpSymtab* symtab = this->getSymtab();
      result = symtab->lookup(token);
      if(result == NULL)
	pcpParseError("Unknown identifier");
    }
  return result;
}

// Parse ast(array access ast) and return the created array access.  
PcpArrayAccess* 
PcpParser::parseArrayAccess(PcpAst* ast)
{
  int numChildren = ast->getNumChildren();
  PcpToken* token = ast->getToken();
  PcpArrayAccessBuilder* builder;
  int i;
  PcpArrayAccess* result;
  PcpObject* base;

  if(numChildren < 1)
    {
      pcpParseError("Wrong number of arguments to array access");
    }

  base = this->parseIdentifier(ast->getChild(0));
  if(!base->isVariable())
    pcpParseError("Base object is not a variable in array access");

  builder = new PcpArrayAccessBuilder(base->toVariable());

  for(i = 1; i < numChildren; i++)
    {
      PcpExpr* subscript =
	this->parseLinearExpr(ast->getChild(i));
      builder->addSubscript(subscript);
    }
  
  if(token->isUse())
    builder->setOperator(PcpArrayOperator::use());
  else if(token->isDef())
    builder->setOperator(PcpArrayOperator::def());
  else if(token->isMaydef())
    builder->setOperator(PcpArrayOperator::maydef());
  else
    {
      pcpParseError("Unknown array access token");
      return NULL;
    }

  result = builder->createAccess();
  this->parseCommonObjectAttributes(ast, result);
  return result;
}

// Return true if TOKEN is empty or an unknown symbol in
//   SYMTAB.  
bool
PcpParser::parseTokenIsUnknownSymbol(PcpToken* token)
{
  PcpSymtab* symtab = this->getSymtab();
  return(token->getString() != NULL 
	 && token->isIdentifier()
	 && symtab->lookup(token) == NULL);
}


// Parse USERStmt and return the created pcp user statement.  
PcpUserStmt* 
PcpParser::parseUserStmt(PcpAst* userStmt)
{
  PcpUserStmt* result;
  PcpUserStmtBuilder* builder;
  int numChildren = userStmt->getNumChildren();
  PcpToken* token = userStmt->getToken();
  const char* tokenName = token->getString();
  int i;

  if(!this->parseTokenIsUnknownSymbol(token))
    {
      pcpParseError("User stmt name is invalid");
      return NULL;
    }

  builder = new PcpUserStmtBuilder();

  builder->setName(tokenName);

  for(i = 0; i < numChildren; i++)
    {
      PcpArrayAccess* access =
	this->parseArrayAccess(userStmt->getChild(i));
      builder->addAccess(access);
    }
  result = builder->createUserStmt();
  this->parseCommonObjectAttributes(userStmt, result);
  return result;
}

// Parse COPY and return the created pcp copy.  
PcpCopy* 
PcpParser::parseCopy(PcpAst* copy)
{
  PcpToken* token = copy->getToken();
  int numChildren = copy->getNumChildren();
  PcpArrayAccess* dest;
  PcpArrayAccess* src;
  PcpCopy* result;

  if(!token->isCopy())
    {
      pcpParseError("Token is not a copy");
      return NULL;
    }
  if(numChildren != 2)
    {
      pcpParseError("Wrong number of arguments to copy stmt");
      return NULL;
    }

  dest =
    this->parseArrayAccess(copy->getChild(0));
  src =
    this->parseArrayAccess(copy->getChild(1));

  if(!dest->isDef() || !src->isUse())
    {
      pcpParseError
	("Expected def as first argument and use as second argument in copy");
      return NULL;
    }

  result = new PcpCopy(dest, src);
  this->parseCommonObjectAttributes(copy, result);
  return result;
}

// Parse SEQUENCE and return the created pcp statement sequence.  
PcpSequence* 
PcpParser::parseSequence(PcpAst* sequence)
{
  PcpToken* token = sequence->getToken();
  int numChildren = sequence->getNumChildren();
  PcpSequenceBuilder* builder;
  PcpSequence* result;
  int i;

  if(!token->isSequence())
    {
      pcpParseError("Statement list is not valid sequence");
    }

  builder = new PcpSequenceBuilder();
  for(i = 0; i < numChildren; i++)
    {
      PcpStmt* stmt = this->parseStmt(sequence->getChild(i));
      builder->add(stmt);
    }
  result = builder->createSequence();
  return result;
}

// Parse GUARD and return the created pcp guard.  
PcpGuard* 
PcpParser::parseGuard(PcpAst* guard)
{
  PcpToken* token = guard->getToken();
  int numChildren = guard->getNumChildren();
  PcpBoolExpr* condition;
  PcpStmt* body;
  PcpGuard* result;

  if(!token->isGuard())
    {
      pcpParseError("Invalid guard stmt");
      return NULL;
    }

  if(numChildren != 2)
    {
      pcpParseError("Condition or Body missing in guard");
      return NULL;
    }
  condition = this->parseBoolExpr(guard->getChild(0));
  body = this->parseStmt(guard->getChild(1));
  result = new PcpGuard(condition, body);
  this->parseCommonObjectAttributes(guard, result);
  return result;
}

// Parse IV and return the created pcp iv.  
PcpIv* 
PcpParser::parseIv(PcpAst* iv)
{
  PcpToken* token = iv->getToken();
  int numChildren = iv->getNumChildren();
  const char* tokenString = token->getString();
  PcpIv* result;

  if(!token->isIv())
    {
      pcpParseError("Not an iv declaration");
      return NULL;
    }
  if(!iv->getHasChildren() || !numChildren == 0)
    {
      pcpParseError("Wrong number of arguments to iv declaration");
      return NULL;
    }
  if(tokenString == NULL)
    {
      pcpParseError("No name given to iv");
      return NULL;
    }
  result = new PcpIv(tokenString);
  this->parseCommonObjectAttributes(iv, result);
  return result;
}

// Parse LOOP and return the created pcp loop.  
PcpLoop* 
PcpParser::parseLoop(PcpAst* loop)
{
  PcpToken* token = loop->getToken();
  int numChildren = loop->getNumChildren();
  PcpIv* iv;
  PcpExpr* start;
  PcpBoolExpr* end;
  PcpConstant* stride;
  PcpStmt* body;
  PcpLoop* result;

  if(!token->isLoop())
    {
      pcpParseError("Expected loop construct");
      return NULL;
    }

  if(numChildren != 5)
    {
      pcpParseError("Wrong number of arguments to loop");
      return NULL;
    }

  iv = this->parseIv(loop->getChild(0));
  start =
    this->parseLinearExpr(loop->getChild(1));
  end = this->parseBoolExpr(loop->getChild(2));
  stride = this->parseNumeral(loop->getChild(3));
  body = this->parseStmt(loop->getChild(4));

  result = new PcpLoop(iv, start, end, stride, body);
  this->parseCommonObjectAttributes(loop, result);

  return result;
}


// Parse STMT and return the created pcp statement.  
PcpStmt* 
PcpParser::parseStmt(PcpAst* stmt)
{
  PcpToken* token = stmt->getToken();
  PcpStmt* result = NULL;
  if(token->isCopy())
    result = this->parseCopy(stmt);
  else if(this->parseTokenIsUnknownSymbol(token))
    result = this->parseUserStmt(stmt);
  else if(token->isGuard())
    result = this->parseGuard(stmt);
  else if(token->isSequence())
    result = this->parseSequence(stmt);
  else if(token->isLoop())
    result = this->parseLoop(stmt);
  else
    {
      pcpParseError("Invalid statement in sequence");
      return NULL;
    }
  return result;
}


void
PcpParser::parseScopInoutput(PcpAst* inoutput,
			     PcpScopBuilder* builder)
{
  PcpToken* token = inoutput->getToken();
  int numChildren = inoutput->getNumChildren();
  int i;

  if(!token->isInput() && !token->isOutput())
    {
      pcpParseError("Not an input or output list");
      return;
    }

  for(i = 0; i < numChildren; i++)
    {
      PcpAst* child = inoutput->getChild(i);
      PcpToken* childToken = child->getToken();
      if(childToken->isIdentifier())
	{
	  PcpObject* object = this->parseIdentifier(child);
	  if(object == NULL)
	    pcpParseError("Unknown identifier in scop input/output list");
	  else if(!object->isVariable())
	    pcpParseError("Non variable found in scop input/output list");
	  else
	    {
	      PcpVariable* variable = object->toVariable();
	      builder->addVariable(variable);
	      if(token->isInput())
		variable->setIsInput(true);
	      else
		variable->setIsOutput(true);
	    }
	}
      else
	pcpParseError("Expected identifier in scop inputs/outputs list");
    }
}

void
PcpParser::parseScopParameters(PcpAst* parameters,
			       PcpScopBuilder* builder)
{
  PcpToken* token = parameters->getToken();
  int numChildren = parameters->getNumChildren();
  int i;

  if(!token->isParameters())
    {
      pcpParseError("Not a parameter list");
      return;
    }

  for(i = 0; i < numChildren; i++)
    {
      PcpAst* child = parameters->getChild(i);
      PcpToken* childToken = child->getToken();
      if(childToken->isIdentifier())
	{
	  PcpObject* object = this->parseIdentifier(child);
	  if(object == NULL)
	    pcpParseError("Unknown identifier in scop parameter list");
	  else if(!object->isParameter())
	    pcpParseError("Non parameter found in scop parameter list");
	  else
	    {
	      PcpParameter* parameter = object->toParameter();
	      builder->addParameter(parameter);
	    }
	}
      else
	pcpParseError("Expected identifier in scop parameter list");
    }
}

PcpScop* 
PcpParser::parseScop(PcpAst* scop)
{
  PcpScop* result = NULL;
  PcpToken* token = scop->getToken();
  int numChildren = scop->getNumChildren();
  PcpScopBuilder* builder;
  PcpStmt* body;
  int i;

  if(!token->isScop())
    {
      pcpParseError("Construct is not a scop");
      return NULL;
    }

  if(numChildren > 4 || numChildren < 1)
    {
      pcpParseError("Wrong number of arguments to scop or missing body");
    }

  builder = new PcpScopBuilder();

  // Parse everythin except the body.  
  for(i = 0; i < numChildren - 1; i++)
    {
      PcpAst* child = scop->getChild(i);
      PcpToken* childToken = child->getToken();
      if(childToken->isInput() || childToken->isOutput())
	this->parseScopInoutput(child, builder);
      else if(childToken->isParameters())
	this->parseScopParameters(child, builder);
    }

  // Parse body.  
  body = this->parseStmt(scop->getChild(numChildren - 1));
  builder->setBody(body);
  result = builder->createScop();
  this->parseCommonObjectAttributes(scop, result);
  return result;
}

PcpArrayType* 
PcpParser::parseArrayType(PcpAst* arrayType)
{
  PcpToken* token = arrayType->getToken();
  int numChildren = arrayType->getNumChildren();
  PcpArrayTypeBuilder* builder;
  PcpArrayType* type;
  int i;

  if(!token->isArray())
    {
      pcpParseError("Expected array construct");
      return NULL;
    }

  builder = new PcpArrayTypeBuilder();

  for(i = 0; i < numChildren; i++)
    {
      PcpAst* child = arrayType->getChild(i);
      PcpToken* childToken = child->getToken();
      PcpExpr* dim = NULL;
      if(childToken->isNumeral())
	dim = this->parseNumeral(child);
      else if(childToken->isIdentifier())
	{
	  PcpObject* object = this->parseIdentifier(child);
	  if(!object->isParameter())
	    pcpParseError("Array type dimension is not a constant or parameter");
	  dim = object->toExpr();
	}
      else
	{
	  pcpParseError("unexpected token in array dimension list");
	  return NULL;
	}
      builder->addDimension(dim);
    }

  type = builder->createType();
  this->parseCommonObjectAttributes(arrayType, type);
  return type;
}

PcpVariable* 
PcpParser::parseVariable(PcpAst* array)
{
  PcpToken* token = array->getToken();
  int numChildren = array->getNumChildren();
  PcpArrayType* type;
  PcpVariable* result;
  PcpToken* nameToken = array->getName();
  const char* name = nameToken != NULL ? nameToken->getString() : NULL;
  PcpAst* typeAst;
  PcpToken* typeToken;

  if(!token->isVariable())
    {
      pcpParseError("Expected variable construct");
      return NULL;
    }

  if(numChildren != 1)
    pcpParseError("Wrong number of arguments to variable");

  if(name == NULL)
    {
      pcpParseError("No name given to variable");
      return NULL;
    }

  typeAst = array->getChild(0);
  typeToken = typeAst->getToken();

  if(typeToken->isIdentifier())
    type = this->parseIdentifier(typeAst)->toArrayType();
  else if(typeToken->isArray())
    type = this->parseArrayType(typeAst);
  else 
    type = NULL;

  if(type == NULL)
    pcpParseError("Illegal type construct given to varaible");

  result = new PcpVariable(type, name);
  this->parseCommonObjectAttributes(array, result);
  return result;
}

PcpAnnotTerm*
PcpParser::pcpParseAnnotTerm(PcpAst* termAst)
{
  int numChildren = termAst->getNumChildren();
  PcpToken* token = termAst->getToken();
  PcpAnnotTermBuilder* termBuilder = NULL;
  int i;
  if(!token->isIdentifier())
    {
      pcpParseError("Expected identifier as tag for annotation");
      return NULL;
    }

  termBuilder = new PcpAnnotTermBuilder();
  termBuilder->setTag(token->getString());
  for(i = 0; i < numChildren; i++)
    {
      PcpAnnot* annot = pcpParseAnnot(termAst->getChild(i));
      termBuilder->addArgument(annot);
    }
  return termBuilder->createAnnot();
}

PcpAnnot*
PcpParser::pcpParseAnnot(PcpAst* annotAst)
{
  PcpToken* token = annotAst->getToken();
  if(!annotAst->getHasChildren())
    {
      if(token->isNumeral())
	{
	  PcpConstant* constant = this->parseNumeral(annotAst);
	  int value = constant->getValue();
	  return new PcpAnnotInt(value);
	}
      else if(token->isIdentifier())
	{
	  PcpSymtab* symtab = this->getSymtab();
	  const char* tokenString = token->getString();
	  PcpObject* object = symtab->lookup(token);
	  if(object != NULL)
	    return new PcpAnnotObject(object);
	  else
	    return new PcpAnnotString(tokenString);
	}
      else
	{
	  pcpParseError("Unexpected token in optional arg list");
	  return NULL;
	}
    }
  else
    return pcpParseAnnotTerm(annotAst);
}

void
PcpParser::parseAnnots(PcpObject* object, PcpAst* objectAst)
{
  int numAnnots = objectAst->getNumAnnots();
  int i;
  for(i = 0; i < numAnnots; i++)
    {
      PcpAst* annotAst = objectAst->getAnnot(i);
      PcpAnnotTerm* annot = pcpParseAnnotTerm(annotAst);
      if(annot != NULL)
	{
	  object->addAnnot(annot);
	}
    }
}

PcpParameter* 
PcpParser::parseParameter(PcpAst* parameter)
{
  PcpToken* token = parameter->getToken();
  int numChildren = parameter->getNumChildren();
  PcpToken* nameToken = parameter->getName();
  PcpParameter* result = NULL;
  const char* name = nameToken != NULL ? nameToken->getString() : NULL;

  if(!token->isParameter())
    {
      pcpParseError("Expected parameter construct");
      return NULL;
    }

  if(numChildren != 0)
    {
      pcpParseError
	("Wrong number of arguments to parameter, expected zero");
      return NULL;
    }

  if(name == NULL)
    {
      pcpParseError("No name given to parameter");
      return NULL;
    }

  result = new PcpParameter(name);
  this->parseCommonObjectAttributes(parameter, result);
  
  return result;
}

PcpScop* 
PcpParser::pcpAstParseTopLevel(PcpAst* program)
{
  PcpToken* token = program->getToken();
  int numChildren = program->getNumChildren();
  int i;

  if(!token->isSequence())
    {
      pcpParseError("Program is not a sequence of statements");
    }

  for(i = 0; i < numChildren; i++)
    {
      PcpAst* child = program->getChild(i);
      PcpToken* childToken = child->getToken();

      // We allow parameter, array and scop constructs. 
      if(childToken->isVariable())
	this->parseVariable(child);
      else if(childToken->isArray())
	this->parseArrayType(child);
      else if(childToken->isParameter())
	this->parseParameter(child);
      else if(childToken->isScop())
	return this->parseScop(child);
      else
	pcpParseError("Illegal top level construct.");
    }

  pcpParseError("No scop found in source");
  return NULL;
}

PcpParser::PcpParser()
{
  PcpSymtab* symtab = new PcpSymtab();
  this->setSymtab(symtab);
  this->setFileName("unknown");
  this->setAddLineAnnots(true);
}

PcpScop* 
PcpParser::parse(const char* source)
{
  PcpTokenizer* tokenizer = new PcpTokenizer(source);
  PcpAst* ast = PcpAst::parse(tokenizer);
  PcpScop* scop;
  PcpParser* parser = new PcpParser();
  scop = parser->pcpAstParseTopLevel(ast);
  return scop;
}

PcpScop* 
PcpParser::parseFile(const char* filename)
{
  char* buffer;
  FILE* file = fopen(filename, "r");
  long size;
  long readsize;
  if(file == NULL)
    {
      pcpParseError("Unable to open file");
      return NULL;
    }
  
  fseek(file , 0 , SEEK_END);
  size = ftell(file);
  rewind(file);
  
  buffer =(char*) PCP_NEWVEC(char, size);
  if(buffer == NULL) 
    {
      pcpParseError("Memory error"); 
      return NULL;
    }
  
  readsize = fread(buffer,1,size,file);
  if(readsize != size) 
    {
      pcpParseError("Unable to read file");
      return NULL;
    }
  return PcpParser::parse(buffer);
}



