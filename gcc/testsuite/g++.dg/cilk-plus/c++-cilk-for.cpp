#include <iostream> 
#include <cstdio>
#include <cstring>

#include <cilk/cilk.h>  
#define NUMBER 50
using namespace std;

class CClass {
private: 
  int x[NUMBER],y[NUMBER],r[NUMBER];
public:
  void PrintValues();
  CClass (int, int);

#if 1
  int SomeCalc();
  ~CClass ();
  virtual int some_function(int a, int b) {}; 
#endif

};

CClass::CClass(int a, int b) 
{
  cilk_for (int ii = 0; ii < NUMBER; ++ii) {
    x[ii]=5;
    y[ii]=9;
    r[ii]=33;
  }
}
#if 1
CClass::~CClass() 
{
  cilk_for (int ii = 0; ii < NUMBER; ii++) {
    x[ii]=0;
    y[ii]=0;
    r[ii]=0;
  }
}

int CClass::SomeCalc()
{
  cilk_for (int jj = 0;jj  < NUMBER; jj++) {
    r[jj] = x[jj] * y[jj];
  }
  
  return (r[9]+r[8]+r[7]+r[6]+r[5]+r[4]+r[3]+r[2]+r[1]+r[0]);
}
#endif 
void CClass::PrintValues()
{
  for(int ii = 0; ii < NUMBER; ii++)
  {
    printf("X[%2d]=%2d Y[%2d]=%2d r[%2d]=%3d\n",ii,x[ii],ii,y[ii],ii,r[ii]); 
  }
  return;
}

int main(void) 
{
  CClass vars(5,9);
  
  cout << "Array values BEFORE The Calculation: " << endl;
  vars.PrintValues();
  
  vars.SomeCalc();  


  cout << "Array values AFTER The Calculation: " << endl;
  vars.PrintValues();
  return 5;
}

