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

// As a special exception, if you link this library with other files, so me
// of which are compiled with GCC, to produce an executable, this library
// does not by itself cause the resulting executable to be covered by the
// GNU General Public License.  This exception does not however invalidate
// any other reasons why the executable file might be covered by the GNU
// General Public License.  

#include "pcp_error.h"
#include "pcp_parser.h"
#include "pcp_dynamic_array.h"
#include "pcp_tester.h"
#include "pcp_emitter.h"
#include "pcp_scalar_order.h"
#include "pcp_expr_canonicalizer.h"
#include "pcp_domain.h"
#include "pcp_scattering.h"

// Tester Option Class

// Set action to ACTION.

bool PcpTester::PcpTest::isHelp()
{
  return false;
}

void
PcpTester::PcpTestHelp::setTests(PcpArray<PcpTest*>* tests)
{
  this->tests = tests;
}

PcpArray<PcpTester::PcpTest*>*
PcpTester::PcpTestHelp::getTests()
{
  return this->tests;
}

bool
PcpTester::PcpTestHelp::run(const char* filename)
{
  PcpError::reportErrorNewline("Usage: pcp-tester --option filename.pcp");
  PcpError::reportErrorNewline("Possible options are:");
  int i;
  PcpArray<PcpTester::PcpTest*>* tests = this->getTests();
  for(i = 0; i < tests->getSize(); i++)
    {
      PcpTester::PcpTest* test = tests->get(i);
      if(test != this)
	{
	  PcpError::reportError("--");
	  PcpError::reportError(test->getFlagName());
	  PcpError::reportError(" - ");
	  PcpError::reportErrorNewline(test->getDescription());
	}
    }
}

const char*
PcpTester::PcpTestHelp::getFlagName()
{
  return "help";
}

const char*
PcpTester::PcpTestHelp::getDescription()
{
  return "Display this message";
}

bool
PcpTester::PcpTestHelp::isHelp()
{
  return true;
}

PcpTester::PcpTestHelp::PcpTestHelp(PcpArray<PcpTester::PcpTest*>* tests)
{
  this->setTests(tests);
}

// Run the identity test for the parser and emitter, return true
// if the test passes
bool
PcpTester::PcpTestIdentity::run(const char* filename)
{
  PcpScop* scop = PcpParser::parseFile(filename);
  if(scop == NULL)
    {
      PcpError::reportErrorNewline("Fatal error, aborting");
      return false;
    }

  const char* parsedScop = PcpEmitter::pcpScopToString(scop);
  const char* parsedScop2 = 
    PcpEmitter::pcpScopToString(PcpParser::parse(parsedScop));
  const char* parsedScop3 = 
    PcpEmitter::pcpScopToString(PcpParser::parse(parsedScop2));
  bool compareSuccess = compareScopStrings(parsedScop2, parsedScop3);
  if(!compareSuccess)
    {
      PcpError::reportError("Identity check failed for file: ");
      PcpError::reportErrorNewline(filename);
    }
  return compareSuccess;
}

const char*
PcpTester::PcpTestIdentity::getFlagName()
{
  return "identity";
}

const char*
PcpTester::PcpTestIdentity::getDescription()
{
  return "Identity test for the parser and emitter";
}

PcpTester::PcpTestIdentity::PcpTestIdentity()
{
}

bool
PcpTester::PcpTestScalarOrder::run(const char* filename)
{
  PcpScop* scop = PcpParser::parseFile(filename);
  
  if(scop == NULL)
    {
      PcpError::reportErrorNewline("Fatal error, aborting");
      return false;
    }
  
  PcpScalarOrder order(scop);
  
  return true;
}

const char*
PcpTester::PcpTestScalarOrder::getFlagName()
{
  return "scalarorder";
}

const char*
PcpTester::PcpTestScalarOrder::getDescription()
{
  return "test ordering of scalars (ivs and parameters)";
}

PcpTester::PcpTestScalarOrder::PcpTestScalarOrder()
{
}

