// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1special.h:19:6: internal compiler error: tree check: expected tree that contains 'decl minimal' structure, have 'overload' in context_for_name_lookup, at cp/search.c:570" "" { xfail *-*-* } 0 }
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
