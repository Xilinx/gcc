// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1keyno.cc:10:1: internal compiler error: in cgraph_analyze_functions, at cgraphunit.c:1193" "" { xfail *-*-* } 0 }

#include "x0keyno1.h"

int main()
{
    keyno variable;
    return variable.mix( 3 );
}
