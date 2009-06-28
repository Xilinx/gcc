/* { dg-do compile } */

static int __attribute__ ((ifunc)) foo (int);

/* { dg-error "indirect function .foo. never defined" ""  { target *-*-* } 3 } */
