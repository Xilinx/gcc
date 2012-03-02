// {    xfail-if "BOGUS RTTI" { "*-*-*" } { "-fpph-map=pph.map" } }
// FIXME pph - We should make this a run test.

#include "x5rtti1.h"

int main()
{
    bool a = poly1() != nonp1(); // { dg-bogus "no match for 'operator!='" "" { xfail *-*-* } }
    bool b = hpol1() == hnpl1(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    return !(a && b);
}
