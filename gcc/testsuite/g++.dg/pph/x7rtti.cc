// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x5rtti1.h:13:0: warning: .__STDC_IEC_559_COMPLEX__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti1.h:13:0: warning: .__STDC_ISO_10646__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti1.h:13:0: warning: .__STDC_IEC_559__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:15:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:15:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:16:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:16:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:17:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:17:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:18:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:18:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:19:1: error: redefinition of .const char _ZTS15non_polymorphic ..." "" { xfail *-*-* } 0 }
// { dg-bogus "x5rtti2.h:19:1: error: redefinition of .const char _ZTS11polymorphic ..." "" { xfail *-*-* } 0 }


//FIXME We should make this a run test.

#include "x5rtti1.h"
#include "x5rtti2.h"

int main()
{
    return    poly1() == poly2() && nonp1() == nonp2()
           && hpol1() == hpol2() && hnpl1() == hnpl2()
           && poly1() != nonp1() && hpol1() == hnpl1()
           && poly2() != nonp2() && hpol2() == hnpl2();
}
