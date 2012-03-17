// FIXME pph: This should be a { dg=do run } (with '=' replaced by '-')
// { xfail-if "BOGUS RTTI" { "*-*-*" } { "-fpph-map=pph.map" } }

#include "x5rtti1.h"
#include "x5rtti2.h"

int main()
{
    bool a = poly1() == poly2(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    bool b = nonp1() == nonp2(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    bool c = hpol1() == hpol2(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    bool d = hnpl1() == hnpl2(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    bool e = poly1() != nonp1(); // { dg-bogus "no match for 'operator!='" "" { xfail *-*-* } }
    bool f = hpol1() == hnpl1(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    bool g = poly2() != nonp2(); // { dg-bogus "no match for 'operator!='" "" { xfail *-*-* } }
    bool h = hpol2() == hnpl2(); // { dg-bogus "no match for 'operator=='" "" { xfail *-*-* } }
    return !(a && b && c && d && e && f && g && h);
}
