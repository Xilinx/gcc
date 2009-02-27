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

#ifndef _PCP_TESTER_H_
#define _PCP_TESTER_H_

#include "pcp_dynamic_array.h"

class PcpTester
{
protected:
  // Options class
  class Option
  {
  protected:
    enum Action
    {
      PCP_TESTER_OPTION_UNKNOWN,
      PCP_TESTER_OPTION_IDENTITY,
      PCP_TESTER_OPTION_HELP
    };

    Action action;

    void setAction(Action action);
    Action getAction();
    
    Option(Action action);

  public:
    static Option* getIdentity();
    static Option* getHelp();
    
    bool isUnknown();
    bool isIdentity();
    bool isHelp();

    Option();
  };

  // Report expected command syntax.
  void reportCommandLineInfo();

  // Return true if STRING is an option string (starting with '--')
  bool isOptionString(const char* string);

  // Parse option.
  Option* parseOption(const char* optionString);

  // Parse options.
  PcpArray<Option*>* parseOptions(int argc, char** argv);

  // Parse file name
  const char* parseFileName(int arc, char** argv);

  // Compare strings
  bool compareScopStrings(const char* str1, const char* str2);

  // Run identity test
  bool runIdentity(const char* filename);

  // Start the tester.
  bool start(const char* filename, PcpArray<Option*>* options);


  public:
  
  // Entry point to the tester.
  bool run(int argc, char** argv);
};

#endif // _PCP_TESTER_H_
