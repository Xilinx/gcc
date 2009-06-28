/* { dg-do compile } */

int
__attribute__ ((ifunc))
foo (int, int)
{ 
  return 1;
}

extern int bar1 (int, int);

void
__attribute__ ((ifunc))
bar (int, int)
{ 
  return bar1;
}

/* { dg-warning "return makes pointer from integer without a cast" "" { target *-*-* } 7 } */
/* { dg-warning "return from incompatible pointer type" "" { target *-*-* } 16 } */
