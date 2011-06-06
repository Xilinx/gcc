// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1special.h:10:5: internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1special.h"

B b(1);

int H() {
    F(b);
    B a(3.2);
    B c = b;
    B d(b);
    c = b;
    d = 4;
    G(d);
}
