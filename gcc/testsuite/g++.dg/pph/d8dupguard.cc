#include "c0dupguard1.h"
#include "c0dupguard2.h" // { dg-error "fails macro validation" "" { xfail *-*-* } }
int foo() { return x; }
