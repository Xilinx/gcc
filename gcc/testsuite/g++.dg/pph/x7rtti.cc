// { dg-do run }
// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x7rtti.cc:21:0: warning: .__STDC_IEC_559_COMPLEX__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:21:0: warning: .__STDC_ISO_10646__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:21:0: warning: .__STDC_IEC_559__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:24:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:24:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:25:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:25:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:26:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:26:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:27:32: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:27:54: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:28:1: error: redefinition of .const char _ZTS15non_polymorphic ..." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:28:1: error: redefinition of .const char _ZTS11polymorphic ..." "" { xfail *-*-* } 0 }

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
