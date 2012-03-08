// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4keyno.cc:13:1: internal compiler error: in cgraph_analyze_functions, at cgraphunit.c:1210" "" { xfail *-*-* } 0 }

// was asm xokay 32642

#include "x0keyno1.h"
#include "x0keyno2.h"

int main()
{
    keyno variable;
    return variable.mix( 3 );
}
