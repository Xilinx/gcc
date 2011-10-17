// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x6rtti.cc:14:1: internal compiler error: in cgraph_analyze_functions, at cgraphunit.c:1193" "" { xfail *-*-* } 0 }
// { dg-excess-errors "callgraph problems" }

// Previously:
// "BOGUS POSSIBLE TYPE MISMERGE"
//FIXME We should make this a run test.

#include "x5rtti1.h"

int main()
{
    return poly1() != nonp1() && hpol1() == hnpl1();
}
