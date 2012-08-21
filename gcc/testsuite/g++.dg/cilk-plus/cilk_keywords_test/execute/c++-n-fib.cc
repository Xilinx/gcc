/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>
#include <cilk/cilk.h>
#define NUMBER 30

using namespace std;

class Fib
{
private:
  int iterations;
  int *fibNumbers;
  int *fibNumbers_Serial;
  int n;
  int createNumbers(int currentNumber);
  int createNumbers_Serial(int currentNumber);
public:
  Fib(int nn, int iter);
  ~Fib();
  void populateArray();
  void CheckNumbers();
};

Fib::Fib(int nn, int iter)
{
  fibNumbers = new int[nn];
  fibNumbers_Serial = new int[nn];

  for (int ii = 0; ii < nn; ii++) {
    fibNumbers[ii] = 0;
    fibNumbers_Serial[ii] = 0;
  }
  iterations=iter;
  n=nn;
}

Fib::~Fib()
{
  delete fibNumbers;
  delete fibNumbers_Serial;
}

void Fib::populateArray()
{
  for (int ii = 0; ii < n; ii++)
  {
    fibNumbers[ii] = createNumbers(ii);
    fibNumbers_Serial[ii] = createNumbers_Serial(ii);
  }
}

void Fib::CheckNumbers()
{
  for (int ii = 0; ii < n; ii++)
    if (fibNumbers_Serial[ii] != fibNumbers[ii])
      abort ();
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

int Fib::createNumbers_Serial(int currentNumber)
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

    for (int ii = 0; ii < iterations; ii++) {
      x[ii]= createNumbers(currentNumber-ii-1);
    }
    for (int ii = 0; ii < iterations; ii++) {
      y += x[ii];
    }
    return y;
  }
}
int main(int argc, char **argv)
{

  int iter = 0;
  int number_of_fibs = 0;
#if 0
  if (argc == 3)
  {
    iter = atoi (argv[1]);
    number_of_fibs = atoi(argv[2]);
  }
  else if (argc == 2)
  {
    iter=atoi(argv[1]);
    number_of_fibs = NUMBER;
  }
  else
#endif
  {
    iter = 1;
    number_of_fibs = NUMBER;
  }
  
    
       
  Fib fib(number_of_fibs,iter);
  fib.populateArray();
  fib.CheckNumbers();
  return 0;
}
  