bool
PcpTester::PcpTestExprCanonicalize::run(const char* filename)
{
  PcpScop* scop = PcpParser::parseFile(filename);

  if(scop == NULL)
    {
      PcpError::reportErrorNewline("Fatal error, aborting");
      return false;
    }

  PcpScalarOrder order(scop);
  printf("SCOP BEFORE CANONICALIZATION:\n");
  printf("%s", PcpEmitter::pcpScopToString(scop));
  PcpExprCanonicalizer canonicalizer(&order);
  canonicalizer.canonicalize(scop);
  printf("SCOP AFTER CANONICALIZATION:\n");
  printf("%s", PcpEmitter::pcpScopToString(scop));
  return true;
}

const char*
PcpTester::PcpTestExprCanonicalize::getFlagName()
{
  return "canonicalize";
}

const char*
PcpTester::PcpTestExprCanonicalize::getDescription()
{
  return "Test canonicalization of expressions";
}

PcpTester::PcpTestExprCanonicalize::PcpTestExprCanonicalize()
{
}

bool
PcpTester::PcpTestBuildDomain::run(const char* filename)
{
  PcpScop* scop = PcpParser::parseFile(filename);

  if(scop == NULL)
    {
      PcpError::reportErrorNewline("Fatal error, aborting");
      return false;
    }

  PcpScalarOrder order(scop);
  //printf("SCOP BEFORE CANONICALIZATION:\n");
  //printf("%s", PcpEmitter::pcpScopToString(scop));
  PcpExprCanonicalizer canonicalizer(&order);
  canonicalizer.canonicalize(scop);
  printf("SCOP AFTER CANONICALIZATION:\n");
  printf("%s", PcpEmitter::pcpScopToString(scop));

  printf("DomainExpressions:\n");
  PcpDomainMap map(scop, &canonicalizer);

  return true;
}

const char*
PcpTester::PcpTestBuildDomain::getFlagName()
{
  return "builddomain";
}

const char* 
PcpTester::PcpTestBuildDomain::getDescription()
{
  return "Test building of statment domains";
}

PcpTester::PcpTestBuildDomain::PcpTestBuildDomain()
{
}

bool
PcpTester::PcpTestBuildScattering::run(const char* filename)
{
  PcpScop* scop = PcpParser::parseFile(filename);

  if(scop == NULL)
    {
      PcpError::reportErrorNewline("Fatal error, aborting");
      return false;
    }
  printf("ScatteringExpressions:\n");
  PcpScatteringMap map(scop);
  return true;
}

const char*
PcpTester::PcpTestBuildScattering::getFlagName()
{
  return "buildscattering";
}

const char* 
PcpTester::PcpTestBuildScattering::getDescription()
{
  return "Test building of scattering functions";
}

PcpTester::PcpTestBuildScattering::PcpTestBuildScattering()
{
}


void
PcpTester::reportCommandLineInfo()
{
  PcpError::reportErrorNewline ("Expected syntax: pcptester <action> filename");
}

// Return true if STRING is an option string (starting with '--')

bool 
PcpTester::isOptionString(const char* string)
{
  // Option syntax: --option
  int length = strlen(string);
  return length >= 3 && string[0] == '-' && string[1] == '-';
}

// Parse OPTIONSTRING and return the corresponding option, if parsing fails
// return NULL

PcpTester::PcpTest* 
PcpTester::parseOption(const char* optionString)
{
  // Option syntax: --option

  if(!isOptionString(optionString))
    {
      PcpError::reportError("Illegal option string:");
      PcpError::reportErrorNewline(optionString);
      return NULL;
    }

  PcpArray<PcpTester::PcpTest*>* tests = this->getTests();
  for(int i = 0; i < tests->getSize(); i++)
    {
      PcpTester::PcpTest* test = tests->get(i);
      if(strcmp(test->getFlagName(), &(optionString[2])) == 0)
	return test;
    }

  PcpError::reportError("Unknown option:");
  PcpError::reportErrorNewline(optionString);
  PcpTestHelp testHelp(this->getTests());
  testHelp.run(NULL);
  return NULL;
}

