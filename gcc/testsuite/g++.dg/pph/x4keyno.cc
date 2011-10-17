// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4keyno.cc:15:1: internal compiler error: in cgraph_analyze_functions, at cgraphunit.c:1193" "" { xfail *-*-* } 0 }
// { dg-excess-errors "typeinfo name duplicatd" }

// Previously.
// The variable for the typeinfo name for 'keyno' is duplicated.

#include "x0keyno1.h"
#include "x0keyno2.h"

int main()
{
    keyno variable;
    return variable.mix( 3 );
}
