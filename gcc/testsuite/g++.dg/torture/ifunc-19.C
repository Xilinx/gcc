/* { dg-do compile } */

extern int foo1 (int);
extern int foo (int) __attribute__ ((ifunc));

int
foo (int)
{ 
  return foo1;
}

/* { dg-error "definition of function .int foo.int.. conflicts with" ""  { target *-*-* } 7 } */
/* { dg-error "previous declaration of indirect function .int foo.int.. here" ""  { target *-*-* } 4 } */
/* { dg-error "invalid conversion from .int ....int.. to .int." ""  { target *-*-* } 9 } */
/* { dg-error "indirect function .int foo.int.. never defined" ""  { target *-*-* } 4 } */
