// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1autometh.h:8:1: internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1autometh.h"

void function() {
    base var1;
    base var2( var1 );
    var1 = var2;
}
