/* { dg-do compile } */

extern int foo1 (int);
extern int foo (int) __attribute__ ((ifunc));

int
foo (int)
{ 
  return foo1;
}

/* { dg-error "definition of function .foo. conflicts with" ""  { target *-*-* } 7 } */
/* { dg-error "previous declaration of indirect function .foo. here" ""  { target *-*-* } 4 } */
