//***************************************************************************
//                                                                           
// File: stl_test.cpp                                                        
//                                                                           
// Created: Thu Apr 28 15:04:45 2011                                         
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
#include <vector>
#include <algorithm>
#include <list>


using namespace std;


int main(int argc, char **argv)
{
  vector <int> number_list;
  int new_number = 0;
  int no_elements = 0;
  
  if (argc != 2)
  {
    printf("Usage: %s <Number Of Elements in List>\n",argv[0]);
    return -1;
  }

  no_elements = atoi(argv[1]);

  number_list.clear();
  for (int ii = 0; ii < no_elements; ii++)
  {
    number_list.push_back(new_number);
  }

  cilk_for (int jj = 0; jj < no_elements; jj++)
  {
    number_list[jj] = jj + no_elements;
  }

  for (int kk = 0; kk <number_list.size(); kk++)
    cout << "number_list[" << kk <<"] = " << number_list[kk] << endl;

  return 0;
}
