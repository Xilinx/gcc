#include <stdlib.h> // { dg-error "fails macro validation" "" { xfail *-*-* } }
// { dg-excess-errors "In file included from" { xfail *-*-* } }
// { dg-excess-errors "regular compilation failed" { xfail *-*-* } }

int f(const char* s)
{
    return atoi(s);
}
