//***************************************************************************
//                                                                           
// File: steal_check.c                                                       
//                                                                           
// Created: Tue Mar 29 16:09:27 2011                                         
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

extern void __cilkrts_set_param (char *x, char *y);

void foo(volatile int *);

void main2(void);

int main(void)
{
  __cilkrts_set_param ("nworkers", "2");
  main2();
  return 0;
}


void main2(void)
{
  int sundari = 0;

  _Cilk_spawn foo(&sundari);

  sundari=1;
  sundari=5;
  sundari=3;
  sundari=4;

  _Cilk_sync; 
  return;
}

void foo(volatile int *Gayathri)
{
  while (*Gayathri == 0)
  {
   ;
  }
}


