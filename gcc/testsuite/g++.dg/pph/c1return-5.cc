// { dg-options "-mpreferred-stack-boundary=4" }
// { dg-final { scan-assembler-not "and\[lq\]?\[^\\n\]*-64,\[^\\n\]*sp" } }

#include "c1return-5.h"
