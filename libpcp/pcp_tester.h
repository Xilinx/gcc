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

  class PcpTest
  {
  public:
    virtual bool run(const char* filename) = 0;
    virtual const char* getFlagName() = 0;
    virtual const char* getDescription() = 0;
    virtual bool isHelp();
  };

  class PcpTestHelp : public PcpTest
  {
  protected:
    PcpArray<PcpTest*>* tests;

    virtual void setTests(PcpArray<PcpTest*>* tests);
    virtual PcpArray<PcpTest*>* getTests();

  public:
    virtual bool run(const char* filename);
    virtual const char* getFlagName();
    virtual const char* getDescription();
    PcpTestHelp(PcpArray<PcpTest*>* tests);
    virtual bool isHelp();
  };

  class PcpTestIdentity : public PcpTest
  {
  protected:
    bool compareScopStrings(const char* str1,
			    const char* str2);

  public:
    virtual bool run(const char* filename);
    virtual const char* getFlagName();
    virtual const char* getDescription();
    PcpTestIdentity();
  };

  class PcpTestScalarOrder : public PcpTest
  {
  public:
    virtual bool run(const char* filename);
    virtual const char* getFlagName();
    virtual const char* getDescription();
    PcpTestScalarOrder();
  };

  class PcpTestExprCanonicalize : public PcpTest
  {
  public:
    virtual bool run(const char* filename);
    virtual const char* getFlagName();
    virtual const char* getDescription();
    PcpTestExprCanonicalize();
  };

  class PcpTestBuildDomain : public PcpTest
  {
  public:
    bool run(const char* filename);
    const char* getFlagName();
    const char* getDescription();
    PcpTestBuildDomain();
  };

  class PcpTestBuildScattering : public PcpTest
  {
  public:
    bool run(const char* filename);
    const char* getFlagName();
    const char* getDescription();
    PcpTestBuildScattering();
  };

  
  PcpArray<PcpTest*>* tests;

  void setTests(PcpArray<PcpTest*>* tests);
  PcpArray<PcpTest*>* getTests();


  // Report expected command syntax.
  void reportCommandLineInfo();

  // Return true if STRING is an option string (starting with '--')
  bool isOptionString(const char* string);

  // Parse option.
  PcpTest* parseOption(const char* optionString);

  // Parse options.
  PcpArray<PcpTest*>* parseOptions(int argc, char** argv);

  // Parse file name
  const char* parseFileName(int arc, char** argv);

  // Compare strings
  bool compareScopStrings(const char* str1, const char* str2);

  // Start the tester.
  bool start(const char* filename, PcpArray<PcpTest*>* options);

  public:
  
  // Entry point to the tester.
  bool run(int argc, char** argv);

  // Constructor
  PcpTester();
};

#endif // _PCP_TESTER_H_
