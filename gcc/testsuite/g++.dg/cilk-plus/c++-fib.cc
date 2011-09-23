//***************************************************************************
//                                                                           
// File: c++-fib.cpp                                                         
//                                                                           
// Created: Tue Feb 15 13:45:06 2011                                         
//                                                                           
// Author: Balaji V. Iyer                                                    
//                                                                           
// $Id$                                                                      
//                                                                           
// Description:                                                              
//                                                                           
//***************************************************************************

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cilk/cilk.h>
#define NUMBER 40

using namespace std;

class Fib
{
private:
  int *fibNumbers;
  int createNumbers(int currentNumber);
public:
  Fib(int n);
  ~Fib();
  void populateArray(int n);
  void PrintNumbers(int n);
};

Fib::Fib(int n)
{
  fibNumbers = new int[n+1];
}

Fib::~Fib()
{
  delete fibNumbers;
}

void Fib::populateArray(int n)
{
  for (int ii = 1; ii <= n; ii++)
  {
    fibNumbers[ii] = createNumbers(ii);
  }
}

void Fib::PrintNumbers(int n)
{
  for (int ii = 1; ii <= n; ii++)
  {
    cout << "Fib(" << (ii) << ") = " << fibNumbers[ii] << endl;
  }
  return;
}

int Fib::createNumbers(int currentNumber)
{

  if(currentNumber < 2)
  {
    return currentNumber;
  }
  else
  {
    int x = 0, y = 0, z= 0;
    x = cilk_spawn createNumbers(currentNumber-1);
    y = createNumbers(currentNumber - 2);
    cilk_sync;
    return (x+y+z);
  }
}

int main(int argc, char **argv)
{
  int number_of_iterations;

  if (argc == 2)
  {
    number_of_iterations = atoi(argv[1]);
  }
  else
  {
    number_of_iterations = NUMBER;
  } 

  Fib fib(number_of_iterations);

  fib.populateArray(number_of_iterations);

  fib.PrintNumbers(number_of_iterations);
  return 0;
}
  

