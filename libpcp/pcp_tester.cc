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
#include "pcp_parser.h"
#include "pcp_dynamic_array.h"
#include "pcp_tester.h"
#include "pcp_emitter.h"

// Tester Option Class

// Set action to ACTION.

void 
PcpTester::Option::setAction(PcpTester::Option::Action action)
{
  this->action = action;
}

// Get action.

PcpTester::Option::Action
PcpTester::Option::getAction()
{
  return this->action;
}

// Create a new option with given ACTION

PcpTester::Option::Option(Action action)
{
  this->setAction(action);
}

// Create a new identity option
PcpTester::Option* 
PcpTester::Option::getIdentity()
{
  return new PcpTester::Option(PCP_TESTER_OPTION_IDENTITY);
}

// Create a new help option

PcpTester::Option*
PcpTester::Option::getHelp()
{
  return new PcpTester::Option(PCP_TESTER_OPTION_HELP);
}    

// Return true if this option is identity

bool
PcpTester::Option::isIdentity()
{
  return this->getAction() == PCP_TESTER_OPTION_IDENTITY;
}

// Return true if this option is help

bool
PcpTester::Option::isHelp()
{
  return this->getAction() == PCP_TESTER_OPTION_HELP;
}

// Return true if this option is unknown

bool 
PcpTester::Option::isUnknown()
{
  return this->getAction() == PCP_TESTER_OPTION_UNKNOWN;
}

// Create new unknown option. This will not be used, but a default constructor
// must exist.
PcpTester::Option::Option()
{
  this->setAction(PCP_TESTER_OPTION_UNKNOWN);
}

// Print the expected command syntax in case of errors

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

PcpTester::Option*
PcpTester::parseOption(const char* optionString)
{
  // Option syntax: --option

  if(!isOptionString(optionString))
    {
      PcpError::reportError("Illegal option string:");
      PcpError::reportErrorNewline(optionString);
      return NULL;
    }
  
  if(strcmp("identity", &(optionString[2])) == 0)
    return PcpTester::Option::getIdentity();
  if(strcmp("help", &(optionString[2])) == 0)
    return PcpTester::Option::getHelp();
  else
    {
      PcpError::reportError("Unknown option:");
      PcpError::reportErrorNewline(optionString);
      return NULL;
    }
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

PcpArray<PcpTester::Option*>*
PcpTester::parseOptions(int argc, char** argv)
{
  PcpDynamicArray<PcpTester::Option*>* options = 
    new PcpDynamicArray<PcpTester::Option*>(1);

  int currentArg = 1;

  // Parse options
  while(currentArg < argc - 1)
    {
      const char* arg = argv[currentArg];
      PcpTester::Option* option = parseOption(arg);
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
PcpTester::compareScopStrings(const char* str1,
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

// Run the identity test for the parser and emitter, return true
// if the test passes

bool
PcpTester::runIdentity(const char* filename)
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

// Start the tester with given FILENAME and OPTIONS

bool
PcpTester::start(const char* filename, PcpArray<PcpTester::Option*>* options)
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
  PcpIterator<PcpTester::Option*>* iter = options->getIterator();
  for (;iter->hasNext(); iter->next())
    {
      PcpTester::Option* option = iter->get();
      if(option->isIdentity())
	success = this->runIdentity(filename);
      if(option->isHelp())
	this->reportCommandLineInfo();
    }
  delete iter;

  return success;
}

// Start method for the tester.

bool
PcpTester::run(int argc, char** argv)
{
  PcpArray<PcpTester::Option*>* options;
  const char* filename;

  if(argc < 2)
    {
      PcpError::reportError("Too few arguments");
      this->reportCommandLineInfo();
      return false;
    }

  // Special case for handling "pcptester --help"
  if(argc == 2 
     && this->isOptionString(argv[1])
     && this->parseOption(argv[1])->isHelp())
    {
      this->reportCommandLineInfo();
      return true;
    }

  options = parseOptions(argc, argv);
  if(options == NULL)
    return false;

  filename = parseFileName(argc, argv);
  return start(filename, options);
}

// Main function for the pcptester

int main(int argc, char** argv)
{
  PcpTester* tester = new PcpTester();
  bool result = tester->run(argc, argv);
  delete tester;
  return result == false;
}
