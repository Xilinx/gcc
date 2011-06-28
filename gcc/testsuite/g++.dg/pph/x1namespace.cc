// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1namespace.h:18:13: internal compiler error: in resume_scope" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1namespace.h"

namespace A {
int x = 3;
} // namespace A

int y = 4;

int D::method()
{ return y; }

int main()
{ }
