/* { dg-do compile { target { x32 || lp64 } } } */
/* { dg-options "-mno-sse" } */

double foo(void) { return 0; }	/* { dg-error "SSE disabled" } */
void bar(double x) { }
