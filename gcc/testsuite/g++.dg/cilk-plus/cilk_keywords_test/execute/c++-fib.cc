/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

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
  int *fibNumbers_Serial;
  int createNumbers(int currentNumber);
  int createNumbers_Serial(int currentNumber);
public:
  Fib(int n);
  ~Fib();
  void populateArray(int n);
  void CheckNumbers(int n);
};

Fib::Fib(int n)
{
  fibNumbers = new int[n+1];
  fibNumbers_Serial = new int[n+1];
}

Fib::~Fib()
{
  delete fibNumbers;
  delete fibNumbers_Serial;
}

void Fib::populateArray(int n)
{
  for (int ii = 1; ii <= n; ii++)
  {
    fibNumbers[ii] = createNumbers(ii);
  }
  for (int ii = 1; ii <= n; ii++)
  {
    fibNumbers_Serial[ii] = createNumbers_Serial(ii);
  }
}

void Fib::CheckNumbers(int n)
{
  for (int ii = 1; ii <= n; ii++)
    if (fibNumbers_Serial[ii] != fibNumbers[ii])
      abort ();
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

int Fib::createNumbers_Serial (int currentNumber)
{
  if (currentNumber < 2)
    return currentNumber;
  else
    {
      int x, y;
      x = createNumbers_Serial (currentNumber-1);
      y = createNumbers_Serial (currentNumber-2);
      return (x+y);
    }
  return 0;
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
  fib.CheckNumbers(number_of_iterations);
  exit (0);
  return 0;
}
  