// Parse the different options given the argument count and strings from 
// command line
//
// Expected syntax:
// pcptester [options] filename
//
// Where options may be one or more of the following:
//
// --identity
// --help

PcpArray<PcpTester::PcpTest*>*
PcpTester::parseOptions(int argc, char** argv)
{
  PcpDynamicArray<PcpTester::PcpTest*>* options = 
    new PcpDynamicArray<PcpTester::PcpTest*>(1);

  int currentArg = 1;

  // Parse options
  while(currentArg < argc - 1)
    {
      const char* arg = argv[currentArg];
      PcpTester::PcpTest* option = parseOption(arg);
      if(option != NULL)
	options->add(option);
      else
	{
	  delete options;
	  return NULL;
	}
      currentArg++;
    }
  return options;
}

// Get the file name from the command arguments

const char*
PcpTester::parseFileName(int argc, char** argv)
{
  return argv[argc -1];
}

// Compare two strings, return true if they are equal,
// false otherwise. 

bool
PcpTester::PcpTestIdentity::compareScopStrings(const char* str1,
					       const char* str2)
{
  int i = 0;
  while (str1[i] != '\0' && str2[i] != '\0'
         && str1[i] == str2[i])
    {
      i = i + 1;
    }
  if (str1[i] == '\0' && str2[i] == '\0')
    return true;
  return false;
}

void
PcpTester::setTests(PcpArray<PcpTester::PcpTest*>* tests)
{
  this->tests = tests;
}

PcpArray<PcpTester::PcpTest*>*
PcpTester::getTests()
{
  return this->tests;
}

// Start the tester with given FILENAME and OPTIONS
bool
PcpTester::start(const char* filename, PcpArray<PcpTester::PcpTest*>* options)
{

  if(filename == NULL || options == NULL)
    {
      PcpError::reportErrorNewline("Bad arguments to tester, aborting");
      this->reportCommandLineInfo();
      return false;
    }

  if(options->getSize() < 1)
    {
      PcpError::reportErrorNewline("No actions given to tester");
      this->reportCommandLineInfo();
      return false;
    }


  bool success = true;
  PcpIterator<PcpTester::PcpTest*>* iter = options->getIterator();
  for (;iter->hasNext(); iter->next())
    {
      PcpTester::PcpTest* test = iter->get();
      test->run(filename);
    }
  delete iter;

  return success;
}

// Start method for the tester.

bool
PcpTester::run(int argc, char** argv)
{
  PcpArray<PcpTester::PcpTest*>* options;
  const char* filename;

  if(argc < 2)
    {
      PcpError::reportError("Too few arguments");
      this->reportCommandLineInfo();
      return false;
    }

  // Special case for handling "pcptester --help"
  if(argc == 2 
     && this->isOptionString(argv[1]))
    {
      PcpTest* test = this->parseOption(argv[1]);
      if(test->isHelp())
	{
	  test->run(NULL);
	  return true;
	}
      else
	{
	  PcpError::reportErrorNewline("Error: No filename given");
	  PcpError::reportErrorNewline("Usage: pcp-tester --option filename.pcp");
	  return false;
	}
    }

  options = parseOptions(argc, argv);
  if(options == NULL)
    return false;
 
  
  filename =  this->parseFileName(argc, argv);

  return this->start(filename, options);
}

PcpTester::PcpTester()
{
  PcpDynamicArray<PcpTest*>* tests = new PcpDynamicArray<PcpTest*>();
  tests->add(new PcpTestHelp(tests));
  tests->add(new PcpTestIdentity());
  tests->add(new PcpTestScalarOrder());
  tests->add(new PcpTestExprCanonicalize());
  tests->add(new PcpTestBuildDomain());
  tests->add(new PcpTestBuildScattering());
  this->setTests(tests);
}

// Main function for the pcptester

int main(int argc, char** argv)
{
  PcpTester* tester = new PcpTester();
  bool result = tester->run(argc, argv);
  delete tester;
  return result == false;
}
