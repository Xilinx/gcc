// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x5rtti1.h:13:29: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti1.h:13:51: error: no match for .operator" "" { xfail *-*-* } 0 }


//FIXME We should make this a run test.

#include "x5rtti1.h"

int main()
{
    return poly1() != nonp1() && hpol1() == hnpl1();
}
