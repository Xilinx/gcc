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
#include <cilk/cilk_api.h>

void foo(volatile int *);

void main2(void);

int main(void)
{
  __cilkrts_set_param ("nworkers", "2");
  main2();

  fprintf(stdout,"Out of CILK.\n");
  fflush(stdout);
  return 0;
}


void main2(void)
{
  int sundari = 0;

  _Cilk_spawn foo(&sundari);

  sundari=1;
  sundari=5;
  sundari=0;
  sundari=4;

 /*  _Cilk_sync; */
  
  fprintf(stdout,"Steal Occurred!\n");
  fflush(stdout);
  return;
}

void foo(volatile int *Gayathri)
{/*
  printf("Entering Foo!\n");
  printf("*Gayathri = %d\n", *Gayathri);
  fflush(stdout);
*/
  while (*Gayathri == 0)
  {
/*
    fprintf(stderr,"*Gayathri = %d\n", *Gayathri);
    fflush(stderr);
*/
   ;
  }

  printf("Leaving Foo!\n");
  /* printf("*Gayathri = %d\n", *Gayathri); */
  fflush(stdout);
}


