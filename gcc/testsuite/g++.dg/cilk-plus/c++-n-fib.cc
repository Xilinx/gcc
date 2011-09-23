//*************************************************************************** //                                                                           
// File: c++-n-fib.cpp                                                         
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
#include <assert.h>
#include <cilk/cilk.h>
#define NUMBER 30

using namespace std;

class Fib
{
private:
  int iterations;
  int *fibNumbers;
  int n;
  int createNumbers(int currentNumber);
public:
  Fib(int nn, int iter);
  ~Fib();
  void populateArray();
  void PrintNumbers();
};

Fib::Fib(int nn,int iter)
{
  fibNumbers = new int[nn];

  for (int ii = 0; ii < nn; ii++)
    fibNumbers[ii] = 0;
  iterations=iter;
  n=nn;
}

Fib::~Fib()
{
  delete fibNumbers;
}

void Fib::populateArray()
{
  for (int ii = 0; ii < n; ii++)
  {
    fibNumbers[ii] = createNumbers(ii);
    // cout << "FibNumbers[" << ii << "]: " << fibNumbers[ii] << endl;
  }
}

void Fib::PrintNumbers()
{
  for (int ii = 0; ii < n; ii++)
  {
    cout << "Fib(" << (ii+1) << ") = " << fibNumbers[ii] << endl;
  }
  return;
}

int Fib::createNumbers(int currentNumber)
{
  if ((currentNumber < iterations) || (iterations < 2))
  {
    return currentNumber;
  }
  else
  {
    int *x, y = 0, z= 0;
    int input= 0;
    x = new int[iterations];
    assert (x != NULL);

    for (int ii = 0; ii < iterations; ii++) {
      x[ii]=cilk_spawn createNumbers(currentNumber-ii-1);
    }
    /*y = createNumbers(currentNumber-iterations-1);*/
    cilk_sync; 
    for (int ii = 0; ii < iterations; ii++) {
      y += x[ii];
    }
    return (y);
  }
}

int main(int argc, char **argv)
{

  int iter = 0;
  int number_of_fibs = 0;

  if (argc == 3)
  {
    iter = atoi (argv[1]);
    number_of_fibs = atoi(argv[2]);
  }
  else if (argc == 2)
  {
    iter=atoi(argv[1]);
    cout << "Assuming Number of Nibonacci = " << NUMBER << endl;
    number_of_fibs = NUMBER;
  }
  else
  {
    cerr << "<EXECUTABLE> <NUMBER OF PREVIOUS ITERATION SUMS>" << endl;
    cerr << "Assuming Iteration = 1 and Number of Nibonacci = " << NUMBER << endl;
    iter = 1;
    number_of_fibs = NUMBER;
  }
  
    
       
  Fib fib(number_of_fibs,iter);
  fib.populateArray();
  fib.PrintNumbers();
  return 0;
}
  
