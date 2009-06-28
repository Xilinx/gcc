/* { dg-do compile } */

extern int foo1 (int, int); 
extern int foo2 (int, int); 

int
__attribute__ ((ifunc))
foo (int x, int y)
{ 
  if ((x + y) == 34)
    return foo1;
  else
    return foo2;
}

/* { dg-error "parameter .\[x|y\]. used in indirect function .foo." ""  { target *-*-* } 8 } */
