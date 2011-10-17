// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4keyed.cc:14:1: internal compiler error: in cgraph_analyze_functions, at cgraphunit.c:1193" "" { xfail *-*-* } 0 }
// { dg-excess-errors "callgraph problems" }

#include "x0keyed1.h"
#include "x0keyed2.h"

int keyed::key( int arg ) { return mix( field & arg ); }

int main()
{
    keyed variable;
    return variable.key( 3 );
}
