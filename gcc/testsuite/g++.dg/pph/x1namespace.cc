// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in resume_scope" "" { xfail *-*-* } 0 }
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
