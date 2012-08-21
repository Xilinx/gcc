//***************************************************************************
//                                                                           
// File: fib.c                                                               
//                                                                           
// Created: Tue Oct 26 16:24:01 2010                                         
//                                                                           
// Author: Balaji V. Iyer                                                    
//                                                                           
// $Id$                                                                      
//                                                                           
// Description:$Log$                                                         
//                                                                           
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cilk/cilk.h>

int fib(int );

int main(int argc, char **argv)
{
  int x = 0;
  for (int ii = 0; ii < 10; ii++)
  {
    printf("Fib(%d) = %d\n", ii, fib(ii)); 
   x += fib(ii);
  }

  return x;
}


int fib(int n)
{
  int x = 0, y = 0;
  if (n < 2)
  {
    return n;
  }
  else
  {
    x = cilk_spawn fib(n-1);
    y = fib(n-2);
    cilk_sync; 
    return (x+y);
  }
}
