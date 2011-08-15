//***************************************************************************
//                                                                           
// File: varargs_test.cpp                                                    
//                                                                           
// Created: Thu Apr 21 14:15:23 2011                                         
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
#include <cstdarg>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

double compute_total (int no_elements, ...);

int main(int argc, char **argv)
{
  double array[5] = {5.0, 4.0, 9.0, 3.0, 4.0};
  double array2[5] = {5.0, 6.0, 8.0, 6.0};
  double yy=0, xx=0;
  yy = _Cilk_spawn compute_total(5,array[0],array[1],array[2],
                                 array[3], array[4]);
  xx= compute_total(4,array2[0],array2[1],array2[2], array2[3]);
  
  _Cilk_sync;

  printf("Sum of XX & YY(Should be 50) = %lf\n", xx+yy);
 
  return 0;
  
}


double compute_total (int no_elements, ...)
{
  double total = 0;
  va_list args;
  va_start(args, no_elements);

  for (int ii = 0; ii < no_elements; ii++)
  {
    total += va_arg(args,double);
  }
  va_end(args);

  return total;
}

