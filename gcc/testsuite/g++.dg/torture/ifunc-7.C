/* { dg-do compile } */

static int __attribute__ ((ifunc)) foo (int);

/* { dg-error "indirect function .int foo.int.. never defined" ""  { target *-*-* } 3 } */
