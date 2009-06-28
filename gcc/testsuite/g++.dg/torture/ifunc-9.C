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

/* { dg-error "invalid conversion from .int. to .int ....int, int.." ""  { target *-*-* } 7 } */
/* { dg-error "invalid conversion from .int ....int, int.. to .void ....int, int.." ""  { target *-*-* } 16 } */
