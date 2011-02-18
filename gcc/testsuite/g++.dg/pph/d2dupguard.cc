#include "c2dupguard1.h"
#include "c2dupguard2.h"
// { dg-error "duplicate PPH guard header" "" { xfail *-*-* } }
int foo() { return x; }
