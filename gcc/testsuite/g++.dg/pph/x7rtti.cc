// FIXME pph: This should be a { dg=do run } (with '=' replaced by '-')
// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x7rtti.cc:19:0: warning: .__STDC_IEC_559_COMPLEX__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:19:0: warning: .__STDC_ISO_10646__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:19:0: warning: .__STDC_IEC_559__. redefined .enabled by default." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:22:26: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:23:22: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:24:29: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:25:22: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:26:29: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:27:22: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:28:29: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:29:22: error: no match for .operator" "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:33:1: error: redefinition of .const char _ZTS15non_polymorphic ..." "" { xfail *-*-* } 0 }
// { dg-bogus "x7rtti.cc:33:1: error: redefinition of .const char _ZTS11polymorphic ..." "" { xfail *-*-* } 0 }

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
