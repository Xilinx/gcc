/* { dg-do compile } */
/* { dg-options "-Wall -Wextra" } */

static int
__attribute__ ((ifunc))
foo (int, int)
{ 
}

/* { dg-error "control reaches end of indirect function" ""  { target *-*-* } 8 } */
