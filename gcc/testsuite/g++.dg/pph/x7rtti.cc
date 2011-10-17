// FIXME pph: This should be a { dg=do run } (with '=' replaced by '-')
// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x7rtti.cc:22:1: internal compiler error: in cgraph_analyze_functions, at cgraphunit.c:1193" "" { xfail *-*-* } 0 }
// { dg-excess-errors "macro redefinition and operator match problems" }

#include "x5rtti1.h"
#include "x5rtti2.h"

int main()
{
    if (poly1() == poly2()
	&& nonp1() == nonp2()
        && hpol1() == hpol2()
	&& hnpl1() == hnpl2()
        && poly1() != nonp1()
	&& hpol1() == hnpl1()
        && poly2() != nonp2()
	&& hpol2() == hnpl2())
      return 0;
    else
      return 1;
}
