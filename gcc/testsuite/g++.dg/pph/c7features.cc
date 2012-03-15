// {    xfail-if "UNKNOWN" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "warning: .__STDC_IEC_559_COMPLEX__. redefined" "" { xfail *-*-* } 0 }
// { dg-bogus "warning: .__STDC_ISO_10646__. redefined" "" { xfail *-*-* } 0 }
// { dg-bogus "warning: .__STDC_IEC_559__. redefined" "" { xfail *-*-* } 0 }

#include "c5features1.h"
#include "c5features2.h"
