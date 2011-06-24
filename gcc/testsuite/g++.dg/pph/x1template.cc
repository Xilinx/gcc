// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1template.h:18:13: internal compiler error: in resume_scope, at cp/name-lookup.c:1568" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1template.h"

namespace A {
int x = 3;
} // namespace A

int y = 4;

int D::method()
{ return y; }

int main()
{ }
