#include "c2dupguard1.h"
#include "c2dupguard2.h" // { dg-error "fails macro validation" "" { xfail *-*-* } }
int foo() { return x; }
