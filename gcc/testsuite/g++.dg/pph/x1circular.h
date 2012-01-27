#ifndef X1_CIRCULAR_H
#define X1_CIRCULAR_H
/* We are purposely generating a circular #include chain.  Neither
   header will be able to open the other one as their images are
   being generated.  */
#include "x2circular.h" // { dg-warning "cannot open PPH file x2circular.pph.*" }
int foo(int, int);
#endif
